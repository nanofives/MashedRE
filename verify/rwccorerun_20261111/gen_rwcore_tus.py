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

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
OUT = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'RwCore')
RSP = os.path.join(ROOT, 'mashedmod', 'exe_sources.rsp')
ANCHOR = os.environ.get('MASHED_UNPATCHED', os.path.normpath(os.path.join(ROOT, '..', '..', 'original',
                                                                           'MASHED.exe.unpatched')))

ROOTS = [
    '004c32b0', '004c30b0', '004c2fb0', '004c2f60', '004c3040', '004c3270',   # RwEngineInit/Open/Start/Stop/Close/Term
    '004c9f50', '004c9eb0', '004c9f60',                                        # driver setters 0x00493710 calls
]
# Not roots in this row: 0x00493710's calls after RwEngineStart (GetCurrentMode 0x004c2f00, GetModeInfo 0x004c2ed0,
# 0x004cbc60..0x004cbc90, RwTexDictionarySetCurrent 0x004c5c80) are not reached, since the knob ends the boot at Start.
# Data ranges of the RW core (file-backed, seeded from the anchored binary under the knob). Every dword in them whose
# value is a code address becomes a fix-up and its target joins the closure.
DATA_RANGES = [(0x00617f00, 0x00618600)]
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
    return 0x4c0700 <= v < 0x516000 or 0x52df70 <= v < 0x558c50 or 0x5cb000 <= v < 0x5cc000


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


NAME = re.compile(r'\b((?:thunk_)?FUN_([0-9a-f]{8})|LAB_([0-9a-f]{8}))\b')
MATCHLINE = re.compile(r'^// MATCH: 0x([0-9a-f]{8}) (\S+)', re.M)
# 004c7a70 has no ledger row yet (RWC-DRIVER); its source is the RWC-DRIVER one.
PIN = {'004c7a70': 're/match/rwd01/004c7a70.c'}
PCT = {'004c7a70': 93.2}


DEFN = re.compile(r'\b([A-Za-z_]\w*)\s*\(([^;{}()]|\([^;{}()]*\))*\)\s*\{')
FUNDEF = re.compile(r'^[\w \t\*]*?\b((?:thunk_)?FUN_([0-9a-f]{8}))\s*\(([^;{}()]|\([^;{}()]*\))*\)\s*\{', re.M)
# _rwcseg 0x005cb000..0x005cc000: SSE routines the original carries as hand asm; their re/match sources are raw g2s
# output at 6-24% (rwl06 / s28) with register-argument signatures. Treated as missing (a call stops the boot).
FORCE_MISSING = {'005cb000', '005cb07f', '005cb0ef', '005cb160', '005cb2a0'}


def defs_of(t):
    # (rva, defined identifier): the function defined after each MATCH line (its name can differ from the label,
    # e.g. rwDeviceSystemFn under "MATCH: 0x004c7a70 _rwDeviceSystemFn"), plus FUN_<rva> definitions with no MATCH line
    body = body_text(t)
    out = []
    for m in MATCHLINE.finditer(t):
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
    return re.sub(r'//[^\n]*', ' ', t)


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
    led = ledger()
    srcs = all_sources()
    read = anchor_image()

    def pick(a):
        if a in PIN:
            return PIN[a]
        if a in led:
            return led[a][1]
        s = srcs.get(a)
        return s[0] if s else None

    # code pointers in the seeded data ranges
    fixups = []
    for lo, hi in DATA_RANGES:
        b = read(lo, hi - lo)
        for o in range(0, hi - lo, 4):
            v = struct.unpack_from('<I', b, o)[0]
            if 0x401000 <= v < 0x5cc000:
                fixups.append((lo + o, '%08x' % v))
    roots = list(ROOTS) + [t for _, t in fixups]

    tu_of = {}      # rva -> source that defines it
    defined_by = {}  # source -> [(rva, name)]
    refs = {}       # source -> set of names
    order = []
    seen = set()
    st = list(roots)
    missing, ext_unbound = [], {}
    rdata = set()
    while st:
        a = st.pop()
        if a in seen:
            continue
        seen.add(a)
        if a in EXE_BODY or a in HAND:
            continue
        src = None if a in FORCE_MISSING else pick(a)
        if not src:
            missing.append(a)
            continue
        if src not in defined_by:
            t = open(os.path.join(ROOT, src), encoding='utf-8', errors='replace').read()
            defined_by[src] = defs_of(t)
            body = body_text(t)
            refs[src] = set(m.group(1) for m in NAME.finditer(body))
            for m in re.finditer(r'\b(?:DAT|PTR|s|u|_DAT|PTR_DAT)_\w*?([0-9a-f]{8})\b|\b0x00(5[c-e][0-9a-f]{4})\b', body):
                v = int(m.group(1) or m.group(2), 16)
                if 0x5cc000 <= v < 0x5ea000:
                    rdata.add(v)
            order.append(src)
            for r_, n_ in defined_by[src]:
                tu_of.setdefault(r_, src)
            own = {n for _, n in defined_by[src]}
            for n in refs[src]:
                if n in own:
                    continue
                r = n[-8:]
                if inrw(int(r, 16)):
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
        if r in EXE_BODY:
            return EXE_BODY[r][0]
        if r in HAND:
            return HAND[r]
        if r in tu_of:
            return 'RwCore_' + r
        return 'RwCoreMissing_' + r

    rows = []
    for a in sorted(seen):
        if a in EXE_BODY:
            rows.append((a, 'exe', EXE_BODY[a][1], '', EXE_BODY[a][0]))
        elif a in HAND:
            rows.append((a, 'hand', 'RwCore/RwCore_Hand.cpp', '', HAND[a]))
        elif a in tu_of:
            src = tu_of[a]
            p = PCT.get(a, led.get(a, (None,))[0])
            kind = 'match100' if p == 100.0 else 'provisional'
            rows.append((a, kind, src, '' if p is None else '%.1f' % p, 'RwCore_' + a))
        else:
            rows.append((a, 'missing', '', '', 'RwCoreMissing_' + a))

    print('closure %d: %s' % (len(rows), ', '.join('%s %d' % (k, sum(1 for r in rows if r[1] == k))
                                                   for k in ('match100', 'provisional', 'hand', 'exe', 'missing'))))
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
            r = ownmap.get(n, n[-8:])
            if n in ownmap and tu_of.get(r) != src:
                lines.append('#define %s RwCoreCopy_%s_%s   // second copy of 0x%s in this source; the body is in %s'
                             % (n, main_rva, r, r, tu_of.get(r)))
                continue
            if n in EXT:
                lines.append('#define %s %s   // %s' % (n, EXT[n][0], EXT[n][1]))
                continue
            if not inrw(int(r, 16)):
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

    # missing stubs
    miss = sorted(r for r in seen if r not in EXE_BODY and r not in HAND and r not in tu_of)
    out = ['// RWC-CORE-RUN: logged stops for RW-band RVAs in the closure with no source and no hand body. Generated by',
           '// verify/rwccorerun_20261111/gen_rwcore_tus.py. Each logs its RVA and ends the process with exit code',
           '// 0x52570000 | index, so a knob boot names the first missing body. Not bodies of these RVAs.',
           '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', 'extern "C" void RwCore_Stop(unsigned rva, unsigned code);',
           'extern "C" {']
    for i, r in enumerate(miss):
        cc = CONV.get(r, '')
        out.append('int %sRwCoreMissing_%s() { RwCore_Stop(0x%su, 0x52570000u | %du); return 0; }' % (cc, r, r, i + 1))
    out += ['}', '#endif // MASHED_STANDALONE', '']
    open(os.path.join(OUT, 'RwCore_Missing.cpp'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out))

    # data seed
    inc = ['// RWC-CORE-RUN: generated by verify/rwccorerun_20261111/gen_rwcore_tus.py from the anchored',
           '// MASHED.exe.unpatched. RWC_SEED(va, n, "bytes") / RWC_FIX(va, symbol, original target).']
    for lo, hi in DATA_RANGES:
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
    decl = sorted(set(sym_of(t) for _, t in fixups))
    open(os.path.join(OUT, 'RwCore_DataSeedDecl.inc'), 'w', encoding='utf-8', newline='\n').write(
        '\n'.join('extern "C" int %s();' % s for s in decl) + '\n')

    with open(os.path.join(HERE, 'RwCore_Bodies.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('rva\tkind\tsource\tbest_pct\texe_symbol\n')
        for r in rows:
            f.write('0x%s\t%s\t%s\t%s\t%s\n' % r)
    with open(os.path.join(HERE, 'PROVISIONAL_RWC.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('rva\tsource\tbest_pct\texe_symbol\tpath\tnote\n')
        for r in rows:
            if r[1] in ('provisional', 'hand'):
                f.write('0x%s\t%s\t%s\t%s\tMASHED_ORIG_TOP + MASHED_RWC_CORE\t%s\n' % (
                    r[0], r[2], r[3], r[4], 'NOT-PROVEN source (L-126 (6)), knob path only, never C4 by this status'
                    if r[1] == 'provisional' else 'hand body from the disassembly, knob path only'))

    # exe_sources.rsp generated block
    rsp = open(RSP, encoding='utf-8').read().splitlines()
    b0, b1 = '# RWC-CORE-RUN generated begin', '# RWC-CORE-RUN generated end'
    if b0 in rsp:
        i, j = rsp.index(b0), rsp.index(b1)
        rsp = rsp[:i] + rsp[j + 1:]
    fixed = ['RwCore\\RwCore_Knob.cpp', 'RwCore\\RwCore_Crt.cpp', 'RwCore\\RwCore_Missing.cpp']
    if HAND:
        fixed.append('RwCore\\RwCore_Hand.cpp')
    fixed += sorted(set('RwCore\\' + os.path.basename(v[1]) for v in CALL_VIA.values()))
    rsp += [b0] + ['"%s"' % t for t in fixed + tus] + [b1]
    open(RSP, 'w', encoding='utf-8', newline='').write('\r\n'.join(rsp) + '\r\n')
    print('wrote %d TUs, %d missing stubs' % (len(tus), len(miss)))


if __name__ == '__main__':
    main('--check' in sys.argv)
