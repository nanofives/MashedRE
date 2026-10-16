#!/usr/bin/env python3
"""Dump the Arctic freighter / fishing-boat placement tables and tick constants (read-only).

Tables (decomp of 0x00448770 / 0x00448880 / 0x00448cb0 / 0x00448e20):
  freighter positions  0x005f95d0 + i*0xc  (i < 2)   angles 0x005f95e8 + i*4
  boat positions       0x005f95f0 + i*0xc  (i < 4)   angles 0x005f9620 + i*4
Constants read by the tick helpers (floats): 0x005cd030 0x005ce1d8 0x005ce1f0 0x005ce1c8 0x005ce1f8
accumulator steps 0x005ce18c (0x00683258) 0x005cd18c (0x00683260) 0x005cc328 (0x0068325c)
Accumulator globals 0x00683258 / 0x0068325c / 0x00683260 are .data/.bss: file bytes shown if backed.

Usage: py -3.12 verify/rprops_20261016/dump_ship_tables.py [exe]
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from dump_hooktable import load, va2off  # noqa: E402


def rd(d, ib, secs, va, n=1):
    try:
        o = va2off(va, ib, secs)
    except ValueError:
        return None
    if o + 4 * n > len(d):
        return None
    return struct.unpack_from('<%df' % n, d, o)


def main():
    exe = sys.argv[1] if len(sys.argv) > 1 else \
        r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
    d, ib, secs = load(exe)
    print("freighter positions/angles")
    for i in range(2):
        p = rd(d, ib, secs, 0x005F95D0 + i * 12, 3)
        a = rd(d, ib, secs, 0x005F95E8 + i * 4, 1)
        print(f"  [{i}] pos={p} angle={a}")
    print("boat positions/angles")
    for i in range(4):
        p = rd(d, ib, secs, 0x005F95F0 + i * 12, 3)
        a = rd(d, ib, secs, 0x005F9620 + i * 4, 1)
        print(f"  [{i}] pos={p} angle={a}")
    print("constants")
    for va, nm in [(0x005CD030, 'rot amp (cos roll / sin pitch)'),
                   (0x005CE1D8, 'freighter bob amp'), (0x005CE1F0, 'freighter bob offs'),
                   (0x005CE1C8, 'boat bob amp'), (0x005CE1F8, 'boat bob offs'),
                   (0x005CE18C, 'step 0x00683258'), (0x005CD18C, 'step 0x00683260'),
                   (0x005CC328, 'step 0x0068325c'), (0x005CC9C0, 'step 0x00683250'),
                   (0x005CD0C8, 'bias on smoke bob'), (0x005CE200, 'scale on smoke bob')]:
        v = rd(d, ib, secs, va, 1)
        raw = struct.unpack_from('<I', d, va2off(va, ib, secs))[0] if v else None
        print(f"  0x{va:08x} {nm:32s} = {v[0] if v else None!r}  raw={raw:#010x}" if raw is not None else f"  0x{va:08x} {nm} unbacked")
    print("accumulators (file bytes; absent = BSS zero)")
    for va in (0x00683250, 0x00683258, 0x0068325C, 0x00683260):
        v = rd(d, ib, secs, va, 1)
        print(f"  0x{va:08x} = {v}")


if __name__ == '__main__':
    main()
