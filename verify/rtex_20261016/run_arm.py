"""rtex: one measuring arm = d1r1's 9 matched-pose frames on one standalone exe.

    py -3.12 run_arm.py <exe> <arm> [--tracks training,arctic,city] [--skip-run] [KNOB | KNOB=V ...]

Per track: run_replay3d.py (d1r1 recipe, MEASUREMENT ONLY, own PID only) into
log/rtex/<arm>/<track>/, then re/tools/drawlist_diff.py 3D mode on each shot against d1r1's
original record with --trace and --cite verify/d1r1_20261016/cites_d1r1.json (JSON to
log/rtex/<arm>/diff_<track>_<K>.json), and re/tools/imgdiff.py against the original BMP.
Prints one line per frame: rows, red, matched draws/tris, pixel mean abs diff.
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
HERE = Path(__file__).resolve().parent
WT = HERE.parents[1]
LOG = ROOT / "log" / "rtex"
D1 = ROOT / "log" / "d1r1"
CITE = WT / "verify" / "d1r1_20261016" / "cites_d1r1.json"
TRACKS = {
    "training": ("Training", [884, 924, 959]),
    "arctic": ("Arctic", [1051, 1091, 1126]),
    "city": ("City", [761, 801, 836]),
}
FRAMES = 1450


def pixel_mad(a: Path, b: Path) -> float:
    """re/tools/imgdiff.py's all-channel mean abs diff (the number d1r1's scores.txt reports)."""
    out = subprocess.run([sys.executable, str(WT / "re" / "tools" / "imgdiff.py"), str(a), str(b)],
                         capture_output=True, text=True).stdout
    m = re.search(r"\(all ([\d.]+)\)", out)
    return float(m.group(1)) if m else float("nan")


def matched_tri(oa: Path, sb: Path) -> int:
    """Triangles of the original's draws that matched (drawlist3d.diff3d pairs)."""
    sys.path.insert(0, str(WT / "re" / "tools"))
    import drawlist3d
    da, db = json.loads(oa.read_text()), json.loads(sb.read_text())
    A, B, pairs, rows, info = drawlist3d.diff3d(da, db)
    return sum((A[x]["tri"] or 0) for x, y in pairs)


def main() -> int:
    args = sys.argv[1:]
    if len(args) < 2:
        print(__doc__)
        return 2
    exe, arm = args[0], args[1]
    rest = args[2:]
    tracks = list(TRACKS)
    skip = False
    knobs = []
    i = 0
    while i < len(rest):
        if rest[i] == "--tracks":
            tracks = rest[i + 1].split(","); i += 2; continue
        if rest[i] == "--skip-run":
            skip = True; i += 1; continue
        knobs.append(rest[i]); i += 1
    base = LOG / arm
    base.mkdir(parents=True, exist_ok=True)
    lines = []
    for t in tracks:
        view, shots = TRACKS[t]
        out = base / t
        if not skip:
            cmd = [sys.executable, str(HERE / "run_replay3d.py"), exe, str(out),
                   "--trace", str(D1 / f"orig_{t}" / "trace.csv"),
                   "--shots", ",".join(str(k) for k in shots), "--track", view,
                   "--frames", str(FRAMES)] + knobs
            print(" ".join(cmd), flush=True)
            subprocess.run(cmd, check=False)
        for k in shots:
            oa = D1 / f"orig_{t}" / f"orig_row_{k:05d}.draws.json"
            sb = out / "d1r2" / f"stand_row_{k:05d}.draws.json"
            dj = base / f"diff_{t}_{k:05d}.json"
            if not sb.exists():
                lines.append(f"{t:<9}{k:>5}  MISSING standalone record {sb}")
                continue
            subprocess.run([sys.executable, str(WT / "re" / "tools" / "drawlist_diff.py"), str(oa), str(sb),
                            "--trace", str(D1 / f"orig_{t}" / "trace.csv"), "--cite", str(CITE),
                            "--json", str(dj)], capture_output=True, text=True)
            d = json.loads(dj.read_text())
            rows = d["rows"]
            red = sum(1 for r in rows if not r.get("cite"))
            info = d.get("info", {})
            mad = pixel_mad(D1 / f"orig_{t}" / f"orig_row_{k:05d}.bmp",
                            out / "d1r2" / f"stand_row_{k:05d}.bmp")
            lines.append(f"{t:<9}{k:>5}  rows {len(rows):>3}  red {red:>2}  matched {d.get('matched')}/{d.get('na')}"
                         f"  tri {matched_tri(oa, sb)}/{d.get('tri_a')}  pix {mad:.2f}")
    print(f"== arm {arm} exe={exe} knobs={knobs}")
    for ln in lines:
        print(ln)
    (base / "summary.txt").write_text(f"arm {arm} exe={exe} knobs={knobs}\n" + "\n".join(lines) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
