#!/usr/bin/env python3
"""Round-restart grid Stage B oracle: the per-car grid slot value FUN_0040e590 passes to FUN_00408b00, from the
original bytes in Unicorn, exhaustively over the rank codes DAT_008a9520[0..3] (FUN_0040b410).

Brief: verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md (Stage B). FUN_0040e590 (0x0040e590..0x0040eec8) runs
verbatim with FUN_0040b410 / FUN_0042f500 / FUN_0046c6d0 (pure getters) verbatim; every other callee is a hook
that returns 0 (cdecl, caller cleans). FUN_00408b00's hook records its 2nd argument (the slot value) per call, in
call order = the active slots 0..3.
Globals set: DAT_008a94d0 (mode, the D3 Training value 4 unless --mode), DAT_007f0fd0 (rule 0), DAT_0067ea64 0
(FUN_0042f500), DAT_007f1a14 + i*0x10 = i (slots enabled), *(0x005f2770) -> a table with +0x34 + i*4 = 1 (active).
Row: c0 c1 c2 c3 -> slot values s0 s1 s2 s3.
Usage: py -3.12 re/tools/slotorder_unicorn.py <out.tsv> [--mode N] [--corrupt N]
"""
import itertools, random, struct, sys

import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_EIP

EXE = "original/MASHED.exe.unpatched"
BASE = 0x400000
STK, MAGIC, SCR = 0x20000000, 0x30000000, 0x10000000
FN = 0x0040e590
STUBS = [0x408a70, 0x408b00, 0x409290, 0x40b250, 0x40e3a0, 0x41ede0, 0x41ee50, 0x41ef80, 0x41f000, 0x429820,
         0x430790, 0x45ba00, 0x46b1c0, 0x46b540, 0x46baa0, 0x46d4a0, 0x4704c0, 0x48f680, 0x48f740]
CODES = (-1000, -2, -1, 0, 1, 2)


def u32(uc, a):
    return struct.unpack("<I", uc.mem_read(a, 4))[0]


def main():
    out = sys.argv[1]
    mode = int(sys.argv[sys.argv.index("--mode") + 1]) if "--mode" in sys.argv else 4
    corrupt = int(sys.argv[sys.argv.index("--corrupt") + 1]) if "--corrupt" in sys.argv else 0
    pe = pefile.PE(EXE)
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    uc.mem_map(BASE, (pe.OPTIONAL_HEADER.SizeOfImage + 0xfff) & ~0xfff)
    for s in pe.sections:
        uc.mem_write(BASE + s.VirtualAddress, s.get_data()[: max(s.Misc_VirtualSize, 0) or len(s.get_data())])
    for a in (STK, MAGIC, SCR):
        uc.mem_map(a, 0x10000)
    rec = []

    def hook(u, addr, size, _):
        esp = u.reg_read(UC_X86_REG_ESP)
        if addr == 0x408b00:
            rec.append(struct.unpack("<i", u.mem_read(esp + 8, 4))[0])
        u.reg_write(UC_X86_REG_EAX, 0)
        u.reg_write(UC_X86_REG_EIP, u32(u, esp))
        u.reg_write(UC_X86_REG_ESP, esp + 4)
    for a in STUBS:
        uc.hook_add(UC_HOOK_CODE, hook, begin=a, end=a)
    tab = SCR + 0x100
    rows = []
    for codes in itertools.product(CODES, repeat=4):
        uc.mem_write(0x008a94d0, struct.pack("<i", mode))
        uc.mem_write(0x007f0fd0, struct.pack("<i", 0))
        uc.mem_write(0x0067ea64, struct.pack("<i", 0))
        uc.mem_write(0x008a94b0, struct.pack("<i", 1))
        for i in range(4):
            uc.mem_write(0x007f1a14 + i * 0x10, struct.pack("<i", i))
            uc.mem_write(0x008a9520 + i * 4, struct.pack("<i", codes[i]))
            uc.mem_write(tab + 0x34 + i * 4, struct.pack("<i", 1))
        uc.mem_write(0x005f2770, struct.pack("<I", tab))
        rec.clear()
        esp = STK + 0x8000
        uc.mem_write(esp, struct.pack("<I", MAGIC))
        uc.reg_write(UC_X86_REG_ESP, esp)
        uc.emu_start(FN, MAGIC, count=200000)
        if len(rec) != 4:
            raise SystemExit(f"codes {codes}: {len(rec)} FUN_00408b00 calls")
        rows.append(list(codes) + rec[:])
    if corrupt:
        rng = random.Random(5)
        for r in rng.sample(rows, corrupt):
            r[4] ^= 1
    with open(out, "w", newline="") as f:
        f.write(f"# Stage B slot-order oracle (re/tools/slotorder_unicorn.py), mode {mode}: c0 c1 c2 c3 s0 s1 s2 s3\n")
        for r in rows:
            f.write(" ".join(str(x) for x in r) + "\n")
    print(f"wrote {len(rows)} rows -> {out} (mode {mode}); distinct slot tuples {len({tuple(r[4:]) for r in rows})}")


if __name__ == "__main__":
    main()
