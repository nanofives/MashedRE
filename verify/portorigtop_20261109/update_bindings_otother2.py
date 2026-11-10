"""PORT-OT-OTHER-2 (L-111 (a) step 1): BINDINGS.tsv rows for the particle zero fill 0x0048a830, swapped onto its matched
body on the MASHED_ORIG_TOP path, and the reasons the two particle teardowns 0x0048cf70 / 0x0048ffd0 stay stand-ins.
Runs update_bindings_othud2.py first (the tip on orch/integration 7e6eeff6c: it runs GAMEPLAY, VEHICLE and OTHER, and
each of those runs HUD and the FRONTEND / OT-BOOT / 04J chain), then writes the rows below. Idempotent.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otother2.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-OTHER-2: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x0048a830: (None, 'matched', 'Orig_0048a830',
                 T + 'Match/PortTop_0048a830.cpp = re/match/s14/0048a830.c (BYTE-IDENTICAL 29 B, relocheck OK 3/3): '
                 'DAT_0071fa34 = 0, 0x004b6480(&DAT_007151f0, 0x200). ORIGTOP-ONLY CROSS-TARGET vs '
                 'Match/MatchS14_0048a830.cpp', 3, 0x0040bbb0),
    0x0048cf70: (None, 'stand-in', 'PortTop_StandIn04C_0048cf70',
                 'PORT-TOP-04C: Match/PortTop_StandIns04C.cpp (logs, does nothing). ' + T + 'kept: runs 0x004768c0 on '
                 'the 17 records 0x00722470..0x007228b0, built only by 0x0048cee0 from 0x0048e820 (85.5%, C2, no matched '
                 'body), so on the knob path the records are 0 and 0x004768c0 reads *(0 + 4)', 3, 0x0040bd00),
    0x0048ffd0: (None, 'stand-in', 'PortTop_StandIn04C_0048ffd0',
                 'PORT-TOP-04C: Match/PortTop_StandIns04C.cpp (logs, does nothing). ' + T + 'kept: its table 0x0086a4a4 '
                 'is written only by 0x0048ff20 (a stand-in), whose callback 0x0048fed0 has no matched source and no '
                 'hooks.csv row, so on the knob path entry +8 is 0 and 0x004e8090 reads *0', 3, 0x0040bd00),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_othud2.py')])
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in FIXED.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
