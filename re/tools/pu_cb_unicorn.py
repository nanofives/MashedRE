#!/usr/bin/env python3
"""Mode 7 leg M3: synthetic arm coverage for the pickup callback FUN_00458920, original bytes in Unicorn.

Runs FUN_00458920 (original/MASHED.exe.unpatched, version anchor in CLAUDE.md) on a grid of synthetic
inputs that reaches every arm, and writes the rows in the PK format of scenario_launch.py --cb-probe
(<out>.cb.tsv), so the exe's replay (MASHED_PKCB_REPLAY, Gameplay/PickupTick.cpp) reads both the live
original capture and this file with one parser.

Executed verbatim in the emulator: FUN_00458920, FUN_00405890, FUN_00407580, FUN_0040e350, FUN_0045c330,
FUN_0045bff0, FUN_00458570, FUN_0046d4a0. Their inputs are set as globals: DAT_007f0fd0, DAT_0063a5d0,
DAT_0063a5d4, DAT_00639dc4, DAT_0063ba8c, slot +0xa8 at 0x0088fc88 + v*0xb4.
Stubbed (bytes overwritten in the emulator's private copy; calls recorded from the stack):
  FUN_004588c0 (burst particles) ret, FUN_0045c010 (grant) ret, FUN_00465ca0 (sfx) ret,
  FUN_004c3ac0 / FUN_004c39b0 (RW length / normalize, LUT through RwGlobals, which is NULL in a static
  image) fldz; ret. Both only feed FUN_00458570's output, which FUN_00458920 discards (fstp st(0) at
  0x004589e2; the ESI out buffer is a dead local at [esp0-0xc]).
x87 control word PC_24 (0x007f), as the original's race loop (RESULT_P2 section 1).

Usage: py -3.12 re/tools/pu_cb_unicorn.py <out.tsv>
"""
import itertools, random, struct, sys

import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UcError
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_FPCW

EXE = "original/MASHED.exe.unpatched"
BASE = 0x400000
FN = 0x00458920
POOL = 0x0068b198
SLOTS = 0x006dccb8
SCR = 0x10000000          # fake RW objects / frames
STK = 0x20000000
MAGIC = 0x30000000


def load():
    pe = pefile.PE(EXE)
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    size = (pe.OPTIONAL_HEADER.SizeOfImage + 0xfff) & ~0xfff
    uc.mem_map(BASE, size)
    for s in pe.sections:
        uc.mem_write(BASE + s.VirtualAddress, s.get_data()[: max(s.Misc_VirtualSize, 0) or len(s.get_data())])
    uc.mem_map(SCR, 0x10000)
    uc.mem_map(STK, 0x10000)
    uc.mem_map(MAGIC, 0x1000)
    stubs = {0x004588c0: b"\xc3", 0x0045c010: b"\xc3", 0x00465ca0: b"\xc3",
             0x004c3ac0: b"\xd9\xee\xc3", 0x004c39b0: b"\xd9\xee\xc3"}
    for a, b in stubs.items():
        uc.mem_write(a, b)
    return uc


def u32(uc, a):
    return struct.unpack("<I", uc.mem_read(a, 4))[0]


def main():
    out = sys.argv[1]
    uc = load()
    wit = []

    def hook(u, addr, size, _):
        esp = u.reg_read(UC_X86_REG_ESP)
        if addr == 0x0045c010:
            wit.append(f"c010:{u32(u, esp + 4)}:{u32(u, esp + 8)}")
        elif addr == 0x004588c0:
            wit.append("88c0")
        elif addr == 0x00465ca0:
            wit.append(f"5ca0:{u32(u, esp + 4)}")
        elif addr == 0x00458570:
            wit.append(f"8570:{u.reg_read(UC_X86_REG_EAX)}")
    for a in (0x0045c010, 0x004588c0, 0x00465ca0, 0x00458570):
        uc.hook_add(UC_HOOK_CODE, hook, begin=a, end=a)

    rng = random.Random(20261010)
    grid = itertools.product(
        (7, 9),                                   # slot_a type
        (1, 2, 0),                                # pickup state +0x20
        (0, 5),                                   # DAT_007f0fd0
        ((0, 0), (2, 2), (2, 1)),                 # DAT_0063a5d0, DAT_0063a5d4
        (0x0a, 0x11, 0xfeff, 0xff00, 0xff01, 0xff02, 0xff03, 0xff07, 0xff08),  # slot_b type
        (0x0a, 0x0b),                             # DAT_00639dc4 (FUN_00407580(0))
        (6, 3, 5),                                # DAT_0063ba8c (FUN_0040e350)
        (0, 0x005f9998))                          # slot +0xa8 of car v
    rows = []
    n = 0
    for ta, st, fd0, (a5d0, a5d4), tb, dc4, ba8c, a8 in grid:
        n += 1
        idx = n % 25
        ent = POOL + idx * 0x50
        e = bytearray(0x50)
        obj = SCR + idx * 0x100
        frame = obj + 0x10
        struct.pack_into("<I", e, 0x00, obj)
        struct.pack_into("<f", e, 0x18, rng.choice((120.0, 30.0, 7.5)))
        struct.pack_into("<f", e, 0x1c, rng.uniform(-5.0, 120.0))
        struct.pack_into("<i", e, 0x20, st)
        struct.pack_into("<i", e, 0x24, rng.choice((7, 9, 0x0c, 0x13)))
        for k in range(3):
            struct.pack_into("<f", e, 0x2c + 4 * k, rng.uniform(-50.0, 50.0))
            struct.pack_into("<f", e, 0x38 + 4 * k, rng.uniform(-50.0, 50.0))
            struct.pack_into("<f", e, 0x44 + 4 * k, rng.uniform(-1.0, 1.0))
        uc.mem_write(ent, bytes(e))
        uc.mem_write(obj, struct.pack("<II", 0, frame))
        uc.mem_write(frame + 0x40, bytes(e[0x38:0x44]))
        ia, ib = 4 + idx % 5, idx % 4
        sa = bytearray(0x20); sb = bytearray(0x20)
        struct.pack_into("<fffI", sa, 0, *struct.unpack_from("<fff", e, 0x38), 0x3f000000)
        struct.pack_into("<I", sa, 0x10, ta); struct.pack_into("<I", sa, 0x14, ent)
        struct.pack_into("<I", sa, 0x1c, FN)
        struct.pack_into("<fffI", sb, 0, rng.uniform(-50, 50), 0.5, rng.uniform(-50, 50), 0x3f088c2d)
        struct.pack_into("<I", sb, 0x10, tb)
        pa, pb = SLOTS + ia * 0x8c, SLOTS + ib * 0x8c
        uc.mem_write(pa, bytes(sa) + bytes(0x8c - 0x20))
        uc.mem_write(pb, bytes(sb) + bytes(0x8c - 0x20))
        uc.mem_write(0x007f0fd0, struct.pack("<i", fd0))
        uc.mem_write(0x0063a5d0, struct.pack("<ii", a5d0, a5d4))
        uc.mem_write(0x00639dc4, struct.pack("<i", dc4))
        uc.mem_write(0x0063ba8c, struct.pack("<i", ba8c))
        v = tb - 0xff00 if 0xff00 <= tb <= 0xff07 else -1
        for c in range(8):
            uc.mem_write(0x0088fc88 + c * 0xb4, struct.pack("<I", a8 if c == v else 0))
        esp = STK + 0x8000
        uc.mem_write(esp, struct.pack("<III", MAGIC, pa, pb))
        uc.reg_write(UC_X86_REG_ESP, esp)
        uc.reg_write(UC_X86_REG_FPCW, 0x007f)
        wit.clear()
        try:
            uc.emu_start(FN, MAGIC, count=20000)
        except UcError as ex:
            raise SystemExit(f"vector {n}: emulation error {ex}")
        e_out = bytes(uc.mem_read(ent, 0x50))
        a8s = f"{a8:x}" if v >= 0 else ""
        rows.append(["PK", n, -1, ia, ib, f"{ta:x}", f"{tb:x}", fd0, ba8c, a8s, a8s, a5d0, a5d4, dc4,
                     -1, "", idx, e.hex(), sa.hex(), sb.hex(), e_out.hex(), "", a8s, a8s, "|".join(wit)])
    with open(out, "w", newline="") as f:
        f.write("# PK synthetic (pu_cb_unicorn.py): same columns as scenario_launch.py --cb-probe PK rows\n")
        for r in rows:
            f.write("\t".join(str(x) for x in r) + "\n")
    arms = {}
    for r in rows:
        w = r[-1]
        e_in, e_o = bytes.fromhex(r[17]), bytes.fromhex(r[20])
        if "c010" in w: k = "collect"
        elif "8570" in w: k = "bump"
        elif e_in != e_o: k = "vanish(state2,t18-1)"
        else: k = "no-op"
        arms[k] = arms.get(k, 0) + 1
    print(f"wrote {len(rows)} rows -> {out}; arms: {arms}")


if __name__ == "__main__":
    main()
