"""PORT-TOP-04F: BINDINGS.tsv rows for the 0x00402750 loaders moved off their stand-ins and the RW stream / texture
bindings they call. Runs update_bindings_top04e.py first (which re-reads the callee #defines of
Match/PortTop_00402750.cpp), then writes the level-3 rows below. Idempotent.
usage: py -3.12 verify/porttop_20261108/update_bindings_top04f.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
T = 'PORT-TOP-04F: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00425bc0: (None, 'matched', 'Orig_00425bc0', T + 'Match/PortTop_00425bc0.cpp = re/match/s05/00425bc0.c (BYTE-IDENTICAL 46 B, '
                 'relocheck OK 5/5); Copters.txd from perm.piz -> DAT_00644150', 2, 0x00402750),
    0x004b3d80: (None, 'matched', 'Orig_004b3d80', T + 'Match/PortTop_004b3d80.cpp = re/match/s18/004b3d80.c (BYTE-IDENTICAL 82 B, '
                 'relocheck OK 4/4); also called by 0x00425bc0', 2, 0x00402750),
    0x004283a0: (None, 'matched', 'Orig_004283a0', T + 'Match/PortTop_004283a0.cpp = re/match/s06/004283a0.c (BYTE-IDENTICAL 92 B, '
                 'relocheck OK 15/15); four logo textures', 2, 0x00402750),
    0x004274e0: (None, 'matched', 'LocalizationFileLoad', T + 'Match/MatchS06_004274e0.cpp (now in both targets) = '
                 're/match/s06/004274e0.c (BYTE-IDENTICAL 148 B, relocheck OK 12/12); english.dat -> DAT_0066d828', 2, 0x00402750),
    0x004cc230: ('RwStreamOpen', 'shim', 'PortTop_RwStreamOpen_004cc230',
                 T + 'Match/PortTop_RwStream04F.cpp: type 2 reads the file from the piz mount stack or the disk under original/ '
                 'and returns a type-3 memory stream over the copy (no RtFSManager in the exe)', 3, 0x004274e0),
    0x004cc160: ('RwStreamClose', 'shim', 'PortTop_RwStreamClose_004cc160',
                 T + 'Match/PortTop_RwStream04F.cpp: frees the record and the copy (the original returns it to the RwGlobals '
                 '+0x11c free list)', 3, 0x004274e0),
    0x004cbd30: ('RwStreamRead', 'bound', 'PortTop_RwStreamRead_004cbd30',
                 T + 'Match/PortTop_RwStream04F.cpp -> exe body mashed_re::Render::RwStreamCoreRead (Render/RwStreamCore.cpp)',
                 3, 0x004274e0),
    0x004cc5e0: ('RwStreamFindChunk', 'bound', 'PortTop_RwStreamFindChunk_004cc5e0',
                 T + 'Match/PortTop_RwStream04F.cpp -> exe body mashed_re::D3d9Render::Rwl_StreamFindChunk (RwWorldStream.cpp)',
                 3, 0x004b3d80),
    0x0054f8d0: (None, 'shim', 'MashedRw_TexDictStreamRead_0054f8d0',
                 T + 'LibRw/RwApiBind.cpp: chunk-0x23 dictionary decoded by Txd/TxdDecoder.h from the memory stream, built on '
                 'librw (rw::TexDictionary + RwRasterBridge textures)', 3, 0x004b3d80),
    0x004c5cb0: ('RwTextureRead', 'bound', 'MashedRw_TextureRead_004c5cb0',
                 T + 'LibRw/RwApiBind.cpp: rw::Texture::findCB (current dictionary), else <name>.png / .bmp from the piz mount '
                 'stack or disk, WIC decode, C8888 rw::Texture linked into the current dictionary', 3, 0x004283a0),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_top04e.py')])
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
