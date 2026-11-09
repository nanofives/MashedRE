"""PORT-TOP-04E: BINDINGS.tsv rows for the matched 0x00402750 AppInitialiseOnBootup, its 58 direct callees (read from the
#define lines of mashedmod/src/mashed_re/Match/PortTop_00402750.cpp) and the four RW rows of the AppDestroy tree bound
on librw. Runs update_bindings_top04d.py first. Idempotent.
usage: py -3.12 verify/porttop_20261108/update_bindings_top04e.py
"""
import os, re, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
TU = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'Match', 'PortTop_00402750.cpp')
T = 'PORT-TOP-04E: '
F = 0x00402750

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00402750: ('AppInitialiseOnBootup', 'matched', 'Orig_00402750 (PortTop_Bound_00402750)',
                 T + 'Match/PortTop_00402750.cpp = re/match/s01/00402750.c (BYTE-IDENTICAL 739 B after MGLOBAL conversion, '
                 'relocheck OK 93/93); MASHED_TOP04E_HANDINIT=1 runs the old stand-in HandTop_InitRest', 1, 0x00492370),
    0x004c5a60: ('RwTextureDestroy', 'bound', 'MashedRw_TextureDestroy_004c5a60',
                 T + 'LibRw/RwApiBind.cpp: rw::Texture::destroy (null skipped)', 3, 0x00428400),
    0x004ce2d0: ('RwImageSetPath', 'bound', 'MashedRw_ImageSetPath_004ce2d0',
                 T + 'LibRw/RwApiBind.cpp: rw::Image::setSearchPath', 3, 0x00428400),
    0x004e5700: ('RpWorldDestroy', 'bound', 'MashedRw_WorldDestroy_004e5700',
                 T + 'LibRw/RwApiBind.cpp: rw::World::destroy (null skipped)', 3, 0x0042c2a0),
    0x004d41e0: (None, 'shim', 'MashedRw_Destroy4d41e0_004d41e0',
                 T + 'LibRw/RwApiBind.cpp: no librw counterpart; null skipped, non-null logged and left alone', 3, 0x00496ce0),
}

DEF = re.compile(r'^#define FUN_([0-9a-f]{8}) (\S+)\s*(?://\s*(.*))?$')


def callee_rows():
    out = {}
    for ln in open(TU, encoding='utf-8'):
        m = DEF.match(ln.rstrip())
        if not m:
            continue
        rva, sym, cmt = int(m.group(1), 16), m.group(2), (m.group(3) or '').strip()
        if rva == F:
            continue
        if sym.startswith('PortTop_Piece_'):
            kind, note = 'stand-in', 'hand piece (HandTop_InitRest split at this call point). ' + cmt
        elif sym.startswith('PortTop_PizFs_'):
            kind, note = 'bound', 'piz file system on the exe piz reader (Match/PortTop_00402750_Pieces.cpp)'
        elif sym.startswith('PortTop_StandIn04E_'):
            kind, note = 'stand-in', 'Match/PortTop_00402750_Pieces.cpp (logs its first call, returns 0)'
        elif sym.startswith('MashedRw_'):
            kind, note = 'bound', cmt or 'LibRw/RwApiBind.cpp'
        elif sym == 'PortTop_CfgLogDebug':
            kind, note = 'stand-in', 'log stand-in, Match/PortTop_00492370.cpp'
        else:
            kind, note = 'matched', cmt
        out[rva] = (None, kind, sym, T + note.strip(), 2, F)
    return out


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_top04d.py')])
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    want = callee_rows()
    want.update(FIXED)
    for rva, (name, kind, sym, note, lvl, caller) in want.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)), 'callees of 0x00402750:', len(want) - len(FIXED) + 0)


if __name__ == '__main__':
    main()
