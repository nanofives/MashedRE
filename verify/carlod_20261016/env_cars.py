"""carlod: which cars carry env draws in the ORIGINAL at a cited frame, against each car's distance.

For one frame of one track: the original's draws whose stage-1 texture is CarGloss (single pass) or whose
texture is CarGloss (second pass), each assigned to the nearest car frame origin of the trace row, and every
car's |frame origin - camera|^2. FUN_005412d0 draws plain at coefficient byte 0 (0x00541...: `uVar8 == 0`),
and the coefficient is lerp * (1 - clamp(d2, 0, 100) / 100) (FUN_00420290), so a car with d2 >= 100 has none.

    py -3.12 env_cars.py <track> <row>
"""
import csv
import json
import math
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import drawlist3d  # noqa: E402

MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
t, k = sys.argv[1], int(sys.argv[2])
trace = list(csv.DictReader(open(MAIN / "log" / "d1r1" / f"orig_{t}" / "trace.csv")))
row = trace[k]
cam = [float(row[f"M_p{a}"]) for a in "xyz"]
cars = [tuple(float(row[f"F{i}_p{a}"]) for a in "xyz") for i in range(4)]
doc = json.load(open(ROOT / "verify" / "d1r1_20261016" / "evidence" / f"{t}_orig_row_{k:05d}.draws.json"))
ds = [drawlist3d.norm(e, i) for i, e in enumerate(drawlist3d.cut_frame(doc))]
print(f"{t} row {k}: car |origin - camera|^2:",
      [round(sum((c - m) ** 2 for c, m in zip(car, cam)), 1) for car in cars])
for kind, sel in (("single-pass (stage-1 CarGloss)", lambda d: d["tex1l"] == "cargloss"),
                  ("second pass (texture CarGloss)", lambda d: d["texl"] == "cargloss")):
    cnt = [0] * 4
    for d in ds:
        if sel(d) and d["bb"]:
            c = [(d["bb"][a] + d["bb"][a + 3]) / 2 for a in range(3)]
            cnt[min(range(4), key=lambda j: math.dist(c, cars[j]))] += 1
    print(f"  {kind}: draws per car {cnt}")
