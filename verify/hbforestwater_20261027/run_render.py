# HB-FOREST-WATER render arms (PREREG.md section 3), copied from verify/hbarctic_20261027/run_render.py. Same recipe as
# verify/sea_fix_20260930/run_race.py: the standalone's own race-flow driver
# (MASHED_RACE_DEMO=1 MASHED_GOTO=6), deterministic clock, pinned 12-float
# MASHED_CAM_POSE basis from a committed original capture, capture race1/01_grid.bmp.
# Private root (MASHED_ROOT), explicit PIDs only, killed only on timeout.
#
# usage: py -3.12 run_render.py <exe> <root> <outdir> [case_arm ...]
import os, shutil, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
CASES = {
    "forest_s8":  (3, W / "verify/water_refs/forest_s8/orig_cambasis.txt"),
    "sands":      (10, None),
    "training":   (12, None),
    "warzone":    (8, None),
}
ARMS = {"OFF": {}, "OFF2": {}, "ANIM": {"MASHED_TEXANIM": "1"}}
DEFAULT = ["forest_s8_OFF", "forest_s8_OFF2", "forest_s8_ANIM",
           "sands_ANIM", "training_ANIM", "warzone_ANIM"]
TIMEOUT = 220.0


def run(exe, root, out, case, arm):
    sel, basis = CASES[case]
    dst = out / f"{case}_{arm}"
    if dst.exists():
        shutil.rmtree(dst)
    dst.mkdir(parents=True)
    for f in ("mashed_re.log", "log/librw_race.txt"):
        p = root / f
        if p.exists():
            p.unlink()
    env = {k: v for k, v in os.environ.items()
           if not k.startswith("MASHED_")}
    env.update({
        "MASHED_ROOT": str(root),
        "MASHED_VERIFY_OUT": dst.as_posix(),
        "MASHED_TRACK_SEL": str(sel),
        "MASHED_RACE_DEMO": "1",
        "MASHED_GOTO": "6",
        "MASHED_DETERMINISTIC": "1",
        "MASHED_MUTE": "1",
        "MASHED_WIN_POS": "left-bl",
        "MASHED_TITLE": f"HB-FOREST-WATER {case} {arm}",
    })
    if basis is not None:
        env["MASHED_CAM_POSE"] = basis.read_text().strip()
    env.update(ARMS[arm])
    shot = dst / "race1" / "01_grid.bmp"
    for attempt in range(1, 4):
        proc = subprocess.Popen([str(exe)], cwd=str(root), env=env,
                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        t0 = time.time()
        rc = None
        while time.time() - t0 < TIMEOUT:
            rc = proc.poll()
            if rc is not None:
                break
            time.sleep(0.25)
        if rc is None:
            proc.kill()
            proc.wait(timeout=10)
            rc = "killed"
        for f, name in (("mashed_re.log", "mashed_re.log"), ("log/librw_race.txt", "librw_race.txt")):
            p = root / f
            if p.exists():
                shutil.copy2(p, dst / name)
        (dst / "PROVENANCE.txt").write_text(
            f"exe={exe}\npid={proc.pid}\nrc={rc}\nattempt={attempt}\nseconds={time.time() - t0:.1f}\n"
            + "".join(f"{k}={v}\n" for k, v in sorted(env.items()) if k.startswith("MASHED_")))
        if shot.exists():
            return "PASS", proc.pid, rc
    return "FAIL", proc.pid, rc


def main():
    exe, root, out = Path(sys.argv[1]).resolve(), Path(sys.argv[2]), Path(sys.argv[3])
    todo = sys.argv[4:] or DEFAULT
    out.mkdir(parents=True, exist_ok=True)
    for i, ca in enumerate(todo, 1):
        case, _, arm = ca.rpartition("_")
        st, pid, rc = run(exe, root, out, case, arm)
        print(f"[{i}/{len(todo)}] {ca} {st} pid={pid} rc={rc}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
