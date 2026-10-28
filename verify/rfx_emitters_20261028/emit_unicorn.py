#!/usr/bin/env python3
"""Arm A1 of PREREG_D5-RFX-EMITTERS.md: the emitter API's original bytes in Unicorn vs the port's core.

Executed verbatim from original/MASHED.exe.unpatched (version anchor in CLAUDE.md):
  FUN_004770c0, FUN_00534b60, FUN_00474db0, FUN_004b6520 / FUN_004b64e0, the setters 0x004769a0 0x004769d0
  0x004769f0 0x00476a10 0x00476a30 0x00476a40 0x00476a80, the batch setters 0x00476ae0 0x00476b30 0x00476b90
  0x00476be0 0x00476c10 0x00476c60, FUN_00476d00, FUN_00476df0, FUN_00535700, FUN_00535910, the selector
  0x00536780 and the instance callbacks 0x00536e90 0x00536fe0 0x00537b20 0x00537d20 0x00537d70 0x00537e60.
Stubbed (a RET written over the entry in the emulator's private copy, the effect done by the hook):
  RwMalloc ([RwGlobals]+0x108): bump allocator, zeroed. FUN_00534d00: a fake atomic and plugin with the
  fields 0x00534d67..0x00534d80 write (+0 max, +4 0, +0xa0 flags, +0x40 0xffffffff, +0x3c 0) and one array per
  data flag at the FUN_00535700 offsets. FUN_004e8090 / FUN_004e7e30: ret. FUN_004c0b30: a fake frame.
DAT_007dc57c (the plugin offset, BSS) is set to 0x200. x87 control word 0x007f (PC_24) for every call.

Usage: py -3.12 emit_unicorn.py <port_exe> <workdir>   (writes cases.txt / orig.txt / port.txt, prints a
summary and exits 1 on any mismatch)
"""
import random, struct, subprocess, sys, os

import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_FPCW

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
EXE = os.path.join(ROOT, "original", "MASHED.exe.unpatched")
if not os.path.exists(EXE):  # worktrees junction original/ only in some layouts
    EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
BASE = 0x400000
STK, STK_N = 0x20000000, 0x40000
HEAP, HEAP_N = 0x40000000, 0x10000000
SCR = 0x30000000
MAGIC = 0x31000000
STUB = 0x32000000        # RwMalloc stub
GLOB = 0x33000000        # fake RwGlobals
PLUG_OFF = 0x200
DAT_PLUG = 0x007dc57c

F_ALLOC, F_534D00, F_4E8090, F_4C0B30, F_4E7E30 = STUB, 0x00534d00, 0x004e8090, 0x004c0b30, 0x004e7e30
LOCK_OFF = {1: (0x4c, 12), 2: (0x6c, 4), 4: (0x64, 8), 8: (0x54, 64), 0x20: (0x7c, 4), 0x80: (0x84, 16),
            0x100: (0x8c, 32)}
ARR_ORDER = [(1, 3), (4, 2), (2, 1), (8, 16), (0x20, 1), (0x80, 4), (0x100, 8)]   # FUN_00476df0 order
E_ARR = [(4, 3), (5, 2), (6, 1), (7, 16), (8, 1), (9, 4), (10, 8)]                 # param_1[4..10]


class Emu:
    def __init__(self):
        pe = pefile.PE(EXE)
        self.uc = uc = Uc(UC_ARCH_X86, UC_MODE_32)
        size = (pe.OPTIONAL_HEADER.SizeOfImage + 0xfff) & ~0xfff
        uc.mem_map(BASE, size)
        for s in pe.sections:
            uc.mem_write(BASE + s.VirtualAddress, s.get_data())
        for a, n in ((STK, STK_N), (HEAP, HEAP_N), (SCR, 0x100000), (MAGIC, 0x1000), (STUB, 0x1000), (GLOB, 0x1000)):
            uc.mem_map(a, n)
        self.heap = HEAP
        self.sizes = {}
        uc.mem_write(STUB, b"\xc3")
        for a in (F_534D00, F_4E8090, F_4C0B30, F_4E7E30):
            uc.mem_write(a, b"\xc3")
        self.w32(0x007d3ff8, GLOB)
        self.w32(GLOB + 0x108, STUB)
        self.w32(DAT_PLUG, PLUG_OFF)
        uc.hook_add(UC_HOOK_CODE, self._stub, begin=STUB, end=STUB)
        for a in (F_534D00, F_4C0B30):
            uc.hook_add(UC_HOOK_CODE, self._stub, begin=a, end=a)

    def w32(self, a, v): self.uc.mem_write(a, struct.pack("<I", v & 0xffffffff))
    def r32(self, a): return struct.unpack("<I", self.uc.mem_read(a, 4))[0]
    def rd(self, a, n): return list(struct.unpack("<%dI" % n, self.uc.mem_read(a, 4 * n))) if n else []

    def alloc(self, n):
        p = self.heap
        self.heap = (self.heap + n + 0xf) & ~0xf
        self.uc.mem_write(p, b"\0" * max(n, 1))
        self.sizes[p] = n
        return p

    def arg(self, i): return self.r32(self.uc.reg_read(UC_X86_REG_ESP) + 4 + 4 * i)

    def _stub(self, uc, addr, size, _):
        if addr == STUB:
            uc.reg_write(UC_X86_REG_EAX, self.alloc(self.arg(0)))
        elif addr == F_4C0B30:
            uc.reg_write(UC_X86_REG_EAX, self.alloc(0x100))
        elif addr == F_534D00:
            mx, fl = self.arg(0), self.arg(1)
            uc.reg_write(UC_X86_REG_EAX, self.make_atomic(mx, fl))

    def make_atomic(self, mx, fl):
        atom = self.alloc(0x400)
        plug = self.alloc(0x200)
        geo, matl, mat = self.alloc(0x100), self.alloc(0x10), self.alloc(0x40)
        self.w32(atom + 0x18, geo); self.w32(geo + 0x20, matl); self.w32(matl, mat)
        self.w32(atom + PLUG_OFF, plug)
        self.w32(plug + 0, mx); self.w32(plug + 4, 0); self.w32(plug + 0xa0, fl)
        self.w32(plug + 0x40, 0xffffffff); self.w32(plug + 0x3c, 0); self.w32(plug + 0xc, 0)
        for bit, (off, el) in LOCK_OFF.items():
            if fl & bit:
                self.w32(plug + off, self.alloc(mx * el)); self.w32(plug + off + 4, el)
        self.w32(plug + 0x9c, 0)
        self.last = (atom, plug, mat)
        return atom

    def call(self, fn, *args):
        uc = self.uc
        sp = STK + STK_N - 0x100
        for a in reversed(args):
            sp -= 4; self.w32(sp, a)
        sp -= 4; self.w32(sp, MAGIC)
        uc.reg_write(UC_X86_REG_ESP, sp)
        uc.reg_write(UC_X86_REG_FPCW, 0x007f)
        uc.emu_start(fn, MAGIC)
        return uc.reg_read(UC_X86_REG_EAX)

    def buf(self, words):
        p = self.alloc(4 * max(len(words), 1))
        if words:
            self.uc.mem_write(p, struct.pack("<%dI" % len(words), *words))
        return p


def f2u(f): return struct.unpack("<I", struct.pack("<f", f))[0]
def h(ws): return "".join(" %08x" % w for w in ws)


def orig_create(emu, fl, cap):
    e = emu.alloc(0x40)
    emu.call(0x004770c0, e, fl, cap, 0)
    return e


def orig_create_dump(emu, e):
    ew = emu.rd(e, 13)
    atom = ew[1]; plug = emu.r32(atom + PLUG_OFF); mat = emu.r32(emu.r32(emu.r32(atom + 0x18) + 0x20))
    out = [ew[0], ew[2], ew[3]]
    out += [emu.sizes.get(ew[i], 0) if ew[i] else 0 for i, _ in E_ARR]
    out += [emu.r32(plug), emu.r32(plug + 4), emu.r32(plug + 0x40), emu.r32(plug + 0xa0), emu.r32(plug + 0xa4),
            emu.r32(plug + 0xa8), emu.r32(plug + 0xac), emu.r32(plug + 0xb8), emu.r32(plug + 0xbc),
            emu.r32(plug + 0xc0), emu.r32(plug + 0xc4)]
    out += emu.rd(plug + 0xd8, 8) + emu.rd(plug + 0xf8, 16) + [emu.r32(mat + 4)] + emu.rd(0x00692558, 16)
    return out


def rnd_f(rng, lo, hi): return f2u(rng.uniform(lo, hi))


def main():
    port_exe, wd = sys.argv[1], sys.argv[2]
    os.makedirs(wd, exist_ok=True)
    rng = random.Random(20261028)
    emu = Emu()
    cases, orig = [], []
    counts = {"C": 0, "S": 0, "L": 0, "I": 0}
    callers = [0x807, 0x80c, 0x817, 0x427, 0x895, 0]
    seen_flags = set()

    # ---- C1 create ----
    for fl in callers + [rng.randrange(0x2000) for _ in range(256)]:
        cap = rng.randint(1, 300)
        e = orig_create(emu, fl, cap)
        d = orig_create_dump(emu, e)
        seen_flags.add(d[13])
        cases.append("C %x %x" % (fl, cap)); orig.append("C" + h(d)); counts["C"] += 1

    # ---- C2 setter / submit / flush sequences ----
    def seq_ok(fl):
        e = orig_create(emu, fl, 1)
        ew = emu.rd(e, 13); plug = emu.r32(ew[1] + PLUG_OFF); df = emu.r32(plug + 0xa0)
        return all((not ew[i]) or (df & bit) for (i, _), (bit, _) in zip(E_ARR, ARR_ORDER))
    pool = [f for f in callers + [rng.randrange(0x2000) for _ in range(400)] if seq_ok(f)]
    for k in range(240):
        fl = pool[k % len(pool)] if k < len(pool) else rng.choice(pool)
        cap = rng.randint(1, 8)
        ops, toks = [], []
        for _ in range(rng.randint(10, 60)):
            op = rng.choice([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 15])
            a = []
            if op == 1: a = [rnd_f(rng, -500, 500) for _ in range(3)]
            elif op == 3: a = [rnd_f(rng, 0, 5) for _ in range(2)]
            elif op == 5: a = [rng.getrandbits(32)]
            elif op == 7: a = [rng.getrandbits(32) for _ in range(16)]
            elif op == 9: a = [rnd_f(rng, -13, 13)]
            elif op == 10: a = [rnd_f(rng, 0, 1) for _ in range(4)]
            elif op == 12: a = [rnd_f(rng, 0, 1) for _ in range(8)]
            ops.append((op, a)); toks.append("%x" % op + h(a))
        cases.append("S %x %x %x %s" % (fl, cap, len(ops), " ".join(toks)))
        e = orig_create(emu, fl, cap)
        for op, a in ops:
            if op in (1, 3, 5, 7, 10, 12):
                fn = {1: 0x004769a0, 3: 0x004769d0, 5: 0x004769f0, 7: 0x00476a10, 10: 0x00476a40, 12: 0x00476a80}[op]
                emu.call(fn, emu.buf(a))
            elif op in (2, 4, 6, 8, 11, 13):
                fn = {2: 0x004769a0, 4: 0x004769d0, 6: 0x004769f0, 8: 0x00476a10, 11: 0x00476a40, 13: 0x00476a80}[op]
                emu.call(fn, 0)
            elif op == 9: emu.call(0x00476a30, a[0])
            elif op == 14: emu.call(0x00476d00, e)
            elif op == 15: emu.call(0x00476df0, e)
        ew = emu.rd(e, 13); plug = emu.r32(ew[1] + PLUG_OFF); df = emu.r32(plug + 0xa0); cp = ew[2]
        out = [ew[3]]
        for (i, per) in E_ARR:
            if ew[i]: out += emu.rd(ew[i], cp * per)
        out += [emu.r32(plug + 4), emu.r32(plug + 0x40)]
        mx = emu.r32(plug)
        for bit, per in ARR_ORDER:
            if df & bit: out += emu.rd(emu.r32(plug + LOCK_OFF[bit][0]), mx * per)
        orig.append("S" + h(out)); counts["S"] += 1

    # ---- C3 selector ----
    for df in sorted(seen_flags) + [rng.getrandbits(32) for _ in range(64)]:
        atom = emu.make_atomic(1, df); plug = emu.r32(atom + PLUG_OFF)
        emu.call(0x00536780, atom)
        cases.append("L %x" % df); orig.append("L" + h(emu.rd(plug + 0x24, 6))); counts["L"] += 1

    # ---- C4 instancing ----
    sets = [(0x10080027, 40), (0x10080007, 10), (0x10000007, 6), (0x10000087, 6), (0x10080005, 6), (0x10080025, 6)]
    for df, ncase in sets:
        for c in range(ncase):
            n = rng.randint(1, 60)
            r = [rng.uniform(-1, 1) for _ in range(3)]; u = [rng.uniform(-1, 1) for _ in range(3)]
            r, u = [f2u(x) for x in r], [f2u(x) for x in u]
            d8 = [rnd_f(rng, 0, 1) for _ in range(4)] + [0, 0, 0, 0]
            c4 = rng.getrandbits(32)
            parts = []
            for i in range(n):
                rot = 0 if i % 7 == 0 else rng.choice([rnd_f(rng, -12.6, 12.6), f2u(rng.uniform(-720, 720) * 0.017453292)])
                parts.append([rnd_f(rng, -500, 500) for _ in range(3)] + [rnd_f(rng, 0.01, 5) for _ in range(2)]
                             + [rot, rng.getrandbits(32)] + [rnd_f(rng, 0, 1) for _ in range(4)])
            cases.append("I %x %x" % (df, n) + h(r + u + d8 + [c4]) + "".join(h(p) for p in parts))
            atom = emu.make_atomic(n, df); plug = emu.r32(atom + PLUG_OFF); desc = plug + 0x48
            emu.call(0x00536780, atom)
            cb = emu.rd(plug + 0x24, 6)
            ok = cb[1] in (0x00536e90, 0x00536fe0) and cb[2] in (0, 0x00537d70, 0x00537e60) and \
                cb[3] in (0x00537b20, 0x00537d20) and cb[4] == 0
            if not ok:
                orig.append("I" + h([0])); counts["I"] += 1; continue
            emu.w32(desc + 4, emu.buf([w for p in parts for w in p[0:3]])); emu.w32(desc + 8, 12)
            emu.w32(desc + 0x1c, emu.buf([w for p in parts for w in p[3:5]])); emu.w32(desc + 0x20, 8)
            emu.w32(desc + 0x34, emu.buf([p[5] for p in parts])); emu.w32(desc + 0x38, 4)
            emu.w32(desc + 0x24, emu.buf([p[6] for p in parts])); emu.w32(desc + 0x28, 4)
            emu.w32(desc + 0x3c, emu.buf([w for p in parts for w in p[7:11]])); emu.w32(desc + 0x40, 16)
            emu.w32(desc + 0x7c, c4)
            for j in range(4): emu.w32(desc + 0x90 + 4 * j, d8[j])
            vb, ub, cbuf = emu.alloc(n * 48), emu.alloc(n * 32), emu.alloc(n * 16)
            vd, ud, cd = emu.buf([vb, 12]), emu.buf([ub, 8]), emu.buf([cbuf, 4])
            rp, up = emu.buf(r), emu.buf(u)
            emu.call(cb[1], vd, rp, up, n, desc)
            if cb[2]: emu.call(cb[2], ud, n, desc)
            emu.call(cb[3], cd, n, desc)
            out = [1] + emu.rd(vb, n * 12) + (emu.rd(ub, n * 8) if cb[2] else []) + emu.rd(cbuf, n * 4)
            orig.append("I" + h(out)); counts["I"] += 1

    cpath, opath, ppath = (os.path.join(wd, x) for x in ("cases.txt", "orig.txt", "port.txt"))
    open(cpath, "w").write("\n".join(cases) + "\n")
    open(opath, "w").write("\n".join(orig) + "\n")
    subprocess.run([port_exe, cpath, ppath], check=True)
    port = open(ppath).read().split("\n")[: len(orig)]
    mism = {"C": 0, "S": 0, "L": 0, "I": 0}
    words = 0
    first = []
    for i, (o, p) in enumerate(zip(orig, port)):
        words += len(o.split()) - 1
        if o.split() != p.split():
            mism[o[0]] += 1
            if len(first) < 5:
                ow, pw = o.split(), p.split()
                k = next((j for j in range(min(len(ow), len(pw))) if ow[j] != pw[j]), min(len(ow), len(pw)))
                first.append("case %d (%s) word %d: orig %s port %s (len %d/%d) | %s" % (
                    i, o[0], k, ow[k] if k < len(ow) else "-", pw[k] if k < len(pw) else "-", len(ow), len(pw),
                    cases[i][:80]))
    nparticles = sum(int(c.split()[2], 16) for c in cases if c[0] == "I")
    print("cases", counts, "particles", nparticles, "dwords compared", words)
    print("mismatch", mism)
    for f in first: print("  ", f)
    sys.exit(1 if any(mism.values()) or len(port) != len(orig) else 0)


if __name__ == "__main__":
    main()
