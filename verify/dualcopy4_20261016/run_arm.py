"""dualcopy3 standalone arm runner: the G-CTRL / (b) / Warzone recipes pointed at an explicit exe, private outputs.

    py -3.12 run_arm.py <exe> <name> [--track Training] [--frames 1200] [--ring-skip K] [--gates] [--ptrace]
                        [KNOB | KNOB=VALUE ...]

Recipes (the same env the lane's predecessors used, verify/d3_b2flip_20261009/run_fk.ps1 and
verify/dupinstall_20261016/run_gctrl_private.py):
  G-CTRL        --frames 14400 --gates --ptrace                       (TRAINING, rule 4)
  (b) per K     --frames 1200 --ring-skip K                           (TRAINING, rule 4, step dump only)
  G-WARZONE     --track Warzone --frames 3600 --gates                  (course 33 = 0x21)

MEASUREMENT ONLY. Spawns and stops ONLY its own PID. The step / gates / ptrace dumps go to arms/ in THIS lane's verify directory,
never to the shared main root; the traces are MB-sized and are deleted before the commit, the sha8 lines in the run logs stay.
"""
import argparse
import hashlib
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
OUT = Path(__file__).resolve().parent / "arms"


def sha8(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()[:8]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe")
    ap.add_argument("name")
    ap.add_argument("--track", default="Training")
    ap.add_argument("--frames", type=int, default=1200)
    ap.add_argument("--ring-skip", default="")
    ap.add_argument("--gates", action="store_true")
    ap.add_argument("--ptrace", action="store_true")
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    step = OUT / f"{a.name}.step.csv"
    gates = OUT / f"{a.name}.gates.csv"
    ptc = OUT / f"{a.name}.ptrace.log"
    for f in (step, gates, ptc):
        if f.exists():
            f.unlink()
    env = dict(os.environ)
    for k in ("MASHED_SUBSTATE", "MASHED_MODE5_RESET", "MASHED_U9186_GATES", "MASHED_PLAYERTRACE", "MASHED_RING_SKIP",
              "MASHED_PLAYERTRACE_OUT"):
        env.pop(k, None)
    env.update({
        "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": a.track,
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(a.frames), "MASHED_AI_STEPDUMP": str(step),
    })
    if a.ring_skip != "":
        env["MASHED_RING_SKIP"] = a.ring_skip
    if a.gates:
        env["MASHED_U9186_GATES"] = str(gates)
    if a.ptrace:
        env["MASHED_PLAYERTRACE"] = "1"
        env["MASHED_PLAYERTRACE_OUT"] = str(ptc)
    for k in a.knobs:
        x, _, y = k.partition("=")
        env[x] = y if _ else "1"
    print(f"=== {a.name} exe={a.exe} track={a.track} frames={a.frames} ring_skip={a.ring_skip or '-'} knobs={a.knobs} ===", flush=True)
    p = subprocess.Popen([a.exe], cwd=str(ROOT), env=env)
    print(f"    pid={p.pid}", flush=True)
    try:
        rc = p.wait(900)
        done = True
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill()
        rc = p.wait(15)
        done = False
    print(f"    exit={rc} self_exited={done}", flush=True)
    for f in (step, gates, ptc):
        if f.exists():
            print(f"    {f.name} lines={sum(1 for _ in f.open('rb'))} sha8={sha8(f)}", flush=True)
        elif (f is step) or (f is gates and a.gates) or (f is ptc and a.ptrace):
            print(f"    MISSING {f.name}", flush=True)
    print("ARM DONE", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
