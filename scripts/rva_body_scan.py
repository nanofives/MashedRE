"""Shared scanner: which TU, in which build target, carries a body for which RVA.

Used by two consumers:
  * `scripts/backfill_exe_file.py` -- fills the `exe_file` column in hooks.csv.
  * `scripts/lint_rva_bodies.py`   -- the one-copy build guard.

Method is the one used by `re/analysis/DUAL_COPY_AUDIT_2026-09-29.md` §2, restated
here mechanically so both consumers agree:

  TU sets      `mashedmod/exe_sources.rsp` and `mashedmod/asi_sources.rsp` (one
               quoted, backslashed relative path per line), plus the five TUs
               `mashedmod/build.bat` compiles outside the rsp lists
               (`Collision/QhullBridge.cpp` and the four `LibRw/*.cpp`), which are
               EXE-ONLY (`build.bat:60-95`).

  Body         a comment line whose first token after `//` is an RVA
               (`// 0x004xxxxx ...`; leading bullets/box-drawing allowed), bound
               to the NEXT following function DEFINITION -- a signature line that
               reaches an opening brace, skipping intervening comments, rejecting
               `if/for/while/switch/return/#...`. RVAs >= 0x005d0000 are dropped:
               that is past `.text`, so those comments annotate data addresses,
               not code (extern-global declarations sit under such comments).

  Install      `RH_ScopedInstall(<Symbol>, 0x<RVA>)`, EXCLUDING commented-out
               lines. A plain regex over-counts by 62 RVAs; see the audit §2.5 and
               memory `stale-c3-body-behind-disabled-install`.

NO-GUESSING note: this is a TEXT scanner. It proves a body is *compiled into* a
target; it does NOT prove the target's call graph reaches it (in the exe
`RH_ScopedInstall` is a no-op -- `Stubs/HookSystemNoOp.cpp:19`), and it cannot see
a port whose author wrote no RVA comment above the definition. Both limits are
stated in the audit and hold here.
"""
from __future__ import annotations

import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parent.parent
SRC = ROOT / "mashedmod" / "src" / "mashed_re"

# TUs build.bat compiles directly, outside either .rsp list. All exe-only.
BUILD_BAT_EXE_ONLY = [
    "Collision/QhullBridge.cpp",
    "LibRw/RwBridge.cpp",
    "LibRw/RwRasterBridge.cpp",
    "LibRw/RwSceneBuild.cpp",
    "LibRw/RwRaceSubmit.cpp",
    "LibRw/RwApiBind.cpp",          # PORT-TOP-03
]

# `.text` window of MASHED.exe. Below TEXT_BEG the comment is a struct offset or
# a literal (`// 0x00000000`), at/above TEXT_END it is `.rdata`/`.data` (those
# comments annotate extern globals, not code). Both ends are exclusions the audit
# §2.2 applies; the low end is added here because the raw regex also picks up
# `// 0x00000341`-style comments that are not function starts.
TEXT_BEG = 0x00401000
TEXT_END = 0x005D0000

# `// 0x004xxxxx` possibly behind a bullet / box-drawing prefix.
RE_RVA_COMMENT = re.compile(r"^\s*(?://+|/\*|\*)[\s─-╿\-=*#|+>•]*"
                            r"(0x00[0-9a-fA-F]{6})\b")
# A function DEFINITION: something that looks like a signature and opens a brace.
RE_NOT_A_DEF = re.compile(r"^\s*(?:if|for|while|switch|do|else|return|case|"
                          r"#|//|/\*|\*|\}|struct\b|class\b|enum\b|typedef\b)")
RE_DEF = re.compile(r"^[A-Za-z_~].*\)")   # starts at col 0-ish with a name, has ')'
# A matched source compiled as the body (C4 by match): `#include "<...>/re/match/<...>.c"`.
RE_MATCH_INCLUDE = re.compile(r'^\s*#\s*include\s+"((?:\.\./)*re/match/[^"]+\.c)"')
RE_INSTALL = re.compile(r"RH_ScopedInstall\s*\(\s*([A-Za-z_][\w:<>]*)\s*,\s*"
                        r"(0x[0-9a-fA-F]{6,8})\s*\)")

# --- second anchoring pass (U-9156, 2026-09-30) -----------------------------
# An RVA *header* comment: the RVA and then the function's NAME, in either
# spelling the tree uses, with the separators it uses. This is deliberately as
# strict as the first pass's comment form -- what it drops is the requirement
# that the DEFINITION be the next thing in the file.
#     // 0x0046b1c0 - VehicleBuildContactHull(slot, box)
#     // FUN_0046b1c0 - VehicleBuildContactHull(slot, box)
#     // 0x0046b1c0  VehicleBuildContactHull
# It does NOT match `// 0x00470afe  call 0x0046ef70` (the token after the RVA
# is not a function this TU defines) or a multi-RVA line.
RE_RVA_HEADER = re.compile(
    r"^\s*(?://+|/\*|\*)[\s─-╿\-=*#|+>•]*"
    r"(?:0x|FUN_)(00[0-9a-fA-F]{6})\b"
    r"[\s–—:,\-]*"
    r"([A-Za-z_~][\w:]*)")


def _rsp_paths(rsp: pathlib.Path) -> list[str]:
    out = []
    for ln in rsp.read_text(encoding="utf-8", errors="replace").splitlines():
        ln = ln.strip().strip('"')
        if ln:
            out.append(ln.replace("\\", "/"))
    return out


def load_targets() -> dict[str, set[str]]:
    """{'exe': {rel paths}, 'asi': {rel paths}} -- repo-relative to SRC, '/'-joined."""
    exe = set(_rsp_paths(ROOT / "mashedmod" / "exe_sources.rsp"))
    exe |= set(BUILD_BAT_EXE_ONLY)
    asi = set(_rsp_paths(ROOT / "mashedmod" / "asi_sources.rsp"))
    return {"exe": exe, "asi": asi}


# HB-LINT-408A50 (2026-11-01): an `#include` of a re/match/ source or a local .inc that does not
# resolve used to be skipped in silence. The LAN publish (scripts/lan_release/publish.sh) built
# from a `git archive` without re/match/, so the scanner lost MatchS01.cpp's body of 0x00408a50,
# reported the other two as a NEW pair the 3-file allowlist key did not match, and the train
# (full worktree) never saw it. Every such miss is recorded here; lint_rva_bodies.py fails on it.
MISSING_INCLUDES: list[tuple[str, int, str]] = []   # (TU rel or included file, 1-based line, include text)


def _is_comment_or_blank(ln: str) -> bool:
    s = ln.strip()
    return (not s) or s.startswith("//") or s.startswith("/*") or s.startswith("*")


def scan_tu(rel: str) -> tuple[dict[int, tuple[str, int]], dict[int, list[tuple[str, int]]]]:
    """Return (bodies, installs) for one TU.

    bodies  : {rva -> (function_name, 1-based line of the definition)}
    installs: {rva -> [(symbol, 1-based line), ...]}  -- ACTIVE installs only
    """
    p = SRC / rel
    if not p.exists():
        return {}, {}
    lines = p.read_text(encoding="utf-8", errors="replace").splitlines()
    bodies, installs = _scan_lines(lines)
    # ------------------------------------------------------------------
    # THIRD PASS (MATCH-S02 2026-10-31): byte-matched bodies. A port wrapper TU
    # `#include`s a matched source from `re/match/` (C4 by byte match: the port
    # compiles that exact source as the function's body). Each `// MATCH: 0x<rva>
    # <symbol>` line of the included file is a body of THIS TU, anchored at the
    # include line, so the one-body rule still sees it.
    for i, ln in enumerate(lines):
        mi = RE_MATCH_INCLUDE.match(ln)
        if not mi:
            continue
        inc = (p.parent / mi.group(1)).resolve()
        if not inc.is_file():
            MISSING_INCLUDES.append((rel, i + 1, mi.group(1)))
            continue
        for mln in inc.read_text(encoding="utf-8", errors="replace").splitlines():
            mm = RE_MATCH_LINE.match(mln)
            if mm:
                rva = int(mm.group(1), 16)
                if TEXT_BEG <= rva < TEXT_END:
                    bodies.setdefault(rva, (mm.group(2), i + 1))

    # FOURTH PASS (HB-LINT-INC-SCAN 2026-10-31): `#include "<x>.inc"`. A TU that
    # includes a local .inc owns every body in it (the naked-asm files generated by
    # re/tools/gen_naked_asm.py). Before this pass they were invisible, which hid
    # the second exe body of 0x0040e370 (AiTargetClosest_naked.inc) from the lint.
    # Bodies are anchored at the include line; nested .inc includes are followed.
    _scan_inc(p, lines, bodies, set())
    return bodies, installs


def _scan_inc(p: pathlib.Path, lines: list[str], bodies: dict, seen: set,
              anchor: int | None = None) -> None:
    for i, ln in enumerate(lines):
        mi = RE_INC_INCLUDE.match(ln)
        if not mi:
            continue
        inc = (p.parent / mi.group(1)).resolve()
        if not inc.is_file():
            try:
                where = p.resolve().relative_to(SRC.resolve()).as_posix()
            except ValueError:
                where = str(p)
            MISSING_INCLUDES.append((where, i + 1, mi.group(1)))
            continue
        if inc in seen:
            continue
        seen.add(inc)
        ilines = inc.read_text(encoding="utf-8", errors="replace").splitlines()
        ib, _ii = _scan_lines(ilines)
        at = anchor if anchor is not None else i + 1
        for rva, (name, _ln) in ib.items():
            bodies.setdefault(rva, (name, at))
        _scan_inc(inc, ilines, bodies, seen, at)


def _scan_lines(lines: list[str]):
    """Passes 1-2 over one file's lines: (bodies, installs)."""
    bodies: dict[int, tuple[str, int]] = {}
    installs: dict[int, list[tuple[str, int]]] = {}
    # def_line -> (rva, name). When several RVA comments precede ONE definition
    # (a header block listing related addresses), only the CLOSEST one is that
    # definition's anchor -- the convention is `// 0x00467650` directly above.
    # Without this the scanner reports mid-function address annotations as bodies.
    claimed: dict[int, tuple[int, str]] = {}
    inc_claims: dict[int, dict[int, str]] = {}   # include line -> {rva: name} (C4-by-match swaps)

    for i, ln in enumerate(lines):
        m = RE_RVA_COMMENT.match(ln)
        if m:
            rva = int(m.group(1), 16)
            if not (TEXT_BEG <= rva < TEXT_END):
                continue
            # Walk forward to the next non-comment, non-blank line; it must be a
            # definition (signature reaching an opening brace) for this to count.
            j = i + 1
            while j < len(lines) and _is_comment_or_blank(lines[j]):
                j += 1
            if j >= len(lines):
                continue
            cand = lines[j]
            # C4-by-match body swap (MATCH-S03, 2026-10-31): the port TU compiles a matched source
            # from re/match/ through `#include`, with the RVA comment directly above the include.
            # The included file holds the definition, so the include line IS the body here.
            mi = RE_MATCH_INCLUDE.match(cand)
            if mi:
                # only the run of RVA comment lines directly above the include (no blank or other
                # comment line in between), so a disassembly block further up is not bound to it
                if not all(RE_RVA_COMMENT.match(lines[k]) for k in range(i + 1, j)):
                    continue
                hm = RE_RVA_HEADER.match(ln)
                # every RVA comment of the comment block directly above the include is a body of it
                # (one matched file may hold a static callee and its caller)
                inc_claims.setdefault(j, {})[rva] = (hm.group(2) if hm and int(hm.group(1), 16) == rva
                                                     else "match:" + mi.group(1))
                continue
            if RE_NOT_A_DEF.match(cand) or not RE_DEF.search(cand):
                continue
            # The signature may span lines; require an opening brace within 6 lines.
            # A declaration (`...);`) is not a body even when a brace follows within
            # 6 lines: RwpSolverCore23.cpp's extern for 0x0055e200 was reported as a
            # second body that way (dcmix 2026-10-22).
            blob = "\n".join(lines[j:j + 6])
            if "{" not in blob:
                continue
            if re.sub(r"//.*", "", cand.split("{")[0]).rstrip().endswith(";"):
                continue
            # Strip leading attribute macros (`__declspec(noinline)`, `extern
            # "C"`, `static`, `inline`, ...) before taking the function name,
            # otherwise every annotated definition is reported as `__declspec`.
            sig = re.sub(r'^\s*(?:extern\s+"C"\s+|__declspec\s*\([^)]*\)\s*|'
                         r'static\s+|inline\s+|__forceinline\s+|naked\s+|'
                         r'__cdecl\s+|__stdcall\s+|__fastcall\s+)+', "", cand)
            name_m = re.search(r"([A-Za-z_~][\w:]*)\s*\(", sig)
            name = name_m.group(1) if name_m else sig.strip()[:40]
            claimed[j] = (rva, name)      # last writer wins = closest comment

        # Active installs: the line must not be commented out.
        stripped = ln.lstrip()
        if stripped.startswith("//") or stripped.startswith("/*") or stripped.startswith("*"):
            continue
        for sym, hx in RE_INSTALL.findall(ln):
            installs.setdefault(int(hx, 16), []).append((sym, i + 1))

    for j, (rva, name) in claimed.items():
        bodies.setdefault(rva, (name, j + 1))
    for j, d in inc_claims.items():
        for rva, name in d.items():
            bodies.setdefault(rva, (name, j + 1))

    # ------------------------------------------------------------------
    # SECOND PASS (U-9156 2026-09-30): bodies whose RVA comment is NOT
    # directly above the definition.
    #
    # The walk above binds an RVA only to the NEXT definition after the
    # comment. `Vehicle/VehicleInit.cpp` annotated `VehicleBuildContactHull`
    # in a header block ~50 lines above its definition, with a `static const
    # float kContactHullBox[6] = {` in between — not a signature, so the walk
    # stopped and the body was invisible. The guard then reported `NEW=0`
    # while a SECOND body for 0x0046b1c0 was compiled into the same target as
    # `Vehicle/VehicleSlotAabbExpand.cpp` (U-9155, commit 7908af78; memory
    # `rva-lint-misses-unanchored-bodies`).
    #
    # CORRECTION to that incident's write-up: the scanner anchored the NAKED
    # copy fine (VehicleSlotAabbExpand.cpp:34). It missed only the
    # VehicleInit.cpp one — which is enough, because DUP-IN-TARGET needs both.
    #
    # Method: bind by FUNCTION NAME. Any comment line carrying a .text RVA
    # *and* the name of a function this TU defines anchors that definition.
    # Conservative on purpose: it never overrides a first-pass binding, it
    # only considers names this TU really defines, and when one name is
    # mentioned next to several RVAs it takes the comment NEAREST the
    # definition.
    defs = _function_defs(lines)
    if defs:
        already = {ln for _r, (_n, ln) in bodies.items()}
        cands: dict[tuple[str, int], list[tuple[int, int]]] = {}   # (name,defline)->[(dist,rva)]
        for i, ln in enumerate(lines):
            m2 = RE_RVA_HEADER.match(ln)
            if not m2:
                continue
            rva = int(m2.group(1), 16)
            if not (TEXT_BEG <= rva < TEXT_END):
                continue
            nm = m2.group(2).split("::")[-1]
            for dline in defs.get(nm, ()):
                if dline in already:
                    continue
                cands.setdefault((nm, dline), []).append((abs(dline - (i + 1)), rva))
        for (nm, dline), lst in cands.items():
            lst.sort()
            # one name annotated with SEVERAL distinct RVAs is ambiguous; skip it
            # rather than pick, so the guard never invents a finding.
            if len({r for _d, r in lst}) != 1:
                continue
            bodies.setdefault(lst[0][1], (nm, dline))

    return bodies, installs


# `#include "<...>/re/match/<...>.c"` in a port TU, and the MATCH line of a matched source.
RE_INC_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+\.inc)"')
RE_MATCH_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]*re/match/[^"]+\.c)"')
RE_MATCH_LINE = re.compile(r"^//\s*MATCH:\s*(?:MASHED\.exe\s+)?(0x[0-9a-fA-F]{6,8})\s+(\S+)")


def _function_defs(lines: list[str]) -> dict[str, list[int]]:
    """{function name -> [1-based definition lines]} for one TU.

    Same signature heuristics the first pass uses, applied to every line
    instead of only to the line after an RVA comment.
    """
    out: dict[str, list[int]] = {}
    for j, cand in enumerate(lines):
        if RE_NOT_A_DEF.match(cand) or not RE_DEF.search(cand):
            continue
        blob = "\n".join(lines[j:j + 6])
        if "{" not in blob:
            continue
        sig = re.sub(r'^\s*(?:extern\s+"C"\s+|__declspec\s*\([^)]*\)\s*|'
                     r'static\s+|inline\s+|__forceinline\s+|naked\s+|'
                     r'__cdecl\s+|__stdcall\s+|__fastcall\s+)+', "", cand)
        m = re.search(r"([A-Za-z_~][\w:]*)\s*\(", sig)
        if not m:
            continue
        nm = m.group(1).split("::")[-1]
        out.setdefault(nm, []).append(j + 1)
    return out


def scan_all() -> dict:
    """Scan every TU in either target.

    Returns {'targets': {...}, 'bodies': {rel -> {rva: (name, line)}},
             'installs': {rel -> {rva: [(sym, line)]}}}
    """
    MISSING_INCLUDES.clear()
    targets = load_targets()
    all_tus = sorted(targets["exe"] | targets["asi"])
    bodies, installs = {}, {}
    for rel in all_tus:
        b, ins = scan_tu(rel)
        if b:
            bodies[rel] = b
        if ins:
            installs[rel] = ins
    return {"targets": targets, "bodies": bodies, "installs": installs,
            "missing": sorted(set(MISSING_INCLUDES))}


def rva_to_tus(scan: dict, target: str) -> dict[int, list[str]]:
    """{rva -> [TUs in `target` that define a body for it]}."""
    tus = scan["targets"][target]
    out: dict[int, list[str]] = {}
    for rel, b in scan["bodies"].items():
        if rel not in tus:
            continue
        for rva in b:
            out.setdefault(rva, []).append(rel)
    return out


if __name__ == "__main__":
    s = scan_all()
    e, a = s["targets"]["exe"], s["targets"]["asi"]
    print(f"TUs: exe={len(e)} asi={len(a)} shared={len(e & a)} "
          f"exe-only={len(e - a)} asi-only={len(a - e)}")
    print(f"TUs with >=1 RVA-anchored body: {len(s['bodies'])}")
    eb, ab = rva_to_tus(s, "exe"), rva_to_tus(s, "asi")
    print(f"RVAs with a body in the exe: {len(eb)}   in the asi: {len(ab)}")
    print(f"RVAs with a body in BOTH targets: {len(set(eb) & set(ab))}")
    dup_exe = {r: v for r, v in eb.items() if len(v) > 1}
    dup_asi = {r: v for r, v in ab.items() if len(v) > 1}
    print(f"RVAs with TWO bodies inside one target: exe={len(dup_exe)} asi={len(dup_asi)}")
