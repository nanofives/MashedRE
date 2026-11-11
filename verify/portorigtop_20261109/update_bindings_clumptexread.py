"""CLUMP-TEXREAD: BINDINGS.tsv notes for RW's texture find rule (0x004c7490) on the MASHED_ORIG_TOP bindings
RwTextureRead 0x004c5cb0 and RpClumpStreamRead 0x004e7420.

Runs update_bindings_atomicview2.py first (the chain tip), then this row's rows.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_clumptexread.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'CLUMP-TEXREAD: '
EV = ' (verify/portorigtop_20261109/RESULT_CLUMP-TEXREAD.md)'

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004c7490: ('RwTextureFindDefault', 'bound', 'RwRuleFindTexture (LibRw/RwApiBind.cpp)',
                 T + "RW's texture find callback: the current dictionary only, else every dictionary newest first "
                 '(0x004c7529 walk), case-insensitive; used by the 0x004c5cb0 binding and installed as rw::Texture::findCB '
                 "around the 0x004e7420 binding's librw read (librw's default returns nil with no current dictionary)" + EV,
                 5, 0x004c5cb0),
}
APPEND = {
    0x004c5cb0: ' ' + T + "finds through RwRuleFindTexture (0x004c7490's rule)" + EV,
    0x004e7420: ' ' + T + 'material textures resolve through RwRuleFindTexture during the read' + EV,
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_atomicview2.py')])
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
        elif r is None:
            print('note target missing:', hex(rva))
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
