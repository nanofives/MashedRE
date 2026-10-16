#!/usr/bin/env python3
"""Run the nine-frame matched-pose replays for one arm (exe + knobs) sequentially.

    py -3.12 run_arms.py <exe> <arm> [--tracks training,arctic,city] [KNOB | KNOB=VALUE ...]

Writes log/rprops/<arm>_<track>/d1r2/stand_row_<K>.{bmp,draws.json} via run_replay3d.py (private
game root). MEASUREMENT ONLY: each child is spawned and stopped by run_replay3d.py (own PID only).
"""
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parents[1]
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
TRACKS = {
    "training": ("Training", "884,924,959", 1300),
    "arctic": ("Arctic", "1051,1091,1126", 1450),
    "city": ("City", "761,801,836", 1200),
}


def main():
    a = sys.argv[1:]
    exe, arm = a[0], a[1]
    rest = a[2:]
    tracks = list(TRACKS)
    if rest and rest[0] == "--tracks":
        tracks = rest[1].split(",")
        rest = rest[2:]
    for t in tracks:
        piz, shots, frames = TRACKS[t]
        out = WT / "log" / "rprops" / f"{arm}_{t}"
        cmd = [sys.executable, str(HERE / "run_replay3d.py"), exe, str(out),
               "--trace", str(MAIN / "log" / "d1r1" / f"orig_{t}" / "trace.csv"),
               "--shots", shots, "--track", piz, "--frames", str(frames)] + rest
        print(" ".join(cmd), flush=True)
        r = subprocess.run(cmd, capture_output=True, text=True)
        (WT / "log" / "rprops" / f"{arm}_{t}.txt").write_text(r.stdout + r.stderr)
        print(r.stdout[-600:], flush=True)
    print("ARM DONE", arm, flush=True)


if __name__ == "__main__":
    main()
