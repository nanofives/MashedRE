#!/usr/bin/env python3
"""Print, for every track with a camera-man helper (TrackRenderer.cpp kCamMan), the type float
(row[7]) and the guard / range fields of the camera-table rows 0..N the helper reads (entries 1..N).

Tables are the ones `TableFor` in Race/CameraEntries.cpp selects (switch at 0x0044261b):
row = 12 floats at table + 4 + k*0x30, count = (int)table[0]. Type 1 = the 0x00441d40 step,
type 2 = 0x00442440, 0 = 0x00445aa0, anything else has no dispatch arm.

    py -3.12 verify/rprops2_20261016/dump_camtypes.py
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "rprops_20261016"))
from dump_hooktable import load, va2off  # noqa: E402

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
# (Course_Id, name, table VA, N entries the helper loops over)
TRACKS = [
    (0, "arctic", 0x005F82A8, 5), (2, "rouabout", 0x005F91E0, 4), (3, "egypt", 0x005F7E48, 6),
    (11, "sands", 0x005F9368, 6), (25, "superg", 0x005F8980, 6), (30, "training", 0x005F8D50, 3),
    (33, "warzone", 0x005F7BD0, 9), (34, "forest", 0x005F8798, 6), (36, "storm", 0x005F8B68, 6),
    (37, "dump", 0x005F8EA8, 13), (38, "highway", 0x005F8030, 9), (39, "neustein", 0x005F7A48, 5),
]


def main():
    d, ib, secs = load(EXE)
    for cid, name, va, n in TRACKS:
        o = va2off(va, ib, secs)
        cnt = int(struct.unpack_from("<f", d, o)[0])
        types = []
        for k in range(cnt):
            f = struct.unpack_from("<12f", d, o + 4 + k * 0x30)
            types.append(int(f[7]))
        used = types[1:n + 1]
        print(f"{name:9s} id={cid:2d} table=0x{va:08x} count={cnt:2d} N={n:2d} types={types} "
              f"used(1..N)={used} {'ALL TYPE 1' if all(t == 1 for t in used) and len(used) == n else '** NOT ALL TYPE 1 **'}")
        for k in range(1, min(n, cnt - 1) + 1):
            f = struct.unpack_from("<12f", d, o + 4 + k * 0x30)
            print(f"    [{k}] pos=({f[0]:.4g},{f[1]:.4g},{f[2]:.4g}) pitch0={f[3]:.4g} yaw0={f[4]:.4g} roll={f[5]:.4g} "
                  f"c4={f[6]:.4g} type={f[7]:.0f} guard8={f[8]:.0f} d0={f[9]:.4g} d4={f[10]:.4g} c={f[11]:.4g}")


if __name__ == "__main__":
    main()
