"""x87_inventory.py -> x87_inventory.tsv: one row per (TU in mashedmod/x87_tus.txt, RVA it carries).
Match TUs: every re/match/<dir>/<rva>.c the TU #includes. Other TUs: every RH_ScopedInstall(..., 0xRVA)
and every '#define FUN_<rva>' in the TU. PORT-X87-PC24-AUDIT."""
import re, subprocess, sys
from pathlib import Path
R = Path(__file__).resolve().parents[2]
srcs = subprocess.run(['git', '-C', str(R), 'ls-files', 'mashedmod/src'], capture_output=True, text=True).stdout.split()
by = {Path(s).stem: s for s in srcs if s.endswith(('.cpp', '.c'))}
rows = []
# TUs whose RVAs are named only in comments (x87_tus.txt / file header)
MANUAL = {'PathNodes': ['0x00478660', '0x004783f0'], 'RwpSolverLeaves1X87': ['0x00566200', '0x005667c0']}
sect = ''
for ln in (R / 'mashedmod/x87_tus.txt').read_text().splitlines():
    ln = ln.strip()
    if ln.startswith('# ') and re.match(r'# [A-Za-z0-9]+[-0-9][A-Za-z0-9-]* ', ln):
        sect = ln[2:].split(' ')[0].rstrip(':')
    if not ln or ln.startswith('#'):
        continue
    f = by.get(ln)
    if not f:
        rows.append((ln, sect, '', '', 'NO_SOURCE')); continue
    t = (R / f).read_text(errors='replace')
    seen = []
    for m in re.finditer(r're/match/([a-z0-9_]+)/(?:[a-z0-9]+_)?([0-9a-f]{8})\.c', t):
        k = ('0x' + m.group(2), 'match:' + m.group(1))
        if k not in seen: seen.append(k)
    if not seen:
        for m in re.finditer(r'RH_Scoped\w*\(\s*[\w:]+\s*,\s*0x([0-9a-fA-F]{8})', t):
            k = ('0x' + m.group(1).lower(), 'install')
            if k not in seen: seen.append(k)
        for m in re.finditer(r'#define\s+FUN_([0-9a-f]{8})\b', t):
            k = ('0x' + m.group(1), 'define')
            if k[0] not in [s[0] for s in seen]: seen.append(k)
    if not seen and ln in MANUAL:
        seen = [(r, 'comment') for r in MANUAL[ln]]
    if not seen:
        rows.append((ln, sect, f, '', 'NO_RVA'))
    for rva, how in seen:
        rows.append((ln, sect, f, rva, how))
# PREREG section 1 control row: not in x87_tus.txt (SSE2 since PORT-STATE-03), known answer from PORT-STATE-03-FPU
rows.append(('MatchS08_004325c0', 'CONTROL', 'mashedmod/src/mashed_re/Match/MatchS08_004325c0.cpp', '0x004325c0', 'match:s08'))
out = Path(__file__).with_name('x87_inventory.tsv')
out.write_text('tu\tsection\tsource\trva\thow\n' + ''.join('\t'.join(r) + '\n' for r in rows))
print(len(rows), 'rows ->', out)
for r in rows:
    if r[4] in ('NO_SOURCE', 'NO_RVA'): print(r)
