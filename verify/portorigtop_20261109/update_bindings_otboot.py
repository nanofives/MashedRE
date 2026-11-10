"""PORT-OT-BOOT (L-111 (a) step 1): BINDINGS.tsv rows for the nine boot rows swapped onto their matched bodies on the
MASHED_ORIG_TOP path, plus the two callees they reach (0x00496940, 0x004b6520). Runs
verify/porttop_20261108/update_bindings_top04j.py first, then writes the rows below. Idempotent.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otboot.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-BOOT: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004026d0: (None, 'matched', 'Orig_004026d0', T + 'Match/PortTop_004026d0.cpp = re/match/s01/004026d0.c (BYTE-IDENTICAL), '
                 'callees by name (MatchS01.cpp BootQueueFlush calls them at original addresses the exe does not map); '
                 'ORIGTOP-ONLY DUP-IN-TARGET', 2, 0x00402750),
    0x00402f50: (None, 'matched', 'BootDefaultParamsInit', T + 'Match/MatchS01.cpp = re/match/s01/00402f50.c (both targets, '
                 'the 0x00636ae8 block at its original address)', 2, 0x00402750),
    0x00404830: (None, 'matched', 'Orig_00404830', T + 'Match/PortTop_00404830.cpp = re/match/s01/00404830.c (BYTE-IDENTICAL '
                 '1474 B): the two car material tables at 0x00639cc8 / 0x00636c08 (the hand body in '
                 'Vehicle/CarMaterialTables.cpp keeps its own storage)', 2, 0x00402750),
    0x00412890: (None, 'matched', 'Orig_00412890', T + 'Match/PortTop_00412890.cpp = re/match/s03/h2_00412890.c (BYTE-IDENTICAL '
                 '86 B): quad index table 0x0089d4a0, DAT_0063bc54 = 0x0040bb30("headlight")', 2, 0x00402750),
    0x0047ba00: (None, 'matched', 'Orig_0047ba00', T + 'Match/PortTop_0047ba00.cpp = re/match/topm01/0047ba00.c (BYTE-IDENTICAL '
                 '14 B): returns 0x00496940(param_1)', 2, 0x00402750),
    0x00496940: (None, 'matched', 'Orig_00496940', T + 'Match/PortTop_00496940.cpp = re/match/s16/00496940.c: DAT_00773078 = '
                 'DAT_00771a0c, DAT_0077307c = RwRasterCreate(0, 0, 0, 0x84) (bound)', 3, 0x0047ba00),
    0x00420d00: ('PanelSortInit', 'matched', 'Orig_00420d00', T + 'Match/PortTop_00420d00.cpp = re/match/s05/00420d00.c: '
                 '0x004b6520(0x0063e4b8, 0x90) and four dwords; ORIGTOP-ONLY CROSS-TARGET vs Boot/PanelSortInit.cpp', 2, 0x00402750),
    0x004b6520: ('ZeroFillWrapper', 'matched', 'Orig_004b6520', T + 'Match/PortTop_004b6520.cpp = re/match/s18/004b6520.c: '
                 '0x004b64e0(dst, 0, size) = exe MemsetInline', 3, 0x00420d00),
    0x00471eb0: ('thunk_FUN_00471df0', 'matched', 'Orig_00471eb0', T + 'Match/PortTop_00471eb0.cpp = re/match/s11/00471eb0.c '
                 '(jmp 0x00471df0)', 2, 0x00402750),
    0x00471df0: (None, 'matched', 'Orig_00471df0', T + 'Match/PortTop_00471df0.cpp = re/match/s11/00471df0.c (100%), with its '
                 'file-static callee 0x00471530 (ORIGTOP-ONLY CROSS-TARGET vs Match/MatchS11_00471530.cpp)', 3, 0x00471eb0),
    0x004723d0: ('Init691500', 'matched', 'Orig_004723d0', T + 'Match/PortTop_004723d0.cpp = re/match/s11/004723d0.c: the ten '
                 '0x00691500 entries; ORIGTOP-ONLY CROSS-TARGET vs Match/MatchS11_004723d0.cpp', 2, 0x00402750),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(TOP, 'update_bindings_top04j.py')])
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
