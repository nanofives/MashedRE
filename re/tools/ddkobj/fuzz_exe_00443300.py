"""DDKOBJ-SPIKE part D: 0x00443300 as it sits in a LINKED exe, against the original's bytes, under Unicorn.

    py -3.12 re/tools/ddkobj/fuzz_exe_00443300.py <exe> <map> <symbol> [<exe> <map> <symbol> ...] [--n 20000] [--seed 7]

Same inputs, stub and reference as verify/portstate_20261109/aispline_ddk/fuzz_00443300.py (the original at PC_24,
CW 0x007f): 64 mixed-magnitude {x, z} points, count 4..64, a random segment, t in [0,1] for 90%. Each exe is mapped at
its own base with the original image written over its arena (0x00400000..), which is where the exe reads the original
constants through its absolute addresses. The named body is then called at PC_24 and PC_53. Output: differ counts.
"""
import random
import struct
import sys
from pathlib import Path

import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_PROT_ALL
from unicorn.x86_const import UC_X86_REG_ESP

ROOT = Path(__file__).resolve().parents[3]
MAIN = ROOT.parent.parent if ROOT.parent.name == ".worktrees" else ROOT
ORIG = MAIN / "original" / "MASHED.exe.unpatched"
PAGE = 0x1000
DATA, STK, STUB = 0x20000000, 0x21000000, 0x22000000


def machine(exe):
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    pe = pefile.PE(str(exe))
    base = pe.OPTIONAL_HEADER.ImageBase
    im = bytearray(pe.get_memory_mapped_image())
    ope = pefile.PE(str(ORIG))
    om = bytes(ope.get_memory_mapped_image(ImageBase=0x00400000))
    lo = 0x00400000 - base
    if len(im) < lo + len(om):
        im.extend(b"\0" * (lo + len(om) - len(im)))
    im[lo:lo + len(om)] = om        # the original's bytes in the exe's arena (0x00400000 ..)
    uc.mem_map(base, (len(im) + PAGE - 1) & ~(PAGE - 1), UC_PROT_ALL)
    uc.mem_write(base, bytes(im))
    uc.mem_map(DATA, 0x10000, UC_PROT_ALL)
    uc.mem_map(STK, 0x100000, UC_PROT_ALL)
    uc.mem_map(STUB, PAGE, UC_PROT_ALL)
    PATH, OUT, CWM, MX, FN = DATA, DATA + 0x400, DATA + 0x500, DATA + 0x504, DATA + 0x508
    stub = (b"\xdb\xe3" + b"\xd9\x2d" + struct.pack("<I", CWM) + b"\x0f\xae\x15" + struct.pack("<I", MX)
            + b"\x68" + struct.pack("<I", OUT) + b"\xff\x35" + struct.pack("<I", DATA + 0x510)
            + b"\xff\x35" + struct.pack("<I", DATA + 0x514) + b"\x68" + struct.pack("<I", PATH)
            + b"\xff\x15" + struct.pack("<I", FN) + b"\x83\xc4\x10" + b"\xf4")
    uc.mem_write(STUB, stub)
    end = STUB + len(stub) - 1

    def call(fn, cw, mx=0x1f80):
        uc.mem_write(OUT, b"\xcc" * 8)
        uc.mem_write(CWM, struct.pack("<HH", cw, 0))
        uc.mem_write(MX, struct.pack("<I", mx))
        uc.mem_write(FN, struct.pack("<I", fn))
        uc.reg_write(UC_X86_REG_ESP, STK + 0x80000)
        uc.emu_start(STUB, end, count=5000)
        return bytes(uc.mem_read(OUT, 8))

    def setin(pts, cnt, seg, t):
        uc.mem_write(PATH, pts + struct.pack("<i", cnt))
        uc.mem_write(DATA + 0x510, struct.pack("<f", t))
        uc.mem_write(DATA + 0x514, struct.pack("<i", seg))
    return call, setin


def mapsym(mp, name):
    if name.startswith("0x"):
        return int(name, 16)   # a raw address (0x00443300 = the original's own bytes in the arena: sanity row)
    for l in Path(mp).read_text(errors="replace").splitlines():
        p = l.split()
        if len(p) >= 4 and ":" in p[0] and p[1] == name and len(p[2]) == 8:
            return int(p[2], 16)
    raise SystemExit(f"{name} not in {mp}")


def rf(r):
    k = r.random()
    if k < 0.4:
        return r.uniform(-500.0, 500.0)
    if k < 0.7:
        return r.uniform(-5000.0, 5000.0)
    if k < 0.9:
        return r.uniform(-1.0, 1.0) * 10 ** r.uniform(-3, 4)
    return float(r.randint(-300, 300))


def main():
    args = [x for x in sys.argv[1:]]
    n = int(args[args.index("--n") + 1]) if "--n" in args else 20000
    seed = int(args[args.index("--seed") + 1]) if "--seed" in args else 7
    pos = [x for i, x in enumerate(args) if not x.startswith("--") and (i == 0 or args[i - 1] not in ("--n", "--seed"))]
    targets = []
    for i in range(0, len(pos), 3):
        exe, mp, sym = pos[i:i + 3]
        call, setin = machine(exe)
        targets.append((f"{Path(exe).name}:{sym}", call, setin, mapsym(mp, sym)))
    r = random.Random(seed)
    bad = {(t[0], cw): 0 for t in targets for cw in ("PC_24", "PC_53")}
    for k in range(n):
        pts = b"".join(struct.pack("<ff", rf(r), rf(r)) for _ in range(64))
        cnt = r.randint(4, 64)
        seg = r.randint(0, cnt - 1)
        t = r.random() if r.random() < 0.9 else r.uniform(-0.5, 1.5)
        ref = None
        for name, call, setin, fn in targets:
            setin(pts, cnt, seg, t)
            if ref is None:
                ref = call(0x00443300, 0x007f)      # the original's bytes at PC_24 (same image, arena copy)
            for cwn, cw in (("PC_24", 0x007f), ("PC_53", 0x027f)):
                if call(fn, cw) != ref:
                    bad[(name, cwn)] += 1
    print("| body (exe:symbol) | CW | differs from the original @PC_24 |")
    print("|---|---|---|")
    for (name, cwn), v in bad.items():
        print(f"| {name} | {cwn} | {v} / {n} |")


if __name__ == "__main__":
    main()
