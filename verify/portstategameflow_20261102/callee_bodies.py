"""MATCH-PORTSTATE-GAMEFLOW: which of 0x004929d0's depth-1 callees have a body in each target.

A body = a TU with `RH_ScopedInstall(<sym>, 0x<rva>)` (uncommented); target = whether that TU is listed in
mashedmod/exe_sources.rsp / asi_sources.rsp. Callee list: re/analysis/game_state/0x004929d0.md frontmatter.
Run from the worktree root: py -3.12 verify/portstategameflow_20261102/callee_bodies.py
"""
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / 'mashedmod' / 'src' / 'mashed_re'
CALLEES = ('004927c0 0042c2e0 0042c2d0 0042b940 0042c2f0 0042b930 0042b8d0 0040d440 0042c1d0 0042c220 0042c280 '
           '0042c1c0 0042f6a0 0040e350 005c9d00 00432080 0040e450 0042b910 004331a0 0040b6e0 0040b540 0042f500 '
           '0040b6d0 0040b430 0040b700 00448700 0040e460 0042b8f0 0042b8e0 0040d270').split()


def rsp(name):
    return {l.strip().strip('"').replace('\\', '/').lower()
            for l in (ROOT / 'mashedmod' / name).read_text(encoding='utf-8').splitlines() if l.strip()}


exe, asi = rsp('exe_sources.rsp'), rsp('asi_sources.rsp')
inst = {}
for f in SRC.rglob('*.cpp'):
    for line in f.read_text(encoding='utf-8', errors='replace').splitlines():
        s = line.strip()
        if s.startswith('//'):
            continue
        m = re.search(r'RH_ScopedInstall\(\s*(\w+)\s*,\s*0x([0-9a-fA-F]{8})\s*\)', s)
        if m:
            inst.setdefault(m.group(2).lower(), []).append((m.group(1), f.relative_to(SRC).as_posix()))
for r in CALLEES:
    rows = inst.get(r, [])
    if not rows:
        print(f'0x{r}  none')
        continue
    for sym, rel in rows:
        t = ('exe+asi' if rel.lower() in exe and rel.lower() in asi else 'exe' if rel.lower() in exe
             else 'asi' if rel.lower() in asi else 'neither')
        print(f'0x{r}  {t:8s} {sym}  {rel}')
