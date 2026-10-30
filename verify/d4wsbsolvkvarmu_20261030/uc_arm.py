"""Arm U of PREREG_D4-WSB-SOLV-KV-ARMU.md (copy of the KV step's uc_arm.py + the 0056adb0B set
and the fistp coverage probe at 0x0056b264): offline bit-compare of the four PGS rows.

Original side: Unicorn runs MASHED.exe.unpatched's own bytes at 0x0056a7a0 / 0x0056aae0 /
0x0056ac40 / 0x0056adb0 under x87 CW 0x007f. Port side: a harness exe compiled from the
RwpVtableKV12.cpp text (extracted verbatim, or one mutant M1..M4). Same arena at 0x20000000
on both sides, compared bitwise after every call.

Usage: py -3.12 uc_arm.py [--cases N] [--adaptive N] [--big N] [--out DIR]
Prints one "[N/M] name STATUS" line per variant, writes uc_summary.tsv + uc_summary.json.
"""
import argparse, json, random, struct, subprocess, sys
from pathlib import Path

import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UcError, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP

HERE = Path(__file__).resolve().parent
WT = HERE.parents[1]
SRC = WT / 'mashedmod' / 'src' / 'mashed_re' / 'Collision' / 'RwpVtableKV12.cpp'
EXE = WT.parents[1] / 'original' / 'MASHED.exe.unpatched'   # main checkout's install
VCVARS = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
ANCHOR = 'bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e'

ARENA, ARENA_MAX = 0x20000000, 0x100000
STACK_LO, STACK_SZ = 0x30000000, 0x100000
STUB = 0x40000000
RVAS = [0x0056a7a0, 0x0056aae0, 0x0056ac40, 0x0056adb0]
NAMES = ['0056a7a0', '0056aae0', '0056ac40', '0056adb0']
SEED = 20261029

MUTANTS = {
    'M1': ('            v = _mm_add_ps(v, m0);\n            v = _mm_add_ps(v, m1);\n',
           '            v = _mm_add_ps(v, m1);\n            v = _mm_add_ps(v, m0);\n'),
    'M2': ('sum = sum + xi * xi;',
           'sum = static_cast<float>(static_cast<double>(sum) + static_cast<double>(xi) * xi);'),
    'M3': ('acc[i3] = (k53 * k55 + k54) * delta + acc[i3];',
           'acc[i3] = static_cast<float>((static_cast<double>(k53) * k55 + k54) * delta + acc[i3]);'),
    'M4': ('X87Fistp(scaled)', 'static_cast<int>(scaled)'),
}
ADB = {'0056adb0', '0056adb0A', '0056adb0B'}
MUTANT_FN = {'M1': {'0056a7a0', '0056aae0'} | ADB, 'M2': {'0056ac40'} | ADB,
             'M3': {'0056aae0'} | ADB, 'M4': set(ADB)}   # labels that execute the site

# ---------------------------------------------------------------------------- cases

def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]

def fbits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


class Arena:
    def __init__(self):
        self.buf = bytearray()

    def alloc(self, nbytes):
        while len(self.buf) % 16:
            self.buf.append(0)
        a = ARENA + len(self.buf)
        self.buf += bytes(nbytes)
        return a

    def put(self, addr, fmt, vals):
        struct.pack_into('<' + fmt * len(vals), self.buf, addr - ARENA, *vals)

    def floats(self, vals):
        a = self.alloc(4 * len(vals)); self.put(a, 'I', [fbits(v) for v in vals]); return a

    def ints(self, vals):
        a = self.alloc(4 * len(vals)); self.put(a, 'i', vals); return a


def rnd_floats(rng, n, lo=-2.0, hi=2.0):
    out = []
    for _ in range(n):
        if out and rng.random() < 0.1:
            out.append(rng.choice(out))          # repeated values: equal-lane branches
        elif rng.random() < 0.05:
            out.append(0.0)
        else:
            out.append(f32(rng.uniform(lo, hi)))
    return out


def junk(rng):
    return rng.randrange(0x10000000, 0x1fffffff) & ~3


def pgs_block(rng, ar, n):
    """The 34-word block a7a0 reads as v4..v37 (W3..W36 / w15..w48). Returns (block, acc-rows n)."""
    G = rng.randint(1, 4)
    cnt = [rng.randint(0, 3) for _ in range(G)]
    start, s = [], 0
    for c in cnt:
        start.append(s); s += c
    NB = rng.randint(1, 4)
    NJ = max(s, NB, 1)
    jac = ar.floats(rnd_floats(rng, NJ * 24, -1.0, 1.0))
    a_start = ar.ints(start)
    a_cnt = ar.ints(cnt)
    a_arow = ar.ints([rng.randrange(n) for _ in range(NJ)])
    a_pair = ar.ints([(-1 if rng.random() < 0.25 else rng.randrange(G)) for _ in range(2 * n)])
    a_blk = ar.ints([rng.randrange(NB) for _ in range(2 * n)])
    blk = [junk(rng) for _ in range(34)]
    blk[0], blk[4], blk[7], blk[16], blk[25], blk[28] = jac, a_start, a_arow, a_cnt, a_pair, a_blk
    return blk


def masks(rng, n):
    return [(-1 if rng.random() < 0.6 else 0) for _ in range(4 * n)]


def case_a7a0(rng):
    ar = Arena(); n = rng.randint(1, 6)
    acc = ar.floats(rnd_floats(rng, 4 * n))
    blk = pgs_block(rng, ar, n)
    w = [acc, junk(rng), junk(rng)] + blk + [fbits(f32(rng.uniform(-2, 2))), rng.randrange(4 * n)]
    return 0, w, ar


def case_ac40(rng):
    ar = Arena(); n = rng.choice([0] + list(range(1, 7)))
    nn = max(n, 1)
    x = ar.floats(rnd_floats(rng, 4 * nn))
    mA = ar.ints(masks(rng, nn)); mB = ar.ints(masks(rng, nn))
    outs = ar.ints([0x11111111, 0x22222222])
    thr = rng.choice([0.0, 0.05, 0.5, 1.5, -0.25])
    w = [outs, outs + 4, x, n, junk(rng), junk(rng), junk(rng), junk(rng), mA, junk(rng), junk(rng),
         mB, junk(rng), junk(rng), fbits(thr)]
    return 2, w, ar


def case_aae0(rng):
    ar = Arena(); n = rng.randint(1, 6)
    idx = rng.randrange(4 * n)
    ij = ar.ints([idx // 4, idx % 4])
    acc = ar.floats(rnd_floats(rng, 4 * n))
    flo = ar.ints([0x33333333] * (4 * n)); fhi = ar.ints([0x44444444] * (4 * n))
    val = ar.floats(rnd_floats(rng, 4 * n))
    blk = pgs_block(rng, ar, n)
    lo = f32(-abs(rng.uniform(0, 2))); hi = f32(abs(rng.uniform(0, 2)))
    if rng.random() < 0.15: lo, hi = hi, lo
    x = rng.choice([0.0, f32(rng.uniform(-2, 2))])
    tgt = rng.choice([lo, hi, f32(rng.uniform(-3, 3))]) if x == 0.0 else f32(rng.uniform(-3, 3))
    k = rnd_floats(rng, 3, -1.0, 1.0)
    w = [ij, ij + 4, acc, junk(rng), junk(rng), flo, junk(rng), junk(rng), fhi, junk(rng), junk(rng),
         val, junk(rng), junk(rng)] + blk + [fbits(v) for v in (x, lo, hi, tgt, k[0], k[1], k[2])] + [junk(rng)]
    return 1, w, ar


def case_adb0(rng, adaptive=False, nrange=(1, 6)):
    ar = Arena(); n = rng.randint(*nrange)
    p1 = ar.ints([0] * 7); p2 = ar.ints([0] * 6)
    P1_3 = ar.floats(rnd_floats(rng, 4 * n)); P1_0 = ar.floats(rnd_floats(rng, 4 * n))
    P2_0 = ar.ints(masks(rng, n)); P2_3 = ar.ints(masks(rng, n))
    lo = [f32(-abs(v)) for v in rnd_floats(rng, 4 * n)]
    hi = [f32(abs(v)) for v in rnd_floats(rng, 4 * n)]
    s49 = []
    for i in range(4 * n):
        r = rng.random()
        s49.append(lo[i] if r < 0.25 else hi[i] if r < 0.5 else f32(rng.uniform(-2, 2)))
    W37 = ar.floats(lo); W40 = ar.floats(hi)
    W43 = ar.floats(rnd_floats(rng, 4 * n, -1.0, 1.0))
    W46 = ar.floats(rnd_floats(rng, 4 * n)); W49 = ar.floats(s49)
    blk = pgs_block(rng, ar, n)
    ar.put(p1, 'i', [P1_0, junk(rng), junk(rng), P1_3, n, junk(rng)])
    ar.put(p2, 'i', [P2_0, junk(rng), junk(rng), P2_3, junk(rng), junk(rng)])
    w = [junk(rng) for _ in range(69)]
    w[0], w[1] = p1, p2
    w[2:36] = blk
    w[36], w[39], w[42] = W37, W40, W43
    w[45], w[46], w[48] = W46, rng.randint(0, n), W49
    w[63], w[64] = fbits(f32(rng.uniform(-1, 1))), fbits(f32(rng.uniform(-1, 1)))
    w[65] = fbits(rng.choice([0.0, 0.05, 0.5]))
    w[67] = rng.randint(1, 40)
    w[68] = rng.randint(0, 1)
    if adaptive:                       # amendment 1: W69 = 1, W68 in 60..160, W66 = 0.0
        w[65] = fbits(0.0)
        w[67] = rng.randint(60, 160)
        w[68] = 1
    return 3, w, ar


LABELS = NAMES + ['0056adb0A', '0056adb0B']


def make_cases(ncase, nadapt, nbig):
    rng = random.Random(SEED)
    cases = []
    for gen in (case_a7a0, case_aae0, case_ac40, case_adb0):
        for _ in range(ncase):
            fn, w, ar = gen(rng)
            assert len(ar.buf) <= ARENA_MAX
            cases.append((fn, [x & 0xffffffff for x in w], bytes(ar.buf), NAMES[fn]))
    rng2 = random.Random(SEED + 1)     # 20261030, separate stream: pass-1 cases unchanged
    for _ in range(nadapt):
        fn, w, ar = case_adb0(rng2, adaptive=True)
        cases.append((fn, [x & 0xffffffff for x in w], bytes(ar.buf), '0056adb0A'))
    rng3 = random.Random(SEED + 2)     # 20261031: P1_4 in 8..20, adaptive settings (this PREREG)
    for _ in range(nbig):
        fn, w, ar = case_adb0(rng3, adaptive=True, nrange=(8, 20))
        assert len(ar.buf) <= ARENA_MAX
        cases.append((fn, [x & 0xffffffff for x in w], bytes(ar.buf), '0056adb0B'))
    return cases

# ---------------------------------------------------------------------------- original (Unicorn)

def build_uc():
    pe = pefile.PE(str(EXE), fast_load=True)
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    base = pe.OPTIONAL_HEADER.ImageBase
    uc.mem_map(base, (pe.OPTIONAL_HEADER.SizeOfImage + 0xfff) & ~0xfff)
    uc.mem_write(base, pe.header)
    for s in pe.sections:
        d = s.get_data()
        if d:
            uc.mem_write(base + s.VirtualAddress, d[:s.Misc_VirtualSize] if s.Misc_VirtualSize else d)
    uc.mem_map(ARENA, ARENA_MAX)
    uc.mem_map(STACK_LO, STACK_SZ)
    uc.mem_map(STUB, 0x1000)
    # one entry stub per function (Unicorn caches translated code, so a stub is never rewritten)
    for fn, rva in enumerate(RVAS):
        uc.mem_write(STUB + 0x20 * fn, stub(rva))
    uc.mem_write(STUB + 0x100, struct.pack('<HxxI', 0x007f, 0x1f80))
    uc.mem_write(RET_F, b'\xd9\x1d' + struct.pack('<I', STUB + 0x200))
    uc.hook_add(UC_HOOK_CODE, probe_hook, begin=FISTP, end=FISTP)
    return uc


def stub(target):
    # fninit; fldcw [STUB+0x100]; ldmxcsr [STUB+0x104]; mov eax,target; jmp eax
    code = b'\xdb\xe3' + b'\xd9\x2d' + struct.pack('<I', STUB + 0x100) \
         + b'\x0f\xae\x15' + struct.pack('<I', STUB + 0x104) \
         + b'\xb8' + struct.pack('<I', target) + b'\xff\xe0'
    return code


RET_VOID = STUB + 0x80          # return address; emulation stops here
FISTP = 0x0056b264              # fistp dword [esp+0x50]; operand = float32 at [esp+0x48]
PROBE = {'exec': 0, 'sens': 0, 'ops': []}


def rne_vs_trunc(x):
    r = round(x)                   # Python round = round half to even, like fistp under RC=00
    return r != int(x)             # int() truncates toward zero, like the M4 mutant


def probe_hook(uc, addr, size, _):
    esp = uc.reg_read(UC_X86_REG_ESP)
    x = struct.unpack('<f', bytes(uc.mem_read(esp + 0x48, 4)))[0]
    PROBE['exec'] += 1
    if x == x and abs(x) < 2.0 ** 31 and rne_vs_trunc(x):
        PROBE['sens'] += 1
    PROBE['ops'].append(x)
RET_F = STUB + 0x90             # fstp dword [STUB+0x200] then stop at STUB+0x96


def run_original(uc, fn, words, arena):
    uc.mem_write(STUB + 0x200, b'\0' * 4)
    uc.mem_write(ARENA, arena)
    esp = STACK_LO + STACK_SZ - 0x4000 - 4 * (len(words) + 1)
    esp &= ~0xf
    esp -= 4    # [esp] = return address, args at esp+4 (16-aligned like a cdecl call site)
    ret = RET_F if fn == 2 else RET_VOID
    uc.mem_write(esp, struct.pack('<I', ret) + struct.pack('<%dI' % len(words), *words))
    uc.reg_write(UC_X86_REG_ESP, esp)
    until = RET_F + 6 if fn == 2 else RET_VOID
    uc.emu_start(STUB + 0x20 * fn, until, timeout=20_000_000, count=50_000_000)
    if uc.reg_read(UC_X86_REG_EIP) != until:
        raise RuntimeError('did not reach return, eip=%08x' % uc.reg_read(UC_X86_REG_EIP))
    r = struct.unpack('<I', uc.mem_read(STUB + 0x200, 4))[0] if fn == 2 else 0
    return r, bytes(uc.mem_read(ARENA, len(arena)))

# ---------------------------------------------------------------------------- port (harness)

def port_text(mutant):
    t = SRC.read_text(encoding='utf-8')
    a = t.index('namespace mashed_re {')
    b = t.index('// --- registrations')
    body = t[a:b] + '} // namespace Collision\n} // namespace mashed_re\n'
    if mutant:
        old, new = MUTANTS[mutant]
        if body.count(old) != 1:
            sys.exit('mutant %s: site found %d times' % (mutant, body.count(old)))
        body = body.replace(old, new)
    return body


def build_harness(variant, outdir):
    d = outdir / ('build_' + variant)
    d.mkdir(parents=True, exist_ok=True)
    pre = '#include <cstdint>\n#include <cstring>\n#include <xmmintrin.h>\n'
    (d / 'harness.cpp').write_text(pre + port_text(None if variant == 'port' else variant)
                                   + (HERE / 'uc_harness_main.inc').read_text(encoding='utf-8'),
                                   encoding='utf-8')
    exe = d / 'harness.exe'
    cmd = (f'call "{VCVARS}" >nul && cl /nologo /EHsc /W3 /O2 /Fe:harness.exe harness.cpp '
           f'/link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO')
    r = subprocess.run(cmd, shell=True, capture_output=True, text=True, cwd=str(d))
    (d / 'build.log').write_text(r.stdout + r.stderr)
    if r.returncode != 0:
        sys.exit('BUILD FAILED %s:\n%s\n%s' % (variant, r.stdout[-3000:], r.stderr[-2000:]))
    return exe


def launch_with_page(argv):
    """CreateProcess suspended, commit 0x00560000 (64 KB, RWX) in the child, resume, wait.
    The harness's thunks to the port bodies go at 0x0056a7a0.. inside that page."""
    import ctypes
    from ctypes import wintypes as W
    k32 = ctypes.WinDLL('kernel32', use_last_error=True)

    class SI(ctypes.Structure):
        _fields_ = [('cb', W.DWORD), ('r', W.LPWSTR), ('d', W.LPWSTR), ('t', W.LPWSTR)] + \
                   [(n, W.DWORD) for n in ('x', 'y', 'xs', 'ys', 'xc', 'yc', 'fa', 'fl')] + \
                   [('sw', W.WORD), ('cb2', W.WORD), ('r2', ctypes.c_void_p),
                    ('i', W.HANDLE), ('o', W.HANDLE), ('e', W.HANDLE)]

    class PI(ctypes.Structure):
        _fields_ = [('hp', W.HANDLE), ('ht', W.HANDLE), ('pid', W.DWORD), ('tid', W.DWORD)]

    k32.VirtualAllocEx.restype = ctypes.c_void_p
    k32.VirtualAllocEx.argtypes = [W.HANDLE, ctypes.c_void_p, ctypes.c_size_t, W.DWORD, W.DWORD]
    si = SI(); si.cb = ctypes.sizeof(SI); pi = PI()
    cmd = subprocess.list2cmdline(argv)
    if not k32.CreateProcessW(None, ctypes.create_unicode_buffer(cmd), None, None, False,
                              0x4, None, None, ctypes.byref(si), ctypes.byref(pi)):   # CREATE_SUSPENDED
        sys.exit('CreateProcess failed %d' % ctypes.get_last_error())
    try:
        got = k32.VirtualAllocEx(pi.hp, 0x00560000, 0x10000, 0x3000, 0x40)   # RESERVE|COMMIT, RWX
        if got != 0x00560000:
            k32.TerminateProcess(pi.hp, 9)
            sys.exit('VirtualAllocEx(0x00560000) in suspended child failed: %r err %d'
                     % (got, ctypes.get_last_error()))
        k32.ResumeThread(pi.ht)
        k32.WaitForSingleObject(pi.hp, 0xFFFFFFFF)
        rc = W.DWORD()
        k32.GetExitCodeProcess(pi.hp, ctypes.byref(rc))
        return rc.value
    finally:
        k32.CloseHandle(pi.ht); k32.CloseHandle(pi.hp)


def run_port(exe, cases_bin, out_bin, cases):
    rc = launch_with_page([str(exe), str(cases_bin), str(out_bin)])
    if rc != 0:
        sys.exit('harness rc=%d' % rc)
    data = out_bin.read_bytes()
    res, o = [], 0
    for fn, w, ar, _ in cases:
        ret = struct.unpack_from('<I', data, o)[0]; o += 4
        res.append((ret, data[o:o + len(ar)])); o += len(ar)
    return res


def write_cases(path, cases):
    with open(path, 'wb') as f:
        f.write(struct.pack('<I', len(cases)))
        for fn, w, ar, _ in cases:
            f.write(struct.pack('<III', fn, len(w), len(ar)))
            f.write(struct.pack('<%dI' % len(w), *w))
            f.write(ar)

# ---------------------------------------------------------------------------- main

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--cases', type=int, default=300)
    ap.add_argument('--adaptive', type=int, default=300)   # amendment 1
    ap.add_argument('--big', type=int, default=300)        # this PREREG: 0056adb0B
    ap.add_argument('--out', default='D:/MashedWT/D4-WSB-SOLV-KV-ARMU_data/uc')
    a = ap.parse_args()
    out = Path(a.out); out.mkdir(parents=True, exist_ok=True)

    import hashlib
    h = hashlib.sha256(EXE.read_bytes()).hexdigest()
    if h != ANCHOR:
        sys.exit('anchor mismatch: %s' % h)

    cases = make_cases(a.cases, a.adaptive, a.big)
    cases_bin = out / 'cases.bin'
    write_cases(cases_bin, cases)

    uc = build_uc()
    orig = []
    cover = {}
    for i, (fn, w, ar, lab) in enumerate(cases):
        PROBE['exec'] = PROBE['sens'] = 0; PROBE['ops'] = []
        try:
            orig.append(run_original(uc, fn, w, ar))
        except (UcError, RuntimeError) as e:
            orig.append(('ERR', str(e)))
        c = cover.setdefault(lab, {'fistp_exec': 0, 'm4_sensitive': 0, 'cases_with_sensitive': 0,
                                   'op_min': None, 'op_max': None})
        c['fistp_exec'] += PROBE['exec']; c['m4_sensitive'] += PROBE['sens']
        c['cases_with_sensitive'] += 1 if PROBE['sens'] else 0
        for x in PROBE['ops']:
            c['op_min'] = x if c['op_min'] is None else min(c['op_min'], x)
            c['op_max'] = x if c['op_max'] is None else max(c['op_max'], x)
    nerr = sum(1 for r in orig if r[0] == 'ERR')
    print('[0/5] unicorn original: %d cases, %d errors' % (len(cases), nerr), flush=True)
    for lab in LABELS:
        if lab in cover and cover[lab]['fistp_exec']:
            print('      probe 0x0056b264 %s: %s' % (lab, cover[lab]), flush=True)

    variants = ['port', 'M1', 'M2', 'M3', 'M4']
    summary = {'anchor': h, 'seed': SEED, 'cases_per_fn': a.cases, 'adaptive': a.adaptive, 'big': a.big,
               'unicorn_errors': nerr, 'fistp_probe': cover, 'variants': {}}
    rows = ['variant\tfn\tcases\tmatch\tdiff\tfirst_diff']
    for vi, v in enumerate(variants, 1):
        exe = build_harness(v, out)
        port = run_port(exe, cases_bin, out / ('out_%s.bin' % v), cases)
        per = {}
        for ci, ((fn, w, ar, lab), o, p) in enumerate(zip(cases, orig, port)):
            s = per.setdefault(lab, {'cases': 0, 'match': 0, 'diff': 0, 'first': ''})
            s['cases'] += 1
            if o[0] == 'ERR':
                s['diff'] += 1
                s['first'] = s['first'] or 'case %d unicorn: %s' % (ci, o[1])
                continue
            if o == p:
                s['match'] += 1
            else:
                s['diff'] += 1
                if not s['first']:
                    if o[0] != p[0]:
                        s['first'] = 'case %d ret o=%08x n=%08x' % (ci, o[0], p[0])
                    else:
                        off = next(k for k in range(len(o[1])) if o[1][k] != p[1][k])
                        s['first'] = 'case %d arena+0x%x' % (ci, off)
        summary['variants'][v] = per
        for name in LABELS:
            s = per[name]
            rows.append('%s\t%s\t%d\t%d\t%d\t%s' % (v, name, s['cases'], s['match'], s['diff'], s['first']))
        if v == 'port':
            st = 'PASS' if all(per[nm]['diff'] == 0 for nm in LABELS) and nerr == 0 else 'FAIL'
        else:
            hit = sum(per[f]['diff'] for f in MUTANT_FN[v])
            st = 'DETECTED(%d)' % hit if hit else 'NOT-DETECTED'
        print('[%d/5] %s %s  %s' % (vi, v, st,
              ' '.join('%s:%d/%d' % (nm, per[nm]['match'], per[nm]['cases']) for nm in LABELS)), flush=True)

    port_ok = nerr == 0 and all(summary['variants']['port'][nm]['diff'] == 0 for nm in LABELS)
    mut_ok = all(sum(summary['variants'][m][f]['diff'] for f in MUTANT_FN[m]) > 0 for m in MUTANTS)
    covered = cover.get('0056adb0B', {}).get('m4_sensitive', 0) > 0
    verdict = 'RED' if not port_ok else ('GREEN' if (mut_ok and covered) else 'INVALID')
    summary['verdict'] = verdict
    (HERE / 'uc_summary.tsv').write_text('\n'.join(rows) + '\n')
    (HERE / 'uc_summary.json').write_text(json.dumps(summary, indent=1))
    print('ARM U', verdict)


if __name__ == '__main__':
    main()
