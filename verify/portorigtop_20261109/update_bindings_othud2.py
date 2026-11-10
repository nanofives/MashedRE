"""PORT-OT-HUD-2 (L-111 (a) step 1): BINDINGS.tsv rows for the effect pair swapped onto matched bodies on the
MASHED_ORIG_TOP path (0x004210b0 init / 0x004210f0 teardown of the record at 0x0063e548), the two HUD font teardown rows
released by PORT-OT-RWG-MODULES (0x00555830 / 0x00556e40, run only with MASHED_ORIG_TOP_RWMOD=1), and their exe-only
matched callees. Runs update_bindings_othud.py first (which runs the FRONTEND, OT-BOOT and 04J updaters), then writes the
rows below. Idempotent.
0x00556e40's row is `matched` with its selector named: the matched body is what runs when MASHED_ORIG_TOP_RWMOD=1 and the
handle is non-zero. Its knob-path run still takes the stand-in, because the four handles are written only by 0x00427ca0,
still a stand-in (RESULT_PORT-OT-HUD-2.md).
usage: py -3.12 verify/portorigtop_20261109/update_bindings_othud2.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-HUD-2: '
RW = ' Run only with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x11c), selector PortTop_00427620_Call%s in Match/PortTop_00427620.cpp'

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004210b0: (None, 'matched', 'Orig_004210b0',
                 T + 'Match/PortTop_004210b0.cpp = re/match/s05/004210b0.c (BYTE-IDENTICAL 55 B, relocheck OK 7/7): '
                 '0x004770c0(&DAT_0063e548, 0x817, 0x80, 0x0040bb30("smoke")), 0x00421080(), 0x004b6480(&DAT_0063e4a8, 0x80)',
                 3, 0x0040bbb0),
    0x00421080: ('Fill63e5a4', 'matched', 'Orig_00421080',
                 T + 'Match/PortTop_00421080.cpp = re/match/s05/00421080.c (BYTE-IDENTICAL 43 B, relocheck OK 3/3). '
                 'ORIGTOP-ONLY CROSS-TARGET vs Match/MatchS05_00421080.cpp', 4, 0x004210b0),
    0x004b6480: ('BitArrayClear', 'matched', 'Orig_004b6480',
                 T + 'Match/PortTop_004b6480.cpp = re/match/s18/004b6480.c (BYTE-IDENTICAL 89 B, relocheck OK). '
                 'ORIGTOP-ONLY CROSS-TARGET vs Input/DirectInput.cpp', 4, 0x004210b0),
    0x004768c0: (None, 'matched', 'Orig_004768c0',
                 'PORT-OT-HUD: Match/PortTop_004768c0.cpp = re/match/s12/004768c0.c (BYTE-IDENTICAL 211 B) from 0x00413bb0 '
                 '(record 0x0063bd50). ' + T + 'also from 0x004210f0 (record 0x0063e548, built by the matched 0x004210b0); '
                 'the stand-in PortTop_StandIn04C_004768c0 is removed', 4, 0x004210f0),
    0x00555830: (None, 'matched', 'Orig_00555830',
                 T + 'Match/PortTop_00555830.cpp = re/match/s24/00555830.c (BYTE-IDENTICAL 215 B, relocheck OK 7/7).'
                 + RW % '00555830' + ', the stand-in PortTop_StandIn04C_00555830 otherwise', 3, 0x00427620),
    0x00556e40: (None, 'matched', 'Orig_00556e40',
                 T + 'Match/PortTop_00556e40.cpp = re/match/s24/00556e40.c (BYTE-IDENTICAL 67 B, relocheck OK 3/3).'
                 + RW % '00556e40' + ' and a non-zero handle (first read *(handle + 0x74), no null check; the handles '
                 'come from 0x00427ca0, a stand-in), the stand-in PortTop_StandIn04C_00556e40 otherwise', 3, 0x00427620),
    0x005c4d30: ('CondGet5c4d30', 'matched', 'Orig_005c4d30',
                 T + 'Match/PortTop_005c4d30.cpp = re/match/s28/005c4d30.c (BYTE-IDENTICAL 17 B, relocheck OK). '
                 'ORIGTOP-ONLY CROSS-TARGET vs Util/PromoLoop_sessionB.cpp', 4, 0x00555830),
    0x005c4d50: (None, 'matched', 'Orig_005c4d50',
                 T + 'Match/PortTop_005c4d50.cpp = re/match/s28/005c4d50.c (BYTE-IDENTICAL 69 B, relocheck OK 2/2): '
                 'RwGlobals +0x10c on p[0] / p itself', 4, 0x00555830),
    0x00553e80: (None, 'stand-in', 'PortTop_StandInOTH2_00553e80 (from 0x00555830)',
                 T + 'stand-in in Match/PortTop_00555830.cpp (logs its first call, returns 0): no matched source. '
                 'Reached only for a non-zero entry obj of a non-zero 0x00555830 argument', 4, 0x00555830),
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
