"""PORT-OT-FRONTEND (L-111 (a) step 1): BINDINGS.tsv row for the frontend row swapped onto its matched body on the
MASHED_ORIG_TOP path (0x00428390 FrontendStateSet). Runs update_bindings_otboot.py first (which runs the 04J updater),
then writes the row below. Idempotent.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otfrontend.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-FRONTEND: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00428390: ('FrontendStateSet', 'matched', 'Orig_00428390 (then PortTop_Piece_00428390 hand piece 5)',
                 T + 'Match/PortTop_00428390.cpp = re/match/s06/00428390.c (match.py 100%): DAT_0067d960 = param_1. '
                 'Called first by PortTop_Piece_00428390 (Match/PortTop_00402750_Pieces.cpp), which then runs hand piece 5 '
                 '(frontend assets). ORIGTOP-ONLY CROSS-TARGET vs Match/MatchS06_00428390.cpp', 2, 0x00402750),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_otboot.py')])
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
