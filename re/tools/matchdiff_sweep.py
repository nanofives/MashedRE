#!/usr/bin/env python3
"""matchdiff_sweep.py -- TT-2: operand-correspondence check over every hooked function.

Runs the compiler-version-independent half of matchdiff.py (see
re/analysis/matching_compiler_spike_20260909.md) across all RH_ScopedInstall'd
functions and reports, per row, whether the DISTINCT set of absolute addresses and
immediates in our compiled code matches the original's.

    py -3.12 re/tools/matchdiff_sweep.py [--conf C3,C4] [--out re/parity/matchdiff_sweep.csv]

Inputs (all offline -- no Ghidra, no Frida, no running game):
  * mashedmod/src/mashed_re/**/*.cpp  -- RH_ScopedInstall(<symbol>, 0x<rva>) is the
    authoritative symbol -> RVA map (the project's own convention).
  * mashedmod/build/obj/asi/*.obj     -- the .asi build is the right target: it keeps
    the ORIGINAL absolute globals, where the exe build redirects them to named
    standalone symbols under /DMASHED_STANDALONE. Run mashedmod\\build.bat first.
  * re/console/cache/func_bounds.csv  -- original function sizes, cached from Ghidra by
    re/tools/ghidra_scripts/FuncBoundsPC.java. Regenerate only if the master is re-analysed.
  * original/MASHED.exe.unpatched     -- the SHA-256-anchored reference bytes.
  * hooks.csv                          -- confidence per RVA.

WHAT A VERDICT MEANS (do not overclaim -- see re/CONFIDENCE.md):
  PASS  the port touches exactly the addresses and constants the original does. This is
        NOT a promotion gate and moves no C-level: identical operands with wrong control
        flow still passes. Treat as "no transcription defect detected".
  FAIL  a distinct address or immediate is present on one side only. Every FAIL is a
        CANDIDATE transcription defect and must be triaged by hand; some are legitimate
        (see the known false-positive classes below).
  SKIP  not comparable (symbol absent from the obj, no bounds, etc). Not a result.

KNOWN FALSE-POSITIVE CLASSES (triage before filing a defect):
  * multiplicity-only differences -- CSE/duplication. Already advisory, never FAIL.
  * a port that caches a base in a local where the original re-loads an absolute (or
    vice versa) changes which displacements appear literally.
  * inlining: if the original inlined a callee we call (or the reverse), the inlined
    body's addresses appear on one side only.
  * jump tables and switch dispatch move constants into .rdata on one side.
"""
import argparse
import collections
import csv
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from matchdiff import (function_bytes_from_obj, disasm, operand_facts,  # noqa: E402
                       parse_coff, load_func_entries, function_relocs_named)

import pefile  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "mashedmod" / "src" / "mashed_re"
OBJ_ASI = ROOT / "mashedmod" / "build" / "obj" / "asi"
OBJ_EXE = ROOT / "mashedmod" / "build" / "obj" / "exe"
BOUNDS = ROOT / "re" / "console" / "cache" / "func_bounds.csv"
EXE = ROOT / "original" / "MASHED.exe.unpatched"

REG = re.compile(r'RH_ScopedInstall\(\s*([A-Za-z_][A-Za-z0-9_:]*)\s*,\s*(0x[0-9a-fA-F]{8})')


def load_registrations():
    """(symbol, rva) -> source .cpp. Later duplicates are reported, not silently kept.

    A COMMENTED-OUT `// RH_ScopedInstall(...)` still counts: the reimplementation is
    compiled and comparable, only the install is disabled. 66 of 1,336 registrations are
    in that state (e.g. 0x0040d590 RaceRankThreePlayers), which is itself worth knowing,
    so it is recorded per row rather than filtered out."""
    out, dupes = {}, []
    for cpp in SRC.rglob("*.cpp"):
        try:
            text = cpp.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for line in text.splitlines():
            m = REG.search(line)
            if not m:
                continue
            sym, rva = m.group(1), int(m.group(2), 16)
            key = (sym, rva)
            if key in out:
                dupes.append((sym, rva, cpp.name))
                continue
            out[key] = (cpp, not line.lstrip().startswith("//"))
    return out, dupes


def reg_paths(regs):
    """Back-compat view: (symbol, rva) -> cpp path only."""
    return {k: v[0] for k, v in regs.items()}


def load_bounds():
    b = {}
    with open(BOUNDS, encoding="utf-8") as fh:
        for r in csv.DictReader(fh):
            b[int(r["rva"], 16)] = int(r["size"])
    return b


def load_conf():
    c = {}
    with open(ROOT / "hooks.csv", encoding="utf-8", errors="replace") as fh:
        for r in csv.DictReader(fh):
            try:
                c[int(r["rva"], 16)] = (r["confidence"], r["name"], r["subsystem"])
            except (ValueError, KeyError, TypeError):
                pass
    return c



def _wrapper_relief(rec, objdir, regs, bounds, pe, base, ents):
    """If a FAIL is only MISSING addresses, and the same-object callees our exported symbol
    delegates to cover every one of them, the row is a wrapper -- not a defect.
    Returns a PASS detail string, or None. Miss direction only (see caller)."""
    rva = int(rec["rva"], 16)
    cpp = None
    for (sym, r), (path, _inst) in regs.items():
        if r == rva and sym == rec["symbol"]:
            cpp = path
            break
    if cpp is None:
        return None
    objp = objdir / (cpp.stem + ".obj")
    if not objp.exists() or rva not in bounds:
        return None
    data = objp.read_bytes()
    try:
        ours, relocs, _ = function_bytes_from_obj(data, rec["symbol"])
    except SystemExit:
        return None
    orig = pe.get_data(rva - base, bounds[rva])
    fo = operand_facts(disasm(ours, rva), [x[0] for x in relocs], rva, ents)
    fg = operand_facts(disasm(orig, rva), [], rva, ents)
    local = {x["name"] for x in parse_coff(data)[1] if x["sec"] > 0}
    union, via = set(fo[0]), []
    for _off, _t, tname in function_relocs_named(data, rec["symbol"]):
        if tname not in local or tname.lstrip("_") == rec["symbol"]:
            continue
        try:
            b, br, _ = function_bytes_from_obj(data, tname.lstrip("_"))
        except SystemExit:
            continue
        union |= set(operand_facts(disasm(b, rva), [x[0] for x in br], rva, ents)[0])
        via.append(tname.lstrip("_"))
    if via and not (set(fg[0]) - union):
        short = [v.split("@")[0].lstrip("?") for v in via][:2]
        return "WRAPPER: resolved via " + ",".join(short)
    return None

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--conf", default="C3,C4")
    ap.add_argument("--rva", action="append", default=[],
                    help="check only this RVA (repeatable); ignores --conf. "
                         "Use after authoring a hook, before spending a Frida boot.")
    ap.add_argument("--symbol", action="append", default=[],
                    help="check only this exported symbol (repeatable); ignores --conf.")
    ap.add_argument("--quiet", action="store_true",
                    help="print only the verdict lines (for workflow steps)")
    ap.add_argument("--out", default=str(ROOT / "re" / "parity" / "matchdiff_sweep.csv"))
    ap.add_argument("--obj-dir", default=str(OBJ_ASI))
    a = ap.parse_args()
    want = set(a.conf.split(","))
    pick_rva = {int(x, 16) for x in a.rva}
    pick_sym = set(a.symbol)
    objdir = Path(a.obj_dir)

    regs, dupes = load_registrations()
    bounds, conf = load_bounds(), load_conf()
    ents = load_func_entries()
    pe = pefile.PE(str(EXE), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase

    obj_cache, sym_cache = {}, {}

    def load_obj(path):
        if path not in obj_cache:
            data = path.read_bytes()
            obj_cache[path] = data
            names = set()
            for s in parse_coff(data)[1]:
                if s["sec"] > 0:
                    names.add(s["name"])
            sym_cache[path] = names
        return obj_cache[path]

    rows, tally = [], collections.Counter()
    for (sym, rva), (cpp, installed) in sorted(regs.items(), key=lambda kv: kv[0][1]):
        c = conf.get(rva, ("", "", ""))
        if pick_rva or pick_sym:
            if rva not in pick_rva and sym not in pick_sym:
                continue
        elif c[0] not in want:
            continue
        rec = dict(rva="%08x" % rva, symbol=sym, name=c[1], conf=c[0],
                   subsystem=c[2], file=cpp.name,
                   installed="yes" if installed else "COMMENTED",
                   verdict="", imm="", detail="")
        objp = objdir / (cpp.stem + ".obj")
        if not objp.exists():
            rec.update(verdict="SKIP", detail="no obj (TU not in this target's list)")
        elif rva not in bounds:
            rec.update(verdict="SKIP", detail="no Ghidra bounds for RVA")
        else:
            data = load_obj(objp)
            if sym not in sym_cache[objp] and ("_" + sym) not in sym_cache[objp]:
                rec.update(verdict="SKIP", detail="symbol absent from obj (mangled/static/inlined)")
            else:
                try:
                    ours, relocs, _sec = function_bytes_from_obj(data, sym)
                    orig = pe.get_data(rva - base, bounds[rva])
                    ro = [r[0] for r in relocs]
                    fo = operand_facts(disasm(ours, rva), ro, rva, ents)
                    fg = operand_facts(disasm(orig, rva), [], rva, ents)

                    def delta(i):
                        so, sg = set(fo[i]), set(fg[i])
                        if so == sg:
                            return ""
                        return "-%s +%s" % (
                            ",".join(hex(x) for x in sorted(sg - so)[:6]) or "-",
                            ",".join(hex(x) for x in sorted(so - sg)[:6]) or "-")

                    d_addr, d_imm = delta(0), delta(3)
                    rec["imm"] = "DIFF" if d_imm else "same"
                    if d_addr:
                        rec.update(verdict="FAIL", detail="data " + d_addr +
                                   (("; imm " + d_imm) if d_imm else ""))
                    else:
                        rec.update(verdict="PASS",
                                   detail=("imm " + d_imm) if d_imm else
                                   "data=%d ours=%dB orig=%dB" % (
                                       len(set(fg[0])), len(ours), len(orig)))
                except Exception as exc:                      # noqa: BLE001
                    rec.update(verdict="SKIP", detail="error: %s" % exc)
        tally[rec["verdict"]] += 1
        rows.append(rec)

    if not rows:
        print("no rows matched (unknown RVA/symbol, or the TU is not in this target)")
        return 2

    outp = Path(a.out)
    outp.parent.mkdir(parents=True, exist_ok=True)
    with open(outp, "w", encoding="utf-8", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=list(rows[0].keys()) if rows else
                           ["rva", "symbol", "name", "conf", "subsystem", "file", "verdict", "detail"])
        w.writeheader()
        w.writerows(rows)

    if pick_rva or pick_sym:
        # single-hook mode: terse verdict + nonzero exit on FAIL, so it can gate a
        # workflow step (promote-c3-batch per-function step 6).
        #
        # The GATE is stricter about false positives than the census above, on purpose:
        # a thin wrapper (a naked ABI shim, or a Core/ShadowAB.h SHADOW_AB site, where the
        # exported symbol delegates to a *_impl in the same TU) leaves the exported body
        # with none of the original's globals, and every one reads as "missing". Union the
        # same-object callees into OUR side before judging -- MISS direction only, never
        # extras (a helper's own globals say nothing about the original; folding them both
        # ways made CANDIDATE go UP when tried in triage). Deeper/cross-TU chains still
        # need matchdiff_triage.py.
        bad = 0
        for r in rows:
            if r["verdict"] == "FAIL" and r["detail"].startswith("data -") and                "+-" in r["detail"].split("; imm")[0]:
                rr = _wrapper_relief(r, objdir, regs, bounds, pe, base, ents)
                if rr:
                    r["verdict"], r["detail"] = "PASS", rr
            print("%s %s  %-30s %s" % (r["verdict"], r["rva"], r["symbol"][:30], r["detail"][:90]))
            bad += (r["verdict"] == "FAIL")
        if bad:
            print("\n%d FAIL -- the port references .data addresses the original does not "
                  "(or misses ones it does). Re-read the plate BEFORE spending a Frida boot; "
                  "see the artifact classes in re/analysis/matchdiff_triage_20260909.md." % bad)
        return 1 if bad else 0

    print("registrations parsed: %d (%d duplicate sites)" % (len(regs), len(dupes)))
    print("rows compared (%s): %d" % (a.conf, len(rows)))
    for k in ("PASS", "FAIL", "SKIP"):
        print("  %-5s %d" % (k, tally[k]))
    comparable = tally["PASS"] + tally["FAIL"]
    if comparable:
        print("  data-address PASS rate over comparable rows: %.1f%%" %
              (100.0 * tally["PASS"] / comparable))
    nimm = sum(1 for r in rows if r["imm"] == "DIFF")
    print("  advisory: immediate-set differs on %d of %d comparable rows" % (nimm, comparable))
    print("wrote %s" % outp)
    if tally["FAIL"]:
        print("\nFAIL rows (triage each; see the false-positive classes in the docstring):")
        for r in rows:
            if r["verdict"] == "FAIL":
                print("  %s %-34s %-3s %-10s %s" %
                      (r["rva"], r["symbol"][:34], r["conf"], r["subsystem"][:10], r["detail"][:96]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
