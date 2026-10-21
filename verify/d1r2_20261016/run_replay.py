"""d1r2 standalone camera run: the G-CTRL boot recipe (TRAINING, one round, deterministic) with the
camera diagnostics on, optionally replaying an original-side camera_probe.py --frame trace.

    py -3.12 run_replay.py <exe> <out_dir> [--trace trace_v3.csv] [--shots K,...] [--frames N]
                           [KNOB | KNOB=VALUE ...]

Writes <out_dir>/cam_dump.csv (+ .nodes.txt), <out_dir>/librw_dump.csv and, with --shots,
<out_dir>/d1r2/stand_row_<K>.bmp. MEASUREMENT ONLY: spawns and stops ONLY its own PID.
"""
import argparse
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe"); ap.add_argument("out")
    ap.add_argument("--trace", default="")
    ap.add_argument("--shots", default="")
    ap.add_argument("--frames", type=int, default=3000)
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    out = Path(a.out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    for f in ("cam_dump.csv", "cam_dump.csv.nodes.txt", "librw_dump.csv"):
        if (out / f).exists():
            (out / f).unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": "Training",
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "0", "MASHED_GAME_MODE": "10",   # [fflip 2026-10-21] recipe F
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(a.frames), "MASHED_VERIFY_OUT": str(out),
        "MASHED_CAM_DUMP": str(out / "cam_dump.csv"),
        "MASHED_CAM_DUMP_LIBRW": str(out / "librw_dump.csv"),
    })
    if a.trace:
        env["MASHED_CAM_REPLAY"] = str(Path(a.trace).resolve())
    if a.shots:
        env["MASHED_CAM_REPLAY_SHOTS"] = a.shots
    for k in a.knobs:
        if "=" in k:
            x, y = k.split("=", 1); env[x] = y
        else:
            env[k] = "1"
    print(f"=== {out.name} exe={a.exe} trace={a.trace or '-'} shots={a.shots or '-'} "
          f"knobs={a.knobs}", flush=True)
    p = subprocess.Popen([a.exe], cwd=str(ROOT), env=env)
    print(f"    pid={p.pid}", flush=True)
    try:
        rc = p.wait(900)
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill(); rc = p.wait(15)
    print(f"    exit={rc}", flush=True)
    for f in ("cam_dump.csv", "cam_dump.csv.nodes.txt", "librw_dump.csv"):
        q = out / f
        print(f"    {f}: {sum(1 for _ in q.open()) if q.exists() else 'MISSING'} lines", flush=True)
    for b in sorted((out / "d1r2").glob("stand_row_*.bmp")) if (out / "d1r2").exists() else []:
        print(f"    shot {b.name}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
