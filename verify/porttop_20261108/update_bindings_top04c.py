"""PORT-TOP-04C: AppDestroy 0x00402a40 and its callee tree in BINDINGS.tsv (one row per RVA, idempotent).
Built from the tables of gen_top04c_tus.py. usage: py -3.12 verify/porttop_20261108/update_bindings_top04c.py
"""
import os, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gen_top04c_tus as G       # noqa: E402

TSV = os.path.join(HERE, 'BINDINGS.tsv')
T = 'PORT-TOP-04C: '
A = 0x00402a40
L1 = ['00403660', '00494f20', '00494bc0', '004c5930', '00496ce0', '0040cf80', '00426ba0', '00426c00', '0040cfd0',
      '004114c0', '0041a3d0', '0041de70', '0041c2c0', '0041da80', '0041e0d0', '0041d890', '0041ccf0', '0041c0e0',
      '0041b660', '00489250', '0045b930', '0041ffb0', '004189e0', '00421590', '004014b0', '0040bd00', '00484130',
      '0045b350', '005581f0', '004671a0', '00467020', '00467070', '004b4880', '0042c2a0', '00427620', '0047ba10',
      '00425ed0', '004015a0', '004b6550', '00467010', '004d8560', '00428400', '00494ef0']
# callee -> parent for the deeper rows (first compiled caller)
PARENT = {
    '004196f0': '004189e0', '004210f0': '00421590', '00494460': '00494f20', '00496970': '0047ba10', '004b6700': '004b6550',
    '00494320': '00494460', '0041beb0': '0041c0e0', '0041cb00': '0041ccf0', '0041d6d0': '0041d890', '0041f320': '0040cf80',
    '004e6e00': '0041c2c0', '00484c90': '0040cfd0', '00556cd0': '00427620', '004e5700': '0042c2a0',
}
for r in ('00413bb0 00475d90 00475ea0 00477870 00485e10 00486350 00487140 0048af70 0048bc10 0048cf70 0048fdd0 '
          '0048ffd0').split():
    PARENT[r] = '0040bd00'
for r in '0041a980 00422140 0041ec00 0040cf40'.split():
    PARENT[r] = '0040cf80'
for r in '00490000 0045bed0 0045bf30 004725f0 00426b40 00405400 00471df0 004114e0'.split():
    PARENT[r] = '0040cfd0'
for r in '00412010 00458880 004593b0 00404820'.split():
    PARENT[r] = '0045b930'
for r, p in (('0045e040', '00467010'), ('00466a50', '00467010'), ('00555830', '00427620'), ('00556e40', '00427620'),
             ('00552b90', '00427620'), ('004ce2d0', '00428400'), ('004c5a60', '00428400'), ('004d41e0', '00496ce0'),
             ('004d7ff0', '005581f0'), ('004d8480', '005581f0'), ('004768c0', '004210f0')):
    PARENT[r] = p

EXEBODY = {   # rva -> (kind, symbol, where)
    '00426ba0': ('matched', 'HudDrawEnabled', 'Match/MatchS06_00426ba0.cpp (exe body already)'),
    '00426c00': ('matched', 'GameStateFlagGet', 'Match/MatchS06_00426c00.cpp (exe body already)'),
    '004114c0': ('matched', 'VtableTeardown_114c0', 'Match/MatchS02_004114c0.cpp (exe body already)'),
    '0045b350': ('matched', 'RwInitNullStub', 'Match/MatchS09_0045b350.cpp (exe body already)'),
    '004d8560': ('port', 'UtilReturnOne4d8560', 'Util/UtilLeaves_ab6.cpp (exe body already)'),
    '004671a0': ('matched', 'BackBuffRasterOpen', 'Match/MatchS11_004671a0.cpp (PORT-TOP-04B)'),
    '00467020': ('matched', 'Orig_00467020', 'Match/PortTop_00467020.cpp (PORT-TOP-04B): destroys the 0x004671a0(-1) camera record'),
    '00484c90': ('matched', 'Zero484c90', 'Match/MatchS14_00484c90.cpp (exe body already)'),
    '00556cd0': ('port', 'GetDat00912a20', 'Frontend/BatchAA_s6.cpp (exe body already)'),
    '00494ef0': ('port', 'ThunkVideoStateGet', 'Boot/Teardown.cpp: the thunk 0x00494ef0 AppDestroy calls (thunk_FUN_00493f70), reads DAT_00771a04'),
    '0041f320': ('matched', 'Table63d830Get', 'Match/MatchS04_0041f320.cpp, now also in the exe list'),
    '00494460': ('matched', 'IntroVideoClose', 'Match/MatchS15_00494460.cpp, now also in the exe list; MASHED_STANDALONE binds 0x00494320 / 0x004c7650 by name'),
    '004c5930': ('bound', 'MashedRw_TexDictionaryDestroy_004c5930', 'LibRw/RwApiBind.cpp: rw::TexDictionary::destroy'),
    '004e6e00': ('bound', 'MashedRw_ClumpDestroy_004e6e00', 'LibRw/RwApiBind.cpp: RpClumpDestroy -> rw::Clump::destroy (new in PORT-TOP-04C)'),
}
STATIC = {'0041beb0': '0041c0e0', '0041cb00': '0041ccf0', '0041d6d0': '0041d890'}


def level(rva):
    if rva == '00402a40':
        return 1
    if rva in L1:
        return 2
    return level(PARENT[rva]) + 1


def caller(rva):
    if rva == '00402a40':
        return 0x00492370
    if rva in L1:
        return A
    return int(PARENT[rva], 16)


def rows_new():
    out = {}
    for rva, (src, sym, what) in G.TUS.items():
        name = 'AppDestroy' if rva == '00402a40' else None
        note = T + 'Match/PortTop_%s.cpp (re/match/%s): %s' % (rva, src, what)
        if rva == '00402a40':
            note = (T + 'Match/PortTop_00402a40.cpp (re/match/s01/raw/00402a40.c, 100%% byte match) through '
                    'PortTop_Bound_00402a40 (Match/PortTop_00492370.cpp), which then runs HandTop_AppDestroy for the exe '
                    'audio / DirectInput / small.mpg (state outside the original globals)')
        out[rva] = (name, 'matched', sym if rva != '00402a40' else 'AppDestroy', note)
    for rva, owner in STATIC.items():
        out[rva] = (None, 'matched', 'FUN_%s (static)' % rva, T + 'file-static in the source of 0x%s, compiled in Match/PortTop_%s.cpp' % (owner, owner))
    for rva, (k, sym, where) in EXEBODY.items():
        out[rva] = (None, k, sym, T + where)
    for rva, why in G.STAND.items():
        out[rva] = (None, 'stand-in', 'PortTop_StandIn04C_%s' % rva, T + 'Match/PortTop_StandIns04C.cpp (logs, does nothing): ' + why)
    return out


def main():
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note) in rows_new().items():
        k = int(rva, 16)
        r = idx.get(k)
        if r is None:
            r = ['0x%08x' % k, name or 'FUN_%08x' % k, '', '', '', '', '']
            rows.append(r)
            idx[k] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(level(rva)), '0x%08x' % caller(rva)
    left = [r[0] for r in rows if r[2] == 'under-stand-in' and r[6] == '0x00402a40']
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)), 'under-stand-in left under 0x00402a40:', left)


if __name__ == '__main__':
    main()
