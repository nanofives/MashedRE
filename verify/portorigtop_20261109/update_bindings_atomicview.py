"""ATOMIC-VIEW (L-111 step 1): BINDINGS.tsv rows for type-table entry 7's 0x0040cda0 on its matched body (the RW-layout
atomic view for its RpClumpForAllAtomics callback chain) and the new MatFX bindings.

Runs update_bindings_powerupinit_c.py first (the chain tip on orch/integration c85b80fbd), then this row's FIXED rows.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_atomicview.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'ATOMIC-VIEW: '
EV = ' (verify/portorigtop_20261109/RESULT_ATOMIC-VIEW.md)'
RWB = 'LibRw/RwApiBind.cpp'

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x0040cda0: (None, 'matched', 'Orig_0040cda0 (via PortTop_PWI_Call0040cda0)',
                 T + 'Match/PortTop_0040cda0.cpp = re/match/s02/0040cda0.c. The entry-7 forwarder (Match/PortTop_004576f0.cpp) '
                 'runs it when the camera record 0x004672d0 reads is built (DAT_0067ecb0 != 3, DAT_006905b0 and its +0x04 '
                 'frame non-zero), else logs a stand-in; every knob-ON boot ran the matched body 4 times' + EV, 4, 0x004576f0),
    0x0040bb10: (None, 'matched', 'Orig_0040bb10', T + 'Match/PortTop_0040bb10.cpp = re/match/s29/0040bb10.c (C4); its atomic '
                 'is the RW-layout view' + EV, 5, 0x0040cda0),
    0x0040bae0: (None, 'matched', 'Orig_0040bae0', T + 'Match/PortTop_0040bae0.cpp = re/match/s02/0040bae0.c: reads the '
                 'geometry at atomic +0x18, which the view carries' + EV, 6, 0x0040bb10),
    0x0040baa0: (None, 'matched', 'Orig_0040baa0', T + 'Match/PortTop_0040baa0.cpp = re/match/s02/0040baa0.c (C++-only '
                 'prototypes added, DDK text unchanged); runs on librw rw::Material (texture at +0x00)' + EV, 7, 0x0040bae0),
    0x004672d0: (None, 'matched', 'Orig_004672d0', T + 'Match/PortTop_004672d0.cpp = re/match/s11/004672d0.c: the camera '
                 'frame *(DAT_006905b0 + 4); DAT_006905b0 is built by the matched 0x00467110 before the 0x0045bae0 loop'
                 + EV, 5, 0x0040cda0),
    0x0053fb70: ('RpMatFXAtomicEnableEffects', 'bound', 'MashedRw_MatFXAtomicEnableEffects_0053fb70',
                 T + RWB + ': rw::MatFX::enableEffects (an RW-layout view resolves to its librw atomic)' + EV, 7, 0x0040bae0),
    0x0053fbb0: ('RpMatFXMaterialSetEffects', 'bound', 'MashedRw_MatFXMaterialSetEffects_0053fbb0',
                 T + RWB + ': rw::MatFX::setEffects (same effect numbering)' + EV, 8, 0x0040baa0),
    0x0053fd30: ('RpMatFXMaterialSetupEnvMap', 'bound', 'MashedRw_MatFXMaterialSetupEnvMap_0053fd30',
                 T + RWB + ': MatFX env texture (addRef) / frame / FB alpha / coefficient; not reached on the knob path '
                 '(the oilslicker material has no texture)' + EV, 8, 0x0040baa0),
}
APPEND = {
    0x004e66d0: ' ' + T + 'call-site variant MashedRw_ClumpForAllAtomicsAsViews_004e66d0 for 0x0040cda0 (each atomic as its '
                'RW-layout view)' + EV,
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_powerupinit_c.py')])
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
        r = idx.get(rva)
        if r is not None and T not in r[4]:
            r[4] += extra
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
