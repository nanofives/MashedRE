"""PORT-TOP-04H: BINDINGS.tsv rows for the perm-clump loader 0x004025f0 (family 3a) moved off its stand-in, the bindings
and stand-ins under it, and the later 04H families (FIXED_LATER, added per family commit). Runs update_bindings_top04g.py
first (which runs the 04F / 04E updaters), then writes the rows below. Idempotent.
usage: py -3.12 verify/porttop_20261108/update_bindings_top04h.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
T = 'PORT-TOP-04H: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004025f0: (None, 'matched', 'Orig_004025f0', T + 'Match/PortTop_004025f0.cpp = re/match/s01/00401690.c (0x004025f0 '
                 'BYTE-IDENTICAL 211 B, relocheck OK 6/6); 13 perm slots at 0x00636578: clump from "map_%d.dff", bounding '
                 'box, slots +0x40 / +0x50 from the two stand-ins below', 2, 0x00402750),
    0x00401630: (None, 'matched', 'Orig_00401630', T + 'Match/PortTop_00401630.cpp = re/match/s01/raw/00401630.c (BYTE-IDENTICAL '
                 '87 B, relocheck OK; the source now returns 0x004b3bf0\'s result, same bytes)', 3, 0x004025f0),
    0x00401690: (None, 'port', 'FUN_00401690 (static)', T + 'the static of re/match/s01/00401690.c in Match/PortTop_004025f0.cpp: '
                 'the C2 reconstruction (match.py 60.2%, residual = the vertex-loop unroll factor x8 vs x4), not a matched '
                 'body', 3, 0x004025f0),
    0x004b3bf0: (None, 'matched', 'Orig_004b3bf0', T + 'Match/PortTop_004b3bf0.cpp = re/match/s18/004b3bf0.c (BYTE-IDENTICAL '
                 '98 B): stream open, find chunk 0x10, RpClumpStreamRead, RwFrameUpdateObjects, close', 4, 0x00401630),
    0x004a2b60: ('sprintf', 'crt', 'PortTop_Sprintf04H_004a2b60', T + 'Match/PortTop_00401630.cpp -> CRT vsprintf', 4,
                 0x00401630),
    0x004e7420: ('RpClumpStreamRead', 'rw', 'MashedRw_ClumpStreamRead_004e7420', T + 'LibRw/RwApiBind.cpp -> '
                 'rw::Clump::streamRead on the knob path\'s memory stream (base + position), position moved by what librw '
                 'read', 5, 0x004b3bf0),
    0x004c0e50: ('RwFrameUpdateObjects', 'rw', 'MashedRw_FrameUpdateObjects_004c0e50', T + 'LibRw/RwApiBind.cpp -> '
                 'rw::Frame::updateObjects (same root test, dirty list and flag bits; the matched body reads RwGlobals +0xbc)',
                 5, 0x004b3bf0),
    0x004e66d0: ('RpClumpForAllAtomics', 'rw', 'MashedRw_ClumpForAllAtomics_004e66d0', T + 'LibRw/RwApiBind.cpp: librw '
                 'atomic list in RW order (backwards: RW links read atomics at the head), stops at the first 0; callbacks get '
                 'the librw atomic. No knob-path caller yet', 5, 0x004b3f90),
    0x004b3f90: (None, 'shim', 'MashedRw_ClumpFirstAtomic_004b3f90', T + 'LibRw/RwApiBind.cpp: the first atomic in RW order '
                 '(0x004b3f90 = RpClumpForAllAtomics with 0x004b4070, store and stop) as an RW-layout view (+0x04 frame, '
                 '+0x18 geometry -> +0x14 numVertices, +0x5c morph target): librw Atomic / Geometry put those at +0x14 / '
                 '+0x48', 4, 0x00401690),
    0x004c3ac0: ('Vec3Magnitude', 'port', 'Vec3Magnitude', T + 'exe body Math/Vec3.cpp (Math::LutLength, the RW sqrt LUT)', 4,
                 0x00401690),
    0x00402240: (None, 'stand-in', 'PortTop_StandIn04H_00402240', T + 'Match/PortTop_004025f0.cpp: returns 0 (0x00401fe0 '
                 'RW-layout geometry writes, RpGeometryCreate 0x004e8c70 / RpMaterialCreate 0x004e7fa0 tree unbound, '
                 '0x004c1040 no source)', 3, 0x004025f0),
    0x00402590: (None, 'stand-in', 'PortTop_StandIn04H_00402590', T + 'Match/PortTop_004025f0.cpp: returns 0 (0x004022a0, '
                 'same tree as 0x00402240)', 3, 0x004025f0),
}
FIXED_LATER = {}
FIXED.update(FIXED_LATER)


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_top04g.py')])
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
