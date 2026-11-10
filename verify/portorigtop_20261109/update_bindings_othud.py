"""PORT-OT-HUD (L-111 (a) step 1): BINDINGS.tsv rows for the HUD pair swapped onto matched bodies on the MASHED_ORIG_TOP
path (0x00413b80 init / 0x00413bb0 teardown of the record at 0x0063bd50) and their exe-only matched callees. Runs
update_bindings_otfrontend.py first (which runs the OT-BOOT and 04J updaters), then writes the rows below. Idempotent.
0x004768c0 keeps kind stand-in: the matched 0x004210f0 still calls PortTop_StandIn04C_004768c0 (its record 0x0063e548
is built by the stand-in 0x004210b0); only 0x00413bb0 calls the matched copy.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_othud.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-HUD: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00413b80: (None, 'matched', 'Orig_00413b80',
                 T + 'Match/PortTop_00413b80.cpp = re/match/s03/raw_00413b80.c (BYTE-IDENTICAL 43 B): '
                 '0x004770c0(&DAT_0063bd50, 0x37, 0x14, 0x0040bb30("VehicleIcons")), 0x00476cb0(&DAT_0063bd50, 5, 6)',
                 3, 0x0040bbb0),
    0x004770c0: (None, 'matched', 'Orig_004770c0',
                 T + 'Match/PortTop_004770c0.cpp = re/match/s12/004770c0.c (BYTE-IDENTICAL 801 B), exe-only copy for '
                 '0x00413b80: PTank record via RpPTankAtomicCreate, arrays via 0x00474db0. ORIGTOP-ONLY DUP-IN-TARGET '
                 'vs Render/RfxEmitter.cpp Create (hand body, port Emitter struct, default path)', 4, 0x00413b80),
    0x00474db0: ('RwMallocArrayZeroFill', 'matched', 'Orig_00474db0',
                 T + 'Match/PortTop_00474db0.cpp = re/match/s11/00474db0.c (BYTE-IDENTICAL 42 B): RwGlobals +0x108 '
                 '(librw malloc), then 0x004b6520. ORIGTOP-ONLY CROSS-TARGET vs Match/MatchS11_00474db0.cpp', 5, 0x004770c0),
    0x00413bb0: (None, 'matched', 'Orig_00413bb0',
                 T + 'Match/PortTop_00413bb0.cpp = re/match/s03/raw_00413bb0.c (BYTE-IDENTICAL 12 B): '
                 '0x004768c0(&DAT_0063bd50). ORIGTOP-ONLY CROSS-TARGET vs Render/PromoLoop_round256.cpp', 3, 0x0040bd00),
    0x004768c0: (None, 'stand-in', 'PortTop_StandIn04C_004768c0 (from 0x004210f0); Orig_004768c0 (from 0x00413bb0)',
                 'PORT-TOP-04C: Match/PortTop_StandIns04C.cpp (logs, does nothing) for the caller 0x004210f0, whose record '
                 '0x0063e548 the stand-in 0x004210b0 never builds. ' + T + 'the matched copy Match/PortTop_004768c0.cpp = '
                 're/match/s12/004768c0.c (BYTE-IDENTICAL 211 B) runs from 0x00413bb0 on the record 0x0063bd50', 4, 0x004210f0),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_otfrontend.py')])
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
