# HB-TEXANIM-REFS render arms (PREREG.md section 3), from verify/hbforestwater_20261027/run_render.py.
# Cases are <track>_s<settle>; the pinned 12-float MASHED_CAM_POSE basis is the original capture's
# orig_cambasis.txt under REFS (raw data, not committed). Arms OFF / OFF2 (default) and ANIM
# (MASHED_TEXANIM=1). After the default flip, arm DEF = no knob env at all. Arm MASK = the PREREG2
# mask arm (MASHED_TEXANIM_MASKTEX). HBTR_POSES=<json> adds the PREREG2 pose cases.
#
# usage: py -3.12 run_render.py <exe> <root> <outdir> case_arm [case_arm ...]
import os, shutil, subprocess, sys, time
from pathlib import Path

W = Path(__file__).resolve().parents[2]
REFS = Path("D:/MashedWT/HB-TEXANIM-REFS_data/orig")
SEL = {"training": 12, "sands": 10, "warzone": 8}
CASES = {f"{t}_s{s}": (SEL[t], REFS / f"{t}_s{s}" / "orig_cambasis.txt")
         for t in SEL for s in (8, 14, 20)}
# PREREG2: pinned water-view poses (case -> [sel, 12-float basis]) from a JSON file; the basis file
# of a pose case is REFS2/<case>/orig_cambasis.txt once the original side is captured, else the JSON.
POSES = os.environ.get("HBTR_POSES")
if POSES:
    import json
    REFS2 = Path("D:/MashedWT/HB-TEXANIM-REFS_data/orig2")
    for k, (sel_, basis_) in json.loads(Path(POSES).read_text()).items():
        bf = REFS2 / k / "orig_cambasis.txt"
        CASES[k] = (sel_, bf if bf.exists() else basis_)
MASKTEX = {"training": "Brick", "sands": "BarrelRed", "warzone": "BarrelRed"}
ARMS = {"OFF": {}, "OFF2": {}, "ANIM": {"MASHED_TEXANIM": "1"}, "DEF": {}, "MASK": {"MASHED_TEXANIM": "1"}}
DEFAULT = [f"{c}_{a}" for c in CASES for a in ("OFF", "ANIM")] + [f"{t}_s8_OFF2" for t in SEL]
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
        "MASHED_TITLE": f"HB-TEXANIM-REFS {case} {arm}",
    })
    if basis is not None:
        env["MASHED_CAM_POSE"] = basis if isinstance(basis, str) else basis.read_text().strip()
    env.update(ARMS[arm])
    if arm == "MASK":
        env["MASHED_TEXANIM_MASKTEX"] = MASKTEX[case.split("_")[0]]
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
