#!/usr/bin/env python3
"""Round-restart grid Stage A, gate G-RG-SNAP: FUN_00409290 (and its callees FUN_00426bc0 / FUN_00426bd0)
executed verbatim from original/MASHED.exe.unpatched in Unicorn over synthetic Safe_Start_Lines tables.

Brief: verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md. The exe replays the rows through its naked
RG2_409290 (MASHED_ROUND_GRID_REPLAY, mashedmod/src/mashed_re/Race/RoundGrid.cpp).

Row: n  node_in  count  table(34 x (u16 lo, u16 hi) = 136 B hex)  node_out
Inputs: count at 0x0066d6e0, the table at 0x0066d658; node_in in an int at a scratch address passed as arg 0.
Nodes cover: below the first range, at every lo / hi, inside, between ranges, past the last range, negatives.
Usage: py -3.12 re/tools/roundgrid_unicorn.py <out.tsv> [--corrupt N]
"""
import random, struct, sys

import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP

EXE = "original/MASHED.exe.unpatched"
BASE = 0x400000
STK, MAGIC, SCR = 0x20000000, 0x30000000, 0x10000000


def main():
    out = sys.argv[1]
    corrupt = int(sys.argv[sys.argv.index("--corrupt") + 1]) if "--corrupt" in sys.argv else 0
    pe = pefile.PE(EXE)
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    uc.mem_map(BASE, (pe.OPTIONAL_HEADER.SizeOfImage + 0xfff) & ~0xfff)
    for s in pe.sections:
        uc.mem_write(BASE + s.VirtualAddress, s.get_data()[: max(s.Misc_VirtualSize, 0) or len(s.get_data())])
    for a in (STK, MAGIC, SCR):
        uc.mem_map(a, 0x10000)
    rng = random.Random(20261012)
    rows = []
    n = 0
    for t in range(400):
        cnt = rng.choice((0, 1, 2, 3, 5, 8, 12, 34))
        tab = bytearray(136)
        lo = rng.randrange(0, 4)
        pairs = []
        for i in range(cnt):
            a = lo + rng.randrange(0, 4)
            b = a + rng.randrange(0, 5)
            pairs.append((a, b)); lo = b + 1 + rng.randrange(0, 3)
            struct.pack_into("<HH", tab, i * 4, a, b)
        for i in range(cnt, 34):                         # trailing garbage beyond the count is never read
            struct.pack_into("<HH", tab, i * 4, rng.randrange(0, 0x10000), rng.randrange(0, 0x10000))
        cands = {-5, -1, 0, 1, 60, 255, 0x7fff}
        for a, b in pairs:
            cands |= {a - 1, a, a + 1, b - 1, b, b + 1}
        for node in sorted(cands):
            uc.mem_write(0x0066d658, bytes(tab))
            uc.mem_write(0x0066d6e0, struct.pack("<i", cnt))
            uc.mem_write(SCR, struct.pack("<i", node))
            esp = STK + 0x8000
            uc.mem_write(esp, struct.pack("<II", MAGIC, SCR))
            uc.reg_write(UC_X86_REG_ESP, esp)
            uc.emu_start(0x00409290, MAGIC, count=10000)
            got = struct.unpack("<i", uc.mem_read(SCR, 4))[0]
            n += 1
            rows.append([n, node, cnt, bytes(tab).hex(), got])
    if corrupt:
        for r in rng.sample(rows, corrupt):
            r[4] += 1
    with open(out, "w", newline="") as f:
        f.write("# G-RG-SNAP rows (re/tools/roundgrid_unicorn.py): n node_in count table node_out\n")
        for r in rows:
            f.write(" ".join(str(x) for x in r) + "\n")
    changed = sum(1 for r in rows if r[1] != r[4])
    print(f"wrote {len(rows)} rows -> {out}; node changed by the snap on {changed}; corrupt {corrupt}")


if __name__ == "__main__":
    main()
