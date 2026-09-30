# Acceptance driver for CAR_BRIGHTNESS_2026-09-30.md A1-A4.
#
# Replaces the investigation session's PrintWindow + external-keystroke recipe
# with the standalone's OWN race-flow driver (MASHED_RACE_DEMO=1 MASHED_GOTO=6,
# exe_main.cpp:1438 RunRaceDemoStep). Three reasons this is the better
# instrument here, not just a convenience:
#
#   1. It injects the Enter/Esc edges INTERNALLY (NavDemoTap), so no run steals
#      the foreground window from the user's desktop.
#   2. It captures through DumpBackbufferBMP, i.e. the real 640x480 backbuffer,
#      the same channel the original side is dumped through -- not a PrintWindow
#      grab of a resampled client area with window chrome in it.
#   3. With MASHED_DETERMINISTIC=1 the wall clock is replaced by a frame counter
#      (exe_main.cpp:376,403), so a pre-fix and a post-fix run reach the SAME
#      frame with the SAME pose. A4's terrain/sea guard then compares identical
#      poses instead of two poses within run-to-run jitter.
#
# Captures land in <outdir>/<idx>_<name>/race1/ : 00_challengeselect.bmp,
# 01_grid.bmp, 01_inrace_track.bmp, 01_action.bmp, 02_back_to_menu.bmp, plus the
# run's mashed_re.log and mashed_re_carlight.log slices alongside.
#
# MASHED_VERIFY_OUT is pointed at that per-track directory. verify/race1/ is
# NEVER written or deleted here: it holds nine TRACKED, cited acceptance stills
# (DEFERRED.md, ROADMAP.md, CHANGELOG.md all cite them), and VOut's default root
# is verify/run_<pid> precisely so a harness cannot clobber them by accident
# (exe_main.cpp:1055-1074).
#
# Waits for process EXIT before reading any capture (memory
# `race-capture-wait-for-exit`: polling for the BMP returns a menu frame).
# Kills ONLY the pid it spawned, and only on timeout.
#
# Usage: py -3.12 verify/car_bright_fix_20260930/run_race.py <exe> <outdir> <idx,...>
import os, shutil, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LOG = ROOT / "mashed_re.log"
CARLOG = ROOT / "mashed_re_carlight.log"

AREAS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
         "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]
TIMEOUT = 150.0


def run_one(exe, idx, outdir):
    name = AREAS[idx]
    dst = outdir / f"{idx:02d}_{name}"
    if dst.exists():
        shutil.rmtree(dst)
    dst.mkdir(parents=True)
    for p in (LOG, CARLOG):
        if p.exists():
            p.unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_VERIFY_OUT": dst.as_posix(),
        "MASHED_TRACK_SEL": str(idx),
        "MASHED_RACE_DEMO": "1",
        "MASHED_GOTO": "6",
        # Deterministic clock ON by default (A4 needs bit-identical poses). Set
        # MASHED_DETERMINISTIC=0 in the parent env to drop it -- the frame-counter
        # clock also freezes the car (heading stays -1.57603 on every capture), so
        # any measurement that needs the car to actually TURN must run real-time.
        "MASHED_DETERMINISTIC": "1",
        "MASHED_DBG_CARLIGHT": "1",
        "MASHED_MUTE": "1",
        "MASHED_WIN_POS": "left-bl",
        "MASHED_TITLE": f"car-bright acceptance {idx}={name}",
    })
    env.pop("MASHED_NAV_DEMO", None)
    if os.environ.get("MASHED_DETERMINISTIC") == "0":
        env.pop("MASHED_DETERMINISTIC", None)
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
    for p, nm in ((LOG, "mashed_re.log"), (CARLOG, "mashed_re_carlight.log")):
        if p.exists():
            shutil.copy2(p, dst / nm)
    (dst / "PROVENANCE.txt").write_text(
        f"exe={exe}\npid={proc.pid}\nrc={rc}\nseconds={dur:.1f}\n"
        + "".join(f"{k}={v}\n" for k, v in sorted(env.items())
                  if k.startswith("MASHED_")))
    ws = car = ""
    lp = dst / "mashed_re.log"
    if lp.exists():
        for ln in lp.read_text(errors="replace").splitlines():
            if "WS-E lights:" in ln and not ws:
                ws = ln.strip()
    cp = dst / "mashed_re_carlight.log"
    if cp.exists():
        for ln in cp.read_text(errors="replace").splitlines():
            if ln.startswith("CARLIGHT") and not car:
                car = ln.strip()
    shots = sorted(p.name for p in dst.glob("race1/*.bmp"))
    print(f"  rc={rc} {dur:.0f}s shots={shots}")
    print(f"  {ws or 'NO WS-E LINE'}")
    if car:
        print(f"  {car}")
    return ws, car


def main():
    exe = Path(sys.argv[1]).resolve()
    outdir = Path(sys.argv[2])
    outdir.mkdir(parents=True, exist_ok=True)
    idxs = ([int(v) for v in sys.argv[3].split(",")]
            if len(sys.argv) > 3 else list(range(len(AREAS))))
    if not exe.exists():
        print(f"error: {exe} not found")
        return 2
    print(f"=== run_race exe={exe}")
    rows = []
    for i in idxs:
        rows.append((i, AREAS[i]) + run_one(exe, i, outdir))
    print("\n=== summary ===")
    for i, nm, ws, car in rows:
        d = ws.split("dir=", 1)[1] if "dir=" in ws else "?"
        l = car.split("L=", 1)[1].split(")")[0] + ")" if "L=" in car else "?"
        print(f"{i:2d} {nm:<9} dir={d}  L={l}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
