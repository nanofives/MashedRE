"""rfx: rows / red / C04-C08-C09 rows / matched draws per frame for each arm (log/rfx/<arm>).

    py -3.12 scores.py <arm> [<arm> ...] > scores.txt
"""
import json
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed") / "log" / "rfx"
FRAMES = {"training": [884, 924, 959], "arctic": [1051, 1091, 1126], "city": [761, 801, 836]}
GROUPS = ("C04", "C08", "C09")


def main() -> int:
    arms = sys.argv[1:]
    print("rows (red) [C04/C08/C09 rows] matched-draws, per arm; cites: verify/rfx_20261016/cites_rfx.json")
    print("frame          " + "".join(f"{a:>26}" for a in arms))
    for tr, ks in FRAMES.items():
        for k in ks:
            cells = []
            for a in arms:
                p = ROOT / a / tr / f"diff_{k:05d}.json"
                if not p.exists():
                    cells.append("-"); continue
                d = json.loads(p.read_text())
                rows = d["rows"]
                red = sum(1 for r in rows if not r.get("cite"))
                g = [sum(1 for r in rows if (r.get("cite") or "").startswith(x)) for x in GROUPS]
                cells.append(f"{len(rows)} ({red}) [{g[0]}/{g[1]}/{g[2]}] {d.get('matched')}")
            print(f"{tr:<8} {k:<5} " + "".join(f"{c:>26}" for c in cells))
    return 0


if __name__ == "__main__":
    sys.exit(main())
