"""RWC-CORE-RUN (L-124 (C) step 2): generate the exe-only TUs that compile the RenderWare core bodies of the engine
bring-up (RwEngineInit / Open / Start / Stop / Close / Term, the D3D9 device system function and the device block's
other entries) for the MASHED_ORIG_TOP path under the default-OFF knob MASHED_RWC_CORE.

usage: py -3.12 verify/rwccorerun_20261111/gen_rwcore_tus.py [--check]

Writes mashedmod/src/mashed_re/RwCore/:
  RwCore_<rva>.cpp      one per source in the closure: the re/match source compiled verbatim as C++ (MATCH_PORT), its
                        own name renamed RwCore_<rva>, every callee and every code-pointer name bound by #define
  RwCore_Missing.cpp    one logged stub per RW-band RVA in the closure with no source and no hand body: it logs the RVA
                        and ends the process (exit code 0x52574300 | n), so a boot names the first missing body
  RwCore_DataSeed.inc   the original's file-backed bytes of the RW core's data ranges, and the code-pointer fix-ups
                        inside them (original target RVA -> exe symbol)
  RwCore_Bodies.tsv     (in this folder) one row per RVA: kind (match100 / provisional / hand / exe / missing), source,
                        best_pct, exe symbol
and rewrites the generated block of mashedmod/exe_sources.rsp.

Closure: from ROOTS, every (thunk_)FUN_<rva> / LAB_<rva> name in a source, RW band only (rw_census.py's bands plus
_rwcseg 0x005cb000..0x005cc000). Non-RW callees bind through EXT. A source that defines more than one MATCH function
binds every name it defines. Sources are picked from re/analysis/match_ledger.tsv (best best_pct), else the only file
re/match/**/<rva>.c.
"""
import csv
import glob
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lift_rwcore  # noqa: E402  (RWC-PLUGIN-ATTACH)

# RWC-PLUGIN-ATTACH: lift closure RVAs that have no source (lift_rwcore.py) instead of emitting a missing stop.
# --no-lift restores the run-38 behaviour.
LIFT = '--no-lift' not in sys.argv
LIFT_CHUNK = 64   # lifted bodies per generated file
# Provisional (NOT-PROVEN, raw g2s) sources that do not compile as C++ under VS2022 (C++ type strictness: argument
# conversions, call arity, extern "C" overloads; RESULT_RWC-PLUGIN-ATTACH.md lists the errors). Their functions are
# lifted from the disassembly instead. Only non-100% sources may be listed here: a 100% source gets a cast fix.
LIFT_INSTEAD = {
    're/match/rwl01/004ce5d0.c', 're/match/rwl02/004ea9a0.c', 're/match/rwl03/004f1fb0.c', 're/match/rwl03/004f2570.c',
    're/match/rwl03/004f3e90.c', 're/match/rwl03/00532b80.c', 're/match/rwl03/0053e830.c', 're/match/rwl04/005493e0.c',
    're/match/s19/004c6580.c', 're/match/s19/004c6ba0.c',
    # run 40 (node-definition callbacks joined the closure): C++ type errors under VS2022 (build_vs9.log)
    're/match/rwl03/00530650.c', 're/match/rwl03/00530c00.c', 're/match/rwl04/0054ba20.c', 're/match/rwl04/0054bda0.c',
}
# run 40 (librw binding RVAs as roots): sources that failed VS2022 C++ in a build, one per line (collected from the
# build logs by the build loop, see RESULT 3.2)
_LI = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'LIFT_INSTEAD_RUN40.txt')
if os.path.exists(_LI):
    LIFT_INSTEAD |= {ln.strip() for ln in open(_LI, encoding='utf-8') if ln.strip() and not ln.startswith('#')}

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
OUT = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'RwCore')
RSP = os.path.join(ROOT, 'mashedmod', 'exe_sources.rsp')
ANCHOR = os.environ.get('MASHED_UNPATCHED', os.path.normpath(os.path.join(ROOT, '..', '..', 'original',
                                                                           'MASHED.exe.unpatched')))

ROOTS = [
    '004c32b0', '004c30b0', '004c2fb0', '004c2f60', '004c3040', '004c3270',   # RwEngineInit/Open/Start/Stop/Close/Term
    '004c9f50', '004c9eb0', '004c9f60',                                        # driver setters 0x00493710 calls
    '00493640',                                                                # RenderwareAttachPlugins (run 38)
    '004b3eb0',                                                                # run 40: TXD loader, frame knob
    '00428450',                                                                # RWC-FIRST-FRAME-IM2D: loading-disc draw
    '00492e90',                                                                # RWC-FRAME-492E90: frame render
]
# Run 38: RVAs outside the RW bands that the plugin attaches reach and the closure follows like RW code (each has a
# re/match source or becomes a logged stop): 0x00493640 itself, its non-band attaches 0x00472380 / 0x0052d8e0 /
# 0x0057c270 / 0x00561ee0 and the callbacks they register, and four 100% callees the attach sources call
# (0x004522d0, 0x0057c440, 0x005c4640, 0x005c4780).
XFOLLOW = {0x493640, 0x472380, 0x472330, 0x472360, 0x52d8e0, 0x52d910, 0x52d950, 0x57c270, 0x57c2b0, 0x57c2e0,
           0x561ee0, 0x561f40, 0x561f50, 0x561f80, 0x561ff0, 0x4522d0, 0x57c440, 0x5c4640, 0x5c4780,
           # RWC-PLUGIN-ATTACH: callees of lifted plugin bodies just outside the RW bands (LIFT_REFUSED.tsv, first pass)
           0x4722e0, 0x52d980, 0x52ddc0, 0x52ddf0, 0x558c50, 0x558c80, 0x558cb0, 0x562560,
           0x4721e0, 0x52da20, 0x55dec0, 0x5c4ab0, 0x5c4ad0, 0x5c4d50, 0x5c4e10,
           0x4a3370, 0x4a3907,   # two CRT leaves (20 / 21 bytes) the lifted d3dx bodies call
           # run 40: callees of re/match/rwl02/004e7420.c (a binding root), sources in re/match/s28
           0x5c4670, 0x5c47b0, 0x5c47e0,
           # run 40 (MASHED_RWC_FRAME): the TXD-from-memory loader that DisplayInit calls (PortTop_Port_004b3eb0 is librw)
           0x4b3eb0,
           # RWC-FIRST-FRAME-IM2D (MASHED_RWC_FRAME): ViewportInit 0x00428590's loading-disc draw 0x00428450
           # (re/match/s06, 84.6%), its Im2D quad 0x00450b10 (re/match/s08, 86.6%) and the two screen-dim getters
           # (exe bodies, EXE_BODY below)
           0x428450, 0x450b10, 0x42b8b0, 0x42b8c0}
# RWC-FRAME-492E90 (MASHED_RWC_FRAME): the frame render 0x00492e90 and the game functions of its menu-executed slice
# (verify/rwcframe_20261113/SLICE_EXEC_reach_menu.tsv: reachable from 0x00492e90 through functions the INT3 census
# re/coverage/reach.tsv marks reached_menu = 1) are followed like RW code and compiled from their re/match sources. A game
# callee of a followed source outside the slice (the original does not execute it at the menu, or only from callers
# outside this closure) is a logged stop (FRAME_STOP): never lifted (L-137 / L-141), so a boot names the first one the exe
# reaches and the next slice.
_SLICE = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'rwcframe_20261113', 'SLICE_EXEC_reach_menu.tsv')
FRAME_SLICE = set()
if os.path.exists(_SLICE):
    for _ln in open(_SLICE, encoding='utf-8').read().splitlines()[1:]:
        _f = _ln.split('\t')
        if len(_f) > 1 and _f[1] != 'RW':
            FRAME_SLICE.add(int(_f[0], 16))
XFOLLOW |= FRAME_SLICE
XFOLLOW.add(0x5c4d30)   # RWC-FRAME-492E90: re/match/s28/005c4d30.c (100%), called by RtCharset sources 0x005554d0 / 0x005555b0
FRAME_STOP = set()   # filled while walking: 8-digit hex RVAs
# Not roots in this row: 0x00493710's calls after RwEngineStart (GetCurrentMode 0x004c2f00, GetModeInfo 0x004c2ed0,
# 0x004cbc60..0x004cbc90, RwTexDictionarySetCurrent 0x004c5c80) are not reached, since the knob ends the boot at Start.
# Data ranges of the RW core (file-backed, seeded from the anchored binary under the knob). Every dword in them whose
# value is a code address becomes a fix-up and its target joins the closure.
# Run 38: extended past 0x00618600 to 0x00618a00 for the plugin registries the attaches use (0x0061862c .. 0x006189e4,
# 0x18-byte records as 0x00617fe0), plus the one-dword 0x00623fac (0xffffffff in the file; 0x00561ee0 returns 0 unless
# it is -1 on entry).
DATA_RANGES = [(0x00617f00, 0x00618a00), (0x00623fac, 0x00623fb0)]
# RWC-PLUGIN-ATTACH: the rest of the RW core's file-backed .data that the closure's sources and lifted bodies read
# (0x00618a00 .. 0x00624400: 138 nonzero initialized dwords referenced and unseeded before, among them the d3dx
# constant blocks at 0x0061bcb0 ..). Seeded verbatim. A dword in it becomes a fix-up only when it equals a known function
# start (function_inventory / match_ledger / a re/match source): the range holds packed 16-bit pairs such as 0x00560039
# that fall inside .text by value.
DATA_RANGES_STRICT = [(0x00618a00, 0x00623fac), (0x00623fb0, 0x00624400)]
# .rdata windows the closure's sources read (strings / constants); seeded verbatim, no code pointer is followed.
RDATA_WINDOW = 0x100

# Non-RW callees: name in a source -> (exe symbol, why). CRT: exe CRT thunks of RwCore/RwCore_Crt.cpp.
EXT = {
    'FUN_0045b350': ('RwInitNullStub', 'exe body, Match/MatchS09_0045b350.cpp'),
    'FUN_005c9d00': ('GetRaceEndTrigger', 'exe body, Util/GameStateGetters.cpp'),
    'FUN_004a2b60': ('RwCoreCrt_sprintf', 'CRT sprintf'),
    'FUN_004a42c5': ('RwCoreCrt_vsprintf', 'CRT vsprintf'),
    'FUN_004ac560': ('RwCoreCrt_strcpy', 'CRT strcpy'),
    'FUN_004a34b0': ('RwCoreCrt_strncpy', 'CRT strncpy'),
    'FUN_004ac570': ('RwCoreCrt_strcat', 'CRT strcat'),
    'FUN_005c2300': ('RwCoreCrt_strncat', 'CRT strncat'),
    'FUN_004a3860': ('RwCoreCrt_strstr', 'CRT strstr (RwGlobals +0xe4, RW 3.x RwStringFunctions order)'),
    'FUN_004af340': ('RwCoreCrt_strcmp', 'CRT strcmp'),
    'FUN_004a3a90': ('RwCoreCrt_strncmp', 'CRT strncmp (RwGlobals +0xec)'),
    'FUN_004a9410': ('RwCoreCrt_strlen', 'CRT strlen'),
    'FUN_004a3ac9': ('RwCoreCrt_strtok', 'CRT strtok'),
    'FUN_005c22bd': ('RwCoreCrt_sscanf', 'CRT sscanf'),
    'FUN_005aead0': ('RwCoreCrt_realloc', 'CRT realloc (RwGlobals +0x110)'),
    '_strncpy': ('RwCoreCrt_strncpy', 'CRT strncpy called by name (re/match/s24/00550b00.c, rwl04/00551ad0.c)'),
    '_strstr': ('RwCoreCrt_strstr', 'CRT strstr called by name (re/match: 0x005507b0, run 40)'),
    'FUN_004a3440': ('_chkstk', 'CRT __chkstk (function_inventory 0x004a3440; eax = frame size), called by lifted bodies'),
    'FUN_005c24e0': ('RwCoreCrt_qsort', 'CRT qsort (RWC-PLUGIN-ATTACH; 4 args base, n, width, compare)'),
    'FUN_004a2d30': ('RwCoreCrt_pow', 'CRT pow (exe CRT, not the DDK libcmt body: L-120 b not yet available)'),
    'FUN_004a3a90_': None,
}
# RW-band RVAs whose body in the exe already exists (same RVA): bind to it, generate nothing.
EXE_BODY = {
    '004d8560': ('UtilReturnOne4d8560', 'Util/UtilLeaves_ab6.cpp'),
    '004d8680': ('RwStricmp', 'Render/RwStricmp.cpp'),
    '004d86d0': ('RwStrupr', 'Render/RwStrCase.cpp'),
    '004d8700': ('RwStrlwr', 'Render/RwStrCase.cpp'),
    '004d8730': ('RwStrchr', 'Render/RwStrSearch.cpp'),
    '004d8750': ('RwStrrchr', 'Render/RwStrSearch.cpp'),
    '004d7ff0': ('Orig_004d7ff0', 'Match/PortTop_004d7ff0.cpp'),
    '004c3730': ('RwV3dTransformPoint', 'Math/RwV3dTransform.cpp'),
    '004c3880': ('RwV3dTransformVector', 'Math/RwV3dTransform.cpp'),
    # RWC-FIRST-FRAME-IM2D: callees of 0x00428450 (100% sources already compiled into the exe)
    '0042b8b0': ('ScreenWidthGet', 'Match/MatchS07_0042b8b0.cpp'),
    '0042b8c0': ('ScreenHeightGet', 'Match/MatchS07_0042b8c0.cpp'),
    # RWC-FRAME-492E90: leaves of the frame-render slice whose 100% source is already an exe-only top-down body (a second
    # ORIGTOP-ONLY copy of each is a DUP-IN-TARGET the lint does not pair)
    '00552750': ('Orig_00552750', 'Match/PortTop_00552750.cpp'),
    '005c4d30': ('Orig_005c4d30', 'Match/PortTop_005c4d30.cpp'),
}
# RWC-BIND-4C5890 (L-129 b): RW-band RVAs whose callers in other TUs bind to an exe binding entry instead of the body.
# The body is still generated as RwCore_<rva> from its source; every top-down caller (RW core and game) reaches the
# matched body through the one entry (knob check + self-test).
CALL_VIA = {
    '004c5890': ('RwCoreBind_TexDictionaryCreate_004c5890', 'RwCore/RwCore_Bind_004c5890.cpp'),
}
# RW-band RVAs with a hand body in RwCore/RwCore_Hand.cpp (no source; written from the disassembly, provisional).
HAND = {}
HAND_FILE = os.path.join(OUT, 'RwCore_Hand.cpp')
# Calling convention of a missing stub when a caller declares one (decorated names must agree).
CONV = {}


def inrw(v):
    return v in XFOLLOW or 0x4c0700 <= v < 0x516000 or 0x52df70 <= v < 0x558c50 or 0x5cb000 <= v < 0x5cc000


def load_hand():
    if not os.path.exists(HAND_FILE):
        return
    for m in re.finditer(r'^// HAND 0x([0-9a-f]{8}) (\S+)', open(HAND_FILE, encoding='utf-8').read(), re.M):
        HAND[m.group(1)] = m.group(2)


def ledger():
    led = {}
    for r in csv.DictReader(open(os.path.join(ROOT, 're', 'analysis', 'match_ledger.tsv'), encoding='utf-8'),
                            delimiter='\t'):
        k = r['rva'].lower()[2:]
        try:
            p = float(r['best_pct'] or 0)
        except ValueError:
            p = 0.0
        src = (r['source'] or '').replace('\\', '/')
        if src and os.path.exists(os.path.join(ROOT, src)) and p >= led.get(k, (-1, ''))[0]:
            led[k] = (p, src)
    return led


def all_sources():
    srcs = {}
    for p in glob.glob(os.path.join(ROOT, 're', 'match', '**', '*.c'), recursive=True):
        b = os.path.basename(p)[:-2].lower()
        if re.fullmatch(r'[0-9a-f]{8}', b):
            srcs.setdefault(b, []).append(os.path.relpath(p, ROOT).replace('\\', '/'))
    return srcs


NAME = re.compile(r'\b((?:thunk_)?FUN_([0-9a-f]{6,8})|LAB_([0-9a-f]{6,8}))\b')


def rva_of(n):
    # RWC-PLUGIN-ATTACH gap 1: 'FUN_4e4e60' (no leading zeros, re/match/rwl02/004e5d30.c) is 0x004e4e60
    m = re.search(r'([0-9a-f]{6,8})$', n)
    return m.group(1).zfill(8) if m else None


# RWC-PLUGIN-ATTACH gaps 2 and 4: the calling convention and argument bytes a source declares or defines for a name
# (`void * __stdcall FUN_004ecf53(V3 *out, V3 *in, M *m);`). A missing stop and a data-seed declaration must carry that
# convention, or the decorated names (_x@12, @x@8) do not link.
SIGRE = re.compile(r'\b(__stdcall|__fastcall|__thiscall|WINAPI)\s+\**\s*([A-Za-z_]\w*)\s*\(((?:[^;{}()]|\([^;{}()]*\))*)\)')
SIG = {}   # rva -> (convention, argument bytes)


def argbytes(params):
    params = params.strip()
    if not params or params == 'void':
        return 0
    parts, depth, cur = [], 0, ''
    for ch in params:
        if ch == ',' and depth == 0:
            parts.append(cur)
            cur = ''
            continue
        depth += ch == '('
        depth -= ch == ')'
        cur += ch
    parts.append(cur)
    return sum(8 if (('double' in q or '__int64' in q or 'long long' in q) and '*' not in q) else 4 for q in parts)


def note_sigs(body, own):
    # own: {defined name: rva}; other names resolve through NAMED / rva_of
    for m in SIGRE.finditer(body):
        conv = '__stdcall' if m.group(1) == 'WINAPI' else m.group(1)
        n = m.group(2)
        r = own.get(n) or NAMED.get(n) or (rva_of(n) if NAME.fullmatch(n) else None)
        if r:
            SIG.setdefault(r, (conv, argbytes(m.group(3))))


# RWC-PLUGIN-ATTACH gap 3: callees named in sources by their RW name (RpAtomicRegisterPlugin in 0x0052e310 / 0x00534a80
# / 0x0053d0b0 / 0x0053eaa0). Built from the `// MATCH: 0x<rva> <Name>` labels with no hex in the name; leading-underscore
# labels, Direct3DCreate9 (hand body bound by RVA) and the _rwcseg labels (forced missing) are left out.
NAMED = {}


def load_named():
    for p in glob.glob(os.path.join(ROOT, 're', 'match', '**', '*.c'), recursive=True):
        for m in MATCHLINE.finditer(open(p, encoding='utf-8', errors='replace').read()):
            r, n = m.group(1), m.group(2)
            if n.startswith('_') or re.search(r'[0-9a-f]{6,8}', n) or n == 'Direct3DCreate9':
                continue
            if 0x5cb000 <= int(r, 16) < 0x5cc000:
                continue
            NAMED.setdefault(n, r)
    # RW API names a source calls by name with no MATCH label of that name (RpWorldForAllWorldSectors in
    # re/match/*/00538bd0.c): hooks.csv names starting Rw / Rp / Rt
    for row in csv.DictReader(open(os.path.join(ROOT, 'hooks.csv'), encoding='utf-8', errors='replace')):
        n = (row.get('name') or '').strip()
        if re.fullmatch(r'R[wpt][A-Z]\w+', n) and not re.search(r'[0-9a-f]{6,8}', n):
            NAMED.setdefault(n, row['rva'].lower()[2:].zfill(8))
MATCHLINE = re.compile(r'^// MATCH: 0x([0-9a-f]{8}) (\S+)', re.M)
# 004c7a70 has no ledger row yet (RWC-DRIVER); its source is the RWC-DRIVER one.
PIN = {'004c7a70': 're/match/rwd01/004c7a70.c'}
PCT = {'004c7a70': 93.2,
       # RWC-FIRST-FRAME-IM2D: re/match/s21/004d7100.c now returns int 1 (100%, relocheck OK); the ledger row says 21.4
       '004d7100': 100.0}


DEFN = re.compile(r'\b([A-Za-z_]\w*)\s*\(([^;{}()]|\([^;{}()]*\))*\)\s*\{')
FUNDEF = re.compile(r'^[\w \t\*]*?\b((?:thunk_)?FUN_([0-9a-f]{8}))\s*\(([^;{}()]|\([^;{}()]*\))*\)\s*\{', re.M)
# _rwcseg 0x005cb000..0x005cc000: SSE routines the original carries as hand asm; their re/match sources are raw g2s
# output at 6-24% (rwl06 / s28) with register-argument signatures. Treated as missing (a call stops the boot).
FORCE_MISSING = {'005cb000', '005cb07f', '005cb0ef', '005cb160', '005cb2a0'}
# RWC-FRAME-492E90: missing stops of the previous generation that gained raw g2sh / rwtb sources since; those sources do
# not compile as C++ under VS2022 (C2632 on the preamble's bool / wchar_t typedefs, C2664 / C2197), so they stay stops,
# not lifted either (their new sources moved them out of the lifter's "inside a known function" refusal).
PIN_MISSING = {'004d1630', '004deb20', '005518f0'}
FORCE_MISSING |= PIN_MISSING


def defs_of(t):
    # (rva, defined identifier): the function defined after each MATCH line (its name can differ from the label,
    # e.g. rwDeviceSystemFn under "MATCH: 0x004c7a70 _rwDeviceSystemFn"), plus FUN_<rva> definitions with no MATCH line
    body = body_text(t)
    out = []
    for m in MATCHLINE.finditer(t):
        # RWC-PLUGIN-ATTACH: when the label itself is defined in the file, it is the function (re/match/s24/00550b00.c
        # has the inline helper GetFS between "MATCH: 0x00550b00 FUN_00550b00" and FUN_00550b00's definition)
        if re.search(r'^[\w \t\*]*?\b%s\s*\(([^;{}()]|\([^;{}()]*\))*\)\s*\{' % re.escape(m.group(2)), body, re.M):
            out.append((m.group(1), m.group(2)))
            continue
        d = DEFN.search(t, m.end())
        out.append((m.group(1), d.group(1) if d else m.group(2)))
    seen = {n for _, n in out}
    for m in FUNDEF.finditer(body):
        if m.group(1) not in seen:
            out.append((m.group(2), m.group(1)))
            seen.add(m.group(1))
    return out


def body_text(t):
    # drop comments so names in prose are not callees
    t = re.sub(r'/\*.*?\*/', ' ', t, flags=re.S)
    t = re.sub(r'//[^\n]*', ' ', t)
    # run 40: the port compiles sources as C++, so keep only the C++ branch of `#ifdef __cplusplus .. #else .. #endif`
    # (re/match/rwl01/004c1d30.c defines a C stand-in of 0x004c1a70 in the #else branch and only declares it for C++)
    t = re.sub(r'#ifdef\s+__cplusplus\b(.*?)#else\b.*?#endif', r'\1', t, flags=re.S)
    return re.sub(r'#ifndef\s+__cplusplus\b.*?#endif', ' ', t, flags=re.S)


def anchor_image():
    d = open(ANCHOR, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
    s0 = pe + 24 + opt
    secs = []
    for i in range(nsec):
        s = s0 + i * 40
        vsz, va, rsz, roff = struct.unpack_from('<IIII', d, s + 8)
        secs.append((base + va, vsz, roff, rsz))

    def read(va, n):
        for sva, vsz, roff, rsz in secs:
            if sva <= va and va + n <= sva + max(vsz, rsz):
                o = va - sva
                b = d[roff + o: roff + min(o + n, rsz)] if o < rsz else b''
                return b + b'\0' * (n - len(b))
        return None
    return read


def main(check):
    load_hand()
    load_named()
    led = ledger()
    srcs = all_sources()
    read = anchor_image()

    def pick0(a):
        if a in PIN:
            return PIN[a]
        if a in led:
            return led[a][1]
        s = srcs.get(a)
        return s[0] if s else None

    # RWC-FRAME-492E90: an RVA the previous generation lifted (RwCore_Bodies.tsv kind lift) stays lifted. Since
    # RWC-FIRST-FRAME-IM2D the match lanes added sub-100% sources for 156 of them; taking those would swap verbatim
    # transcriptions for NOT-PROVEN sources outside this row's scope. A later row may unpin them on evidence.
    pin_lift = set()
    _bt = os.path.join(HERE, 'RwCore_Bodies.tsv')
    if os.path.exists(_bt):
        for _r in csv.DictReader(open(_bt, encoding='utf-8'), delimiter='\t'):
            if _r['kind'] == 'lift':
                pin_lift.add(_r['rva'][2:].lower())

    def pick(a):
        if a in pin_lift:
            return None
        s = pick0(a)
        return None if s in LIFT_INSTEAD else s

    # code pointers in the seeded data ranges
    fixups = []
    strict_starts = {int(k, 16) for k in list(led) + list(srcs)}
    extents0 = []
    for ln in open(os.path.join(ROOT, 're', 'analysis', 'function_inventory.csv'), encoding='utf-8'):
        f = ln.strip().split(',')
        if re.fullmatch(r'[0-9a-f]{8}', f[0]):
            strict_starts.add(int(f[0], 16))
            if len(f) >= 3 and f[2].isdigit():
                extents0.append((int(f[0], 16), int(f[0], 16) + int(f[2])))

    # RWC-PLUGIN-ATTACH run 40: code pointers Ghidra never made functions. The RxNodeDefinition records at
    # 0x00623c30 .. (nodeD3D9SkinAtomicAllInOne.csl etc.) hold body / init callbacks reached only through data
    # (0x00623c40 -> 0x00532770, called by 0x004d4380 from 0x005327d0: the run-40 boot AV at EIP 0x00532770).
    # None is in function_inventory / match_ledger / re/match. A strict-range dword also becomes a fix-up when its
    # target is 16-byte aligned, the byte before it is MSVC padding or a return (0x90 / 0xcc / 0xc3), and it lies
    # inside no known function body, and its low 16 bits are not zero. The packed 16-bit pairs of the d3dx blocks fail
    # at least one of these (0x0061b8d0 = 0x004c0000 sits in a table of pairs 0x0070007f 0x00720071 .., so a value
    # whose low half is 0 is refused). CODEPTR_RULE.tsv lists every accepted and refused candidate.
    def padded_start(v):
        if v % 16 or not v & 0xffff or any(lo < v < hi for lo, hi in extents0):
            return False
        p = read(v - 1, 1)
        return p is not None and p[0] in (0x90, 0xcc, 0xc3)

    rule_rows = []
    for lo, hi in DATA_RANGES + DATA_RANGES_STRICT:
        b = read(lo, hi - lo)
        strict = (lo, hi) in DATA_RANGES_STRICT
        for o in range(0, hi - lo, 4):
            v = struct.unpack_from('<I', b, o)[0]
            if not 0x401000 <= v < 0x5cc000:
                continue
            if not strict or v in strict_starts:
                fixups.append((lo + o, '%08x' % v))
            elif padded_start(v):
                fixups.append((lo + o, '%08x' % v))
                rule_rows.append('0x%08x\t0x%08x\taccepted' % (lo + o, v))
            else:
                rule_rows.append('0x%08x\t0x%08x\trefused' % (lo + o, v))
    if not check:
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'CODEPTR_RULE.tsv'), 'w',
                  encoding='utf-8', newline='\n') as fh:
            fh.write('slot\tvalue\tverdict\n' + '\n'.join(rule_rows) + '\n')
    # RWC_EXTRA_ROOTS (sizing only, with --check): extra closure roots, comma-separated 8-digit hex RVAs
    extra = [x.strip().lower() for x in os.environ.get('RWC_EXTRA_ROOTS', '').split(',') if x.strip()]
    # RWC-PLUGIN-ATTACH run 40 (MASHED_RWC_FRAME): every RW-band RVA a librw binding (LibRw/*.cpp MashedRw_<Name>_<rva>)
    # stands for is a root, so route_bindings.py can send the binding to its core body under the frame knob.
    for f in glob.glob(os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'LibRw', '*.cpp')):
        for m in re.finditer(r'MashedRw_\w+?_(00[0-9a-f]{6})\(', open(f, encoding='utf-8', errors='replace').read()):
            if inrw(int(m.group(1), 16)) and m.group(1) not in extra and m.group(1) not in EXE_BODY:
                extra.append(m.group(1))
    roots = list(ROOTS) + extra + [t for _, t in fixups]

    tu_of = {}      # rva -> source that defines it
    defined_by = {}  # source -> [(rva, name)]
    refs = {}       # source -> set of names
    order = []
    seen = set()
    st = list(roots)
    missing, ext_unbound = [], {}
    rdata = set()

    # RWC-PLUGIN-ATTACH: function starts and extents known to the project, for the lifter (lift_rwcore.py)
    known_starts, inside = set(), []
    for ln in open(os.path.join(ROOT, 're', 'analysis', 'function_inventory.csv'), encoding='utf-8'):
        f = ln.strip().split(',')
        if len(f) >= 3 and re.fullmatch(r'[0-9a-f]{8}', f[0]):
            known_starts.add(int(f[0], 16))
            if f[2].isdigit():
                inside.append((int(f[0], 16), int(f[0], 16) + int(f[2])))
    for r in csv.DictReader(open(os.path.join(ROOT, 're', 'analysis', 'match_ledger.tsv'), encoding='utf-8'),
                            delimiter='\t'):
        v = int(r['rva'], 16)
        known_starts.add(v)
        if (r['bytes_n'] or '').isdigit():
            inside.append((v, v + int(r['bytes_n'])))
    known_starts |= {int(k, 16) for k in list(srcs) + list(HAND) + list(EXE_BODY)}
    known_starts |= {int(t, 16) for _, t in fixups}
    # run 40: an extent ends at the next known function start. match_ledger's bytes_n is not always the original's
    # extent: 0x004c2620 lists 1425 bytes (function_inventory 1040, ending at the start 0x004c2a30), which put the
    # camera begin / end update callbacks 0x004c2bb0 / 0x004c2b80 (ret + nop before each, both open with
    # mov eax, [0x007d3ff8]) "inside" it, so they were refused as labels.
    starts_sorted = sorted(known_starts)
    import bisect

    def clip(lo, hi):
        i = bisect.bisect_right(starts_sorted, lo)
        return (lo, min(hi, starts_sorted[i])) if i < len(starts_sorted) else (lo, hi)
    inside = [clip(lo, hi) for lo, hi in inside]

    def mid_function(v):
        return any(lo < v < hi for lo, hi in inside)

    import pefile   # run 40: import slots for lift_rwcore (calls through the IAT)
    _pe = pefile.PE(ANCHOR, fast_load=True)
    _pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_IMPORT']])
    lift_rwcore.IAT.clear()
    lift_rwcore.IAT_USED.clear()
    for _e in _pe.DIRECTORY_ENTRY_IMPORT:
        for _i in _e.imports:
            if _i.name:
                lift_rwcore.IAT[_i.address] = (_e.dll.decode(), _i.name.decode())
    lifted = {}      # rva -> (text with @@SYM_<rva>@@ placeholders, refs)
    lift_refused = {}

    def lift_resolve(r):
        if 'FUN_' + r in EXT:
            return EXT['FUN_' + r][0]
        if inrw(int(r, 16)):
            st.append(r)
            return '@@SYM_%s@@' % r
        return None

    while st:
        a = st.pop()
        if a in seen:
            continue
        seen.add(a)
        if a in EXE_BODY or a in HAND or ('FUN_' + a) in EXT:
            continue
        src = pick(a) if a not in FORCE_MISSING and a not in FRAME_STOP else None
        if not src:
            if not LIFT or mid_function(int(a, 16)) or a in FRAME_STOP or a in PIN_MISSING:
                if LIFT and a not in FRAME_STOP and a not in PIN_MISSING:
                    lift_refused[a] = 'inside a known function (a label, not a function start)'
                missing.append(a)
                continue
            try:
                txt, lrefs, lrd = lift_rwcore.lift(read, int(a, 16), '@@NAME_%s@@' % a, known_starts, lift_resolve,
                                                   ('@@CONV_%s@@' % a, 0))
            except lift_rwcore.Refused as e:
                lift_refused[a] = str(e)
                missing.append(a)
                continue
            lifted[a] = txt
            rdata |= lrd
            continue
        if src not in defined_by:
            t = open(os.path.join(ROOT, src), encoding='utf-8', errors='replace').read()
            defined_by[src] = defs_of(t)
            body = body_text(t)
            refs[src] = set(m.group(1) for m in NAME.finditer(body))
            refs[src] |= {w for w in re.findall(r'[A-Za-z_][A-Za-z0-9_]*', body) if w in NAMED or w in EXT}
            note_sigs(body, {n_: r_ for r_, n_ in defined_by[src]})
            for m in re.finditer(r'\b(?:DAT|PTR|s|u|_DAT|PTR_DAT)_\w*?([0-9a-f]{8})\b|\b0x00(5[c-e][0-9a-f]{4})\b', body):
                v = int(m.group(1) or m.group(2), 16)
                if 0x5cc000 <= v < 0x5ea000:
                    rdata.add(v)
            order.append(src)
            for r_, n_ in defined_by[src]:
                tu_of.setdefault(r_, src)
            own = {n for _, n in defined_by[src]}
            for n in refs[src]:
                if n in own or (n in EXT and not n.startswith('FUN_')):
                    continue
                r = NAMED.get(n) or rva_of(n)
                if inrw(int(r, 16)):
                    st.append(r)
                elif (n not in EXT and ('FUN_' + r) not in EXT and r not in EXE_BODY
                      and any(int(x, 16) in FRAME_SLICE for x, _ in defined_by[src])):
                    # RWC-FRAME-492E90: game callee of a slice source, outside the slice: a logged stop
                    FRAME_STOP.add(r)
                    XFOLLOW.add(int(r, 16))
                    st.append(r)
                elif n not in EXT and ('FUN_' + r) not in EXT:
                    ext_unbound.setdefault(n, []).append(src)
        tu_of.setdefault(a, src)

    # a function defined in more than one compiled source belongs to its own picked source when that one is compiled
    for r_ in list(tu_of):
        own_src = pick(r_)
        if own_src in defined_by and any(x == r_ for x, _ in defined_by[own_src]):
            tu_of[r_] = own_src

    def sym_of(r):
        if ('FUN_' + r) in EXT and EXT['FUN_' + r]:
            return EXT['FUN_' + r][0]
        if r in EXE_BODY:
            return EXE_BODY[r][0]
        if r in HAND:
            return HAND[r]
        if r in tu_of:
            return 'RwCore_' + r
        if r in lifted:
            return 'RwCoreLift_' + r
        return 'RwCoreMissing_' + r

    rows = []
    for a in sorted(seen):
        if a in EXE_BODY:
            rows.append((a, 'exe', EXE_BODY[a][1], '', EXE_BODY[a][0]))
        elif ('FUN_' + a) in EXT and EXT['FUN_' + a]:
            rows.append((a, 'exe', EXT['FUN_' + a][1], '', EXT['FUN_' + a][0]))
        elif a in HAND:
            rows.append((a, 'hand', 'RwCore/RwCore_Hand.cpp', '', HAND[a]))
        elif a in tu_of:
            src = tu_of[a]
            p = PCT.get(a, led.get(a, (None,))[0])
            kind = 'match100' if p == 100.0 else 'provisional'
            rows.append((a, kind, src, '' if p is None else '%.1f' % p, 'RwCore_' + a))
        elif a in lifted:
            rows.append((a, 'lift', 'RwCore/RwCore_Lifted_NN.cpp', '', 'RwCoreLift_' + a))
        elif a in FRAME_STOP:
            rows.append((a, 'stop', pick0(a) or '', '', 'RwCoreMissing_' + a))   # RWC-FRAME-492E90: outside the slice
        else:
            rows.append((a, 'missing', '', '', 'RwCoreMissing_' + a))

    print('closure %d: %s' % (len(rows), ', '.join('%s %d' % (k, sum(1 for r in rows if r[1] == k))
                                                   for k in ('match100', 'provisional', 'hand', 'lift', 'exe', 'missing', 'stop'))))
    print('sources %d, fixups %d, rdata refs %d' % (len(order), len(fixups), len(rdata)))
    if ext_unbound:
        print('UNBOUND non-RW:', {k: sorted(set(v))[:3] for k, v in sorted(ext_unbound.items())})
    if check:
        return

    os.makedirs(OUT, exist_ok=True)
    # remove old generated TUs
    for f in glob.glob(os.path.join(OUT, 'RwCore_*.cpp')):
        if re.search(r'RwCore_[0-9a-f]{8}\.cpp$', f):
            os.remove(f)
    tus = []
    for src in order:
        own = defined_by[src]
        if not own:
            print('WARN no MATCH line in', src)
            continue
        mine = [r_ for r_, _ in own if tu_of.get(r_) == src]
        main_rva = mine[0] if mine else own[0][0]
        lines = []
        ownmap = {n: r_ for r_, n in own}
        names = set(refs[src]) | set(ownmap)
        for n in sorted(names):
            r = ownmap.get(n) or NAMED.get(n) or rva_of(n)
            if n in ownmap and tu_of.get(r) != src:
                lines.append('#define %s RwCoreCopy_%s_%s   // second copy of 0x%s in this source; the body is in %s'
                             % (n, main_rva, r, r, tu_of.get(r)))
                continue
            if n in EXT:
                lines.append('#define %s %s   // %s' % (n, EXT[n][0], EXT[n][1]))
                continue
            if n not in ownmap and not inrw(int(r, 16)):   # own definitions always bind (fix-up roots)
                continue
            if r in CALL_VIA and tu_of.get(r) != src:
                lines.append('#define %s %s   // binding entry, %s' % (n, CALL_VIA[r][0], CALL_VIA[r][1]))
                continue
            lines.append('#define %s %s' % (n, sym_of(r)))
        kinds = ', '.join('0x%s %s' % (r, next((x[1] for x in rows if x[0] == r), '?')) for r, _ in own)
        txt = ('// RWC-CORE-RUN (L-124 (C) step 2): exe-only RW core body, generated by '
               'verify/rwccorerun_20261111/gen_rwcore_tus.py.\n'
               '// Source re/match/%s compiled verbatim as C++ (MATCH_PORT): %s. Runs only under MASHED_ORIG_TOP=1 +\n'
               '// MASHED_RWC_CORE=1 (RwCore/RwCore_Knob.cpp). A provisional body is a NOT-PROVEN source (L-126 (6),\n'
               '// verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv), never C4 by this status.\n'
               '// RVA-LINT: ORIGTOP-ONLY\n'
               '#ifdef MASHED_STANDALONE\n#define MATCH_PORT 1\n%s\nextern "C" {\n#include "../../../../%s"\n}\n'
               '#endif // MASHED_STANDALONE\n') % (src[len('re/match/'):], kinds, '\n'.join(lines), src)
        fn = 'RwCore_%s.cpp' % main_rva
        open(os.path.join(OUT, fn), 'w', encoding='utf-8', newline='\n').write(txt)
        tus.append('RwCore\\' + fn)

    # RWC-PLUGIN-ATTACH: lifted bodies (lift_rwcore.py), placeholders resolved now that every kind is known
    def decl_sym(r):
        s_ = sym_of(r)
        if r in EXE_BODY or r in HAND or r not in SIG:
            return 'extern "C" int %s();' % s_
        cc, nb = SIG[r]
        return 'extern "C" int %s %s(%s);' % (cc, s_, ', '.join('int' for _ in range(nb // 4)) or 'void')
    lifted_ok = {r: t for r, t in lifted.items() if r not in tu_of}
    ltxt, lrefs = [], set()
    for r in sorted(lifted_ok):
        t = lifted_ok[r]
        cc, nb = SIG.get(r, ('', 0))
        t = t.replace('@@NAME_%s@@' % r, 'RwCoreLift_' + r)
        t = t.replace('int @@CONV_%s@@ ' % r, 'int %s' % (cc + ' ' if cc else ''))
        t = t.replace('(void) {', '(%s) {' % (', '.join('int' for _ in range(nb // 4)) or 'void'), 1)
        for m in re.finditer(r'@@SYM_([0-9a-f]{8})@@', t):
            lrefs.add(m.group(1))
        t = re.sub(r'@@SYM_([0-9a-f]{8})@@', lambda m: sym_of(m.group(1)), t)
        ltxt.append(t)
    ext_syms = {v[0] for v in EXT.values() if v}
    ext_used = sorted(set(w for w in re.findall(r'call (\w+)|offset (\w+)', '\n'.join(ltxt)) for w in w if w in ext_syms))
    lines = ['// RWC-PLUGIN-ATTACH (L-124 (C) step 3): verbatim naked-asm transcriptions of closure RVAs with no re/match',
             '// source, generated by verify/rwccorerun_20261111/gen_rwcore_tus.py with lift_rwcore.py from the anchored',
             '// MASHED.exe.unpatched. Hand bodies from the disassembly, provisional, MASHED_ORIG_TOP + MASHED_RWC_CORE only,',
             '// never C4 by this status (verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv).',
             '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#pragma warning(disable: 4035 4100)']
    # split into RwCore_Lifted_NN.cpp of LIFT_CHUNK bodies each (one 2.8 MB file is a merge hazard); each chunk declares
    # only the symbols it uses
    sym2rva = {sym_of(r): r for r in lrefs | set(lifted_ok)}
    for f in glob.glob(os.path.join(OUT, 'RwCore_Lifted*.cpp')):
        os.remove(f)
    lifted_files = []
    # run 40: the import slots lifted bodies call (RwCoreIat_<slot> = the exe's own import of that name)
    if lift_rwcore.IAT_USED:
        iat = ['// RWC-PLUGIN-ATTACH run 40: import slots of the anchored MASHED.exe that lifted RW core bodies call through',
               '// (call dword ptr [RwCoreIat_<slot>]). Each points at the import of the same name in this exe: the OS is the',
               '// declared platform boundary (ROADMAP v5 item 4). Generated by verify/rwccorerun_20261111/gen_rwcore_tus.py.',
               '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#include <windows.h>', 'extern "C" {']
        for slot in sorted(lift_rwcore.IAT_USED):
            dll, nm = lift_rwcore.IAT[slot]
            iat.append('void* RwCoreIat_%08x = reinterpret_cast<void*>(&::%s);   // 0x%08x %s' % (slot, nm, slot, dll))
        iat += ['}', '#endif // MASHED_STANDALONE', '']
        open(os.path.join(OUT, 'RwCore_Iat.cpp'), 'w', encoding='utf-8', newline='\n').write('\n'.join(iat))
        lifted_files.append('RwCore\\RwCore_Iat.cpp')
    for ci in range(0, len(ltxt), LIFT_CHUNK):
        chunk = ltxt[ci:ci + LIFT_CHUNK]
        used = set(re.findall(r'(?:call|jmp|offset) ([A-Za-z_]\w*)', '\n'.join(chunk)))
        used |= set(re.findall(r'naked\) int (?:__\w+ )?(\w+)\(', '\n'.join(chunk)))
        decls = sorted(set(decl_sym(sym2rva[u]) for u in used if u in sym2rva))
        decls += ['extern "C" int %s();' % e for e in ext_used if e in used]
        decls += ['extern "C" void* RwCoreIat_%s;' % x
                  for x in sorted(set(re.findall(r'RwCoreIat_([0-9a-f]{8})', '\n'.join(chunk))))]
        fn = 'RwCore_Lifted_%02d.cpp' % (ci // LIFT_CHUNK)
        open(os.path.join(OUT, fn), 'w', encoding='utf-8', newline='\n').write(
            '\n'.join(lines + decls + [''] + chunk + ['#endif // MASHED_STANDALONE', '']))
        lifted_files.append('RwCore\\' + fn)
    with open(os.path.join(HERE, 'LIFT_REFUSED.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('rva\treason\n')
        for r in sorted(lift_refused):
            f.write('0x%s\t%s\n' % (r, lift_refused[r]))

    # missing stubs
    miss = sorted(r for r in seen if r not in EXE_BODY and r not in HAND and r not in tu_of and r not in lifted_ok
                  and ('FUN_' + r) not in EXT)
    out = ['// RWC-CORE-RUN: logged stops for RW-band RVAs in the closure with no source and no hand body. Generated by',
           '// verify/rwccorerun_20261111/gen_rwcore_tus.py. Each logs its RVA and ends the process with exit code',
           '// 0x52570000 | index, so a knob boot names the first missing body. Not bodies of these RVAs.',
           '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', 'extern "C" void RwCore_Stop(unsigned rva, unsigned code);',
           'extern "C" {']
    for i, r in enumerate(miss):
        cc, nb = SIG.get(r, ('', 0))
        args = ', '.join('int a%d' % k for k in range(nb // 4))
        out.append('int %s%sRwCoreMissing_%s(%s) { RwCore_Stop(0x%su, 0x52570000u | %du); return 0; }'
                   % (cc, ' ' if cc else '', r, args, r, i + 1))
    out += ['}', '#endif // MASHED_STANDALONE', '']
    open(os.path.join(OUT, 'RwCore_Missing.cpp'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out))

    # data seed
    inc = ['// RWC-CORE-RUN: generated by verify/rwccorerun_20261111/gen_rwcore_tus.py from the anchored',
           '// MASHED.exe.unpatched. RWC_SEED(va, n, "bytes") / RWC_FIX(va, symbol, original target).']
    for lo0, hi0 in DATA_RANGES + DATA_RANGES_STRICT:
        for lo in range(lo0, hi0, 0x800):   # 2 KB per literal: MSVC C2026 caps one string literal
            hi = min(hi0, lo + 0x800)
            inc.append('RWC_SEED(0x%08x, %d, "%s")' % (lo, hi - lo, ''.join('\\x%02x' % c for c in read(lo, hi - lo))))
    # .rdata windows, merged
    wins = []
    for v in sorted(rdata):
        lo, hi = v & ~3, (v & ~3) + RDATA_WINDOW
        if wins and lo <= wins[-1][1]:
            wins[-1][1] = max(wins[-1][1], hi)
        else:
            wins.append([lo, hi])
    for lo, hi in wins:
        hi = min(hi, 0x5ea000)
        inc.append('RWC_SEED(0x%08x, %d, "%s")' % (lo, hi - lo, ''.join('\\x%02x' % c for c in read(lo, hi - lo))))
    for va, t in fixups:
        inc.append('RWC_FIX(0x%08x, %s, 0x%s)' % (va, sym_of(t), t))
    open(os.path.join(OUT, 'RwCore_DataSeed.inc'), 'w', encoding='utf-8', newline='\n').write('\n'.join(inc) + '\n')
    # symbols the fix-ups name, declared for the knob TU
    # (with the convention and argument bytes the sources give the target, so a __stdcall body links: gap 4)
    def decl_of(t):
        s_ = sym_of(t)
        if t in EXE_BODY or t in HAND or t not in SIG:
            return 'extern "C" int %s();' % s_
        cc, nb = SIG[t]
        return 'extern "C" int %s %s(%s);' % (cc, s_, ', '.join('int' for _ in range(nb // 4)) or 'void')
    decl = sorted(set(decl_of(t) for _, t in fixups))
    open(os.path.join(OUT, 'RwCore_DataSeedDecl.inc'), 'w', encoding='utf-8', newline='\n').write(
        '\n'.join(decl) + '\n')

    with open(os.path.join(HERE, 'RwCore_Bodies.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('rva\tkind\tsource\tbest_pct\texe_symbol\n')
        for r in rows:
            f.write('0x%s\t%s\t%s\t%s\t%s\n' % r)
    with open(os.path.join(HERE, 'PROVISIONAL_RWC.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('rva\tsource\tbest_pct\texe_symbol\tpath\tnote\n')
        for r in rows:
            if r[1] in ('provisional', 'hand', 'lift'):
                f.write('0x%s\t%s\t%s\t%s\tMASHED_ORIG_TOP + MASHED_RWC_CORE\t%s\n' % (
                    r[0], r[2], r[3], r[4], 'NOT-PROVEN source (L-126 (6)), knob path only, never C4 by this status'
                    if r[1] == 'provisional' else 'hand body from the disassembly, knob path only' if r[1] == 'hand'
                    else 'lifted naked-asm transcription (lift_rwcore.py), knob path only, never C4 by this status'))

    # exe_sources.rsp generated block
    rsp = open(RSP, encoding='utf-8').read().splitlines()
    b0, b1 = '# RWC-CORE-RUN generated begin', '# RWC-CORE-RUN generated end'
    if b0 in rsp:
        i, j = rsp.index(b0), rsp.index(b1)
        rsp = rsp[:i] + rsp[j + 1:]
    fixed = ['RwCore\\RwCore_Knob.cpp', 'RwCore\\RwCore_Crt.cpp', 'RwCore\\RwCore_Missing.cpp']
    if HAND:
        fixed.append('RwCore\\RwCore_Hand.cpp')
    fixed += lifted_files
    fixed += sorted(set('RwCore\\' + os.path.basename(v[1]) for v in CALL_VIA.values()))
    rsp += [b0] + ['"%s"' % t for t in fixed + tus] + [b1]
    open(RSP, 'w', encoding='utf-8', newline='').write('\r\n'.join(rsp) + '\r\n')
    print('wrote %d TUs, %d lifted, %d missing stubs (%d lift refusals, LIFT_REFUSED.tsv)'
          % (len(tus), len(lifted_ok), len(miss), len(lift_refused)))


if __name__ == '__main__':
    main('--check' in sys.argv)
