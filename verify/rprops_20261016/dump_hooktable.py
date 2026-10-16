#!/usr/bin/env python3
"""Dump the per-track hook table at 0x005f33f8 of MASHED.exe (read-only).

Record layout (memory per-track-hook-table-0x005f33f8): stride 0x48 =
  {char name[0x10]; u32 Course_Id; void* slot[13]}, count at [0x005f37a0].

Usage: py -3.12 verify/rprops_20261016/dump_hooktable.py [exe]
Default exe is original/MASHED.exe.unpatched of the MAIN checkout (read-only).
"""
import struct
import sys
from pathlib import Path

TABLE = 0x005F33F8
COUNT_VA = 0x005F37A0
STRIDE = 0x48


def load(path):
    d = Path(path).read_bytes()
    e = struct.unpack_from('<I', d, 0x3C)[0]
    coff = e + 4
    nsec, = struct.unpack_from('<H', d, coff + 2)
    opt = coff + 20
    sect_off = opt + struct.unpack_from('<H', d, coff + 16)[0]
    ib, = struct.unpack_from('<I', d, opt + 28)
    secs = []
    for i in range(nsec):
        so = sect_off + i * 40
        vsz, vrva, rsz, rptr = struct.unpack_from('<IIII', d, so + 8)
        secs.append((vrva, vsz, rptr, rsz))
    return d, ib, secs


def va2off(va, ib, secs):
    rva = va - ib
    for vrva, vsz, rptr, rsz in secs:
        if vrva <= rva < vrva + max(vsz, rsz):
            return rptr + (rva - vrva)
    raise ValueError(hex(va))


def main():
    exe = sys.argv[1] if len(sys.argv) > 1 else \
        r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
    d, ib, secs = load(exe)
    cnt, = struct.unpack_from('<I', d, va2off(COUNT_VA, ib, secs))
    print(f"count [0x{COUNT_VA:08x}] = {cnt}")
    for i in range(cnt):
        va = TABLE + i * STRIDE
        o = va2off(va, ib, secs)
        name = d[o:o + 0x10].split(b'\0')[0].decode('latin1')
        cid, = struct.unpack_from('<I', d, o + 0x10)
        slots = struct.unpack_from('<13I', d, o + 0x14)
        print(f"[{i:2d}] va=0x{va:08x} {name:<10s} Course_Id={cid:<3d} " +
              " ".join(f"{s:08x}" for s in slots))


if __name__ == '__main__':
    main()
