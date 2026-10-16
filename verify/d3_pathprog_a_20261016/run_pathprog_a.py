"""PATHPROG split driver (verify/d3_pathprog_a_20261016). Recipes: run_h4 (14400 frames) and run_fk (1200 frames, ring skip K).

MASHED_ROOT = a private copy of original/ (ROOT). The exe chdirs there, so mashed_re.log and the append-mode
player_trace.log are private to this driver (see memory mashed-root-main-shares-append-logs). Foreign games seen
during a run are recorded (CPU contention only). MEASUREMENT ONLY. Spawns and stops ONLY its own PID. Every MASHED_*
variable is cleared before each arm.

usage: py -3.12 run_pathprog_a.py [arm names...]      (no args = every arm)
"""
import hashlib
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
WT = MAIN / ".worktrees" / "pathprog_a"
OUT = WT / "verify" / "d3_pathprog_a_20261016"
ROOT = Path(os.environ.get("PPA_ROOT") or (r"C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed"
            r"\c1f3ef78-83d8-4379-bfa7-b8b44efc73ff\scratchpad\gameroot"))   # robocopy of MAIN/original; PPA_ROOT = a second private root for a parallel driver
NEW = WT / "mashedmod" / "build" / "mashed_re.exe"
PA0 = WT / "mashedmod" / "build" / "mashed_re_pa0.exe"   # copy of the 0020d521 build (G-CTRL arms run it while the new build is made)
WAIT_S = 900

M7 = ["MASHED_REGISTRY", "MASHED_PICKUPS_ORIG", "MASHED_REG_CARS", "MASHED_REG_CALLBACKS", "MASHED_MODE7",
      "MASHED_MODE7_TRACE=@OUT@/@N@.c30.tsv", "MASHED_PKCB_TRACE=@OUT@/@N@.cb.tsv",
      "MASHED_REFDIST_TRACE=@OUT@/@N@.rdtrace.tsv"]
OPT = "MASHED_NO_REFDIST_AFTER_AI"      # the order before U-9683 (producer before the AI tick)
PP = "MASHED_PATHPROG"                  # the whole FUN_00408610 port (report-only probe)

ARMS = []
NO_RP = "MASHED_NO_RACEPCT_FUN408610"   # the opt-out of U-9685 (race_pct from the verbatim step, default ON)
ARMS += [
    dict(n="G_DEF", recipe="h4", knobs=[], exe=PA0),
    dict(n="G_OPTOUT", recipe="h4", knobs=[NO_RP], exe=PA0),
]
PUB = "MASHED_PP_PUBLISH"
KS = (0, 2, 1, 4, 27, 26, 54, 57)
for _k in KS:
    ARMS.append(dict(n="D_K%02d" % _k, recipe="fk", k=_k, knobs=[]))
    ARMS.append(dict(n="PP_K%02d" % _k, recipe="fk", k=_k, knobs=[PP]))
    for _m in (0, 1, 2, 3, 4, 7, 8, 15):
        ARMS.append(dict(n="X%d_K%02d" % (_m, _k), recipe="fk", k=_k, knobs=["%s=%d" % (PUB, _m)]))
# stage 3 (h4 recipe and the mode-7 stack)
ARMS += [
    dict(n="X7_H", recipe="h4", knobs=["%s=7" % PUB]),
    dict(n="X7_H2", recipe="h4", knobs=["%s=7" % PUB]),
    dict(n="X15_H", recipe="h4", knobs=["%s=15" % PUB]),
    dict(n="PP_H", recipe="h4", knobs=[PP]),
    dict(n="M_DEF", recipe="h4", knobs=list(M7)),
    dict(n="X7_M", recipe="h4", knobs=M7 + ["%s=7" % PUB]),
    dict(n="X7_M2", recipe="h4", knobs=M7 + ["%s=7" % PUB]),
]
# PREREG amendment A1
BS = "MASHED_PP_BOOSTSEED"
RGK = "MASHED_ROUND_GRID"
for _k in (0, 2):
    for _m in (0, 2, 7):
        ARMS.append(dict(n="X%d_BS_K%02d" % (_m, _k), recipe="fk", k=_k, knobs=["%s=%d" % (PUB, _m), BS]))
    ARMS.append(dict(n="X7_RG_K%02d" % _k, recipe="fk", k=_k, knobs=["%s=7" % PUB, RGK]))
    ARMS.append(dict(n="PP_RG_K%02d" % _k, recipe="fk", k=_k, knobs=[PP, RGK]))
ARMS.append(dict(n="X2_H", recipe="h4", knobs=["%s=2" % PUB]))
# arms added per PREREG are appended below this line


def sha8(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:8]


def nlines(p):
    with open(p, "rb") as f:
        return sum(1 for _ in f)


def game_pids():
    out = subprocess.run(["tasklist", "/FO", "CSV", "/NH"], capture_output=True, text=True).stdout
    pids = {}
    for line in out.splitlines():
        f = [c.strip('"') for c in line.split('","')]
        if len(f) > 1 and (f[0].lower().startswith("mashed_re") or f[0].lower() == "mashed.exe"):
            pids[int(f[1])] = f[0]
    return pids


def main():
    sel = sys.argv[1:]
    known = {a["n"] for a in ARMS}
    unknown = [s for s in sel if s not in known]
    if unknown:
        raise SystemExit("unknown arms: %s" % unknown)
    arms = [a for a in ARMS if a["n"] in sel] if sel else ARMS
    for a in arms:
        env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
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
        print("=== %s recipe=%s knobs=%s ===" % (a["n"], a["recipe"], a["knobs"]), flush=True)
        p = subprocess.Popen([str(a.get("exe", NEW))], cwd=str(WT), env=env)
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
        print("    exit=%s self_exited=%s wall=%.0fs foreign_games_seen=%d" % (
            p.returncode, exited, time.time() - t0, len(foreign)), flush=True)
        if log.exists():
            shutil.copy2(log, OUT / (a["n"] + ".log"))
        if pt.exists():
            shutil.copy2(pt, OUT / (a["n"] + ".ptrace.log"))
        for f in (step, gates, OUT / (a["n"] + ".ptrace.log")):
            if f.exists():
                print("    %s  lines=%d  sha8=%s" % (f.name, nlines(f), sha8(f)), flush=True)
            elif a["recipe"] == "h4" or f == step:
                print("    MISSING %s" % f.name, flush=True)
    print("PPA DONE", flush=True)


if __name__ == "__main__":
    main()
