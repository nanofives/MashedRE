"""librwrest_checks standalone runner (PREREG_librwrest_checks.md section 3).

    py -3.12 run_stand.py <exe> <out_dir> --track Training [--colours 1,2,3,4]
                          [--law <replay_in.txt>] [--frames 150] [KNOB | KNOB=VALUE ...]

Runs the standalone race demo (the d1r1 run_replay3d.py env, minus the camera / car replay)
through a PRIVATE MASHED_ROOT: <out_dir>/gameroot holds a robocopy of the main checkout's
original/ (copied once per out_dir, never a junction), so the CWD-relative mashed_re.log and
log\\ files stay private to this run. With --law the W1 replay input is fed through
MASHED_CARPARTS_LAW and the poses land in <out_dir>/law_out.txt.
MEASUREMENT ONLY: spawns and stops ONLY its own PID.
"""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe")
    ap.add_argument("out")
    ap.add_argument("--track", default="Training", help="MASHED_TRACK_VIEW piz basename")
    ap.add_argument("--colours", default="", help="MASHED_ROUND_COLOURS a,b,c,d")
    ap.add_argument("--law", default="", help="W1 replay input file (MASHED_CARPARTS_LAW)")
    ap.add_argument("--frames", type=int, default=150)
    ap.add_argument("--gameroot", default="", help="reuse a private root (default <out>/gameroot)")
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    out = Path(a.out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    root = Path(a.gameroot).resolve() if a.gameroot else out / "gameroot"
    if not (root / "original" / "MASHED.exe").exists():
        (root).mkdir(parents=True, exist_ok=True)
        # robocopy exit codes 0..7 are success
        rc = subprocess.call(["robocopy", str(MAIN / "original"), str(root / "original"), "/E",
                              "/NFL", "/NDL", "/NJH", "/NJS", "/NP"])
        if rc >= 8:
            print(f"robocopy failed rc={rc}")
            return 2
    (root / "log").mkdir(exist_ok=True)
    for f in ("mashed_re.log", "player_trace.log"):
        p = root / f
        if p.exists():
            p.unlink()
    law_out = out / "law_out.txt"
    if law_out.exists():
        law_out.unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(root), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": a.track,
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(a.frames), "MASHED_VERIFY_OUT": str(out),
        "MASHED_PLAYERTRACE_OUT": str(out / "ptrace.log"),
    })
    if a.colours:
        env["MASHED_ROUND_COLOURS"] = a.colours
    if a.law:
        env["MASHED_CARPARTS_LAW"] = str(Path(a.law).resolve())
        env["MASHED_CARPARTS_LAW_OUT"] = str(law_out)
    for k in a.knobs:
        if "=" in k:
            x, y = k.split("=", 1)
            env[x] = y
        else:
            env[k] = "1"
    print(f"=== {out.name} exe={a.exe} track={a.track} colours={a.colours or '-'} "
          f"law={bool(a.law)} knobs={a.knobs}", flush=True)
    p = subprocess.Popen([a.exe], cwd=str(root), env=env)
    print(f"    pid={p.pid}", flush=True)
    try:
        rc = p.wait(600)
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill()
        rc = p.wait(15)
    print(f"    exit={rc}", flush=True)
    log = root / "mashed_re.log"
    if log.exists():
        shutil.copyfile(log, out / "mashed_re.log")
        n = sum(1 for ln in log.read_text(errors="replace").splitlines() if "[librwrest]" in ln)
        print(f"    mashed_re.log copied ({n} [librwrest] lines)", flush=True)
    else:
        print("    NO mashed_re.log in the private root", flush=True)
    if a.law:
        print(f"    law_out: {'present' if law_out.exists() else 'MISSING'}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
