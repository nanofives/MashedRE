"""PORT-OT-GAMEPLAY (L-111 (a) step 1): BINDINGS.tsv rows for the GAMEPLAY stand-ins swapped onto matched bodies on the
MASHED_ORIG_TOP path (0x00405400, 0x0040cf40, 0x004725f0) and the exe-only matched callee 0x0046be10.
Runs update_bindings_othud.py first (which runs the OT-FRONTEND, OT-BOOT and 04J updaters), then writes the rows below.
0x00458880 keeps its stand-in: the pool 0x0068b198 it destroys is shared with the exe's hand pickup code
(RESULT_PORT-OT-GAMEPLAY.md). Idempotent.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otgameplay.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-GAMEPLAY: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00405400: (None, 'matched', 'Orig_00405400',
                 T + 'Match/PortTop_00405400.cpp = re/match/s01/004053d0.c (BYTE-IDENTICAL 23 B): DAT_00639d74 = 0.0f, '
                 'DAT_00639d78 = DAT_00639d70 = 0. ORIGTOP-ONLY CROSS-TARGET vs Util/PromoLoop_round53.cpp', 3, 0x0040cfd0),
    0x0040cf40: (None, 'matched', 'Orig_0040cf40',
                 T + 'Match/PortTop_0040cf40.cpp = re/match/s02/0040cf40.c (100%): RwTextureDestroy (MashedRw_TextureDestroy_'
                 '004c5a60) on DAT_0063ba90 / DAT_0063ba94 when nonzero. ORIGTOP-ONLY CROSS-TARGET vs Match/MatchS02_0040cf40.cpp',
                 3, 0x0040cf80),
    0x004725f0: (None, 'matched', 'Orig_004725f0',
                 T + 'Match/PortTop_004725f0.cpp = re/match/s11/004725f0.c (100%): clears +0x8 / +0xc of the 10 records at '
                 '0x00691500, 0x0046be10 x2 each. ORIGTOP-ONLY CROSS-TARGET vs Match/MatchS11_004725f0.cpp', 3, 0x0040cfd0),
    0x0046be10: ('Write46be10', 'matched', 'Orig_0046be10',
                 T + 'Match/PortTop_0046be10.cpp = re/match/s11/0046be10.c (100%, 55 B), exe-only copy for 0x004725f0 (the '
                 'existing Write46be10 in Util/PromoLoop_sessionB.cpp is asi-only)', 4, 0x004725f0),
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
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
