"""rtex standalone matched-pose run (copy of verify/d1r1_20261016/run_replay3d.py): verify/d1r2_20261016/run_replay.py's recipe (one round,
deterministic) on any track, replaying an original-side camera_probe.py trace, with the d1r1
per-draw recorder and the pose transplants on.

    py -3.12 run_replay3d.py <exe> <out_dir> --trace <trace.csv> --shots K,... [--track Training]
                             [--no-basis] [--no-cars] [--frames N] [KNOB | KNOB=VALUE ...]

Writes <out_dir>/d1r2/stand_row_<K>.bmp and .draws.json, plus cam_dump.csv / librw_dump.csv.
MEASUREMENT ONLY: spawns and stops ONLY its own PID.
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
    ap.add_argument("--trace", required=True)
    ap.add_argument("--shots", required=True)
    ap.add_argument("--track", default="Training", help="MASHED_TRACK_VIEW piz basename")
    ap.add_argument("--no-basis", action="store_true", help="omit MASHED_CAM_REPLAY_BASIS")
    ap.add_argument("--no-cars", action="store_true", help="omit MASHED_CAR_REPLAY")
    ap.add_argument("--no-rec", action="store_true", help="omit MASHED_DBG_D3DREC")
    ap.add_argument("--frames", type=int, default=3000)
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    out = Path(a.out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    for f in ("cam_dump.csv", "librw_dump.csv"):
        if (out / f).exists():
            (out / f).unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": a.track,
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(a.frames), "MASHED_VERIFY_OUT": str(out),
        "MASHED_CAM_DUMP": str(out / "cam_dump.csv"),
        "MASHED_CAM_DUMP_LIBRW": str(out / "librw_dump.csv"),
        "MASHED_CAM_REPLAY": str(Path(a.trace).resolve()),
        "MASHED_CAM_REPLAY_SHOTS": a.shots,
        # private ptrace path so nothing lands in the shared CWD file (memory: G-CTRL ptrace)
        "MASHED_PLAYERTRACE_OUT": str(out / "ptrace.log"),
    })
    if not a.no_basis:
        env["MASHED_CAM_REPLAY_BASIS"] = "1"
    if not a.no_cars:
        env["MASHED_CAR_REPLAY"] = "1"
    if not a.no_rec:
        env["MASHED_DBG_D3DREC"] = "1"
    for k in a.knobs:
        if "=" in k:
            x, y = k.split("=", 1); env[x] = y
        else:
            env[k] = "1"
    print(f"=== {out.name} exe={a.exe} track={a.track} trace={a.trace} shots={a.shots} "
          f"basis={not a.no_basis} cars={not a.no_cars} knobs={a.knobs}", flush=True)
    p = subprocess.Popen([a.exe], cwd=str(ROOT), env=env)
    print(f"    pid={p.pid}", flush=True)
    try:
        rc = p.wait(900)
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill(); rc = p.wait(15)
    print(f"    exit={rc}", flush=True)
    d = out / "d1r2"
    for b in sorted(d.glob("stand_row_*")) if d.exists() else []:
        print(f"    {b.name} {b.stat().st_size}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
