# Acceptance driver for the Arctic sea-tile fix (re/analysis/SEA_LEVEL_2026-09-29.md).
#
# Same instrument as verify/car_bright_fix_20260930/run_race.py -- the standalone's
# OWN race-flow driver (MASHED_RACE_DEMO=1 MASHED_GOTO=6, exe_main.cpp:1438
# RunRaceDemoStep) -- with one addition: an optional 12-float MASHED_CAM_POSE
# basis, so an Arctic run can be pinned to a committed ORIGINAL pose
# (verify/arctic_ref/sea_search/s8|s14/orig_cambasis.txt).
#
#   - Injects Enter/Esc INTERNALLY (NavDemoTap); never steals the foreground.
#   - Captures through DumpBackbufferBMP (the real 640x480 backbuffer).
#   - MASHED_DETERMINISTIC=1 replaces the wall clock with a frame counter
#     (exe_main.cpp:376,403), so pre-fix and post-fix runs reach the SAME frame
#     at the SAME pose. That is what makes acceptance rule (iii)'s "0 differing
#     pixels" a meaningful comparison rather than run-to-run jitter.
#   - Waits for process EXIT before reading any capture
#     (memory `race-capture-wait-for-exit`).
#   - Kills ONLY the pid it spawned, and only on timeout.
#   - Never touches verify/race1/ (VOut's default root is verify/run_<pid>,
#     exe_main.cpp:1055-1074).
#
# Usage:
#   py -3.12 verify/sea_fix_20260930/run_race.py <exe> <outdir> [idx,...] [--pose FILE] [--env K=V ...]
import os, shutil, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LOG = ROOT / "mashed_re.log"

AREAS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
         "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]
TIMEOUT = 150.0


def run_one(exe, idx, outdir, pose, extra, tag):
    name = AREAS[idx]
    dst = outdir / f"{idx:02d}_{name}"
    if dst.exists():
        shutil.rmtree(dst)
    dst.mkdir(parents=True)
    if LOG.exists():
        LOG.unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_VERIFY_OUT": dst.as_posix(),
        "MASHED_TRACK_SEL": str(idx),
        "MASHED_RACE_DEMO": "1",
        "MASHED_GOTO": "6",
        "MASHED_DETERMINISTIC": "1",
        "MASHED_MUTE": "1",
        "MASHED_WIN_POS": "left-bl",
        "MASHED_TITLE": f"sea-tile acceptance {tag} {idx}={name}",
    })
    if pose:
        env["MASHED_CAM_POSE"] = pose
    else:
        env.pop("MASHED_CAM_POSE", None)
    env.update(extra)
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
    if LOG.exists():
        shutil.copy2(LOG, dst / "mashed_re.log")
    (dst / "PROVENANCE.txt").write_text(
        f"exe={exe}\npid={proc.pid}\nrc={rc}\nseconds={dur:.1f}\n"
        + "".join(f"{k}={v}\n" for k, v in sorted(env.items())
                  if k.startswith("MASHED_")))
    sea = ""
    lp = dst / "mashed_re.log"
    if lp.exists():
        for ln in lp.read_text(errors="replace").splitlines():
            if "SEA-TILE" in ln and not sea:
                sea = ln.strip()
    shots = sorted(p.name for p in dst.glob("race1/*.bmp"))
    print(f"  rc={rc} {dur:.0f}s shots={shots}")
    if sea:
        print(f"  {sea}")
    return rc


def main():
    argv = sys.argv[1:]
    pose = None
    extra = {}
    tag = ""
    pos = []
    i = 0
    while i < len(argv):
        if argv[i] == "--pose":
            pose = Path(argv[i + 1]).read_text().strip()
            i += 2
        elif argv[i] == "--tag":
            tag = argv[i + 1]
            i += 2
        elif argv[i] == "--env":
            k, _, v = argv[i + 1].partition("=")
            extra[k] = v
            i += 2
        else:
            pos.append(argv[i])
            i += 1
    exe = Path(pos[0]).resolve()
    outdir = Path(pos[1])
    outdir.mkdir(parents=True, exist_ok=True)
    idxs = ([int(v) for v in pos[2].split(",")]
            if len(pos) > 2 else list(range(len(AREAS))))
    if not exe.exists():
        print(f"error: {exe} not found")
        return 2
    print(f"=== run_race exe={exe} pose={'yes' if pose else 'no'} extra={extra}")
    for k in idxs:
        run_one(exe, k, outdir, pose, extra, tag)
    return 0


if __name__ == "__main__":
    sys.exit(main())
