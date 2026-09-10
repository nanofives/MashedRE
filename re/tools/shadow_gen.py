#!/usr/bin/env python3
"""shadow_gen.py -- convert installed C2 ports into in-process shadow A/B sites (TT-11).

    py -3.12 re/tools/shadow_gen.py                     # dry run over the default candidate set
    py -3.12 re/tools/shadow_gen.py --apply             # rewrite the sources
    py -3.12 re/tools/shadow_gen.py --rvas 0x0055fe50,0x00561040 --apply
    py -3.12 re/tools/shadow_gen.py --region 0x00442440=param_1+0x4c:0x80 --apply

WHY
---
The shadow lane (mashedmod/src/mashed_re/Core/ShadowAB.h) is the only A/B lane left that
does not die on the four Frida blockers, and the 2026-09-09 session measured its ceiling as
"every site that exists" -- 7 sites, all hand-written. Meanwhile 153 C2 rows already have a
compiled, installed, operand-check-PASSing port (re/parity/matchdiff_sweep_c2.csv). The
gap between the two numbers is boilerplate, and boilerplate is what this script writes.

WHAT IT DOES, PER CANDIDATE
---------------------------
  1. finds the port's definition   `extern "C" RET __cdecl Name(params) {`   in its TU
  2. renames the body to           `static RET __cdecl Name_impl(params) {`
  3. emits, before the RH_ScopedInstall line, a wrapper with the ORIGINAL name/linkage that
     runs ShadowAB::Run (return-value ports) or ShadowAB::RunRegion (--region given)
  4. adds `#include "../Core/ShadowAB.h"` next to the HookSystem include if missing

Every other symbol in the TU keeps calling `Name`, so nested calls hit the wrapper and take
the plain path under ShadowAB::Reentry -- the same shape as the hand-written sites.

WHAT IT REFUSES (and says so in the manifest)
---------------------------------------------
  * void ports with no --region: a region is a claim about what the function writes, and
    that claim has to come from the analysis note, not from a regex. They are listed as
    NEEDS_REGION with their parameter list so the region can be supplied per RVA.
  * __thiscall / naked / variadic / unnamed-parameter definitions.
  * ports whose definition or RH_ScopedInstall line cannot be located unambiguously.
  * RVAs that already carry a SHADOW_AB_COUNTER.

The manifest (re/parity/shadow_sites.tsv) is the input to shadow_batch.py, and a clean run
there is EVIDENCE for re-classify, never an automatic C-level (CLAUDE.md, NO overclaiming).
"""
import argparse
import csv
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "mashedmod" / "src" / "mashed_re"
SWEEP = ROOT / "re" / "parity" / "matchdiff_sweep_c2.csv"
MANIFEST = ROOT / "re" / "parity" / "shadow_sites.tsv"

PHASES = {"race": "ShadowAB::kPhaseRace", "menu": "ShadowAB::kPhaseMenu", "any": "ShadowAB::kPhaseAny"}

# Definition (not declaration): optional linkage/export, return type, optional cc, name,
# params (may span lines), then an opening brace.
DEF_RE_T = (r'^(?P<lead>[ \t]*)(?P<ext>extern\s+"C"\s+)?(?P<exp>__declspec\(dllexport\)\s+)?'
            r'(?P<ret>[\w:]+(?:\s*[\*&]|\s+[\w:]+)*?)\s*(?P<cc>__cdecl|__stdcall|__fastcall|__thiscall)?'
            r'\s+\b(?P<name>{name})\s*\((?P<params>[^)]*)\)\s*(?P<brace>\{{)')


def norm_rva(s):
    s = s.strip().lower()
    if s.startswith("0x"):
        s = s[2:]
    return s.zfill(8)


def load_candidates(args):
    """Yield (rva8, symbol, file) rows to try."""
    if args.rvas:
        want = {norm_rva(x) for x in args.rvas.split(",") if x.strip()}
    else:
        want = None
    rows = []
    with open(args.sweep, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f):
            rva = norm_rva(r["rva"])
            if want is not None and rva not in want:
                continue
            if want is None and not (r.get("verdict") == "PASS" and r.get("installed") == "yes"):
                continue
            rows.append((rva, r["symbol"], r["file"], r.get("subsystem", "")))
    if want is not None:
        missing = want - {r[0] for r in rows}
        for m in sorted(missing):
            rows.append((m, "", "", ""))
    return rows


def index_sources():
    loc = {}
    for dp, _, fn in os.walk(SRC):
        for f in fn:
            if f.endswith((".cpp", ".h")):
                loc.setdefault(f, []).append(Path(dp) / f)
    return loc


def split_params(s):
    out, depth, cur = [], 0, []
    for ch in s:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append("".join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    tail = "".join(cur).strip()
    if tail:
        out.append(tail)
    return out


def param_names(params_text):
    """Return (names, reason). names is None when the list cannot be forwarded verbatim."""
    text = re.sub(r"/\*.*?\*/", "", params_text, flags=re.S)
    text = re.sub(r"//[^\n]*", "", text)
    text = " ".join(text.split())
    if text in ("", "void"):
        return [], None
    names = []
    for p in split_params(text):
        if "..." in p:
            return None, "variadic"
        if "(" in p:
            return None, "function-pointer param"
        p = re.sub(r"=.*$", "", p).strip()          # default args (none expected in C linkage)
        m = re.search(r"(\w+)\s*(\[[^\]]*\])?$", p)
        if not m or m.group(2):
            return None, f"cannot name param `{p}`"
        name = m.group(1)
        # a bare type ("int", "float *") has no separate name token
        if re.fullmatch(r"(unsigned|signed|int|char|short|long|float|double|bool|void|"
                        r"undefined\d?|uint|ushort|byte|ulonglong|longlong|float10|"
                        r"std::u?int\d+_t|u?int\d+_t|size_t)", name):
            return None, f"unnamed param `{p}`"
        names.append(name)
    return names, None


def find_def(src, name):
    rx = re.compile(DEF_RE_T.format(name=re.escape(name)), re.M)
    ms = [m for m in rx.finditer(src) if not m.group("ret").strip().startswith(("return", "//"))]
    return ms


def make_wrapper(m, rva, phase_expr, region, impl_name, tracked=False):
    ext = m.group("ext") or ""
    exp = m.group("exp") or ""
    ret = m.group("ret").strip()
    cc = (m.group("cc") + " ") if m.group("cc") else ""
    name = m.group("name")
    params = " ".join(m.group("params").split())
    names, _ = param_names(m.group("params"))
    fwd = ", ".join(names)
    lines = [
        f"// --- 0x{rva} shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------",
        f"// In-process original-vs-port comparison at the real call site; arm with",
        f"// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.",
    ]
    if tracked:
        lines += [
            f"// LANE 3: page-level write tracking (Core/ShadowTrack.h) -- no region spec; every page",
            f"// the call writes plus the caller's 4 KB stack window is compared, pre-state restored",
            f"// between the two runs. Verifies EFFECTS; read the pages= detail on a DIVERGENT row.",
            f"{ext}{exp}void {cc}{name}({params}) {{",
            f'    SHADOW_AB_COUNTER(ab, "{name}", 0x{rva}u, {phase_expr});',
            f"    ShadowAB::RunTracked(ab, {impl_name}, SHADOW_STACK_WINDOW()" + (f", {fwd}" if fwd else "") + ");",
            "}",
        ]
    elif region:
        expr, nbytes = region
        lines += [
            f"// OUTPUT REGION: {nbytes} bytes at ({expr}) -- supplied by hand via --region;",
            f"// the claim that this span covers every write MUST be backed by the analysis note.",
            f"{ext}{exp}void {cc}{name}({params}) {{",
            f"    SHADOW_AB_COUNTER(ab, \"{name}\", 0x{rva}u, {phase_expr});",
            f"    ShadowAB::RunRegion(ab, {impl_name}, reinterpret_cast<void*>({expr}), {nbytes}"
            + (f", {fwd}" if fwd else "") + ");",
            "}",
        ]
    else:
        lines += [
            f"{ext}{exp}{ret} {cc}{name}({params}) {{",
            f"    SHADOW_AB_COUNTER(ab, \"{name}\", 0x{rva}u, {phase_expr});",
            f"    return ShadowAB::Run(ab, {impl_name}" + (f", {fwd}" if fwd else "") + ");",
            "}",
        ]
    return "\n".join(lines) + "\n"


def add_include(src, tracked=False):
    m = re.search(r'^[ \t]*#include\s+"([^"]*?)Core/HookSystem\.h"[^\n]*\n', src, re.M)
    if not m:
        return None
    prefix = m.group(1)
    add = ""
    if 'Core/ShadowAB.h"' not in src:
        add += '#include "' + prefix + 'Core/ShadowAB.h"\n'
    if tracked and 'Core/ShadowTrack.h"' not in src:
        add += '#include "' + prefix + 'Core/ShadowTrack.h"\n'
    return src[: m.end()] + add + src[m.end():]


def process(rva, symbol, fname, subsystem, loc, args, regions):
    rec = {"rva": rva, "name": symbol, "file": fname, "subsystem": subsystem, "kind": "",
           "ret": "", "cc": "", "params": "", "phase": args.phase, "status": ""}
    if not fname:
        rec["status"] = "SKIP:not-in-sweep"
        return rec, None
    paths = loc.get(fname)
    if not paths:
        rec["status"] = "SKIP:file-not-found"
        return rec, None
    if len(paths) > 1:
        rec["status"] = "SKIP:ambiguous-file " + ";".join(str(p.relative_to(SRC)) for p in paths)
        return rec, None
    path = paths[0]
    src = path.read_text(encoding="utf-8", errors="surrogateescape")
    if re.search(r"SHADOW_AB_COUNTER\([^;]*0x0*%s" % rva.lstrip("0"), src, re.I):
        rec["status"] = "ALREADY:shadow-site-present"
        rec["kind"] = "existing"
        # a site this script generated earlier: recover ret/cc/params from the _impl definition
        # so the manifest keeps its kind (the reporter relies on ret_float10) across re-runs
        impl_defs = find_def(src, symbol + "_impl")
        if len(impl_defs) == 1:
            im = impl_defs[0]
            ret_i = im.group("ret").replace("static", "").strip()
            rec.update(ret=ret_i, cc=im.group("cc") or "__cdecl",
                       params=" ".join(im.group("params").split()))
            rec["kind"] = "ret_float10" if ret_i == "float10" else ("ret" if ret_i != "void" else "region")
        return rec, None
    inst = re.search(r"^[ \t]*RH_ScopedInstall\(\s*%s\s*,\s*0x0*%s\s*\)" % (re.escape(symbol), rva.lstrip("0")),
                     src, re.M | re.I)
    if not inst:
        rec["status"] = "SKIP:no-live-RH_ScopedInstall"
        return rec, None
    defs = find_def(src, symbol)
    if len(defs) != 1:
        rec["status"] = f"SKIP:definition-count={len(defs)}"
        return rec, None
    m = defs[0]
    if m.start() > inst.start():
        rec["status"] = "SKIP:definition-after-install-line"
        return rec, None
    ret = m.group("ret").strip()
    cc = m.group("cc") or "__cdecl"
    rec.update(ret=ret, cc=cc, params=" ".join(m.group("params").split()))
    if cc == "__thiscall":
        rec["status"] = "SKIP:thiscall"
        return rec, None
    head = src[max(0, m.start() - 200): m.start()]
    if "__declspec(naked)" in src[m.start(): m.end()] or "naked" in head.splitlines()[-1:]:
        rec["status"] = "SKIP:naked"
        return rec, None
    names, why = param_names(m.group("params"))
    if names is None:
        rec["status"] = "SKIP:" + why
        return rec, None
    region = regions.get(rva)
    tracked = False
    if ret == "void" and not region:
        if not args.tracked:
            rec["kind"] = "needs_region"
            rec["status"] = "NEEDS_REGION:void port; pass --region 0x%s=<expr>:<bytes> (or --tracked)" % rva
            return rec, None
        tracked = True
    if ret != "void" and region:
        rec["status"] = "SKIP:--region given for a non-void port (use Run, not RunRegion)"
        return rec, None
    rec["kind"] = "tracked" if tracked else ("region" if region else ("ret_float10" if ret == "float10" else "ret"))
    # float10 = x87 80-bit ST0 in the original; MSVC long double is 64-bit, so Run() compares two
    # differently-rounded doubles. A DIVERGENT on such a row is a lane limit, not a port verdict
    # (FUN_005667c0, 48/48 low-mantissa mismatches, 2026-09-10). The reporter labels these.
    impl = symbol + "_impl"
    if re.search(r"\b%s\b" % re.escape(impl), src):
        rec["status"] = f"SKIP:{impl} already exists"
        return rec, None

    # --- rewrite -------------------------------------------------------------------
    lead = m.group("lead")
    # Keep the ORIGINAL name declared from here on: other functions in the TU may call it
    # between this definition and the wrapper (which lands just before RH_ScopedInstall).
    # Without this, the first build after generation failed on three such call sites.
    ext = m.group("ext") or ""
    exp = m.group("exp") or ""
    params_flat = " ".join(m.group("params").split())
    fwd_decl = f"{lead}{ext}{exp}{ret} {cc} {symbol}({params_flat});" + chr(10)
    new_def = f"{fwd_decl}{lead}static {ret} {cc} {impl}({m.group('params')}) {{"
    wrapper = make_wrapper(m, rva, PHASES[args.phase], region, impl, tracked)
    new_src = src[: m.start()] + new_def + src[m.end():]
    # re-locate the install line in the rewritten text (offsets shifted)
    inst2 = re.search(r"^[ \t]*RH_ScopedInstall\(\s*%s\s*,\s*0x0*%s\s*\)" % (re.escape(symbol), rva.lstrip("0")),
                      new_src, re.M | re.I)
    new_src = new_src[: inst2.start()] + wrapper + new_src[inst2.start():]
    with_inc = add_include(new_src, tracked)
    if with_inc is None:
        rec["status"] = "SKIP:no HookSystem.h include to anchor ShadowAB.h"
        return rec, None
    rec["status"] = "GENERATED" if args.apply else "WOULD_GENERATE"
    return rec, (path, with_inc)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--sweep", default=str(SWEEP), help="matchdiff C2 sweep CSV (candidate source)")
    ap.add_argument("--rvas", default="", help="comma list of RVAs to restrict to (else PASS+installed)")
    ap.add_argument("--region", action="append", default=[],
                    help="0xRVA=<c++ expr over param names>:<bytes> -- RunRegion for a void port")
    ap.add_argument("--phase", choices=sorted(PHASES), default="race",
                    help="sampling phase gate (race = single-threaded in-race, the safe default)")
    ap.add_argument("--tracked", action="store_true",
                    help="void ports with no --region: emit a ShadowAB::RunTracked wrapper (Lane 3, "
                         "page-level write tracking, Core/ShadowTrack.h) instead of NEEDS_REGION")
    ap.add_argument("--apply", action="store_true", help="write the rewritten sources (default: dry run)")
    ap.add_argument("--manifest", default=str(MANIFEST))
    args = ap.parse_args()

    regions = {}
    for spec in args.region:
        k, v = spec.split("=", 1)
        expr, nbytes = v.rsplit(":", 1)
        regions[norm_rva(k)] = (expr.strip(), nbytes.strip())

    loc = index_sources()
    cands = load_candidates(args)
    recs, edits = [], {}
    for rva, sym, fname, subsys in cands:
        rec, edit = process(rva, sym, fname, subsys, loc, args, regions)
        recs.append(rec)
        if edit:
            path, text = edit
            # several candidates share a TU: chain the edits on the latest text
            if path in edits:
                # re-run on the accumulated text so offsets are right
                loc_one = {fname: [path]}
                src_path = path
                tmp = src_path.read_text(encoding="utf-8", errors="surrogateescape")
                try:
                    src_path.write_text(edits[path], encoding="utf-8", errors="surrogateescape")
                    rec2, edit2 = process(rva, sym, fname, subsys, loc_one, args, regions)
                finally:
                    src_path.write_text(tmp, encoding="utf-8", errors="surrogateescape")
                recs[-1] = rec2
                if edit2:
                    edits[path] = edit2[1]
            else:
                edits[path] = text

    if args.apply:
        for path, text in edits.items():
            path.write_text(text, encoding="utf-8", errors="surrogateescape")

    # merge into the manifest: keep rows for RVAs not touched this run
    old = {}
    mp = Path(args.manifest)
    if mp.exists():
        with open(mp, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f, delimiter="\t"):
                old[r["rva"]] = r
    for r in recs:
        if r["status"] == "WOULD_GENERATE" and r["rva"] in old:
            continue                               # dry run must not clobber a real row
        old[r["rva"]] = r
    fields = ["rva", "name", "file", "subsystem", "kind", "ret", "cc", "params", "phase", "status"]
    with open(mp, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=fields, delimiter="\t")
        w.writeheader()
        for k in sorted(old):
            w.writerow({fld: old[k].get(fld, "") for fld in fields})

    import collections
    tally = collections.Counter(r["status"].split(":")[0] for r in recs)
    print(f"candidates={len(recs)}  " + "  ".join(f"{k}={v}" for k, v in sorted(tally.items())))
    for r in recs:
        if not r["status"].startswith(("GENERATED", "WOULD")):
            print(f"  {r['rva']}  {r['name']:<34} {r['status']}")
    print(f"{'wrote' if args.apply else 'would write'} {len(edits)} file(s); manifest -> {mp.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
