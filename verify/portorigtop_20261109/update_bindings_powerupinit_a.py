"""POWERUPINIT-A (L-111 step 1): BINDINGS.tsv rows for powerup init 0x0045bae0 on its matched body, its five builders and
their callees, the L-114 binding of 0x0047b9b0, the nine type-table stand-ins, and 0x00412010 swapped.

Runs update_bindings_otpowerupinit.py first (the chain tip: HUD-2 once, then the OTHER-2 and PICKUPPOOL rows), then this
row's FIXED rows, then the APPEND notes.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_powerupinit_a.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'POWERUPINIT-A: '
EV = ' (verify/portorigtop_20261109/RESULT_POWERUPINIT-A.md)'
PWI = 0x0045bae0

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x0045bae0: (None, 'matched', 'Orig_0045bae0 (via PortTop_PWI_Call0045bae0)',
                 T + 'Match/PortTop_0045bae0.cpp = re/match/s10/0045bae0.c (with 0x0045ba10 / 0x0045bac0 static and the uncalled '
                 'Orig_0045bf30 / Orig_0045bba0, whose callees bind to logging traps). The forwarder first stores a logging '
                 'stand-in in each of the nine DAT_005f9998 +0x14 init slots (0 in the exe image pad), knob path only' + EV,
                 2, 0x00402750),
    0x00459290: (None, 'matched', 'Orig_00459290', T + 'Match/PortTop_00459290.cpp = re/match/s09/00459290.c: record '
                 '0x0068b9b8 (Headlight), TargetHUD.txd -> DAT_0068bb58, four lazer.dff slots at 0x0068b9f8' + EV, 3, PWI),
    0x004587a0: (None, 'matched', 'Orig_004587a0 (via PortTop_PWI_Call004587a0)', T + 'Match/PortTop_004587a0.cpp = '
                 're/match/s09/004587a0.c: record 0x0068b968 (puglow), PowerUpIcons.txd -> DAT_0068b9ac, 25 IconCube.dff '
                 'clumps at 0x0068b198 +0x00. The forwarder then calls PickupsOrig_SetPoolBuilt (Gameplay/PickupTick.cpp): '
                 'the clumps belong to the builder and the matched teardown 0x00458880; the hand tick borrows the +0x00 '
                 'word for its fake object and the teardown forwarder puts the clump back' + EV, 3, PWI),
    0x00453f30: (None, 'matched', 'Orig_00453f30', T + 'Match/PortTop_00453f30.cpp = re/match/s09/00453f30.c (0xffffffff '
                 'into 0x006870b4 + 0x24*k); ORIGTOP-ONLY CROSS-TARGET vs .asi-only Match/MatchS09_00453f30.cpp' + EV, 3, PWI),
    0x0047b9b0: (None, 'port', 'PortTop_PWI_Call0047b9b0 -> Save::RunTrackPowerupsScript',
                 T + 'deviation L-114 (decision (b), 2026-11-10): the Lua 4.0 VM is not ported (0x0047b860 / 0x0047b8d0 / '
                 'registrar 0x00440bc0), so the hand runner (Save/GameSaveFormat.h) reads powerups_all.lua from the piz '
                 'mount stack and applies its Set_Track_Powerups calls to the span 0x007f0a40 (rows 0x007f0cb0). The '
                 'registrar is not called. Lua library port: row LUA-PORT' + EV, 3, PWI),
    0x00411f30: (None, 'matched', 'Orig_00411f30', T + 'Match/PortTop_00411f30.cpp = re/match/s02/00411f30.c: six lazer.dff '
                 'clumps -> DAT_0063bb38[i] = 0x004b3f90 atomic view (+0x3c = the clump), LaserTower.dff -> DAT_0063bc40' + EV,
                 3, PWI),
    0x004039c0: (None, 'matched', 'Orig_004039c0', T + 'Match/PortTop_004039c0.cpp = re/match/s01/raw/004039c0.c (C++ '
                 'guards added, DDK text unchanged: BYTE-IDENTICAL 23 B): Bomb.dff -> DAT_00636c00 through 0x0042a5d0' + EV,
                 3, PWI),
    0x0042a5d0: (None, 'matched', 'FUN_0042a5d0', T + 'exe body Match/MatchS06_0042a5d0.cpp (re/match/s06/0042a5d0.c); its '
                 'exe callee 0x004b3bf0 -> Orig_004b3bf0 with 0 / 0 for the out pointers (declared deviation, the '
                 '0x0042a6b0 shape), 0x0042a530 -> FUN_0042a530' + EV, 4, 0x004039c0),
    0x004b3e40: (None, 'matched', 'Orig_004b3e40', T + 'Match/PortTop_004b3e40.cpp = re/match/s18/004b3e40.c: memory '
                 'stream (type 3), find chunk 0x10, RpClumpStreamRead, close' + EV, 4, 0x004587a0),
    0x004b3b70: (None, 'matched', 'Orig_004b3b70', T + 'Match/PortTop_004b3b70.cpp = re/match/s18/004b3b70.c: stream open '
                 '(2, 1, path), read, close; ORIGTOP-ONLY DUP-IN-TARGET vs Save/FileWrappers.cpp (returns 0 in the exe)' + EV,
                 4, 0x004587a0),
    0x004b65a0: (None, 'matched', 'Orig_004b65a0', T + 'Match/PortTop_004b65a0.cpp = re/match/s18/004b65a0.c (-> '
                 '0x004b68e0); ORIGTOP-ONLY CROSS-TARGET vs .asi-only Util/PromoLoop_round30.cpp' + EV, 4, 0x004587a0),
    0x004b68e0: (None, 'matched', 'Orig_004b68e0', T + 'Match/PortTop_004b68e0.cpp = re/match/s18/004b68e0.c (DAT_007d3e4c, '
                 '0 in the exe); ORIGTOP-ONLY CROSS-TARGET vs .asi-only Util/PromoLoop_round30.cpp' + EV, 5, 0x004b65a0),
    0x004b65b0: (None, 'matched', 'Orig_004b65b0', T + 'Match/PortTop_004b65b0.cpp = re/match/s18/004b65b0.c (-> '
                 '0x004b68f0)' + EV, 4, 0x004587a0),
    0x004b68f0: (None, 'bound', 'PortTop_PizFs_EntryLength_004b68f0', T + 'Match/PortTop_00402750_Pieces.cpp: the length '
                 'of the named entry in the newest piz mount holding it, else 0. The matched source reads the directory the '
                 'original piz reader copies to 0x0090dac0 (count 0x008ad9a8, entry +0x78 = length), which the exe reader '
                 'never fills' + EV, 5, 0x004b65b0),
    0x004781b0: (None, 'matched', 'Orig_004781b0', T + 'Match/PortTop_004781b0.cpp = re/match/s12/004781b0.c: file stream '
                 '(type 2), find chunk 0x10, RpClumpStreamRead, close (LaserTower.dff)' + EV, 4, 0x00411f30),
    0x00474d60: (None, 'matched', 'Orig_00474d60', T + 'Match/PortTop_00474d60.cpp = re/match/s11/00474d60.c: '
                 'RpClumpForAllAtomics with the callback 0x00474d30 (same source, atomic flags bit 4) on librw atomics' + EV,
                 4, 0x00459290),
    0x004b5580: (None, 'stand-in', 'PortTop_PWI_StandIn_004b5580', T + 'Match/PortTop_00411f30.cpp (logs, does nothing): '
                 'matched source re/match/s18/004b5580.c, but its RpClumpForAllAtomics callback 0x004b5560 has no hooks.csv '
                 'row and no matched source' + EV, 4, 0x00411f30),
    0x00412010: (None, 'matched', 'Orig_00412010 (via PortTop_0045b930_Call00412010)', T + 'Match/PortTop_00412010.cpp = '
                 're/match/s02/00412010.c: RpClumpDestroy(*(DAT_0063bb38[i] + 0x3c)) x6 and DAT_0063bc40. The forwarder '
                 'runs it when all six records are non-zero (built by the matched 0x00411f30 from 0x0045bae0), the stand-in '
                 'PortTop_StandIn04C_00412010 otherwise' + EV, 3, 0x0045b930),
}

# The nine type-table init slots (DAT_005f9998 + 0x40*i + 0x14), each a logging stand-in on this path.
_INIT = [(0x00456760, 'matched source re/match/s09/00456760.c, callees not on this path (0x004b3fc0 has no source), row POWERUPINIT-C'),
         (0x004548e0, 'matched source re/match/s09/004548e0.c, callees not on this path (0x004b3fc0 has no source), row POWERUPINIT-C'),
         (0x00455400, None), (0x004583a0, None), (0x0045a6b0, None), (0x0045b830, None), (0x00454e70, None),
         (0x004576f0, None), (0x00452f70, None)]
for _i, (_rva, _why) in enumerate(_INIT):
    FIXED[_rva] = (None, 'stand-in', 'PortTop_PWI_InitStandIn_%d' % _i,
                   T + 'Match/PortTop_0045bae0.cpp (logs, does nothing): DAT_005f9998[%d].init, called through the type '
                   'table slot +0x14 the forwarder fills; ' % _i +
                   (_why or 'no hooks.csv row and no matched source, row POWERUPINIT-B') + EV, 3, PWI)

APPEND = {  # rva -> text appended to the existing note
    0x004768c0: ' ' + T + 'both selector records are built now (0x0068b968 by 0x004587a0, 0x0068b9b8 by 0x00459290), and '
                'both knob-ON boots take the matched branch' + EV,
    0x00458880: ' ' + T + 'the pool is built by the matched 0x004587a0 now; the forwarder runs the matched body on the '
                'builder clumps after PickupsOrig_ReleasePool hands back any word the hand tick borrowed' + EV,
    0x004593b0: ' ' + T + 'record 0x0068b9b8 and the four slots are built by the matched 0x00459290 now' + EV,
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_otpowerupinit.py')])
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
    for rva, extra in APPEND.items():
        r = idx[rva]
        if T not in r[4]:
            r[4] += extra
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
