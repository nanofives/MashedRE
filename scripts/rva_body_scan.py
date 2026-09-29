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
            name_m = re.search(r"([A-Za-z_~][\w:]*)\s*\(", cand)
            name = name_m.group(1) if name_m else cand.strip()[:40]
            claimed[j] = (rva, name)      # last writer wins = closest comment

        # Active installs: the line must not be commented out.
        stripped = ln.lstrip()
        if stripped.startswith("//") or stripped.startswith("/*") or stripped.startswith("*"):
            continue
        for sym, hx in RE_INSTALL.findall(ln):
            installs.setdefault(int(hx, 16), []).append((sym, i + 1))

    for j, (rva, name) in claimed.items():
        bodies.setdefault(rva, (name, j + 1))
    return bodies, installs


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
