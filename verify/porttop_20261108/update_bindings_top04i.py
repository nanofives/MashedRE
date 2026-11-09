"""PORT-TOP-04I: BINDINGS.tsv rows for the PTank family (0x004881d0 / 0x004880a0 on the RW-layout PTank record, their
teardown 0x00489250) and the later 04I families (FIXED_LATER, added per family commit). Runs update_bindings_top04h.py
first, then writes the rows below. Idempotent.
usage: py -3.12 verify/porttop_20261108/update_bindings_top04i.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
T = 'PORT-TOP-04I: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004881d0: (None, 'matched', 'Obj7030a8Create', T + 'Match/MatchS14_004881d0.cpp (both targets; exe forwarders call the '
                 'bindings) = re/match/s14/004881d0.c (100%): PTank atomic DAT_007030a8 with the "lensflare-fs8" texture',
                 2, 0x00402750),
    0x004880a0: (None, 'matched', 'Obj7030acCreate', T + 'Match/MatchS14_004880a0.cpp (both targets) = re/match/s14/004880a0.c '
                 '(100%): PTank atomic DAT_007030ac', 3, 0x004881d0),
    0x00534b60: ('RpPTankAtomicCreate', 'shim', 'MashedRw_PTankAtomicCreate_00534b60', T + 'LibRw/RwApiBind.cpp: an RW-layout '
                 'PTank atomic record (lead decision 2026-11-09): +0x04 frame, +0x18 geometry (+0x20 material list, +0x5c '
                 'morph target), PTank block at +0x70; block +0x00 / +0x04 / +0x40 as FUN_00534d00 stores them, the rest 0',
                 3, 0x004881d0),
    0x00493640: ('RenderwareAttachPlugins', 'shim', 'MashedRw_AttachPlugins_00493640', T + 'LibRw/RwApiBind.cpp (TOP-03): now '
                 'also stores 0x70, the PTank record\'s plugin slot, at 0x007dc57c (RpPTankPluginAttach\'s offset)', 2,
                 0x00493710),
    0x004e7e30: ('RpAtomicSetFrame', 'matched', 'Orig_004e7e30', T + 'Match/PortTop_004e7e30.cpp = re/match/rwl02/004e7e30.c: '
                 '0x004c0740, then privateFlags |= 1', 4, 0x004881d0),
    0x004c0740: ('_rwObjectHasFrameSetFrame', 'rw', 'MashedRw_ObjectHasFrameSetFrame_004c0740', T + 'LibRw/RwApiBind.cpp: '
                 'camera records (04B) and now PTank records (frame stored at +0x04, rw::Frame::updateObjects)', 5,
                 0x004e7e30),
    0x004e8090: ('RpMaterialSetTexture', 'matched', 'Orig_004e8090', T + 'Match/PortTop_004e8090.cpp = re/match/rwl02/004e8090.c: '
                 'texture +0x54 count raised (librw refCount is at +0x54 too), old texture destroyed, new one at +0x00', 4,
                 0x004881d0),
    0x00489250: (None, 'matched', 'Orig_00489250', T + 'Match/PortTop_00489250.cpp = re/match/s14/00489250.c (BYTE-IDENTICAL '
                 '56 B): RwFrameDestroy + RpAtomicDestroy for both PTank atomics', 2, 0x00402a40),
    0x004e6920: ('RpAtomicDestroy', 'shim', 'MashedRw_AtomicDestroy_004e6920', T + 'LibRw/RwApiBind.cpp: a PTank record releases '
                 'its material texture (RwTextureDestroy) and is freed; a librw atomic goes to rw::Atomic::destroy', 3,
                 0x00489250),
}
FIXED_LATER = {}
FIXED.update(FIXED_LATER)


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_top04h.py')])
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
