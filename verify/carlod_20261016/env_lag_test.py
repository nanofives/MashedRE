"""carlod: does the original's env coefficient (and LOD state) use this frame's camera and car, or last frame's?

For each of the 9 cited frames x 4 cars: OBSERVED = the original draws env for that car (a draw with
stage-1 texture CarGloss nearest to it; FUN_005412d0 draws plain at coefficient byte 0). PREDICTED from
d2 = |car frame origin - camera|^2 of the trace rows (part 0x13's frame origin is the car's, see
evidence/vislog note), for the four (camera row, car row) combinations: the env coefficient is nonzero when
lerp * (1 - clamp(d2, 0, 100) / 100) * 255 rounds to >= 1 (lerp from the vislog, ~0.645).

    py -3.12 env_lag_test.py
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
LERP = 0.6455


def pos(row, pre):
    return [float(row[f"{pre}_p{a}"]) for a in "xyz"]


def main():
    combos = {"cam K,   car K": (0, 0), "cam K-1, car K": (1, 0), "cam K,   car K-1": (0, 1), "cam K-1, car K-1": (1, 1)}
    wrong = {k: 0 for k in combos}
    total = 0
    for t, rows in TRACKS.items():
        trace = list(csv.DictReader(open(MAIN / "log" / "d1r1" / f"orig_{t}" / "trace.csv")))
        for k in rows:
            doc = json.load(open(ROOT / "verify" / "d1r1_20261016" / "evidence" / f"{t}_orig_row_{k:05d}.draws.json"))
            ds = [drawlist3d.norm(e, i) for i, e in enumerate(drawlist3d.cut_frame(doc))]
            cars = [pos(trace[k], f"F{i}") for i in range(4)]
            obs = [0] * 4
            for d in ds:
                if d["tex1l"] == "cargloss" and d["bb"]:
                    c = [(d["bb"][a] + d["bb"][a + 3]) / 2 for a in range(3)]
                    obs[min(range(4), key=lambda j: math.dist(c, cars[j]))] += 1
            for i in range(4):
                total += 1
                line = []
                for name, (cl, rl) in combos.items():
                    cam = pos(trace[k - cl], "M")
                    car = pos(trace[k - rl], f"F{i}")
                    d2 = sum((a - b) ** 2 for a, b in zip(car, cam))
                    byte = int(round(LERP * (1 - min(max(d2, 0), 100) / 100) * 255)) & 0xff
                    pred = byte > 0
                    if pred != (obs[i] > 0):
                        wrong[name] += 1
                    line.append(f"{d2:7.1f}")
                print(f"{t:<9}{k:>5} car{i} observed_env={obs[i] > 0!s:<5} d2 " + " | ".join(line))
    print("\nmispredicted of", total, "car-frames:")
    for name in combos:
        print(f"  {name}: {wrong[name]}")


if __name__ == "__main__":
    main()
