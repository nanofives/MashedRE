# D1-residue R3: G-CTRL default-build fingerprint for ONE exe.
#
# Same recipe as verify/d3_gatefire_20261008/run_h4.ps1 (default arm, no knobs): TRAINING,
# car 1, round 1, rule 4, 14400 deterministic frames, PLAYERTRACE + U9186_GATES +
# AI_STEPDUMP. Prints sha8 of <name>.step.csv, <name>.gates.csv, <name>.ptrace.log.
# Expected default: 991a5fff / be0d47ea / bc75b405 (memory default-ctrl-hashes-after-b2-flip).
#
# Worktree notes: the exe relocates to the MAIN repo root (no original\ in a worktree), so
# player_trace.log and mashed_re.log are written THERE and copied back out. Every inherited
# MASHED_* variable is dropped first. Spawns and stops ONLY its own pid, and only on timeout.
#
# Usage: py -3.12 run_gctrl.py <exe> <arm-name>
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
MAIN = WT.parent.parent
FRAMES = 14400
WAIT_LIMIT = 900


def sha8(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()[:8]


def lines(p):
    return len(p.read_text(errors="replace").splitlines())


def other_mashed_pids():
    """PIDs of ANY running MASHED/mashed_re process (read-only; never killed here)."""
    out = subprocess.run(["tasklist", "/fo", "csv", "/nh"], capture_output=True,
                         text=True).stdout
    pids = []
    for ln in out.splitlines():
        cols = [c.strip('"') for c in ln.split('","')]
        if len(cols) > 1 and cols[0].lower().startswith(("mashed_re", "mashed.exe")):
            pids.append(cols[1].strip('"'))
    return pids


def ptrace_clean(p):
    """player_trace.log lives in the shared MAIN cwd and is opened with 'a' by every
    session running MASHED_PLAYERTRACE. A clean single-run log has strictly increasing
    f= frame numbers on the car-0 lines; interleaved runs break that."""
    last, bad = -1, 0
    for ln in p.read_text(errors="replace").splitlines():
        if ln.startswith("f="):
            f = int(ln[2:ln.index(" ")])
            if f <= last:
                bad += 1
            last = f
    return bad == 0, bad


def main():
    exe = Path(sys.argv[1]).resolve()
    name = sys.argv[2]
    if not exe.exists():
        print(f"error: {exe} not found")
        return 2
    log, pt = MAIN / "mashed_re.log", MAIN / "player_trace.log"
    # Optional 3rd arg: max seconds to wait for every other MASHED process to exit
    # before launching (shrinks the window in which another session can append to
    # the shared player_trace.log). Waiting only; nothing is killed.
    wait_idle = float(sys.argv[3]) if len(sys.argv) > 3 else 0.0
    t_idle = time.time()
    while wait_idle and other_mashed_pids() and time.time() - t_idle < wait_idle:
        time.sleep(2.0)
    print(f"    foreign MASHED pids at launch: {other_mashed_pids()}", flush=True)
    for p in (log, pt):
        try:
            if p.exists():
                p.unlink()
        except OSError as e:
            print(f"    could not remove {p.name}: {e}")
    step, gates = HERE / f"{name}.step.csv", HERE / f"{name}.gates.csv"
    for p in (step, gates):
        if p.exists():
            p.unlink()
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    # Optional 4th/5th args: track name (default Training) and frame count (default 14400).
    track = sys.argv[4] if len(sys.argv) > 4 else "Training"
    frames = int(sys.argv[5]) if len(sys.argv) > 5 else FRAMES
    # Any further K=V args (6th onward) are added to the child's environment verbatim.
    for kv in sys.argv[6:]:
        if "=" in kv:
            k, v = kv.split("=", 1)
            env[k] = v
    env.update({
        "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": track, "MASHED_CAR": "1",
        "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4", "MASHED_WIN_POS": "primary-bl",
        "MASHED_DETERMINISTIC": "1", "MASHED_DET_FRAMES": str(frames),
        "MASHED_PLAYERTRACE": "1",
        "MASHED_AI_STEPDUMP": str(step), "MASHED_U9186_GATES": str(gates),
        # d1r2 e6498ad6: private ptrace path. Exes built before that commit ignore it and
        # write the shared main-root player_trace.log instead (copied back below).
        "MASHED_PLAYERTRACE_OUT": str(HERE / f"{name}.ptrace.log"),
    })
    print(f"=== {name} exe={exe}", flush=True)
    proc = subprocess.Popen([str(exe)], cwd=str(WT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(f"    pid={proc.pid}", flush=True)
    t0 = time.time()
    rc = None
    while time.time() - t0 < WAIT_LIMIT:
        rc = proc.poll()
        if rc is not None:
            break
        time.sleep(0.5)
    self_exited = rc is not None
    if not self_exited:
        print(f"    TIMEOUT - killing pid {proc.pid} ONLY")
        proc.kill()
        proc.wait(timeout=15)
        rc = "killed"
    print(f"    exit={rc} self_exited={self_exited} seconds={time.time() - t0:.0f}")
    if log.exists():
        shutil.copy2(log, HERE / f"{name}.log")
    priv = HERE / f"{name}.ptrace.log"
    if not priv.exists() and pt.exists():   # pre-e6498ad6 exe: fall back to the shared file
        shutil.copy2(pt, priv)
    for p in (step, gates, HERE / f"{name}.ptrace.log"):
        if p.exists():
            extra = ""
            if p.name.endswith(".ptrace.log"):
                ok, bad = ptrace_clean(p)
                extra = "  CLEAN" if ok else f"  CONTAMINATED ({bad} non-monotonic frame lines: another session wrote the shared file)"
            print(f"    {p.name}  lines={lines(p)}  sha8={sha8(p)}{extra}")
        else:
            print(f"    MISSING {p.name}")
    print(f"    foreign MASHED pids at exit: {other_mashed_pids()}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
