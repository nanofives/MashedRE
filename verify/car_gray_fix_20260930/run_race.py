# Acceptance driver for CAR_GRAY_FIX_ACCEPTANCE_2026-09-30.md G1-G4.
#
# Same instrument as verify/car_bright_fix_20260930/run_race.py (the standalone
# drives its OWN race flow via MASHED_RACE_DEMO=1 MASHED_GOTO=6, so nothing
# steals the foreground and the capture is the real 640x480 backbuffer through
# DumpBackbufferBMP), with three additions this acceptance needs:
#
#   * MASHED_DBG_DRAWSTREAM3D=1 -> the camera-INVARIANT per-category geometry
#     tally (log/drawstream3d_re.json, DrawStreamDump.cpp:127). G1/G3 are
#     scored on its "cars" record: {batches, verts, textured}.
#   * log/mashed_re.log is collected as well as ./mashed_re.log. They are two
#     different files: RaceSession.cpp:14 kLog = "log/mashed_re.log" is where
#     LoadCar/LoadCarLiveries write their R5/R6 lines, while exe_main's
#     kLogPath is CWD-relative "mashed_re.log" (memory
#     `standalone-debug-log-is-cwd-relative`). G1's per-car atomic counts are
#     in the FORMER; a harness that only copies the latter reports NO DATA and
#     every G1 row would read as a false RED.
#   * both logs are truncated before each run so the R5/R6 lines collected
#     belong to this run only (log/mashed_re.log is opened with "a").
#
# MASHED_DETERMINISTIC=1 so frame N pre-fix and frame N post-fix are the same
# pose -- G4 needs bit-identical poses, not jitter-bounded ones.
#
# Waits for process EXIT before reading any capture (memory
# `race-capture-wait-for-exit`). Kills ONLY the pid it spawned, on timeout.
#
# Usage: py -3.12 verify/car_gray_fix_20260930/run_race.py <exe> <outdir> [idx,...]
import os, shutil, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LOGS = [(ROOT / "mashed_re.log", "mashed_re.log"),
        (ROOT / "mashed_re_carlight.log", "mashed_re_carlight.log"),
        (ROOT / "log" / "mashed_re.log", "race_mashed_re.log")]

AREAS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
         "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]
TIMEOUT = 150.0


def run_one(exe, idx, outdir):
    name = AREAS[idx]
    dst = outdir / f"{idx:02d}_{name}"
    if dst.exists():
        shutil.rmtree(dst)
    dst.mkdir(parents=True)
    for p, _ in LOGS:
        if p.exists():
            p.unlink()
    ds3d = dst / "drawstream3d.json"
    env = dict(os.environ)
    env.update({
        "MASHED_VERIFY_OUT": dst.as_posix(),
        "MASHED_TRACK_SEL": str(idx),
        "MASHED_RACE_DEMO": "1",
        "MASHED_GOTO": "6",
        "MASHED_DETERMINISTIC": "1",
        "MASHED_DBG_CARLIGHT": "1",
        "MASHED_DBG_DRAWSTREAM3D": "1",
        "MASHED_DBG_DRAWSTREAM3D_OUT": ds3d.as_posix(),
        "MASHED_MUTE": "1",
        "MASHED_WIN_POS": "left-bl",
        "MASHED_TITLE": f"car-grey acceptance {idx}={name}",
    })
    env.pop("MASHED_NAV_DEMO", None)
    proc = subprocess.Popen([str(exe)], cwd=str(ROOT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(f"  pid={proc.pid} track={idx} ({name})", flush=True)
    t0 = time.time()
    rc = None
    while time.time() - t0 < TIMEOUT:
        rc = proc.poll()
        if rc is not None:
            break
        time.sleep(0.25)
    if rc is None:
        print(f"  TIMEOUT after {TIMEOUT:.0f}s -> killing pid {proc.pid}")
        try:
            proc.kill()
        except Exception:
            pass
        proc.wait(timeout=10)
        rc = "killed"
    dur = time.time() - t0
    for p, nm in LOGS:
        if p.exists():
            shutil.copy2(p, dst / nm)
    (dst / "PROVENANCE.txt").write_text(
        f"exe={exe}\npid={proc.pid}\nrc={rc}\nseconds={dur:.1f}\n"
        + "".join(f"{k}={v}\n" for k, v in sorted(env.items())
                  if k.startswith("MASHED_")))
    shots = sorted(p.name for p in dst.glob("race1/*.bmp"))
    print(f"  rc={rc} {dur:.0f}s shots={shots}")
    for nm, key in (("race_mashed_re.log", "R5 car atomics"),
                    ("race_mashed_re.log", "R6 livery atomics"),
                    ("race_mashed_re.log", "R5 car load"),
                    ("race_mashed_re.log", "R6 car liveries"),
                    ("mashed_re_carlight.log", "CARLIGHT")):
        fp = dst / nm
        if not fp.exists():
            continue
        for ln in fp.read_text(errors="replace").splitlines():
            if key in ln:
                print(f"    {ln.strip()}")
    if ds3d.exists():
        print(f"    drawstream3d -> {ds3d.name} "
              f"({ds3d.stat().st_size} B)")
    else:
        print("    NO drawstream3d.json  <-- G1/G3 channel MISSING")
    return rc


def main():
    exe = Path(sys.argv[1]).resolve()
    outdir = Path(sys.argv[2])
    outdir.mkdir(parents=True, exist_ok=True)
    idxs = ([int(v) for v in sys.argv[3].split(",")]
            if len(sys.argv) > 3 else [12, 0])
    if not exe.exists():
        print(f"error: {exe} not found")
        return 2
    print(f"=== run_race exe={exe}")
    for i in idxs:
        run_one(exe, i, outdir)
    return 0


if __name__ == "__main__":
    sys.exit(main())
