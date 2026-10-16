"""rfx R4 no-op control: the standalone draw records of two arms, compared draw by draw.

    py -3.12 noop_cmp.py <arm_a> <arm_b>

For each of the 9 frames, log/rfx/<arm>/<track>/d1r2/stand_row_<K>.draws.json: the frame's draw
events must be equal in number and, field by field, equal for every key (bb compared to 1e-4).
Prints the first differences per frame and a verdict line.
"""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "tools"))
import drawlist3d as d3  # noqa: E402

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed") / "log" / "rfx2"
FRAMES = {"training": [884, 924, 959], "arctic": [1051, 1091, 1126], "city": [761, 801, 836]}


def eq(a, b):
    if isinstance(a, float) or isinstance(b, float):
        return abs(float(a) - float(b)) <= 1e-4
    if isinstance(a, list) and isinstance(b, list):
        return len(a) == len(b) and all(eq(x, y) for x, y in zip(a, b))
    return a == b


def main() -> int:
    a, b = sys.argv[1], sys.argv[2]
    bad = 0
    for tr, ks in FRAMES.items():
        for k in ks:
            fa = ROOT / a / tr / "d1r2" / f"stand_row_{k:05d}.draws.json"
            fb = ROOT / b / tr / "d1r2" / f"stand_row_{k:05d}.draws.json"
            if not fa.exists() or not fb.exists():
                print(f"{tr} {k}: missing record"); bad += 1; continue
            ea = d3.cut_frame(json.loads(fa.read_text()))
            eb = d3.cut_frame(json.loads(fb.read_text()))
            diffs = []
            if len(ea) != len(eb):
                diffs.append(f"draw count {len(ea)} vs {len(eb)}")
            for i, (x, y) in enumerate(zip(ea, eb)):
                for key in sorted(set(x) | set(y)):
                    if not eq(x.get(key), y.get(key)):
                        diffs.append(f"draw {i} {d3.clean(x.get('tex'))}: {key} {x.get(key)} vs {y.get(key)}")
            print(f"{tr:<9} {k}  draws {len(ea)} / {len(eb)}  {'EQUAL' if not diffs else 'DIFF ' + str(len(diffs))}")
            for d in diffs[:6]:
                print("     ", d[:200])
            bad += 1 if diffs else 0
    print("NO-OP CONTROL:", "PASS 9/9" if bad == 0 else f"FAIL on {bad} frame(s)")
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
