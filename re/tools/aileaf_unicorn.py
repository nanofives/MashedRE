#!/usr/bin/env python3
"""D3 bulk G-AILEAF: synthetic arm coverage for FUN_00414570 / FUN_00415880 / FUN_0046d570, original bytes in Unicorn.

Plan: verify/d3_bulk_20261011/PLAN.md (section 0 amendment, section 3 rank 2). The exe's replay
(MASHED_AILEAF_REPLAY, mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp) reads the rows this writes and runs the
naked transcriptions on the same inputs, under the same x87 control word.

Executed verbatim from original/MASHED.exe.unpatched (version anchor in CLAUDE.md):
  FUN_00414570, FUN_00415880, FUN_0046d570, FUN_00443440, FUN_00443300, FUN_004233e0, FUN_00408af0,
  FUN_00408a50, FUN_00426bb0, FUN_0040e470, FUN_004c39b0, FUN_004c3ac0 (RW sqrt LUT rebuilt here with the
  algorithm of Math/RwLutTables.cpp and hung off 0x007d3ff8/0x007d3ffc), CRT acos 0x004a3384 with its tails
  0x004a70a5 / 0x004a70d5 / 0x004a712e / 0x004a713b / 0x004a7047, __ftol 0x004a2c48.
Served from the row (code hooks that write the outputs and return), mirroring the exe's host bindings:
  FUN_0046d4a0 (record ptr -> fake 0x40 matrix, +0x30 x, +0x38 z), FUN_0046d6d0 (speed), FUN_0046d510 (`at`
  row), FUN_0046c7b0 (alive), FUN_0046cbb0 (state pair), FUN_0046cc10 (record +0x9e0 as int).
Stubbed: 0x004ae17b (CRT error report, called by 0x004a7047; returns), 0x004d7ff0 (xor eax,eax; ret),
  0x004d8480 (ret), FUN_004671a0 / FUN_004b55a0 (debug draw; counted, never expected).
Globals written at their original VAs: 0x007f0fd0, 0x0066d6d8, 0x007f1008, 0x0089a360, 0x0089a368,
  0x0089a374, the AI slot block 0x0089a4c0 + v*0x74, progress 0x008a96e8 + c*0x30c and the vec3 at
  0x008a96dc + c*0x30c, *(0x005f2770) -> a fake slot-state table (entry c at +0x34 + c*4).
Each row starts with fninit + fldcw <row cw> (0x007f = PC_24 as the race loop, 0x027f = PC_53).

Usage: py -3.12 re/tools/aileaf_unicorn.py <out.tsv> [--corrupt N] [--cw HEX]
       py -3.12 re/tools/aileaf_unicorn.py x --check <liverows.tsv>   (G-AILEAF-LIVE)   (--corrupt flips N rows' expected ret: the
negative control for the exe replay)
"""
import ctypes, math, random, struct, sys

import numpy as np
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UcError
from unicorn.x86_const import (UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_FPSW, UC_X86_REG_FPTAG,
                               UC_X86_REG_FPCW, UC_X86_REG_FP0)

EXE = "original/MASHED.exe.unpatched"
BASE = 0x400000
SCR = 0x10000000            # matrices, slot table, spline, outputs
STK = 0x20000000
MAGIC = 0x30000000
LUT = 0x40000000
TRAMP = 0x31000000
CWA = 0x31000100
MAT = SCR                   # 4 x 0x40
SLOTT = SCR + 0x400         # fake slot-state table
SPL = SCR + 0x1000          # spline object (0x204)
OUTB = SCR + 0x2000         # look / cand / d570 out
FN = {"570": 0x00414570, "880": 0x00415880, "d570": 0x0046d570}


def lut_tables():
    i = np.arange(0x1000, dtype=np.uint32)
    xb = np.where(i & 0x800, np.uint32(0x7f), np.uint32(0x80)).astype(np.uint32) << np.uint32(23) | (i & 0x7ff) << np.uint32(12)
    x = xb.view(np.float32)
    s = np.sqrt(x).astype(np.float32)
    r = (np.float32(1.0) / s).astype(np.float32)
    sq = s.view(np.uint32) - (((xb + np.uint32(0x800)) >> np.uint32(1)) & np.uint32(0x3fc00000))
    iv = r.view(np.uint32) - ((~(xb + np.uint32(0x800)) >> np.uint32(1)) & np.uint32(0x3fc00000))
    return sq.astype(np.uint32).tobytes(), iv.astype(np.uint32).tobytes()


def load():
    pe = pefile.PE(EXE)
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    size = (pe.OPTIONAL_HEADER.SizeOfImage + 0xfff) & ~0xfff
    uc.mem_map(BASE, size)
    for s in pe.sections:
        uc.mem_write(BASE + s.VirtualAddress, s.get_data()[: max(s.Misc_VirtualSize, 0) or len(s.get_data())])
    for a in (SCR, STK, MAGIC, TRAMP):
        uc.mem_map(a, 0x10000)
    uc.mem_map(LUT, 0x10000)
    sq, iv = lut_tables()
    uc.mem_write(LUT, sq)
    uc.mem_write(LUT + 0x4000, iv)
    uc.mem_write(LUT + 0x8000, struct.pack("<II", LUT, LUT + 0x4000))
    uc.mem_write(0x007d3ff8, struct.pack("<II", LUT + 0x8000, 0))
    stubs = {0x004ae17b: b"\xc3", 0x004d7ff0: b"\x33\xc0\xc3", 0x004d8480: b"\xc3"}
    for a, b in stubs.items():
        uc.mem_write(a, b)
    # trampoline: fninit; fldcw [CWA]; jmp [CWA+4]
    uc.mem_write(TRAMP, b"\xdb\xe3" + b"\xd9\x2d" + struct.pack("<I", CWA) + b"\xff\x25" + struct.pack("<I", CWA + 4))
    return uc


def selftest(uc):
    """The hook mechanics (TOP, tag, pop) against the host CPU: fld y; fld x; fpatan; fstp tbyte; fld1; fstp."""
    a = TRAMP + 0x200
    code = (bytes.fromhex("dd05") + struct.pack("<I", OUTB) + bytes.fromhex("dd05") + struct.pack("<I", OUTB + 8))
    site = a + len(code)
    code += bytes.fromhex("d9f3") + bytes.fromhex("db3d") + struct.pack("<I", OUTB + 0x10)
    end = a + len(code)
    uc.mem_write(a, code)
    uc.hook_add(UC_HOOK_CODE, fpatan_hook, begin=site, end=site)
    rng = random.Random(7)
    bad = 0
    for k in range(1000):
        y, x = rng.uniform(-4, 4), rng.uniform(-4, 4)
        if k % 10 == 0: x = abs(x)
        cw = (0x007f, 0x027f, 0x037f)[k % 3]
        uc.mem_write(OUTB, struct.pack("<dd", y, x))
        uc.mem_write(CWA, struct.pack("<HHI", cw, 0, a))
        uc.emu_start(TRAMP, end, count=50)
        got = bytes(uc.mem_read(OUTB + 0x10, 10))
        exp, _ = native_fpatan(_to80(y), _to80(x), cw)
        top = (uc.reg_read(UC_X86_REG_FPSW) >> 11) & 7
        if got != exp or top != 0: bad += 1
    print(f"selftest: {1000 - bad}/1000 exact (fpatan via host, TOP back to 0)")


def _to80(d):
    """double -> x87 80-bit extended (exact)."""
    b = struct.unpack("<Q", struct.pack("<d", d))[0]
    sign = b >> 63; e = (b >> 52) & 0x7ff; m = b & ((1 << 52) - 1)
    if e == 0 and m == 0: return struct.pack("<QH", 0, sign << 15)
    return struct.pack("<QH", (1 << 63) | (m << 11), (sign << 15) | (e - 1023 + 16383))


def u32(uc, a):
    return struct.unpack("<I", uc.mem_read(a, 4))[0]


class Row:
    pass


# Unicorn (QEMU 5.0 softfloat) computes FPATAN through a C double atan2: it differs from the real x87 in the
# low mantissa bits on 2000/2000 probes (verify/d3_bulk_20261011/aileaf/RESULT_AILEAF.md). The two FPATAN sites
# these bodies reach (0x004233fb in FUN_004233e0, 0x004a33b6 in CRT acos) are executed on the host CPU instead:
# ST1/ST0 are read as 80-bit values, a native x64 thunk runs `fpatan` under the row's control word, the result
# replaces ST1, ST0 is popped, and the native status word's exception flags are OR-ed into FPSW.
FPATAN_SITES = (0x004233fb, 0x004a33b6)
_k32 = ctypes.windll.kernel32
_k32.VirtualAlloc.restype = ctypes.c_void_p
_k32.VirtualAlloc.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_uint32, ctypes.c_uint32]
_NAT = bytes.fromhex(
    "4883ec10"      # sub rsp,16
    "d93c24"        # fnstcw [rsp]
    "41d928"        # fldcw [r8]
    "dbe2"          # fnclex
    "db29"          # fld tbyte [rcx]      ST1 = y
    "db690a"        # fld tbyte [rcx+10]   ST0 = x
    "d9f3"          # fpatan
    "db3a"          # fstp tbyte [rdx]
    "dfe0"          # fnstsw ax
    "66418901"      # mov [r9],ax
    "d92c24"        # fldcw [rsp]
    "4883c410c3")
_buf = _k32.VirtualAlloc(None, 4096, 0x3000, 0x40)
ctypes.memmove(_buf, _NAT, len(_NAT))
_fpatan = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p)(_buf)


def native_fpatan(y80, x80, cw):
    i = ctypes.create_string_buffer(y80 + x80)
    o = ctypes.create_string_buffer(16)
    c = ctypes.create_string_buffer(struct.pack("<H", cw))
    sw = ctypes.create_string_buffer(4)
    _fpatan(ctypes.addressof(i), ctypes.addressof(o), ctypes.addressof(c), ctypes.addressof(sw))
    return o.raw[:10], struct.unpack("<H", sw.raw[:2])[0]


def fp_get(uc, phys):
    m, e = uc.reg_read(UC_X86_REG_FP0 + phys)
    return struct.pack("<QH", m, e & 0xffff)


def fp_set(uc, phys, b80):
    m, e = struct.unpack("<QH", b80)
    uc.reg_write(UC_X86_REG_FP0 + phys, (m, e))


def fpatan_hook(u, addr, size, _):
    sw = u.reg_read(UC_X86_REG_FPSW)
    top = (sw >> 11) & 7
    x80 = fp_get(u, top)
    y80 = fp_get(u, (top + 1) & 7)
    r80, nsw = native_fpatan(y80, x80, u.reg_read(UC_X86_REG_FPCW))
    fp_set(u, (top + 1) & 7, r80)
    tag = u.reg_read(UC_X86_REG_FPTAG)
    u.reg_write(UC_X86_REG_FPTAG, tag | (3 << (2 * top)))
    sw = (sw & ~(7 << 11)) | (((top + 1) & 7) << 11) | (nsw & 0x3f)
    u.reg_write(UC_X86_REG_FPSW, sw)
    u.reg_write(UC_X86_REG_EIP, addr + size)


def main():
    out = sys.argv[1]
    corrupt = int(sys.argv[sys.argv.index("--corrupt") + 1]) if "--corrupt" in sys.argv else 0
    cw_force = int(sys.argv[sys.argv.index("--cw") + 1], 16) if "--cw" in sys.argv else None
    uc = load()
    cur = {"row": None, "dbg": 0}

    def ret_from(u, eax):
        esp = u.reg_read(UC_X86_REG_ESP)
        u.reg_write(UC_X86_REG_EAX, eax & 0xffffffff)
        u.reg_write(UC_X86_REG_EIP, u32(u, esp))
        u.reg_write(UC_X86_REG_ESP, esp + 4)

    def hook(u, addr, size, _):
        r = cur["row"]
        esp = u.reg_read(UC_X86_REG_ESP)
        a1, a2, a3 = u32(u, esp + 4), u32(u, esp + 8), u32(u, esp + 12)
        if addr == 0x0046d4a0:
            if a2 > 0xf: return ret_from(u, 0)
            u.mem_write(a1, struct.pack("<I", MAT + (a2 & 3) * 0x40)); return ret_from(u, 1)
        if addr == 0x0046d6d0:
            if a2 > 0xf: return ret_from(u, 0)
            u.mem_write(a1, struct.pack("<f", r.cars[a2 & 3]["speed"])); return ret_from(u, 1)
        if addr == 0x0046d510:
            if a2 > 0xf: return ret_from(u, 0)
            u.mem_write(a1, struct.pack("<fff", *r.cars[a2 & 3]["at"])); return ret_from(u, 1)
        if addr == 0x0046c7b0:
            if a1 > 0xf: return ret_from(u, 0xffffffff)
            return ret_from(u, r.cars[a1 & 3]["alive"])
        if addr == 0x0046cbb0:
            if a1 > 0xf: return ret_from(u, 0)
            c = r.cars[a1 & 3]
            u.mem_write(a2, struct.pack("<i", c["stA"])); u.mem_write(a3, struct.pack("<i", c["stB"])); return ret_from(u, 1)
        if addr == 0x0046cc10:
            if a2 >= 0x10: return ret_from(u, 0)
            u.mem_write(a1, struct.pack("<i", r.cars[a2 & 3]["fai"])); return ret_from(u, 1)
        if addr in (0x004671a0, 0x004b55a0):
            cur["dbg"] += 1
            return ret_from(u, 0)
    for a in (0x0046d4a0, 0x0046d6d0, 0x0046d510, 0x0046c7b0, 0x0046cbb0, 0x0046cc10, 0x004671a0, 0x004b55a0):
        uc.hook_add(UC_HOOK_CODE, hook, begin=a, end=a)
    for a in FPATAN_SITES:
        uc.hook_add(UC_HOOK_CODE, fpatan_hook, begin=a, end=a)
    if "--selftest" in sys.argv:
        selftest(uc)
        return

    rng = random.Random(20261011)

    def spline_obj(kind):
        n = rng.choice((12, 24, 40, 64))
        pts = []
        if kind == "line":
            ang = rng.uniform(0, 2 * math.pi)
            for k in range(n):
                t = (k - n / 2) * rng.uniform(3.0, 9.0) / 1.0
                pts.append((t * math.cos(ang) + rng.uniform(-0.2, 0.2), t * math.sin(ang) + rng.uniform(-0.2, 0.2)))
        else:
            rad = rng.choice((6.0, 15.0, 40.0, 120.0))
            for k in range(n):
                a = 2 * math.pi * k / n
                pts.append((rad * math.cos(a) + rng.uniform(-0.5, 0.5), rad * math.sin(a) + rng.uniform(-0.5, 0.5)))
        b = bytearray(0x204)
        for k, (x, z) in enumerate(pts):
            struct.pack_into("<ff", b, k * 8, x, z)
        struct.pack_into("<i", b, 0x200, n)
        return bytes(b), pts

    def car(x, z, speed, at, alive, stA, stB, slot, prog, vec, fai):
        return dict(x=x, z=z, speed=speed, at=at, alive=alive, stA=stA, stB=stB, slot=slot, prog=prog, vec=vec, fai=fai)

    def car_bytes(c):
        return struct.pack("<fff fff Iiii f fff i i".replace(" ", ""), c["x"], c["z"], c["speed"], *c["at"],
                           c["alive"], c["stA"], c["stB"], c["slot"], c["prog"], *c["vec"], c["fai"], 0)

    def unit(a):
        return (math.sin(a), 0.0, math.cos(a))

    rows = []

    def run(kind, v, cw, cars, glob, ai_in, look, cand_in, spl=b"", d570_in=b""):
        if cw_force is not None:
            cw = cw_force
        r = Row(); r.cars = cars
        cur["row"] = r
        for c in range(4):
            m = bytearray(0x40)
            struct.pack_into("<f", m, 0x30, cars[c]["x"]); struct.pack_into("<f", m, 0x38, cars[c]["z"])
            uc.mem_write(MAT + c * 0x40, bytes(m))
            uc.mem_write(0x008a96e8 + c * 0x30c, struct.pack("<f", cars[c]["prog"]))
            uc.mem_write(0x008a96dc + c * 0x30c, struct.pack("<fff", *cars[c]["vec"]))
        st = bytearray(0x80)
        for c in range(4):
            struct.pack_into("<i", st, 0x34 + c * 4, cars[c]["slot"])
        uc.mem_write(SLOTT, bytes(st))
        uc.mem_write(0x005f2770, struct.pack("<I", SLOTT))
        g = struct.pack("<iiifii", *glob)
        uc.mem_write(0x007f0fd0, g[0:4]); uc.mem_write(0x0066d6d8, g[4:8]); uc.mem_write(0x007f1008, g[8:12])
        uc.mem_write(0x0089a360, g[12:16]); uc.mem_write(0x0089a368, g[16:20]); uc.mem_write(0x0089a374, g[20:24])
        for c in range(4):
            uc.mem_write(0x0089a4c0 + c * 0x74, bytes(0x74))
        blk = 0x0089a4c0 + v * 0x74
        uc.mem_write(blk, ai_in)
        lookA, candA, dA = OUTB, OUTB + 0x10, OUTB + 0x20
        uc.mem_write(lookA, struct.pack("<ff", *look)); uc.mem_write(candA, struct.pack("<ff", *cand_in))
        uc.mem_write(dA, bytes(4))
        if kind == "570":
            uc.mem_write(SPL, spl)
            args = struct.pack("<IIIIi", MAGIC, SPL, lookA, candA, v)
        elif kind == "880":
            args = struct.pack("<IIIIi", MAGIC, 0, lookA, candA, v)
        else:
            rec9b0 = 0x008815a0 + v * 0xd04 + 0x9b0
            uc.mem_write(0x008815a0 + v * 0xd04, bytes(0xd04))
            uc.mem_write(rec9b0, d570_in[:12]); uc.mem_write(0x008815a0 + v * 0xd04 + 0x9d4, d570_in[12:])
            args = struct.pack("<IIi", MAGIC, dA, v)
        esp = STK + 0x8000
        uc.mem_write(esp, args)
        uc.reg_write(UC_X86_REG_ESP, esp)
        uc.mem_write(CWA, struct.pack("<HHI", cw, 0, FN[kind]))
        try:
            uc.emu_start(TRAMP, MAGIC, count=200000)
        except UcError as ex:
            raise SystemExit(f"{kind} row {len(rows)}: emulation error {ex} at {uc.reg_read(UC_X86_REG_EIP):#x}")
        ret = uc.reg_read(UC_X86_REG_EAX)
        cand_out = bytes(uc.mem_read(candA, 8))
        ai_out = bytes(uc.mem_read(blk, 0x74))
        d_out = bytes(uc.mem_read(dA, 4))
        if kind == "d570":
            ret = ret & 0xffffffff
        rows.append([kind, len(rows) + 1, v, f"{cw:x}", spl.hex(), struct.pack("<ff", *look).hex(),
                     struct.pack("<ff", *cand_in).hex(), b"".join(car_bytes(c) for c in cars).hex(),
                     g.hex(), ai_in.hex(), d570_in.hex(), struct.unpack("<i", struct.pack("<I", ret))[0],
                     cand_out.hex(), ai_out.hex(), d_out.hex()])
        return ret, cand_out, ai_in != ai_out

    if "--check" in sys.argv:
        # G-AILEAF-LIVE: rows captured in the exe (MASHED_AILEAF_LIVEROWS) -> the same inputs through the original
        # bytes; compare ret / cand / AI block with what the exe's naked body produced.
        path = sys.argv[sys.argv.index("--check") + 1]
        gates = {}
        if "--gates" in sys.argv:
            # diagnostic: ST0 at FUN_00414570's curvature compare 0x004145d8 (folded curvature vs 20.0) and at
            # 0x00414878 (acos result, radians; the gate is *57.2958 < 30.0 at 0x00414878..0x00414888).
            def g80(u):
                top = (u.reg_read(UC_X86_REG_FPSW) >> 11) & 7
                m, e = u.reg_read(UC_X86_REG_FP0 + top)
                sign = -1.0 if e & 0x8000 else 1.0
                return 0.0 if m == 0 else sign * m * 2.0 ** ((e & 0x7fff) - 16383 - 63)
            def gh(u, addr, size, _):
                gates["curv" if addr == 0x004145d8 else "acos_deg"] = g80(u) * (57.29578 if addr == 0x00414878 else 1.0)
            for a in (0x004145d8, 0x00414878):
                uc.hook_add(UC_HOOK_CODE, gh, begin=a, end=a)
        n = ok = rbad = cbad = abad = 0
        byret = {}
        for ln in open(path):
            k = ln.rstrip("\n").split("\t")
            if len(k) < 15 or k[0] != "570":
                continue
            v = int(k[2]); cw = int(k[3], 16)
            cb = bytes.fromhex(k[7]); cars = []
            for c in range(4):
                f = struct.unpack_from("<ffffffIiiiffffii", cb, c * 64)
                cars.append(car(f[0], f[1], f[2], f[3:6], f[6], f[7], f[8], f[9], f[10], f[11:14], f[14]))
            glob = struct.unpack("<iiifii", bytes.fromhex(k[8]))
            look = struct.unpack("<ff", bytes.fromhex(k[5])); cin = struct.unpack("<ff", bytes.fromhex(k[6]))
            gates.clear()
            ret, cand_out, _ = run("570", v, cw, cars, glob, bytes.fromhex(k[9]), look, cin, spl=bytes.fromhex(k[4]))
            if "--gates" in sys.argv:
                print(f"  gates n={k[1]} v={v} ret={struct.unpack('<i', struct.pack('<I', ret))[0]} curv={gates.get('curv')} (gate < 20) acos_deg={gates.get('acos_deg')} (gate < 30)")
            ret = struct.unpack("<i", struct.pack("<I", ret))[0]
            n += 1
            rr = ret == int(k[11]); cc = cand_out.hex() == k[12]; aa = rows[-1][13] == k[13]
            rbad += not rr; cbad += not cc; abad += not aa; ok += rr and cc and aa
            byret[(int(k[11]), ret)] = byret.get((int(k[11]), ret), 0) + 1
            if not (rr and cc and aa) and n - ok <= 10:
                print(f"  MISMATCH n={k[1]} v={v} cw={cw:x} exe ret={k[11]} orig ret={ret} exe cand={k[12]} orig cand={cand_out.hex()}")
        print(f"G-AILEAF-LIVE rows={n} exact={ok} ret_bad={rbad} cand_bad={cbad} ai_bad={abad} (exe_ret, orig_ret) counts={byret}")
        return

    arms = {}

    def arm(k):
        arms[k] = arms.get(k, 0) + 1

    # ---- FUN_00414570 (mode-1 candidate) ----
    for it in range(2400):
        v = rng.randrange(4)
        cw = 0x007f if it % 2 == 0 else 0x027f
        spl, pts = spline_obj("line" if rng.random() < 0.7 else "circle")
        k0 = rng.randrange(len(pts))
        ox, oz = pts[k0][0] + rng.uniform(-0.3, 0.3), pts[k0][1] + rng.uniform(-0.3, 0.3)
        h = rng.uniform(0, 2 * math.pi)
        nodes = rng.choice((30, 12))
        own_p = rng.uniform(0.0, nodes)
        cars = []
        for c in range(4):
            if c == v:
                cars.append(car(ox, oz, rng.choice((3000.0, 2600.0, 2000.0)), unit(h), 1, 0, 0, 1, own_p, (0, 0, 1), 4))
                continue
            d = rng.choice((1.0, 3.0, 5.0, 6.9, 7.5, 12.0))
            off = rng.choice((0.0, 0.1, 0.3, 0.8, 1.5, 3.0))
            cx, cz = ox + d * math.sin(h + off), oz + d * math.cos(h + off)
            dp = rng.choice((0.5, 1.5, 1.99, 2.5, -0.4, -(nodes - 5) - 0.2, -(nodes - 5) + 0.3))
            cars.append(car(cx, cz, rng.choice((3000.0, 2700.0, 2400.0, 1500.0)), unit(h), rng.choice((1, 1, 1, 0)),
                            rng.choice((0, 0, 0, 2)), 0, rng.choice((1, 1, 1, 0, 2)), own_p + dp, (0, 0, 1), 4))
        ai = bytearray(0x74)
        struct.pack_into("<i", ai, 0, rng.choice((0, 1, 5, 10)))
        glob = (rng.choice((0, 0, 8)), nodes, 50, 2.5, 0, 0)
        look = (ox + 5 * math.sin(h), oz + 5 * math.cos(h))
        ret, c_out, ch = run("570", v, cw, cars, glob, bytes(ai), look, (0.0, 0.0), spl=spl)
        arm(f"570 ret={ret}")

    # ---- FUN_00415880 (mode-2 candidate) ----
    for it in range(3000):
        v = rng.randrange(4)
        cw = 0x007f if it % 2 == 0 else 0x027f
        ox, oz = rng.uniform(-50, 50), rng.uniform(-50, 50)
        h = rng.uniform(0, 2 * math.pi)
        nodes = 30
        own_p = rng.uniform(5.0, 25.0)
        tgt = rng.choice([c for c in range(4) if c != v])
        cars = []
        for c in range(4):
            if c == v:
                cars.append(car(ox, oz, rng.choice((2000.0, 800.0)), unit(h), 1, 0, 0, 1, own_p, unit(h), 4))
                continue
            behind = rng.random() < 0.7
            d = rng.choice((0.4, 1.0, 2.0, 2.9, 3.5))
            sgn = -1 if behind else 1
            ang = h + rng.choice((0.0, 0.2, 0.6, 1.2))
            cx, cz = ox + sgn * d * math.sin(ang), oz + sgn * d * math.cos(ang)
            dp = (-rng.choice((0.2, 1.0, 1.9, 2.4))) if behind else rng.choice((0.3, 1.2))
            vec = unit(h + rng.choice((0.0, 0.3, math.pi)))
            cars.append(car(cx, cz, rng.choice((2100.0, 1900.0, 600.0, 400.0, 3600.0)), unit(h + rng.choice((0.0, 0.5))),
                            rng.choice((1, 1, 1, 0)), rng.choice((0, 0, 0, 2)), 0, rng.choice((1, 1, 1, 0)),
                            own_p + dp, vec, rng.choice((4, 4, 4, 3))))
        ai = bytearray(0x74)
        latched = rng.random() < 0.35
        struct.pack_into("<i", ai, 0x1c, 1 if latched else 0)               # 0x0089a4dc
        struct.pack_into("<i", ai, 0x20, tgt if rng.random() < 0.8 else (tgt + 1) % 4)   # 0x0089a4e0
        struct.pack_into("<i", ai, 0x48, rng.choice((0, 30, 100, 28750)))  # 0x0089a508 countdown
        glob = (rng.choice((0, 0, 8)), nodes, rng.choice((50, 0)), rng.choice((2.5, 1.0, 0.6)),
                rng.choice((0, 1, 2, 3)), rng.choice((0, 1, 3)))
        look = (ox + 5 * math.sin(h), oz + 5 * math.cos(h))
        ret, c_out, ch = run("880", v, cw, cars, glob, bytes(ai), look, (0.0, 0.0))
        arm(f"880 ret={ret} latched_in={int(latched)} ai_changed={int(ch)}")

    # ---- FUN_0046d570 ----
    for it in range(600):
        v = rng.randrange(4)
        cw = 0x007f if it % 2 == 0 else 0x027f
        a = [rng.uniform(-3, 3) for _ in range(3)]
        b = [rng.uniform(-3, 3) for _ in range(3)]
        m = rng.random()
        if m < 0.1: b = list(a)                                  # parallel -> acos(+1)
        elif m < 0.2: b = [-x for x in a]                        # anti-parallel -> acos(-1)
        elif m < 0.25: a = [0.0, 0.0, 0.0]                       # zero vector
        cars = [car(0, 0, 0, (0, 0, 1), 1, 0, 0, 1, 0, (0, 0, 0), 0) for _ in range(4)]
        ret, c_out, ch = run("d570", v, cw, cars, (0, 30, 50, 2.5, 0, 0), bytes(0x74), (0.0, 0.0), (0.0, 0.0),
                             d570_in=struct.pack("<6f", *a, *b))
        arm(f"d570 ret={ret}")

    if corrupt:
        for r in rng.sample(rows, corrupt):
            r[11] = r[11] ^ 1
    with open(out, "w", newline="") as f:
        f.write("# G-AILEAF synthetic rows (re/tools/aileaf_unicorn.py); format in Ai/AiLeafTargets.cpp\n")
        for r in rows:
            f.write("\t".join(str(x) for x in r) + "\n")
    print(f"wrote {len(rows)} rows -> {out}; debug-draw calls {cur['dbg']}; corrupt {corrupt}")
    for k in sorted(arms):
        print(f"  {k}: {arms[k]}")


if __name__ == "__main__":
    main()
