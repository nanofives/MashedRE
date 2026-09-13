#!/usr/bin/env python3
"""Spawn mashed_re.exe with the A8 held-lock recipe, wait N seconds, kill ONLY that
PID, and move motion_diag.log into the given output directory.

Usage: py -3.12 re/tools/statediff/a8_run_port.py <out_dir> [seconds=45] [EXTRA=VAL ...]

Recipe (verify/a8_velvec_20260825/PROVENANCE.txt, twenty-third follow-up), plus
MASHED_MUTE=1 per the project's always-launch-muted rule.
"""
import os, shutil, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"
RECIPE = dict(MASHED_REAL_PHYSICS="1", MASHED_RACE_DEMO="1", MASHED_PLAY_DEMO="1",
              MASHED_GOTO="6", MASHED_TRACK_SEL="0", MASHED_CAR_SEL="0",
              MASHED_DRIVE_HOLD="1", MASHED_WIN_POS="left-bl", MASHED_MOTION_DIAG="1",
              MASHED_STEER_HOLD="1", MASHED_STEER_HOLD_AFTER="4", MASHED_MUTE="1")

def main():
    out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
    secs = float(sys.argv[2]) if len(sys.argv) > 2 else 45.0
    env = dict(os.environ); env.update(RECIPE)
    for kv in sys.argv[3:]:
        k, v = kv.split("=", 1); env[k] = v
    log = ROOT / "motion_diag.log"
    if log.exists():
        log.unlink()
    head = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    p = subprocess.Popen([str(EXE)], cwd=ROOT, env=env)
    print(f"spawned PID {p.pid} at HEAD {head}; waiting {secs:.0f}s")
    t0 = time.time()
    while time.time() - t0 < secs and p.poll() is None:
        time.sleep(1.0)
    rc = p.poll()
    if rc is None:
        subprocess.run(["taskkill", "/PID", str(p.pid), "/F"], capture_output=True)
        print("killed PID", p.pid)
    else:
        print("exe exited on its own rc", rc)
    n = 0
    if log.exists():
        dst = out / "motion_diag.log"
        shutil.move(str(log), str(dst))
        n = sum(1 for _ in open(dst, errors="replace"))
        print("moved ->", dst, "lines", n)
    else:
        print("NO motion_diag.log produced")
    with open(out / "PROVENANCE.txt", "a", encoding="utf-8") as f:
        f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} a8_run_port.py HEAD={head} PID={p.pid} secs={secs:.0f} lines={n} env={RECIPE} extra={sys.argv[3:]}\n")

if __name__ == "__main__":
    main()
