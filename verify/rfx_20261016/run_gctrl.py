"""rfx G-CTRL driver (copied from verify/d1r1_20261016/run_gctrl.py, OUT -> log/rfx): the verify/d3_gatefire_20261008/run_h4.ps1 default-arm recipe, pointed at an
explicit exe, so a worktree build or a frozen baseline copy can be fingerprinted.

    py -3.12 run_gctrl.py <exe> <arm_name> [KNOB | KNOB=VALUE ...]

MEASUREMENT ONLY. Spawns and stops ONLY its own PID. Deletes nothing in the shared main root (other
sessions use it as CWD too): the ptrace goes to a private path via MASHED_PLAYERTRACE_OUT.
"""
import hashlib
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
OUT = ROOT / "log" / "rfx"          # the traces are MB-sized; the sha8 lines are the evidence
FRAMES = 14400
WAIT_LIMIT = 900


def sha8(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()[:8]


def main() -> int:
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    exe, name, knobs = Path(sys.argv[1]).resolve(), sys.argv[2], sys.argv[3:]
    pt = ROOT / "player_trace.log"   # shared: never deleted here (other sessions write it too)
    step, gates = OUT / f"{name}.step.csv", OUT / f"{name}.gates.csv"
    ptc = OUT / f"{name}.ptrace.log"
    for f in (step, gates, ptc):
        if f.exists():
            f.unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": "Training",
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(FRAMES), "MASHED_PLAYERTRACE": "1",
        "MASHED_AI_STEPDUMP": str(step), "MASHED_U9186_GATES": str(gates),
        # private ptrace (TrackRenderer.cpp MASHED_PLAYERTRACE_OUT); an exe without the knob
        # still writes the shared CWD file, reported as SHARED below
        "MASHED_PLAYERTRACE_OUT": str(ptc),
    })
    for k in knobs:
        if "=" in k:
            a, b = k.split("=", 1)
            env[a] = b
        else:
            env[k] = "1"
    print(f"=== {name}  exe={exe}  knobs=[{','.join(knobs)}] ===", flush=True)
    p = subprocess.Popen([str(exe)], cwd=str(ROOT), env=env)
    print(f"    pid={p.pid}", flush=True)
    try:
        rc = p.wait(WAIT_LIMIT)
        exited = True
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill()
        rc = p.wait(15)
        exited = False
    print(f"    exit={rc}  self_exited={exited}", flush=True)
    if not ptc.exists() and pt.exists():
        shutil.copy2(pt, ptc)
        print("    ptrace: SHARED CWD file (exe has no MASHED_PLAYERTRACE_OUT); "
              "other sessions may have appended to it", flush=True)
    for f in (step, gates, ptc):
        if f.exists():
            n = sum(1 for _ in f.open("rb"))
            print(f"    {f.name}  lines={n}  sha8={sha8(f)}", flush=True)
        else:
            print(f"    MISSING {f.name}", flush=True)
    print("GCTRL DONE", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
