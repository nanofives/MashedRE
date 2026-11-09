"""fuzz_00443300.py [--n 20000] [--seed 1] [--img D:/MashedWT/PORT-AISPLINE-DDKORDER_data/img] [--out <tsv>]

PORT-AISPLINE-DDKORDER, offline control next to the live-seeded arm (PREREG_AISPLINE_DDKORDER.md section 3). Unicorn
runs the original's bytes of 0x00443300 (MASHED.exe.unpatched; constants 0x005cc31c/20/2c/58/5c, 0x005d757c) and the
port bodies of the X (/arch:IA32) and S (SSE2) images on synthetic inputs: 64 random {x, z} points (mixed magnitudes),
a random count 4..64, a random segment and t (in [0,1] mostly, some outside to hit the clamp). The original is run at
PC_24 (CW 0x007f, the only CW arm 1 of PORT-X87-PC24-AUDIT saw at this RVA) and at PC_53. Each port body at PC_24,
PC_53 and (S) MXCSR 0x1f80. Compared: the two output floats, bit for bit.
"""
import argparse, random, struct
from pathlib import Path
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_PROT_ALL
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP

ap = argparse.ArgumentParser()
ap.add_argument('--n', type=int, default=20000)
ap.add_argument('--seed', type=int, default=1)
ap.add_argument('--img', default='D:/MashedWT/PORT-AISPLINE-DDKORDER_data/img')
ap.add_argument('--orig', default=str(Path(__file__).resolve().parents[3].parent.parent / 'original' / 'MASHED.exe.unpatched'))
ap.add_argument('--out', default='')
a = ap.parse_args()

PAGE = 0x1000
uc = Uc(UC_ARCH_X86, UC_MODE_32)
pe = pefile.PE(a.orig)
om = bytes(pe.get_memory_mapped_image(ImageBase=0x00400000))
osz = (len(om) + PAGE - 1) & ~(PAGE - 1)
uc.mem_map(0x00400000, osz, UC_PROT_ALL)
uc.mem_write(0x00400000, om)
syms = {}
for tag, base in (('X', 0x30000000), ('S', 0x34000000)):
    ipe = pefile.PE(str(Path(a.img) / tag / f'port_{tag}.exe'))
    im = bytes(ipe.get_memory_mapped_image(ImageBase=base))
    uc.mem_map(base, (len(im) + PAGE - 1) & ~(PAGE - 1), UC_PROT_ALL)
    uc.mem_write(base, im)
    for l in (Path(a.img) / tag / f'port_{tag}.map').read_text(errors='replace').splitlines():
        p = l.split()
        if len(p) >= 4 and ':' in p[0] and len(p[2]) == 8:
            try:
                syms.setdefault((tag, p[1]), int(p[2], 16))
            except ValueError:
                pass
DATA, STK, STUB = 0x20000000, 0x21000000, 0x22000000
uc.mem_map(DATA, 0x10000, UC_PROT_ALL)
uc.mem_map(STK, 0x100000, UC_PROT_ALL)
uc.mem_map(STUB, PAGE, UC_PROT_ALL)
PATH, OUT, CWM, MX = DATA, DATA + 0x400, DATA + 0x500, DATA + 0x504
# stub: fninit; fldcw [CWM]; ldmxcsr [MX]; push out; push t; push seg; push path; call [fn]; add esp,16; hlt
FN = DATA + 0x508
stub = (b'\xdb\xe3' + b'\xd9\x2d' + struct.pack('<I', CWM) + b'\x0f\xae\x15' + struct.pack('<I', MX)
        + b'\x68' + struct.pack('<I', OUT) + b'\xff\x35' + struct.pack('<I', DATA + 0x510)
        + b'\xff\x35' + struct.pack('<I', DATA + 0x514) + b'\x68' + struct.pack('<I', PATH)
        + b'\xff\x15' + struct.pack('<I', FN) + b'\x83\xc4\x10' + b'\xf4')
uc.mem_write(STUB, stub)
END = STUB + len(stub) - 1


def call(fn, cw, mx=0x1f80):
    uc.mem_write(OUT, b'\xcc' * 8)
    uc.mem_write(CWM, struct.pack('<HH', cw, 0))
    uc.mem_write(MX, struct.pack('<I', mx))
    uc.mem_write(FN, struct.pack('<I', fn))
    uc.reg_write(UC_X86_REG_ESP, STK + 0x80000)
    uc.emu_start(STUB, END, count=5000)
    return bytes(uc.mem_read(OUT, 8))


def rf(r):
    k = r.random()
    if k < 0.4:
        return r.uniform(-500.0, 500.0)
    if k < 0.7:
        return r.uniform(-5000.0, 5000.0)
    if k < 0.9:
        return r.uniform(-1.0, 1.0) * 10 ** r.uniform(-3, 4)
    return float(r.randint(-300, 300))


VARIANTS = [('O24', 0x00443300, 0x007f), ('O53', 0x00443300, 0x027f)]
for tag in ('X', 'S'):
    for body in ('ddk', 'match'):
        fn = syms[(tag, f'_AiSplineEval_x87_{body}')]
        cws = (('24', 0x007f), ('53', 0x027f))
        for nm, cw in cws:
            VARIANTS.append((f'{tag}{nm}_{body}', fn, cw))
r = random.Random(a.seed)
bad = {v[0]: 0 for v in VARIANTS}
ex = {}
for n in range(a.n):
    pts = b''.join(struct.pack('<ff', rf(r), rf(r)) for _ in range(64))
    cnt = r.randint(4, 64)
    seg = r.randint(0, cnt - 1)
    t = r.random() if r.random() < 0.9 else r.uniform(-0.5, 1.5)
    uc.mem_write(PATH, pts + struct.pack('<i', cnt))
    uc.mem_write(DATA + 0x510, struct.pack('<f', t))
    uc.mem_write(DATA + 0x514, struct.pack('<i', seg))
    ref = call(0x00443300, 0x007f)
    for nm, fn, cw in VARIANTS:
        got = call(fn, cw)
        if got != ref:
            bad[nm] += 1
            ex.setdefault(nm, (n, seg, cnt, t, struct.unpack('<ff', ref), struct.unpack('<ff', got)))
lines = ['variant\tdiffer_from_O24\tof\tfirst_example(n,seg,count,t,O24,variant)']
for nm, *_ in VARIANTS:
    lines.append(f'{nm}\t{bad[nm]}\t{a.n}\t{ex.get(nm, "")}')
print('\n'.join(lines))
if a.out:
    Path(a.out).write_text('\n'.join(lines) + '\n')
