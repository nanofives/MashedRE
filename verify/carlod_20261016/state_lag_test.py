"""carlod: which camera / car rows predict the original's LOD state (S = 1 when d2 < 144, else 2)?

OBSERVED per car and cited frame: S = 2 when the original draws a far-LOD part (texture AdvantageSmall,
>= 600 triangles: parts 0x64..0x67, evidence/car_parts.txt) on that car, else 1. PREDICTED from
d2 = |car frame origin - camera|^2 over (camera row offset, car row offset) in K-0..K-3 (part 0x13's frame
origin is the car's, evidence/env_lag_test.txt note).

    py -3.12 state_lag_test.py
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
TRACKS = {"training": [884, 924, 959], "arctic": [1051, 1091, 1126], "city": [761, 801, 836]}


def pos(row, pre):
    return [float(row[f"{pre}_p{a}"]) for a in "xyz"]


def main():
    combos = [(c, r) for c in range(4) for r in range(4)]
    wrong = {cr: [] for cr in combos}
    total = 0
    for t, rows in TRACKS.items():
        trace = list(csv.DictReader(open(MAIN / "log" / "d1r1" / f"orig_{t}" / "trace.csv")))
        for k in rows:
            doc = json.load(open(ROOT / "verify" / "d1r1_20261016" / "evidence" / f"{t}_orig_row_{k:05d}.draws.json"))
            ds = [drawlist3d.norm(e, i) for i, e in enumerate(drawlist3d.cut_frame(doc))]
            cars = [pos(trace[k], f"F{i}") for i in range(4)]
            far = [0] * 4
            for d in ds:
                if d["texl"] == "advantagesmall" and (d["tri"] or 0) >= 600 and d["bb"]:
                    c = [(d["bb"][a] + d["bb"][a + 3]) / 2 for a in range(3)]
                    far[min(range(4), key=lambda j: math.dist(c, cars[j]))] += 1
            for i in range(4):
                total += 1
                obs = 2 if far[i] else 1
                for (cl, rl) in combos:
                    cam = pos(trace[k - cl], "M")
                    car = pos(trace[k - rl], f"F{i}")
                    d2 = sum((a - b) ** 2 for a, b in zip(car, cam))
                    if (1 if d2 < 144.0 else 2) != obs:
                        wrong[(cl, rl)].append(f"{t}{k}c{i}(d2={d2:.1f},obs=S{obs})")
    print(f"car-frames {total}; mispredicted by (camera row = K-a, car row = K-b):")
    for (cl, rl), w in wrong.items():
        print(f"  cam K-{cl}, car K-{rl}: {len(w)} {w if len(w) <= 3 else ''}")


if __name__ == "__main__":
    main()
