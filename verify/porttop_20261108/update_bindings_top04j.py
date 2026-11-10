"""PORT-TOP-04J: BINDINGS.tsv rows for family 1, the cup world of 0x00401000 and its teardown 0x004014b0, on the
RW-layout light record (LibRw/RwApiBind.cpp MashedRwLight). Runs update_bindings_top04i.py first, then writes the rows
below. Idempotent.
usage: py -3.12 verify/porttop_20261108/update_bindings_top04j.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
T = 'PORT-TOP-04J: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00401000: (None, 'matched', 'Orig_00401000', T + 'Match/PortTop_00401000.cpp = re/match/s01/00401000.c (BYTE-IDENTICAL '
                 '482 B): cup world (RpWorldCreate), cup.dff clump, ambient + soft-spot light records, world adds', 3, 0x0040bbb0),
    0x004014b0: (None, 'matched', 'Orig_004014b0', T + 'Match/PortTop_004014b0.cpp = re/match/s01/raw/004014b0.c (BYTE-IDENTICAL '
                 '182 B): removes / destroys both lights, removes and destroys the clump and the world', 2, 0x00402a40),
    0x004c1800: (None, 'matched', 'Orig_004c1800', T + 'Match/PortTop_004c1800.cpp = re/match/s19/004c1800.c (BYTE-IDENTICAL): '
                 'RwBBoxInitialize', 4, 0x00401000),
    0x004c1840: (None, 'matched', 'Orig_004c1840', T + 'Match/PortTop_004c1840.cpp = re/match/s19/004c1840.c (BYTE-IDENTICAL): '
                 'RwBBoxAddPoint', 4, 0x00401000),
    0x004e5850: ('RpWorldCreate', 'rw', 'MashedRw_WorldCreate_004e5850', T + 'LibRw/RwApiBind.cpp: rw::World::create', 4,
                 0x00401000),
    0x004a2b60: ('sprintf', 'crt', 'PortTop_Sprintf04J_004a2b60', T + 'Match/PortTop_00401000.cpp: the exe CRT vsprintf (libid '
                 'libcmt sprintf.obj)', 4, 0x00401000),
    0x004b5320: (None, 'matched', 'Orig_004b5320', T + 'Match/PortTop_004b5320.cpp = re/match/s18/004b5320.c (BYTE-IDENTICAL '
                 '43 B): RpClumpForAllAtomics(clump, 0x004b5300, {mask, set})', 4, 0x00401000),
    0x004b5300: (None, 'shim', 'MashedRw_AtomicGeometryFlags_004b5300', T + 'LibRw/RwApiBind.cpp (no matched source): the '
                 '0x004b52f0 / 0x004b52c0 flag write on the librw atomic\'s geometry (+0x08, both arms). The two matched '
                 'leaves stay out of the exe: the .asi has hand bodies of both (rva-lint CROSS-TARGET)', 5, 0x004b5320),
    0x004b4080: (None, 'stand-in', 'PortTop_StandIn04J_004b4080', T + 'Match/PortTop_00401000.cpp (logs): called only when '
                 'DAT_00636ad0 != 0 (0 on the knob path: not called)', 4, 0x00401000),
    0x0053fbb0: (None, 'stand-in', 'PortTop_StandIn04J_0053fbb0', T + 'Match/PortTop_00401000.cpp (logs): called only when '
                 'DAT_00636ad0 != 0 (0 on the knob path: not called)', 4, 0x00401000),
    0x004e4dd0: ('RpLightCreate', 'shim', 'MashedRw_LightCreate_004e4dd0', T + 'LibRw/RwApiBind.cpp: an RW-layout light record '
                 '(0x004e4dd0\'s stores: +0x14 radius, +0x18 colour, +0x28 minusCosAngle, +0x3c) owning a rw::Light', 4, 0x00401000),
    0x004e4900: ('RpLightSetColor', 'matched', 'Orig_004e4900', T + 'Match/PortTop_004e4900.cpp = re/match/rwl02/004e4900.c '
                 '(BYTE-IDENTICAL 77 B) on the light record', 4, 0x00401000),
    0x004e4b90: ('RpLightSetConeAngle', 'matched', 'Orig_004e4b90', T + 'Match/PortTop_004e4b90.cpp = re/match/rwl02/004e4b90.c '
                 '(BYTE-IDENTICAL 55 B) on the light record; bounds 0x005d757c / 0x005cd108 from the rdata seed', 4, 0x00401000),
    0x004e48e0: ('RpLightSetRadius', 'matched', 'Orig_004e48e0', T + 'Match/PortTop_004e48e0.cpp = re/match/rwl02/004e48e0.c '
                 '(BYTE-IDENTICAL 32 B) on the light record, then RwFrameUpdateObjects (bound)', 4, 0x00401000),
    0x004c0740: ('_rwObjectHasFrameSetFrame', 'rw', 'MashedRw_ObjectHasFrameSetFrame_004c0740', T + 'LibRw/RwApiBind.cpp: '
                 'camera records (04B), PTank records (04I) and now light records (+0x04, rw::Light::setFrame)', 5, 0x00401000),
    0x004e4810: ('RpWorldAddLight', 'rw', 'MashedRw_WorldAddLight_004e4810', T + 'LibRw/RwApiBind.cpp: the light record\'s '
                 'fields into the rw::Light, then rw::World::addLight', 4, 0x00401000),
    0x004e4450: ('RwFrameAddChild', 'rw', 'MashedRw_WorldAddClump_004e4450', T + 'LibRw/RwApiBind.cpp: (world, clump) -> '
                 'rw::World::addClump (hooks.csv name; naming doubt aq_s4: RpWorldAddClump)', 4, 0x00401000),
    0x004e4860: ('RpWorldRemoveLight', 'rw', 'MashedRw_WorldRemoveLight_004e4860', T + 'LibRw/RwApiBind.cpp: '
                 'rw::World::removeLight', 3, 0x004014b0),
    0x004e4d90: ('RpLightDestroy', 'rw', 'MashedRw_LightDestroy_004e4d90', T + 'LibRw/RwApiBind.cpp: rw::Light::destroy, record '
                 'freed', 3, 0x004014b0),
    0x004e45b0: ('RwFrameRemoveChild', 'rw', 'MashedRw_WorldRemoveClump_004e45b0', T + 'LibRw/RwApiBind.cpp: (world, clump) -> '
                 'rw::World::removeClump (hooks.csv name; naming doubt aq_s4: RpWorldRemoveClump)', 3, 0x004014b0),
}
FIXED_LATER = {}
FIXED.update(FIXED_LATER)


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_top04i.py')])
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
