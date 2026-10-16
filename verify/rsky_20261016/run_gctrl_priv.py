"""rsky G-CTRL driver: verify/d1r1_20261016/run_gctrl.py's recipe (the run_h4.ps1 default-arm recipe), with MASHED_ROOT and the
CWD = a PRIVATE game root (original\\ inside it) and the outputs under an explicit directory.

    py -3.12 verify\\rsky_20261016\\run_gctrl_priv.py <exe> <arm_name> --root <gameroot> --out <dir> [KNOB | KNOB=VALUE ...]

Expected default hashes (memory default-ctrl-hashes-after-b2-flip): 2b4d47bb / 356821c8 / f7a155e1 for step / gates / ptrace.
MEASUREMENT ONLY. Spawns and stops ONLY its own PID. Deletes nothing outside <dir>.
"""
import argparse
import hashlib
import os
import subprocess
import sys
from pathlib import Path

FRAMES = 14400
WAIT_LIMIT = 900


def sha8(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()[:8]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe"); ap.add_argument("name")
    ap.add_argument("--root", required=True); ap.add_argument("--out", required=True)
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    exe, root, out = Path(a.exe).resolve(), Path(a.root).resolve(), Path(a.out).resolve()
    if not (root / "original" / "TOASTART" / "Common" / "Frontend.piz").exists():
        print("REFUSED: the private root needs original\\ inside it")
        return 2
    out.mkdir(parents=True, exist_ok=True)
    step, gates, ptc = out / f"{a.name}.step.csv", out / f"{a.name}.gates.csv", out / f"{a.name}.ptrace.log"
    for f in (step, gates, ptc):
        if f.exists():
            f.unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(root), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": "Training",
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(FRAMES), "MASHED_PLAYERTRACE": "1",
        "MASHED_AI_STEPDUMP": str(step), "MASHED_U9186_GATES": str(gates),
        "MASHED_PLAYERTRACE_OUT": str(ptc),
    })
    for k in a.knobs:
        x, _, y = k.partition("=")
        env[x] = y if y else "1"
    print(f"=== {a.name} exe={exe} knobs=[{','.join(a.knobs)}] root={root} ===", flush=True)
    p = subprocess.Popen([str(exe)], cwd=str(root), env=env)
    print(f"    pid={p.pid}", flush=True)
    try:
        rc = p.wait(WAIT_LIMIT)
        exited = True
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill()
        rc = p.wait(15)
        exited = False
    print(f"    exit={rc} self_exited={exited}", flush=True)
    for f in (step, gates, ptc):
        if f.exists():
            n = sum(1 for _ in f.open("rb"))
            print(f"    {f.name} lines={n} sha8={sha8(f)}", flush=True)
        else:
            print(f"    MISSING {f.name}", flush=True)
    print("GCTRL DONE", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
