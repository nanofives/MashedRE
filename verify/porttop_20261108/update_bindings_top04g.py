"""PORT-TOP-04G: BINDINGS.tsv rows for the sprite lookups (family 1) and sfx.piz 0x0040bbb0 (family 2) moved off their
stand-ins, and the bindings / stand-ins under them. Runs update_bindings_top04f.py first (which runs the 04E updater),
then writes the rows below. Idempotent.
usage: py -3.12 verify/porttop_20261108/update_bindings_top04g.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
T = 'PORT-TOP-04G: '
SI = T + 'stand-in in Match/PortTop_0040bbb0.cpp (logs its first call, does nothing): '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x0040bb50: (None, 'matched', 'SpriteLookupC', T + 'Match/MatchS02_0040bb50.cpp (both targets; the .asi body left '
                 'Frontend/SpriteCluster.cpp) = re/match/s02/0040bb50.c (BYTE-IDENTICAL 21 B, relocheck OK 2/2)', 2, 0x00402750),
    0x0040bb30: (None, 'matched', 'Search40bb30', T + 'Match/MatchS02_0040bb30.cpp (both targets; the .asi body left '
                 'Util/PromoLoop_sessionB.cpp) = re/match/s02/0040bb30.c (BYTE-IDENTICAL 21 B, relocheck OK 2/2)', 2, 0x00402750),
    0x004c5c00: ('RwTexDictionaryFindNamedTexture', 'rw', 'MashedRw_TexDictionaryFindNamedTexture_004c5c00',
                 T + 'LibRw/RwApiBind.cpp -> rw::TexDictionary::find (exe side of the two lookups)', 3, 0x0040bb50),
    0x0040bbb0: (None, 'matched', 'Orig_0040bbb0', T + 'Match/PortTop_0040bbb0.cpp = re/match/s02/0040bbb0.c (100.0%); '
                 'sfx.piz -> fx / badges / TrackImages / Interface dictionaries in DAT_0063b8f8 / fc / 900 / 904', 2, 0x00402750),
    0x0042a6b0: (None, 'matched', 'FUN_0042a6b0', T + 'exe body Match/MatchS06_0042a6b0.cpp (re/match/s06/0042a6b0.c); its '
                 'exe callees bound: 0x004b3d80 -> Orig_004b3d80 with 0 / 0 for the out pointers (declared deviation), '
                 '0x004b3d20 -> stand-in', 3, 0x0040bbb0),
    0x0042a530: (None, 'matched', 'FUN_0042a530', T + 'exe body Match/MatchS06_0042a530.cpp (re/match/s06/0042a530.c)', 4,
                 0x0042a6b0),
    0x0042a470: (None, 'matched', 'FUN_0042a470', T + 'exe body Match/MatchS06_0042a470.cpp (re/match/s06/0042a470.c); '
                 'RwGlobals +0xcc / +0xd4 string functions on the record (LibRw/RwApiBind.cpp)', 5, 0x0042a530),
    0x004b5350: (None, 'shim', 'PortTop_FsFileExists_004b5350', T + 'Match/PortTop_RwStream04F.cpp: 1 when the name is in a '
                 'mounted piz or on the disk under original/ (the original asks the RtFS file system, 0x00550b00)', 6, 0x0042a470),
    0x004b3d20: (None, 'stand-in', 'PortTop_StandIn04G_004b3d20', T + 'Match/PortTop_0040bbb0.cpp: returns 0; its '
                 'chunk-0x16 reader 0x004cf7d0 has no matched source (C2). Not reached by the sfx.piz names', 4, 0x0042a6b0),
    0x0048bbe0: (None, 'matched', 'Orig_0048bbe0', T + 'Match/PortTop_0048bbe0.cpp = re/match/s14/0048bbe0.c; '
                 'RwImageSetPath(""), DAT_00722134 = RwTextureRead("sun")', 3, 0x0040bbb0),
    0x004ce2d0: ('RwImageSetPath', 'bound', 'MashedRw_ImageSetPath_004ce2d0', T + 'LibRw/RwApiBind.cpp (04F), called by '
                 '0x0048bbe0 too', 4, 0x0048bbe0),
    0x004c5bc0: ('RwTexDictionaryAddTexture', 'rw', 'MashedRw_TexDictionaryAddTexture_004c5bc0',
                 T + 'LibRw/RwApiBind.cpp -> rw::TexDictionary::addFront (head insert, as rwLinkListAddLLLink)', 3, 0x0040bbb0),
    0x004a332b: ('exit', 'crt', 'PortTop_Exit04G_004a332b', T + 'Match/PortTop_0040bbb0.cpp -> CRT exit (the "cant open '
                 'sfx dic" branch only)', 3, 0x0040bbb0),
}
for rva, job in ((0x00401000, 'DFF / clump loader'), (0x00413b80, 'effect init, no matched source'),
                 (0x004210b0, 'effect init'), (0x004759b0, '0x00691600 array alloc'), (0x00475a00, 'no matched source (C2)'),
                 (0x00476390, 'effect init'), (0x004775b0, 'atomic / clump builder'), (0x00477e40, 'effect init'),
                 (0x00485d90, 'effect init'), (0x004862d0, 'effect init'), (0x00487e00, 'object-shadow init'),
                 (0x00489290, 'effect init'), (0x0048a460, 'strided clear 0x00709238'), (0x0048a830, 'zero fill'),
                 (0x0048ae00, 'clump builder'), (0x0048e820, 'no matched source (C2)'), (0x0048eac0, 'effect init'),
                 (0x0048ff20, 'file-backed table loader')):
    FIXED[rva] = (None, 'stand-in', 'PortTop_StandIn04G_%08x' % rva, SI + job, 3, 0x0040bbb0)


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_top04f.py')])
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
