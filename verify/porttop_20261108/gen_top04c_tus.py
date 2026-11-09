"""PORT-TOP-04C: generate the exe-only TUs that compile AppDestroy 0x00402a40 and its matched, null-safe callees for the
MASHED_ORIG_TOP path, plus Match/PortTop_StandIns04C.cpp (the named stand-ins). Same recipe as gen_top04b_tus.py.

usage: py -3.12 verify/porttop_20261108/gen_top04c_tus.py   (rewrites Match/PortTop_<rva>.cpp, PortTop_StandIns04C.cpp)
       py -3.12 verify/porttop_20261108/gen_top04c_tus.py --check   (lists declared callees with no binding)
"""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
MATCH = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'Match')

# callee name in a source -> (exe symbol, comment)
RW = {
    'FUN_004c5930': ('MashedRw_TexDictionaryDestroy_004c5930', 'RW: RwTexDictionaryDestroy -> rw::TexDictionary::destroy'),
    'FUN_004c0740': ('MashedRw_ObjectHasFrameSetFrame_004c0740', 'RW: _rwObjectHasFrameSetFrame -> rw::Camera::setFrame'),
    'FUN_004c0c20': ('MashedRw_FrameDestroy_004c0c20', 'RW: RwFrameDestroy -> rw::Frame::destroy'),
    'FUN_004c1cf0': ('MashedRw_CameraDestroy_004c1cf0', 'RW: RwCameraDestroy -> rw::Camera::destroy + the record'),
    'FUN_004c7650': ('MashedRw_RasterDestroy_004c7650', 'RW: RwRasterDestroy -> rw::Raster::destroy'),
    'FUN_004e6e00': ('MashedRw_ClumpDestroy_004e6e00', 'RW: RpClumpDestroy -> rw::Clump::destroy'),
    'RpClumpDestroy': ('MashedRw_ClumpDestroy_004e6e00', 'RW: RpClumpDestroy 0x004e6e00 -> rw::Clump::destroy'),
}
EXE = {
    'FUN_00426ba0': ('HudDrawEnabled', 'exe body, Match/MatchS06_00426ba0.cpp'),
    'FUN_00426c00': ('GameStateFlagGet', 'exe body, Match/MatchS06_00426c00.cpp'),
    'FUN_004114c0': ('VtableTeardown_114c0', 'exe body, Match/MatchS02_004114c0.cpp'),
    'FUN_0045b350': ('RwInitNullStub', 'exe body, Match/MatchS09_0045b350.cpp'),
    'FUN_004d8560': ('UtilReturnOne4d8560', 'exe body, Util/UtilLeaves_ab6.cpp'),
    'FUN_004671a0': ('BackBuffRasterOpen', 'exe body, Match/MatchS11_004671a0.cpp'),
    'FUN_00467020': ('Orig_00467020', 'exe body, Match/PortTop_00467020.cpp (PORT-TOP-04B)'),
    'FUN_00484c90': ('Zero484c90', 'exe body, Match/MatchS14_00484c90.cpp'),
    'FUN_00556cd0': ('GetDat00912a20', 'exe body, Frontend/BatchAA_s6.cpp'),
    'thunk_FUN_00493f70': ('ThunkVideoStateGet', 'exe body of the thunk rva 0x00494ef0, Boot/Teardown.cpp'),
    'FUN_0041f320': ('Table63d830Get', 'matched, Match/MatchS04_0041f320.cpp (exe since PORT-TOP-04C)'),
    'FUN_00494460': ('IntroVideoClose', 'matched, Match/MatchS15_00494460.cpp (exe since PORT-TOP-04C)'),
}

# rva -> (source, exe symbol, mechanical description)
TUS = {
    '00402a40': ('s01/raw/00402a40.c', 'AppDestroy', 'AppDestroy: the teardown WinMain 0x00492370 runs after its loop'),
    '004015a0': ('s01/004015a0.c', 'Orig_004015a0', 'RpClumpDestroy on the 13 x 0x68 slots at 0x00636578, then resets them'),
    '00403660': ('s01/00403660.c', 'Orig_00403660', 'Release of DAT_00636b70 (vtable +8) and RwRasterDestroy of DAT_00636b78, each when nonzero'),
    '0040bd00': ('s02/0040bd00.c', 'Orig_0040bd00', '14 calls, then RwTexDictionaryDestroy on DAT_0063b8f8..0x0063b904'),
    '0040cf80': ('s02/0040cf80.c', 'Orig_0040cf80', 'when 0x0041f320(i) is nonzero for any i < 4: 0x0041a980, 0x00422140, 0x0041ec00(0..3), 0x0045b350, 0x0040cf40'),
    '0040cfd0': ('s02/0040cfd0.c', 'Orig_0040cfd0', '11 calls and DAT_0080332c = 0'),
    '0041a3d0': ('s03/raw_0041a3d0.c', 'Orig_0041a3d0', 'RpClumpDestroy(DAT_0063c620)'),
    '0041c0e0': ('s04/0041beb0.c', 'Orig_0041c0e0', 'RpClumpDestroy on the clump of the 2 records at 0x0063cab8; holds static 0x0041beb0'),
    '0041c2c0': ('s04/0041c2c0.c', 'Orig_0041c2c0', 'RpClumpDestroy(DAT_0063cdb4)'),
    '0041ccf0': ('s04/0041cb00.c', 'Orig_0041ccf0', 'RpClumpDestroy on the 4 records at 0x0063ce20, RwTexDictionaryDestroy(DAT_0063cdd0); holds static 0x0041cb00'),
    '0041d890': ('s04/0041d6d0.c', 'Orig_0041d890', 'RpClumpDestroy on the 2 records at 0x0063d298; holds static 0x0041d6d0'),
    '0041da80': ('s04/0041da80.c', 'Orig_0041da80', 'RpClumpDestroy(DAT_0063d57c)'),
    '0041de70': ('s04/0041de70.c', 'Orig_0041de70', 'RpClumpDestroy(DAT_0063d5e0)'),
    '00425ed0': ('s05/00425ed0.c', 'Orig_00425ed0', 'RwTexDictionaryDestroy(DAT_00644150)'),
    '00427620': ('s06/00427620.c', 'Orig_00427620', '0x00555830 / 0x00556e40 on the DAT_0067d838..0x0067d848 handles, RwTexDictionaryDestroy(0x00556cd0()) when nonzero, 0x00552b90'),
    '00428400': ('s06/00428400.c', 'Orig_00428400', '0x004ce2d0(0x005cc4de), 0x004c5a60 on DAT_0067d964..0x0067d970, DAT_00771964 = 0'),
    '0042c2a0': ('s07/0042c2a0.c', 'Orig_0042c2a0', 'RwTexDictionaryDestroy(DAT_0067ea0c) and RpWorldDestroy(DAT_0067ea00), each when nonzero'),
    '0045b930': ('s09/0045b930.c', 'Orig_0045b930', '5 calls, the DAT_005f9bd8 function-pointer entries of 0x005f99b0 that are nonzero, RwTexDictionaryDestroy(DAT_0068d4e0)'),
    '00467010': ('s10/00467010.c', 'Orig_00467010', '0x0045e040, 0x00466a50'),
    '00467070': ('s10/00467070.c', 'Orig_00467070', 'camera DAT_006905b4: frame unset, RwFrameDestroy(DAT_006905b8), RwCameraDestroy'),
    '00484130': ('s14/00484130.c', 'Orig_00484130', 'when param_1 is nonzero: RwGlobals +0x10c on param_1 +0x10 (when nonzero) and on param_1'),
    '00494bc0': ('s16/00494bc0.c', 'Orig_00494bc0', 'vtable +0x24 on DAT_00771a48, then Release (vtable +8) on 6 pointers at 0x00771a38..0x00771a4c, each when nonzero'),
    '00496ce0': ('s16/00496ce0.c', 'Orig_00496ce0', '0x004d41e0(DAT_00773094), 0x004d41e0(DAT_00773088)'),
    '004b4880': ('s18/004b4880.c', 'Orig_004b4880', 'destroys a camera: frame (+0x04), rasters (+0x60, +0x64) when nonzero, the camera'),
    '005581f0': ('s24/005581f0.c', 'Orig_005581f0', 'RwRasterDestroy(param_1) when nonzero, else an RwError 0x180 / 0x80000016'),
    '004210f0': ('s05/004210f0.c', 'Orig_004210f0', '0x004768c0(&DAT_0063e548)'),
    '00496970': ('s16/00496970.c', 'Orig_00496970', 'RwRasterDestroy(DAT_0077307c) when nonzero; returns 1'),
    '00494320': ('s15/00494320.c', 'Orig_00494320', 'Stop + Release of DAT_00771a34, Release of 5 pointers when nonzero, Release of DAT_00771a10 / DAT_00771a14'),
    # thunks AppDestroy calls (one jmp each)
    '004189e0': ('s03/h1_004189e0.c', 'Orig_004189e0', 'thunk: jmp 0x004196f0'),
    '00421590': ('s05/00421590.c', 'Orig_00421590', 'thunk: jmp 0x004210f0'),
    '00494f20': ('s16/00494f20.c', 'Orig_00494f20', 'thunk: jmp 0x00494460'),
    '0047ba10': ('s13/0047ba10.c', 'Orig_0047ba10', 'thunk: jmp 0x00496970'),
    '004b6550': ('s18/004b6550.c', 'Orig_004b6550', 'thunk: jmp 0x004b6700'),
}
# thunk names AppDestroy's source uses -> the thunk TU symbol
THUNK = {
    'thunk_FUN_004196f0': 'Orig_004189e0', 'thunk_FUN_004210f0': 'Orig_00421590', 'thunk_FUN_00494460': 'Orig_00494f20',
    'thunk_FUN_00496970': 'Orig_0047ba10', 'thunk_FUN_004b6700': 'Orig_004b6550',
}
# rva -> why it is a stand-in (matched sources that are not run, and callees with no source / no exe body)
STAND = {
    '004014b0': 'matched; reads DAT_00636570 +4 / DAT_0063656c +4 with no null check, state 0x00402750 creates (still a stand-in)',
    '0041e0d0': 'matched; reads +4 of each pointer of the 0x0063d610 table with no null check (state of 0x00402750)',
    '00489250': 'matched; reads DAT_007030a8 +4 / DAT_007030ac +4 with no null check (state of 0x00402750)',
    '0041ffb0': 'matched; 0x004b4880(DAT_0063e490) reads +4 of a zero pointer when DAT_00636acc == 0 (state of 0x00402750)',
    '004196f0': 'matched; reads +4 of each pointer of the 0x0063bde0 table with no null check (state of 0x00402750)',
    '0041b660': 'matched only in the group source h1_grp_0041ad60.c, whose other members 0x0041ad60 / 0x0041ae20 / 0x0041b520 / 0x0041b630 already have exe bodies (one body per RVA)',
    '004b6700': 'matched; its only port body Zero4b6700 is in the .asi-only multi-function Util/PromoLoop_sessionB.cpp',
    # level 2 (callees of compiled bodies)
    '00413bb0': 'matched; port body only in .asi-only Util/PromoLoop_round256.cpp',
    '00475d90': 'matched, not compiled (level 2)', '00475ea0': 'matched, not compiled (level 2)',
    '00477870': 'matched in group s12/00477580.c, not compiled (level 2)', '00485e10': 'matched, not compiled (level 2)',
    '00486350': 'matched, not compiled (level 2)', '00487140': 'matched; .asi-only Match/MatchS14_00487140.cpp, raw-address callees',
    '0048af70': 'matched, not compiled (level 2)', '0048bc10': 'matched, not compiled (level 2)',
    '0048cf70': 'matched, not compiled (level 2)', '0048fdd0': 'matched, not compiled (level 2)',
    '0048ffd0': 'matched, not compiled (level 2)',
    '0041a980': 'matched (raw g2s source, no MGLOBAL), not compiled (level 2)',
    '00422140': 'matched, not compiled (level 2)', '0041ec00': 'matched, not compiled (level 2)',
    '0040cf40': 'matched; .asi-only Match/MatchS02_0040cf40.cpp, raw-address callees',
    '00490000': 'matched, not compiled (level 2)', '0045bed0': 'matched, not compiled (level 2)',
    '0045bf30': 'matched in group s10/0045bae0.c, not compiled (level 2)',
    '004725f0': 'matched; .asi-only Match/MatchS11_004725f0.cpp binds 0x0046be10 to the .asi-only Write46be10',
    '00426b40': 'matched, not compiled (level 2)', '00405400': 'matched; port body only in .asi-only Util/PromoLoop_round53.cpp',
    '00471df0': 'matched, but its source also holds static 0x00471530, whose body is the .asi-only Match/MatchS11_00471530.cpp',
    '004114e0': 'matched; port body ReplayCleanup only in .asi-only Replay.cpp',
    '00412010': 'matched; reads DAT_0063bb38[i] +0x3c with no null check (state of 0x00402750)',
    '00458880': 'matched, not compiled (level 2)', '004593b0': 'matched, not compiled (level 2)',
    '00404820': 'matched (raw g2s source, no MGLOBAL), not compiled (level 2)',
    '0045e040': 'matched, not compiled (level 2)', '00466a50': 'matched, not compiled (level 2)',
    '00555830': 'matched, not compiled (level 2)', '00556e40': 'matched, not compiled (level 2)',
    '00552b90': 'matched (FontSys_Shutdown), not compiled (level 2)',
    '004ce2d0': 'RW RwImageSetPath (rw catalog match); no binding yet', '004c5a60': 'RW-range, matched rwl01; no binding yet',
    '004d41e0': 'RW-range, matched s20; no binding yet', '004d7ff0': 'port body RwIdentityPassthrough only in .asi-only Render/HighAB3Helpers_p6.cpp',
    '004d8480': 'port body RwErrSlotWrite only in .asi-only Render/HighAB3Helpers_p6.cpp',
    '004768c0': 'matched, not compiled (level 2)',
    '004e5700': 'RW RpWorldDestroy (hooks.csv name); no librw world binding yet',
}
NAMED = {'RpWorldDestroy': '004e5700'}
# stand-ins whose callers declare a non-cdecl prototype
CONV = {'0048af70': '__fastcall '}

TEMPLATE = '''// PORT-TOP-04C (L-96 top-down lane): exe-only port body of rva 0x{rva} = the matched source re/match/{src}
// ({what}). Generated by verify/porttop_20261108/gen_top04c_tus.py. Reached only on the MASHED_ORIG_TOP path
// (WinMain 0x00492370 -> AppDestroy 0x00402a40). Callees bind by name; no raw-address call. RW core callees bind to
// LibRw/RwApiBind.cpp, callees that do not run are the named stand-ins of Match/PortTop_StandIns04C.cpp.
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define {self} {sym}
{binds}
extern "C" {{
{decls}#include "../../../../re/match/{src}"
}}
#endif // MASHED_STANDALONE
'''

DECL = re.compile(r'^[\w][\w\s\*]*?\b((?:thunk_)?FUN_[0-9a-f]{8}|Rp\w+|Rw\w+)\s*\(', re.M)
CALL = re.compile(r'\b((?:thunk_)?FUN_[0-9a-f]{8}|Rp[A-Z]\w+|Rw[A-Z]\w+)\s*\(')


def selfname(src):
    t = open(os.path.join(ROOT, 're', 'match', src), encoding='utf-8', errors='replace').read()
    m = re.findall(r'^// MATCH: 0x([0-9a-f]{8}) (\S+)', t, re.M)
    return t, m


def bind_of(name):
    if name in RW:
        return RW[name]
    if name in EXE:
        return EXE[name]
    if name in THUNK:
        return THUNK[name], 'thunk, Match/PortTop_%s.cpp' % THUNK[name][5:]
    rva = NAMED.get(name) or (name[-8:] if name.startswith('FUN_') else None)
    if rva and rva in TUS:
        return TUS[rva][1], 'matched, Match/PortTop_%s.cpp' % rva
    if rva and rva in STAND:
        return 'PortTop_StandIn04C_%s' % rva, 'stand-in: ' + STAND[rva]
    return None


def callees(text, own):
    body = text[text.index('// MATCH:'):]
    names = []
    for m in CALL.finditer(body):
        n = m.group(1)
        if n not in own and n not in names:
            names.append(n)
    return names


def gen(check=False):
    missing = {}
    for rva, (src, sym, what) in TUS.items():
        text, matches = selfname(src)
        own = {n for _, n in matches}
        selfn = [n for a, n in matches if a == rva]
        selfn = selfn[0] if selfn else 'FUN_' + rva
        lines, decls = [], ''
        for n in callees(text, own):
            b = bind_of(n)
            if not b:
                missing.setdefault(n, []).append(rva)
                continue
            lines.append('#define %s %s   // %s' % (n, b[0], b[1]))
            # a callee the source names but does not declare (the thunk sources rely on implicit declarations)
            head = re.sub(r'#ifndef MATCH_PORT.*?#endif', '', text[:text.index('// MATCH:')], flags=re.S)
            if not re.search(r'^[\w][\w\s\*]*\b%s\s*\(' % re.escape(n), head, re.M):
                decls += 'int %s();   /* not declared by the source */\n' % n
        if check:
            continue
        txt = TEMPLATE.format(rva=rva, src=src, what=what, self=selfn, sym=sym, binds='\n'.join(lines), decls=decls)
        open(os.path.join(MATCH, 'PortTop_%s.cpp' % rva), 'w', encoding='utf-8', newline='\n').write(txt)
        print('PortTop_%s.cpp' % rva)
    if missing:
        print('UNBOUND:', missing)
    if check:
        return
    # stand-ins
    out = ['// PORT-TOP-04C: named stand-ins for the AppDestroy 0x00402a40 tree on the MASHED_ORIG_TOP path (exe only).',
           '// Generated by verify/porttop_20261108/gen_top04c_tus.py. Each logs its first call and does nothing; the reason',
           '// is in the table below and in verify/porttop_20261108/RESULT_PORT-TOP-04C.md. Not bodies of these RVAs.',
           '#ifdef MASHED_STANDALONE', '#include <cstdio>', '', 'namespace {',
           'void StandInLog(const char* rva, int* once) {', '    if (*once) return;', '    *once = 1;',
           '    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {',
           '        std::fprintf(f, "[PORT-TOP] rva %s: stand-in, not run (PORT-TOP-04C)\\n", rva);', '        std::fclose(f);',
           '    }', '}', '} // namespace', '', 'extern "C" {']
    for rva, why in STAND.items():
        out.append('// rva %s: %s' % (rva, why))
        cc = CONV.get(rva, '')
        out.append('int %sPortTop_StandIn04C_%s() { static int once; StandInLog("0x%s", &once); return 0; }' % (cc, rva, rva))
    out += ['} // extern "C"', '#endif // MASHED_STANDALONE', '']
    open(os.path.join(MATCH, 'PortTop_StandIns04C.cpp'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out))
    print('PortTop_StandIns04C.cpp')


if __name__ == '__main__':
    gen('--check' in sys.argv)
