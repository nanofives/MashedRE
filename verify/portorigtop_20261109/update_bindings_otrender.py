"""PORT-OT-RENDER (L-111 (a) step 1): BINDINGS.tsv rows for the RENDER stand-ins swapped onto their matched bodies on the
MASHED_ORIG_TOP path, and their exe-only matched callees 0x00475ef0 / 0x00477b40. Runs update_bindings_othud.py first
(which runs the FRONTEND, OT-BOOT and 04J updaters), applies the PORT-OT-GAMEPLAY and PORT-OT-VEHICLE tables, then writes
the rows below. Idempotent. A new row goes right after its caller's row, not at the end of the file.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otrender.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-RENDER: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004759b0: (None, 'matched', 'Orig_004759b0',
                 T + 'Match/PortTop_004759b0.cpp = re/match/s12/004759b0.c: DAT_00691600 = n, four 0x00474db0 arrays at '
                 '0x00691604 / 08 / 10 / 14. ORIGTOP-ONLY CROSS-TARGET vs .asi-only Match/MatchS12_004759b0.cpp. Its teardown '
                 '0x00475d90 stays a stand-in (records filled by the 0x00475a00 stand-in)', 3, 0x0040bbb0),
    0x00476390: (None, 'matched', 'Orig_00476390',
                 T + 'Match/PortTop_00476390.cpp = re/match/s12/00476390.c: 0x00475ef0, five 0x004770c0 records at '
                 '0x00691618 / 0x00692418 / 0x00692498', 3, 0x0040bbb0),
    0x00475ef0: (None, 'matched', 'Orig_00475ef0',
                 T + 'Match/PortTop_00475ef0.cpp = re/match/s12/00475ef0.c: 16 records at 0x00691698 zeroed, +0xc0 = '
                 'DAT_00613254. ORIGTOP-ONLY CROSS-TARGET vs .asi-only Match/MatchS12_00475ef0.cpp', 4, 0x00476390),
    0x00477e40: (None, 'matched', 'Orig_00477e40',
                 T + 'Match/PortTop_00477e40.cpp = re/match/s12/00477e40.c: DAT_00693190 = 0x0040bb30(0x005ceb10), '
                 '0x00477b40', 3, 0x0040bbb0),
    0x00477b40: (None, 'matched', 'Orig_00477b40',
                 T + 'Match/PortTop_00477b40.cpp = re/match/s12/00477b40.c: 0x004b6520 on 0x00693198 (0x2c000) and '
                 '0x006bf198 (0x20). ORIGTOP-ONLY CROSS-TARGET vs .asi-only Match/MatchS12_00477b40.cpp', 4, 0x00477e40),
    0x00475ea0: (None, 'matched', 'Orig_00475ea0',
                 T + 'Match/PortTop_00475ea0.cpp = re/match/s12/00475ea0.c: 0x004768c0 on the five records 0x00476390 '
                 'builds', 3, 0x0040bd00),
    0x0041a980: (None, 'matched', 'Orig_0041a980',
                 T + 'Match/PortTop_0041a980.cpp = re/match/s03/raw_0041a980.c (preamble now MATCH-S02, BYTE-IDENTICAL): '
                 'RpClumpDestroy on 0x0063c7b8[0..1], then zero them. Runs only when 0x0041f320(i) != 0', 3, 0x0040cf80),
    0x004d7ff0: (None, 'matched', 'Orig_004d7ff0',
                 T + 'Match/PortTop_004d7ff0.cpp = re/match/s21/004d7ff0.c: returns param_1 (the param_1 == 0 error '
                 'branch of 0x005581f0). ORIGTOP-ONLY CROSS-TARGET vs .asi-only Render/HighAB3Helpers_p6.cpp', 3, 0x005581f0),
}


def apply(fixed):
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in fixed.items():
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
    return rows


def main():
    # update_bindings_otgameplay.py and update_bindings_otvehicle.py each run update_bindings_othud.py first, and that chain
    # resets the other one's rows. So run the othud chain once, then apply both FIXED tables, then this one.
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_othud.py')])
    sys.path.insert(0, HERE)
    import update_bindings_otgameplay, update_bindings_otvehicle
    apply(update_bindings_otgameplay.FIXED)
    apply(update_bindings_otvehicle.FIXED)
    rows = apply(FIXED)
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
