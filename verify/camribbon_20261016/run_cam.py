"""camribbon standalone runner (derived from d1r2's run_gctrl.py / run_replay.py, outputs kept INSIDE this
worktree instead of the shared main-root log dir).

    py -3.12 run_cam.py gctrl <exe> <arm> [KNOB | KNOB=VALUE ...]
        G-CTRL default-build fingerprint (TRAINING, one round, 14400 det frames). Writes
        <wt>/log/camribbon/gctrl/<arm>.{step.csv,gates.csv,ptrace.log} and prints their sha8.
    py -3.12 run_cam.py dump <exe> <arm> --track <Name> [--trace csv] [--frames N] [--shots K,..]
                              [KNOB | KNOB=VALUE ...]
        track load + a short race with the camera diagnostics on. Writes
        <wt>/log/camribbon/dump/<arm>/{cam_dump.csv, cam_dump.csv.nodes.txt, cam_dump.csv.ribbon.txt,
        librw_dump.csv, course.bin (MASHED_PATHPROG_DUMP, when MASHED_PATHPROG is a knob)}.

MEASUREMENT ONLY. Spawns and stops ONLY its own PID. The exe chdirs to MASHED_ROOT (the main checkout,
which owns original/), so its own mashed_re.log is the shared one; nothing is deleted there.
"""
import hashlib
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")          # MASHED_ROOT: owns original/
WT = Path(__file__).resolve().parents[2]                          # this worktree
OUT = WT / "log" / "camribbon"


def sha8(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()[:8]


def parse_knobs(env, knobs):
    for k in knobs:
        if "=" in k:
            a, b = k.split("=", 1)
            env[a] = b
        else:
            env[k] = "1"


def run(exe, env, label):
    print(f"=== {label}  exe={exe}", flush=True)
    p = subprocess.Popen([str(exe)], cwd=str(ROOT), env=env)
    print(f"    pid={p.pid}", flush=True)
    try:
        rc = p.wait(900)
        done = True
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill()
        rc = p.wait(15)
        done = False
    print(f"    exit={rc}  self_exited={done}", flush=True)
    return rc


def gctrl(exe, arm, knobs):
    d = OUT / "gctrl"
    d.mkdir(parents=True, exist_ok=True)
    step, gates, ptc = d / f"{arm}.step.csv", d / f"{arm}.gates.csv", d / f"{arm}.ptrace.log"
    for f in (step, gates, ptc):
        if f.exists():
            f.unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": "Training",
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": "14400", "MASHED_PLAYERTRACE": "1",
        "MASHED_AI_STEPDUMP": str(step), "MASHED_U9186_GATES": str(gates),
        "MASHED_PLAYERTRACE_OUT": str(ptc),
    })
    parse_knobs(env, knobs)
    run(exe, env, f"G-CTRL arm={arm} knobs={knobs}")
    for f in (step, gates, ptc):
        if f.exists():
            print(f"    {f.name}  lines={sum(1 for _ in f.open('rb'))}  sha8={sha8(f)}", flush=True)
        else:
            print(f"    MISSING {f.name}", flush=True)
    print("GCTRL DONE", flush=True)
    return 0


def dump(exe, arm, track, trace, frames, shots, knobs):
    d = OUT / "dump" / arm
    d.mkdir(parents=True, exist_ok=True)
    for f in ("cam_dump.csv", "cam_dump.csv.nodes.txt", "cam_dump.csv.ribbon.txt", "librw_dump.csv", "course.bin"):
        if (d / f).exists():
            (d / f).unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": track,
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(frames), "MASHED_VERIFY_OUT": str(d),
        "MASHED_CAM_DUMP": str(d / "cam_dump.csv"),
        "MASHED_CAM_DUMP_LIBRW": str(d / "librw_dump.csv"),
    })
    if trace:
        env["MASHED_CAM_REPLAY"] = str(Path(trace).resolve())
    if shots:
        env["MASHED_CAM_REPLAY_SHOTS"] = shots
    parse_knobs(env, knobs)
    if "MASHED_PATHPROG" in env:
        env["MASHED_PATHPROG_DUMP"] = str(d / "course.bin")
    run(exe, env, f"dump arm={arm} track={track} trace={trace or '-'} frames={frames} knobs={knobs}")
    for f in ("cam_dump.csv", "cam_dump.csv.nodes.txt", "cam_dump.csv.ribbon.txt", "librw_dump.csv", "course.bin"):
        q = d / f
        if q.exists():
            n = sum(1 for _ in q.open("rb"))
            print(f"    {f}: {n} lines  sha8={sha8(q)}", flush=True)
        elif f != "course.bin":
            print(f"    {f}: MISSING", flush=True)
    return 0


def demo(exe, arm, knobs):
    """MASHED_RACE_DEMO=1 + MASHED_GOTO=6: the race flow launched from Challenge Select (exe_main.cpp:1578)."""
    d = OUT / "dump" / arm
    d.mkdir(parents=True, exist_ok=True)
    for f in ("cam_dump.csv", "cam_dump.csv.nodes.txt", "cam_dump.csv.ribbon.txt", "librw_dump.csv"):
        if (d / f).exists():
            (d / f).unlink()
    env = dict(os.environ)
    env.pop("MASHED_TRACK_VIEW", None)
    env.update({
        "MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_RACE_DEMO": "1", "MASHED_GOTO": "6",
        "MASHED_WIN_POS": "primary-bl", "MASHED_VERIFY_OUT": str(d),
        "MASHED_CAM_DUMP": str(d / "cam_dump.csv"), "MASHED_CAM_DUMP_LIBRW": str(d / "librw_dump.csv"),
    })
    parse_knobs(env, knobs)
    run(exe, env, f"race-demo arm={arm} knobs={knobs}")
    for f in ("cam_dump.csv", "cam_dump.csv.nodes.txt", "cam_dump.csv.ribbon.txt", "librw_dump.csv"):
        q = d / f
        print(f"    {f}: {sum(1 for _ in q.open('rb')) if q.exists() else 'MISSING'} lines", flush=True)
    for b in sorted(d.rglob("*.bmp")):
        print(f"    capture {b.relative_to(d)}", flush=True)
    return 0


def main():
    a = sys.argv[1:]
    if len(a) < 3 or a[0] not in ("gctrl", "dump", "demo"):
        print(__doc__)
        return 2
    mode, exe, arm = a[0], Path(a[1]).resolve(), a[2]
    rest = a[3:]
    if mode == "gctrl":
        return gctrl(exe, arm, rest)
    if mode == "demo":
        return demo(exe, arm, rest)
    track, trace, frames, shots, knobs = "Training", "", 240, "", []
    i = 0
    while i < len(rest):
        if rest[i] == "--track":
            track = rest[i + 1]; i += 2
        elif rest[i] == "--trace":
            trace = rest[i + 1]; i += 2
        elif rest[i] == "--frames":
            frames = int(rest[i + 1]); i += 2
        elif rest[i] == "--shots":
            shots = rest[i + 1]; i += 2
        else:
            knobs.append(rest[i]); i += 1
    return dump(exe, arm, track, trace, frames, shots, knobs)


if __name__ == "__main__":
    sys.exit(main())
