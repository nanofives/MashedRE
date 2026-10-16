#!/usr/bin/env python3
"""rstates 9-frame driver: d1r1's matched-pose replay on TRAINING / Arctic / City, then the 3D
draw-list diff of every shot against the original's d1r1 record.

    py -3.12 run9.py <exe> <arm> [--tracks training,arctic,city] [--no-run] [KNOB[=VALUE] ...]

Replays land in log/rstates/<arm>/<track>/ (run_replay3d.py layout), diffs in
log/rstates/<arm>/<track>/diff_<K>.json, and one summary line per frame is printed.
MEASUREMENT ONLY: run_replay3d.py spawns and stops only its own PID.
"""
import argparse
import json
import subprocess
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parents[1]
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
LOG = WT / "log" / "rstates"
CITES = WT / "verify" / "rstates_20261016" / "cites_rstates.json"
TRACKS = {  # name -> (MASHED_TRACK_VIEW, shots, frames)  (frames as d1r1's cam_dump lengths)
    "training": ("Training", (884, 924, 959), 1300),
    "arctic": ("Arctic", (1051, 1091, 1126), 1450),
    "city": ("City", (761, 801, 836), 1150),
}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe"); ap.add_argument("arm")
    ap.add_argument("--tracks", default="training,arctic,city")
    ap.add_argument("--no-run", action="store_true", help="re-diff existing records only")
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    for t in a.tracks.split(","):
        view, shots, frames = TRACKS[t]
        out = LOG / a.arm / t
        trace = MAIN / "log" / "d1r1" / f"orig_{t}" / "trace.csv"
        if not a.no_run:
            cmd = [sys.executable, str(WT / "verify/d1r1_20261016/run_replay3d.py"), a.exe,
                   str(out), "--trace", str(trace), "--shots", ",".join(map(str, shots)),
                   "--track", view, "--frames", str(frames)] + a.knobs
            subprocess.run(cmd, check=False)
        for k in shots:
            orig = MAIN / "log" / "d1r1" / f"orig_{t}" / f"orig_row_{k:05d}.draws.json"
            stand = out / "d1r2" / f"stand_row_{k:05d}.draws.json"
            dj = out / f"diff_{k:05d}.json"
            if not stand.exists():
                print(f"{t}:{k} NO RECORD ({stand})")
                continue
            r = subprocess.run([sys.executable, str(WT / "re/tools/drawlist_diff.py"), str(orig),
                                str(stand), "--trace", str(trace), "--cite", str(CITES),
                                "--json", str(dj)], capture_output=True, text=True)
            verdict = (r.stdout.strip().splitlines() or ["?"])[-1]
            rows = json.loads(dj.read_text())["rows"] if dj.exists() else []
            c = Counter(x["kind"] for x in rows)
            red = sum(1 for x in rows if not x.get("cite"))
            print(f"{t}:{k} rows={len(rows)} RED={red} " +
                  " ".join(f"{kk}={c[kk]}" for kk in sorted(c)) + f" | {verdict[:80]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
