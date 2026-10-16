"""rfx R5: re/tools/imgdiff.py mean abs diff of each arm's shot BMP against the original's, 9 frames.

    py -3.12 pixels.py <arm> [<arm> ...]
"""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
WT = Path(__file__).resolve().parents[2]
FRAMES = {"training": [884, 924, 959], "arctic": [1051, 1091, 1126], "city": [761, 801, 836]}


def mean(a, b):
    out = subprocess.run([sys.executable, str(WT / "re" / "tools" / "imgdiff.py"), str(a), str(b)],
                         capture_output=True, text=True).stdout
    m = re.search(r"\(all ([0-9.]+)\)", out)
    return float(m.group(1)) if m else None


def main() -> int:
    arms = sys.argv[1:]
    print("frame          " + "".join(f"{a:>14}" for a in arms))
    for tr, ks in FRAMES.items():
        for k in ks:
            ref = ROOT / "log" / "d1r1" / f"orig_{tr}" / f"orig_row_{k:05d}.bmp"
            vals = []
            for a in arms:
                b = ROOT / "log" / "rfx2" / a / tr / "d1r2" / f"stand_row_{k:05d}.bmp"
                v = mean(ref, b) if b.exists() else None
                vals.append("-" if v is None else f"{v:.2f}")
            print(f"{tr:<8} {k:<5} " + "".join(f"{v:>14}" for v in vals))
    return 0


if __name__ == "__main__":
    sys.exit(main())
