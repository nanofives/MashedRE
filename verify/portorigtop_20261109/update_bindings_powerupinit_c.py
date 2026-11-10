"""POWERUPINIT-C (L-111 step 1): BINDINGS.tsv rows for the powerup type-table inits (DAT_005f9998 +0x14) on their
matched bodies under MASHED_ORIG_TOP, their tail targets and callees, and the new librw bindings.

Runs update_bindings_tripool.py first (the chain tip: POWERUPINIT-A, then PORT-OT-TRIPOOL), then this row's FIXED rows.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_powerupinit_c.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'POWERUPINIT-C: '
EV = ' (verify/portorigtop_20261109/RESULT_POWERUPINIT-C.md)'
PWI = 0x0045bae0
RWB = 'LibRw/RwApiBind.cpp'


def init_note(i, src, what):
    return (T + 'Match/PortTop_%s.cpp = %s, DAT_005f9998[%d].init, stored in the +0x14 slot by the 0x0045bae0 forwarder '
            '(Match/PortTop_0045bae0.cpp PortTop_PWI_Init_%d logs the first clone after the call): %s' % (
                src.split('/')[-1][:-2], src, i, i, what) + EV)


FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00456760: (None, 'matched', 'Orig_00456760', init_note(0, 're/match/s09/00456760.c',
                 'GatlingGun.dff, four clones at 0x00688670 + 0x48*k with their atoms filed by part code'), 3, PWI),
    0x004583a0: (None, 'matched', 'Orig_004583a0', init_note(3, 're/match/s29/004583a0.c',
                 'MineDropper.dff / Mine.dff clones in 0x0068b0d0 / 0x0068a310, 0x00458360, lensflare batch 0x0068b090 '
                 'with 0x00476c10(batch, 0x005f9820); 0x005f9820 (16 B .data) added to the port-state seed'), 3, PWI),
    0x0045a6b0: (None, 'matched', 'Orig_0045a6b0', init_note(4, 're/match/s29/0045a6b0.c',
                 'FlameThrower.dff clones in four 0x68-byte records at 0x0068bb60, frame LTM copy, 0x0045a3a0, particle '
                 'pools'), 3, PWI),
    0x0045b830: (None, 'matched', 'Orig_0045b830', init_note(5, 're/match/s29/0045b830.c',
                 'shotgun.dff clones in the four 0x24-byte slots at 0x0068d158, frame scale, geometry flag 0x40, tail '
                 'jump to 0x0045b810'), 3, PWI),
    0x00454e70: (None, 'matched', 'Orig_00454e70', init_note(6, 're/match/s29/00454e70.c',
                 'flashunit.dff clones in the four 0x14-byte entries at 0x00688358, lensflare batch 0x0088fec0, tail jump '
                 'to 0x00454e40'), 3, PWI),
    0x004576f0: (None, 'matched', 'Orig_004576f0', init_note(7, 're/match/s29/004576f0.c',
                 'oil / oilShine batches, four oilslicker.dff clones at 0x0068a250 +0xc; 0x0040cda0 is a stand-in'), 3, PWI),
    0x00452f70: (None, 'matched', 'Orig_00452f70', init_note(8, 're/match/s29/00452f70.c',
                 'mortar.dff / MortarRound.dff clones in 0x00684e3c / 0x00684eac with smoke particle pools'), 3, PWI),
    0x004548e0: (None, 'stand-in', 'PortTop_PWI_InitStandIn_1',
                 T + 'Match/PortTop_0045bae0.cpp (logs, does nothing): DAT_005f9998[1].init. Its 0x004b3f90 atom is the '
                 'RW-layout view MashedRwAtomicView, and 0x004e69a0 / 0x004e7e30 / 0x004c1040 and the 0x004548a0 tree have '
                 'no librw binding for it. Row POWERUPINIT-D1' + EV, 3, PWI),
    0x00455400: (None, 'stand-in', 'PortTop_PWI_InitStandIn_2',
                 T + 'Match/PortTop_0045bae0.cpp (logs, does nothing): DAT_005f9998[2].init. No byte match (94.0%, '
                 'POWERUPINIT-B). Row POWERUPINIT-B2' + EV, 3, PWI),
    0x00454e40: (None, 'matched', 'Orig_00454e40', T + 'Match/PortTop_00454e40.cpp = re/match/s29/00454e40.c (statics '
                 '0x00454bd0 ESI / 0x00454c00 EAX in the same source); tail-jump target of 0x00454e70' + EV, 4, 0x00454e70),
    0x0045b810: (None, 'matched', 'Orig_0045b810', T + 'Match/PortTop_0045b810.cpp = re/match/s29/0045b810.c (static '
                 '0x0045b360 ESI in the same source); tail-jump target of 0x0045b830' + EV, 4, 0x0045b830),
    0x00458360: (None, 'matched', 'Orig_00458360', T + 'Match/PortTop_00458360.cpp = re/match/s09/00458360.c (statics '
                 '0x00458020 / 0x00457ba0; the uncalled 0x00458080 / 0x004582f0 callees bind to logging traps)' + EV,
                 4, 0x004583a0),
    0x004b3fc0: (None, 'matched', 'Orig_004b3fc0', T + 'Match/PortTop_004b3fc0.cpp = re/match/s29/004b3fc0.c; '
                 'RpClumpForAllAtomics -> MashedRw_ClumpForAllAtomics_004e66d0, callback 0x004b3f20 -> Orig_004b3f20' + EV,
                 4, 0x00456760),
    0x004b3f20: (None, 'matched', 'Orig_004b3f20', T + 'Match/PortTop_004b3f20.cpp = re/match/s29/004b3f20.c (BYTE-IDENTICAL '
                 '34 B, relocheck OK, C4 by match): the RpClumpForAllAtomics callback of 0x004b3fc0' + EV, 5, 0x004b3fc0),
    0x004b5240: (None, 'matched', 'Orig_004b5240', T + 'Match/PortTop_004b5240.cpp = re/match/s18/004b5240.c: atomic +0x02 '
                 'bit 0x04 = rw::Object::flags / rw::Atomic::RENDER on the librw atomic' + EV, 4, 0x004583a0),
    0x0045a3a0: (None, 'stand-in', 'PortTop_PWI_StandIn_0045a3a0', T + 'Match/PortTop_0045a6b0.cpp (logs, does nothing): '
                 're/match/s09/0045a3a0.c is 81.0%, residual register allocation, not SIB order, so L-115 does not admit it. '
                 'Left undone: the 0x0068bd00 table zeroing (pad zero) and record +0x0c = 1.0f; no reader on this path' + EV,
                 4, 0x0045a6b0),
    0x0040cda0: (None, 'stand-in', 'PortTop_PWI_StandIn_0040cda0', T + 'Match/PortTop_004576f0.cpp (logs, does nothing): '
                 'matched source re/match/s02/0040cda0.c, but its RpClumpForAllAtomics callback 0x0040bb10 has no matched '
                 'source' + EV, 4, 0x004576f0),
    0x004e6ab0: ('RpClumpClone', 'bound', 'MashedRw_ClumpClone_004e6ab0', T + RWB + ': rw::Clump::clone' + EV, 4,
                 0x00456760),
    0x004e4800: (None, 'bound', 'MashedRw_ClumpGetWorld_004e4800', T + RWB + ': rw::Clump::world (the clones are in no '
                 'world)' + EV, 5, 0x00454e40),
    0x004b5190: (None, 'bound', 'MashedRw_AtomicPartCode_004b5190', T + RWB + ': geometry user-data array / element as an '
                 'int (rw::UserDataArray), -1 when absent' + EV, 4, 0x00456760),
    0x004b52f0: (None, 'bound', 'MashedRw_AtomicGeometryFlags_004b52f0', T + RWB + ': geometry flags set / clear (the '
                 '0x004b5300 work)' + EV, 4, 0x0045b830),
    0x004c13e0: ('RwFrameScale', 'bound', 'MashedRw_FrameScale_004c13e0', T + RWB + ': rw::Frame::scale' + EV, 4,
                 0x0045b830),
    0x004c0ed0: ('RwFrameGetLTM', 'bound', 'MashedRw_FrameGetLTM_004c0ed0', T + RWB + ': rw::Frame::getLTM' + EV, 4,
                 0x0045a6b0),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_tripool.py')])
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
