"""carlod instrument input: which cars have the brake-light plate lit at each cited frame.

FUN_00420870 shows part 0x11 (4 triangles, texture Advantage, the only 4-triangle visible part of the
Advantage car: evidence/car_parts.txt) when F & 0x800 and F & 0x20 is clear (PREREG_carlod section 1 A),
and F & 0x800 is the controller's brake byte `*(0x007f103c + ctrl*0x4c + 1)` (FUN_0041eeb0's fourth
argument, FUN_0040da50). The standalone's default AI does not produce that byte, so the matched-pose
instrument needs it as an input, like the car poses. The ORIGINAL's committed draw record of each cited
frame says which cars had it: a 4-triangle `Advantage` draw whose box sits on a car.

    py -3.12 brake_from_draws.py            -> brake_rows.json  {"<track>": {"<row>": mask, ...}, ...}

CIRCULARITY, stated: the mask comes from the draws the arm is later scored against, so it can make the
0x11 plates match by construction. It is therefore NOT evidence for the 0x11 gate (that rests on the
disassembly at 0x00420b7d and the live witness, vis_model_check.txt); it only keeps the 0x11 plates from
standing in as a new row that has nothing to do with the visibility table. Bit i = slot i.
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
PLATE_TRI = 4          # part 0x11
NEAR = 3.0             # metres, plate centre to the car frame origin


def main():
    out, report = {}, []
    for t, rows in TRACKS.items():
        trace = list(csv.DictReader(open(MAIN / "log" / "d1r1" / f"orig_{t}" / "trace.csv")))
        out[t] = {}
        for k in rows:
            doc = json.load(open(ROOT / "verify" / "d1r1_20261016" / "evidence" / f"{t}_orig_row_{k:05d}.draws.json"))
            ds = [drawlist3d.norm(e, i) for i, e in enumerate(drawlist3d.cut_frame(doc))]
            plates = [d for d in ds if d["texl"] == "advantage" and d["tri"] == PLATE_TRI and d["bb"]]
            row = trace[k]
            cars = [tuple(float(row[f"F{i}_p{a}"]) for a in "xyz") for i in range(4)]
            mask, rep = 0, []
            for d in plates:
                c = [(d["bb"][a] + d["bb"][a + 3]) / 2 for a in range(3)]
                dist = [math.dist(c, car) for car in cars]
                i = min(range(4), key=lambda j: dist[j])
                rep.append((i, round(dist[i], 2)))
                if dist[i] <= NEAR:
                    mask |= 1 << i
            out[t][str(k)] = mask
            report.append(f"{t:<9}{k:>5} plates={len(plates)} nearest(car,dist)={rep} mask={mask:#x}")
    (HERE / "brake_rows.json").write_text(json.dumps(out, indent=1))
    print("\n".join(report))


if __name__ == "__main__":
    main()
