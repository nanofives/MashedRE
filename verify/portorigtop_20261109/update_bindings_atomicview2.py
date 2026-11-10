"""ATOMIC-VIEW-2 (L-111 step 1): BINDINGS.tsv rows for type-table entry 1, the DepthCharge init 0x004548e0, on its
matched body, its 0x004548a0 tree, and the librw bindings on the RW-layout atomic view.

Runs update_bindings_atomicview.py first (the chain tip), then this row's FIXED rows.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_atomicview2.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'ATOMIC-VIEW-2: '
EV = ' (verify/portorigtop_20261109/RESULT_ATOMIC-VIEW-2.md)'
RWB = 'LibRw/RwApiBind.cpp'
PWI = 0x0045bae0

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004548e0: (None, 'matched', 'Orig_004548e0', T + 'Match/PortTop_004548e0.cpp = re/match/s09/004548e0.c, '
                 'DAT_005f9998[1].init (PortTop_PWI_Init_1 logs the first clone): DepthCharge.dff clones at 0x00688240, eight '
                 'DepthChargeRound atom clones with two frames each at 0x00688020; 0x005f9808 seeded' + EV, 3, PWI),
    0x004548a0: (None, 'matched', 'Orig_004548a0', T + 'Match/PortTop_004548a0.cpp = re/match/s09/004548a0.c (statics '
                 '0x004547c0 / 0x00454170)' + EV, 4, 0x004548e0),
    0x004e69a0: ('RpAtomicClone', 'bound', 'MashedRw_AtomicClone_004e69a0', T + RWB + ': rw::Atomic::clone (the '
                 '0x004b3f90 view resolves to its librw atomic)' + EV, 4, 0x004548e0),
    0x004e7e30: (None, None, None, None, None, None),  # note only (APPEND): the PTank body stays
    0x004c1040: ('RwFrameAddChild', 'bound', 'MashedRw_FrameAddChild_004c1040', T + RWB + ': rw::Frame::addChild '
                 '(head insertion, as RW)' + EV, 4, 0x004548e0),
    0x004c39b0: ('RwV3dNormalize', 'bound', 'MashedRw_V3dNormalize_004c39b0', T + RWB + ': Math::LutNormalize, this '
                 'function on the original LUT tables (no zero-length error report)' + EV, 4, 0x004548e0),
    0x004c15c0: ('RwFrameSetIdentity', 'bound', 'MashedRw_FrameSetIdentity_004c15c0', T + RWB + ': matrix.setIdentity + '
                 'updateObjects' + EV, 5, 0x004548a0),
    0x004c1210: ('RwFrameRemoveChild', 'bound', 'MashedRw_FrameRemoveChild_004c1210', T + RWB + ': rw::Frame::removeChild '
                 '(RW library function; the matched body walks the RW child list)' + EV, 5, 0x004548a0),
    0x004e4440: ('RpAtomicGetWorld', 'bound', 'MashedRw_AtomicGetWorld_004e4440', T + RWB + ': rw::Atomic::world' + EV,
                 5, 0x004548a0),
    0x004e43b0: ('RpWorldRemoveAtomic', 'bound', 'MashedRw_WorldRemoveAtomic_004e43b0', T + RWB + ': rw::World::removeAtomic'
                 + EV, 5, 0x004548a0),
}
APPEND = {
    0x004e7e30: ' ' + T + 'librw-atomic variant MashedRw_AtomicSetFrame_004e7e30 at the 0x004548e0 call site; Orig_004e7e30 '
                'stays the PTank body' + EV,
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_atomicview.py')])
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in FIXED.items():
        if kind is None:
            continue
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    for rva, extra in APPEND.items():
        r = idx.get(rva)
        if r is not None and T not in r[4]:
            r[4] += extra
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
