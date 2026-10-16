#!/usr/bin/env python3
"""Boot-and-race smoke over every track piz: the default build must load each track and run a
short deterministic round without crashing (the sector walk now runs on every track).

    py -3.12 smoke_tracks.py <exe> <out_dir> [--frames N] [KNOB[=VALUE] ...]

MEASUREMENT ONLY: spawns and stops ONLY its own PIDs, one at a time.
"""
import argparse
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
TRACKS = ["Training", "SuperG", "Arctic", "rouabout", "sands", "Warzone", "Forest", "Egypt",
          "Storm", "dump", "Highway", "Neustein", "City"]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe"); ap.add_argument("out")
    ap.add_argument("--frames", type=int, default=900)
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    out = Path(a.out).resolve(); out.mkdir(parents=True, exist_ok=True)
    bad = 0
    for t in TRACKS:
        d = out / t; d.mkdir(exist_ok=True)
        env = dict(os.environ)
        env.update({"MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": t,
                    "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
                    "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
                    "MASHED_DET_FRAMES": str(a.frames), "MASHED_VERIFY_OUT": str(d),
                    "MASHED_PLAYERTRACE_OUT": str(d / "ptrace.log"),
                    "MASHED_LIBRW_WALKLOG": str(d / "walk.txt")})
        for k in a.knobs:
            if "=" in k:
                x, y = k.split("=", 1); env[x] = y
            else:
                env[k] = "1"
        p = subprocess.Popen([a.exe], cwd=str(ROOT), env=env)
        try:
            rc = p.wait(600)
        except subprocess.TimeoutExpired:
            p.kill(); rc = p.wait(15); rc = f"TIMEOUT({rc})"
        walk = d / "walk.txt"
        lines = sum(1 for _ in walk.open()) if walk.exists() else 0
        last = ""
        if walk.exists() and lines:
            last = walk.read_text().splitlines()[-1][:90]
        ok = rc == 0
        bad += not ok
        print(f"{t:<10} pid={p.pid} exit={rc} walk_frames={lines} last={last}", flush=True)
    print(f"tracks failing: {bad}/{len(TRACKS)}")
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
