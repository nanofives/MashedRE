#!/usr/bin/env python3
"""Print the per-track camera-entry table rows (FUN_00442600 tables) from MASHED.exe.

Row = 12 dwords at table+4+k*0x30 (CamEntryBuild441b30, Race/CameraEntries.cpp):
  row[0..2] = xyz   (entry +0x3c..+0x44 via MatTranslate, matrix at entry+0xc)
  row[4]    = float at entry+0xb0   (FUN_00442420 returns it)
  row[7]    = type (ftol -> entry+4)
FUN_00442410(n) = entry[n]+0x3c, FUN_00442420(n) = entry[n]+0xb0, entry base 0x008964c0, stride 0xd8.

Usage: py -3.12 verify/rprops_20261016/dump_camrows.py [exe]
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from dump_hooktable import load, va2off  # noqa: E402

TABLES = {  # track(Course_Id) -> table VA, from Race/CameraEntries.cpp TableFor
    0x1e: (0x005F8D50, "training"),
    -1: (0x005F82A8, "default arm (Arctic Course_Id 0 and all others without a table)"),
    0x1a: (0x005F8490, "city"),
}


def main():
    exe = sys.argv[1] if len(sys.argv) > 1 else \
        r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
    d, ib, secs = load(exe)
    for trk, (va, name) in TABLES.items():
        o = va2off(va, ib, secs)
        cnt = int(struct.unpack_from('<f', d, o)[0])
        print(f"track {trk} table 0x{va:08x} {name}: count={cnt}")
        for k in range(cnt):
            w = struct.unpack_from('<12I', d, o + 4 + k * 0x30)
            f = struct.unpack_from('<12f', d, o + 4 + k * 0x30)
            print(f"  [{k}] pos=({f[0]:.6g}, {f[1]:.6g}, {f[2]:.6g}) "
                  f"r3={f[3]:.6g} yaw_r4={f[4]:.6g} r5={f[5]:.6g} r6={f[6]:.6g} "
                  f"type={f[7]:.6g} r8={f[8]:.6g} r9={f[9]:.6g}")


if __name__ == '__main__':
    main()
