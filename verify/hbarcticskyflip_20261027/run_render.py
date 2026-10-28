# HB-ARCTIC-SKY-FLIP standalone arms (L-41 flip check). Recipe of verify/hbarcticopensea_20261027/run_render.py:
# MASHED_RACE_DEMO=1 MASHED_GOTO=6, deterministic clock, Arctic (MASHED_TRACK_SEL 0), pinned 12-float
# MASHED_CAM_POSE basis read back from the original capture (HB-ARCTIC-OPENSEA_data/orig/<pose>/),
# capture race1/01_grid.bmp. DEF/DEF2 = flipped default, no env var. NOD = MASHED_NO_ARCTIC_SKY_RTT=1 (pre-flip).
# Explicit PIDs, killed only on timeout.
#
# usage: py -3.12 run_render.py <exe> <root> <datadir> <pose>_<arm> [...]
import os, shutil, subprocess, sys, time
from pathlib import Path

ORIG_DATA = Path("D:/MashedWT/HB-ARCTIC-OPENSEA_data")
ARMS = {"DEF": {}, "DEF2": {}, "NOD": {"MASHED_NO_ARCTIC_SKY_RTT": "1"}}
TIMEOUT = 220.0


def run(exe, root, data, pose, arm):
    basis = ORIG_DATA / "orig" / pose / "orig_cambasis.txt"
    dst = data / "render" / f"{pose}_{arm}"
    if dst.exists():
        shutil.rmtree(dst)
    dst.mkdir(parents=True)
    for f in ("mashed_re.log", "log/librw_race.txt"):
        p = root / f
        if p.exists():
            p.unlink()
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    env.update({
        "MASHED_ROOT": str(root),
        "MASHED_VERIFY_OUT": dst.as_posix(),
        "MASHED_TRACK_SEL": "0",
        "MASHED_RACE_DEMO": "1",
        "MASHED_GOTO": "6",
        "MASHED_DETERMINISTIC": "1",
        "MASHED_MUTE": "1",
        "MASHED_WIN_POS": "left-bl",
        "MASHED_CAM_POSE": basis.read_text().strip(),
        "MASHED_TITLE": f"HB-ARCTIC-SKY-FLIP {pose} {arm}",
    })
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
    exe, root, data = Path(sys.argv[1]).resolve(), Path(sys.argv[2]), Path(sys.argv[3])
    todo = sys.argv[4:]
    for i, pa in enumerate(todo, 1):
        pose, _, arm = pa.rpartition("_")
        st, pid, rc = run(exe, root, data, pose, arm)
        print(f"[{i}/{len(todo)}] {pa} {st} pid={pid} rc={rc}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
