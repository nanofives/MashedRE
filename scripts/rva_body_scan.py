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


def _is_comment_or_blank(ln: str) -> bool:
    s = ln.strip()
    return (not s) or s.startswith("//") or s.startswith("/*") or s.startswith("*")


def scan_tu(rel: str) -> tuple[dict[int, tuple[str, int]], dict[int, list[tuple[str, int]]]]:
    """Return (bodies, installs) for one TU.

    bodies  : {rva -> (function_name, 1-based line of the definition)}
    installs: {rva -> [(symbol, 1-based line), ...]}  -- ACTIVE installs only
    """
    p = SRC / rel
    bodies: dict[int, tuple[str, int]] = {}
    installs: dict[int, list[tuple[str, int]]] = {}
    if not p.exists():
        return bodies, installs
    lines = p.read_text(encoding="utf-8", errors="replace").splitlines()
    # def_line -> (rva, name). When several RVA comments precede ONE definition
    # (a header block listing related addresses), only the CLOSEST one is that
    # definition's anchor -- the convention is `// 0x00467650` directly above.
    # Without this the scanner reports mid-function address annotations as bodies.
    claimed: dict[int, tuple[int, str]] = {}

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
            if RE_NOT_A_DEF.match(cand) or not RE_DEF.search(cand):
                continue
            # The signature may span lines; require an opening brace within 6 lines.
            blob = "\n".join(lines[j:j + 6])
            if "{" not in blob or ";" in cand.split(")")[-1].split("{")[0]:
                if "{" not in blob:
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
    targets = load_targets()
    all_tus = sorted(targets["exe"] | targets["asi"])
    bodies, installs = {}, {}
    for rel in all_tus:
        b, ins = scan_tu(rel)
        if b:
            bodies[rel] = b
        if ins:
            installs[rel] = ins
    return {"targets": targets, "bodies": bodies, "installs": installs}


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
