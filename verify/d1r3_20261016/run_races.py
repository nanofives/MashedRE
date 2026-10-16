# D1-residue R3 evidence driver: capture one race-demo run per track with a given exe.
#
# Adapted from verify/car_bright_fix_20260930/run_race.py (same RunRaceDemoStep driver,
# MASHED_RACE_DEMO=1 MASHED_GOTO=6, deterministic frame clock, DumpBackbufferBMP).
# Differences, all because this runs from a git WORKTREE:
#   - The exe self-locates the asset root by walking up from its own path and finds the
#     MAIN repo root (the worktree has no original\TOASTART\Common\Frontend.piz). It then
#     chdirs THERE, so mashed_re.log and log/librw_race.txt land in the main checkout.
#     MAIN below is that root; the per-run slices are copied back out of it.
#   - Captures (BMPs, drawstream3d json) go to private ABSOLUTE paths under <outdir>.
#   - Every inherited MASHED_* variable is dropped first, so the arm is the pure default
#     build plus exactly the capture variables set below.
#
# Per track <idx>_<name>/ : race1/*.bmp, drawstream3d.json, mashed_re.log,
# librw_race.txt, PROVENANCE.txt. Waits for process EXIT before reading anything
# (memory race-capture-wait-for-exit). Kills ONLY the pid it spawned, and only on timeout.
#
# Usage: py -3.12 run_races.py <exe> <outdir> [idx,idx,...]   (default: all 13 tracks)
import os, shutil, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
MAIN = WT.parent.parent          # .worktrees/<name> -> main repo root
LOG = MAIN / "mashed_re.log"
RACELOG = MAIN / "log" / "librw_race.txt"

AREAS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
         "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]
TIMEOUT = 150.0


def run_one(exe, idx, outdir):
    name = AREAS[idx]
    dst = outdir / f"{idx:02d}_{name}"
    if dst.exists():
        shutil.rmtree(dst)
    dst.mkdir(parents=True)
    # The two logs live in the MAIN checkout and are shared with every other session's
    # exe. A concurrent session can hold them open (PermissionError on unlink), so this
    # is best effort; the copied librw log is validated below instead.
    for p in (LOG, RACELOG):
        try:
            if p.exists():
                p.unlink()
        except OSError:
            pass
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env.update({
        "MASHED_VERIFY_OUT": dst.as_posix(),
        "MASHED_TRACK_SEL": str(idx),
        "MASHED_RACE_DEMO": "1",
        "MASHED_GOTO": "6",
        "MASHED_DETERMINISTIC": "1",
        "MASHED_DBG_DRAWSTREAM3D": "60:62",
        "MASHED_DBG_DRAWSTREAM3D_OUT": (dst / "drawstream3d.json").as_posix(),
        "MASHED_MUTE": "1",
        "MASHED_WIN_POS": "primary-bl",
        "MASHED_TITLE": f"d1r3 {idx}={name}",
    })
    proc = subprocess.Popen([str(exe)], cwd=str(WT), env=env,
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
        print(f"  TIMEOUT after {TIMEOUT:.0f}s -> killing pid {proc.pid} ONLY")
        try:
            proc.kill()
        except Exception:
            pass
        proc.wait(timeout=10)
        rc = "killed"
    dur = time.time() - t0
    for p, nm in ((LOG, "mashed_re.log"), (RACELOG, "librw_race.txt")):
        try:
            if p.exists():
                shutil.copy2(p, dst / nm)
        except OSError:
            pass
    (dst / "PROVENANCE.txt").write_text(
        f"exe={exe}\npid={proc.pid}\nrc={rc}\nseconds={dur:.1f}\n"
        + "".join(f"{k}={v}\n" for k, v in sorted(env.items())
                  if k.startswith("MASHED_")))
    shots = sorted(p.name for p in dst.glob("race1/*.bmp"))
    warns = 0
    inits = 0
    models = 0
    rl = dst / "librw_race.txt"
    if rl.exists():
        rlines = rl.read_text(errors="replace").splitlines()
        warns = sum(1 for ln in rlines if ln.startswith(("WARN", "FAIL")))
        inits = sum(1 for ln in rlines if ln.startswith("ok: init"))
        models = sum(1 for ln in rlines if " registered:" in ln)
    # inits must be exactly 1: more means another session's exe wrote the same shared log
    print(f"  rc={rc} {dur:.0f}s shots={len(shots)} ds3d={(dst / 'drawstream3d.json').exists()} "
          f"librw: init_lines={inits} models_registered={models} WARN/FAIL={warns}"
          f"{'' if inits == 1 else '  <-- LOG NOT CLEAN, do not trust WARN/FAIL count'}")
    return rc, len(shots), warns


def main():
    exe = Path(sys.argv[1]).resolve()
    outdir = Path(sys.argv[2]).resolve()
    outdir.mkdir(parents=True, exist_ok=True)
    idxs = ([int(v) for v in sys.argv[3].split(",")]
            if len(sys.argv) > 3 else list(range(len(AREAS))))
    if not exe.exists():
        print(f"error: {exe} not found")
        return 2
    print(f"=== run_races exe={exe}")
    bad = 0
    for i in idxs:
        rc, n, w = run_one(exe, i, outdir)
        if rc != 0 or n == 0:
            bad += 1
    print(f"=== done, {bad} track(s) with non-zero rc or no shots")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
