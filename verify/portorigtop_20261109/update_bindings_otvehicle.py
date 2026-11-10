"""PORT-OT-VEHICLE (L-111 (a) step 1): BINDINGS.tsv rows for the VEHICLE stand-in swapped onto its matched body on the
MASHED_ORIG_TOP path (0x004114e0, called by the matched 0x0040cfd0 from AppDestroy 0x00402a40) and its exe-only matched
callee 0x00483a40. Runs update_bindings_othud.py first (which runs the FRONTEND, OT-BOOT and 04J updaters), then writes
the rows below. Idempotent. A new row goes right after its caller's row, not at the end of the file, so this updater and
another step's updater that appends do not touch the same lines.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otvehicle.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-VEHICLE: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004114e0: (None, 'matched', 'Orig_004114e0',
                 T + 'Match/PortTop_004114e0.cpp = re/match/s02/004114e0.c (BYTE-IDENTICAL 77 B): 0x00483a40 on each '
                 'nonzero DAT_0063bb04 / DAT_0063bb08, then DAT_0063bb14 = DAT_0063bb10 = DAT_0063bb2c = 0. No exe TU writes '
                 '0x0063bb04 / 0x0063bb08. ORIGTOP-ONLY CROSS-TARGET vs .asi-only Vehicle/Replay.cpp ReplayCleanup',
                 3, 0x0040cfd0),
    0x00483a40: (None, 'matched', 'Orig_00483a40',
                 T + 'Match/PortTop_00483a40.cpp = re/match/s13/00483a40.c (BYTE-IDENTICAL 41 B): RwGlobals +0x10c '
                 '(librw free) on *(p + 0x14), then on p, returns 0. Exe-only (the .asi calls the original address)',
                 4, 0x004114e0),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_othud.py')])
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in FIXED.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            at = next((i + 1 for i, x in enumerate(rows) if int(x[0], 16) == caller), len(rows))
            rows.insert(at, r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
