"""Print the state / DAT_0088ff50 transitions of one arm's seekobj capture.   py -3.12 timeline.py O2"""
import csv
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent


def f(u) -> float:
    return struct.unpack("<f", struct.pack("<I", int(u) & 0xFFFFFFFF))[0]


rows = list(csv.DictReader((HERE / f"{sys.argv[1]}.msd.seekobj.csv").open()))
prev = None
for i, r in enumerate(rows):
    key = (r["state_in"], r["state_out"], tuple(r[f"ff50_{v}"] != "0" for v in range(4)))
    if key != prev:
        print(i, r["frame"], "state", r["state_in"], "->", r["state_out"], "ff50", [r[f"ff50_{v}"] for v in range(4)],
              "c8=%.4f" % f(r["c8"]), "pos", [round(f(r[k]), 2) for k in ("px", "py", "pz")])
        prev = key
print("last", len(rows) - 1, rows[-1]["frame"] if rows else None)
