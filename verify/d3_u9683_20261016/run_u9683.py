"""U-9683 driver (PREREG_u9683.md section 2). Recipes: run_h4 (14400 frames) and run_fk (1200 frames, ring skip K).

MASHED_ROOT = a private copy of original/ (ROOT). The exe chdirs there, so mashed_re.log and the append-mode
player_trace.log are private to this driver. Foreign games seen during a run are recorded (CPU contention only).
MEASUREMENT ONLY. Spawns and stops ONLY its own PID. Every MASHED_* variable is cleared before each arm.

usage: py -3.12 run_u9683.py [arm names...]      (no args = every arm)
"""
import hashlib
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
WT = MAIN / ".worktrees" / "u9683"
# Private game root: a robocopy of MAIN/original (482 MB, 2026-10-16), so logs are not shared with other sessions.
ROOT = Path(r"C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed"
            r"\c1f3ef78-83d8-4379-bfa7-b8b44efc73ff\scratchpad\gameroot")
OUT = WT / "verify" / "d3_u9683_20261016"
NEW = WT / "mashedmod" / "build" / "mashed_re.exe"
HEAD = WT / "mashedmod" / "build" / "mashed_re_head.exe"
WAIT_S = 900

M7 = ["MASHED_REGISTRY", "MASHED_PICKUPS_ORIG", "MASHED_REG_CARS", "MASHED_REG_CALLBACKS", "MASHED_MODE7",
      "MASHED_MODE7_TRACE=@OUT@/@N@.c30.tsv", "MASHED_PKCB_TRACE=@OUT@/@N@.cb.tsv",
      "MASHED_REFDIST_TRACE=@OUT@/@N@.rdtrace.tsv"]
AFT = "MASHED_REFDIST_AFTER_AI"

ARMS = [
    dict(n="H_HEAD", exe=HEAD, recipe="h4", knobs=[]),
    dict(n="H_OFF", exe=NEW, recipe="h4", knobs=[]),
    dict(n="H_ON", exe=NEW, recipe="h4", knobs=[AFT]),
    dict(n="H_ON2", exe=NEW, recipe="h4", knobs=[AFT]),
    dict(n="M_OFF", exe=NEW, recipe="h4", knobs=list(M7)),
    dict(n="M_ON", exe=NEW, recipe="h4", knobs=M7 + [AFT]),
]
# PREREG amendment A1 (report-only causal probe): the faithful race_pct producer on the mode-7 stack
ARMS.append(dict(n="M_PP", exe=NEW, recipe="h4", knobs=M7 + ["MASHED_PATHPROG"]))
ARMS.append(dict(n="M_PP_ON", exe=NEW, recipe="h4", knobs=M7 + ["MASHED_PATHPROG", AFT]))
for k in (0, 2, 1, 4, 27, 26, 54, 57):
    ARMS.append(dict(n="B_OFF_K%02d" % k, exe=NEW, recipe="fk", k=k, knobs=[]))
    ARMS.append(dict(n="B_ON_K%02d" % k, exe=NEW, recipe="fk", k=k, knobs=[AFT]))


def sha8(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:8]


def game_pids():
    """PIDs of every running game process (any session). Read-only: tasklist, nothing is killed or attached."""
    out = subprocess.run(["tasklist", "/FO", "CSV", "/NH"], capture_output=True, text=True).stdout
    pids = {}
    for line in out.splitlines():
        f = [c.strip('"') for c in line.split('","')]
        if len(f) > 1 and (f[0].lower().startswith("mashed_re") or f[0].lower() == "mashed.exe"):
            pids[int(f[1])] = f[0]
    return pids


def nlines(p):
    with open(p, "rb") as f:
        return sum(1 for _ in f)


def main():
    sel = sys.argv[1:]
    arms = [a for a in ARMS if a["n"] in sel] if sel else ARMS
    unknown = [s for s in sel if s not in {a["n"] for a in ARMS}]
    if unknown:
        raise SystemExit("unknown arms: %s" % unknown)
    for a in arms:
        env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
        # The exe chdirs to MASHED_ROOT (exe_main.cpp:8769), so its CWD-relative logs land there. With MASHED_ROOT =
        # the main repo they were shared with every other session (append-mode player_trace.log came back with
        # foreign lines on 2 of 2 runs), so MASHED_ROOT is a private copy of original/ (mashed-root-asset-ab).
        log, pt = ROOT / "mashed_re.log", ROOT / "player_trace.log"
        for f in (log, pt):
            if f.exists():
                f.unlink()
        frames = 14400 if a["recipe"] == "h4" else 1200
        env.update(MASHED_ROOT=str(ROOT), MASHED_MUTE="1", MASHED_TRACK_VIEW="Training", MASHED_CAR="1",
                   MASHED_ROUND="1", MASHED_ROUND_RULE="4", MASHED_WIN_POS="primary-bl",
                   MASHED_DETERMINISTIC="1", MASHED_DET_FRAMES=str(frames))
        step, gates = OUT / (a["n"] + ".step.csv"), OUT / (a["n"] + ".gates.csv")
        for f in (step, gates):
            if f.exists():
                f.unlink()
        env["MASHED_AI_STEPDUMP"] = str(step)
        if a["recipe"] == "h4":
            env["MASHED_PLAYERTRACE"] = "1"
            env["MASHED_U9186_GATES"] = str(gates)
        else:
            env["MASHED_RING_SKIP"] = str(a["k"])
        for kv in a["knobs"]:
            if "=" in kv:
                k, v = kv.split("=", 1)
                env[k] = v.replace("@OUT@", str(OUT)).replace("@N@", a["n"])
            else:
                env[kv] = "1"
        t0 = time.time()
        print("=== %s exe=%s recipe=%s knobs=%s ===" % (a["n"], Path(a["exe"]).name, a["recipe"], a["knobs"]), flush=True)
        p = subprocess.Popen([str(a["exe"])], cwd=str(WT), env=env)
        print("    pid=%d" % p.pid, flush=True)
        foreign = {}
        exited = False
        while time.time() - t0 < WAIT_S:
            try:
                p.wait(timeout=5)
                exited = True
                break
            except subprocess.TimeoutExpired:
                foreign.update({k: v for k, v in game_pids().items() if k != p.pid})
        if not exited:
            print("    TIMEOUT - stopping pid %d ONLY" % p.pid, flush=True)
            p.kill()
            p.wait(timeout=15)
        print("    exit=%s self_exited=%s wall=%.0fs foreign_games_seen=%s%s" % (
            p.returncode, exited, time.time() - t0, sorted(foreign.items()), "  CONTENDED" if foreign else ""), flush=True)
        if log.exists():
            shutil.copy2(log, OUT / (a["n"] + ".log"))
        if pt.exists():
            shutil.copy2(pt, OUT / (a["n"] + ".ptrace.log"))
        for f in (step, gates, OUT / (a["n"] + ".ptrace.log")):
            if f.exists():
                print("    %s  lines=%d  sha8=%s" % (f.name, nlines(f), sha8(f)), flush=True)
            elif a["recipe"] == "h4" or f == step:
                print("    MISSING %s" % f.name, flush=True)
    print("U9683 DONE", flush=True)


if __name__ == "__main__":
    main()
