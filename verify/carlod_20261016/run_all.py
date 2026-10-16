"""carlod: the rebased measurement campaign, one arm after another (never two standalone runs at once).

    py -3.12 run_all.py <exe_base> <exe_new> [ARM ...]

Arms (name: exe, knobs): base2 (base exe), final2 (new exe, defaults), visx (MASHED_CAR_ENV=0: F-VIS only),
visoff (MASHED_CAR_VIS=0: no-op control), nolagx (MASHED_CAR_ENV_PREVCAM=0), then G-CTRL on both exes.
A launch that wrote no record (PREREG amendment 2) is re-run once for that track, and the arm's summary is then
regenerated from the records on disk (run_arm.py --skip-run).
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
LOG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\log\carlod")
PY = sys.executable


def run_arm(exe, arm, knobs):
    base = [PY, str(HERE / "run_arm.py"), exe, arm]
    out = LOG / f"run_{arm}.txt"
    with open(out, "w") as f:
        subprocess.run(base + knobs, stdout=f, stderr=subprocess.STDOUT)
    s = (LOG / arm / "summary.txt").read_text()
    missing = sorted(set(re.findall(r"^(\w+)\s+\d+\s+MISSING", s, re.M)))
    if missing:
        with open(out, "a") as f:
            f.write("\nRE-RUN of: %s\n" % missing)
        for t in missing:
            with open(out, "a") as f:
                subprocess.run(base + ["--tracks", t] + knobs, stdout=f, stderr=subprocess.STDOUT)
        with open(out, "a") as f:
            subprocess.run(base + ["--skip-run"] + knobs, stdout=f, stderr=subprocess.STDOUT)
        s = (LOG / arm / "summary.txt").read_text()
    print(s, flush=True)


def main():
    exe_base, exe_new = sys.argv[1], sys.argv[2]
    which = sys.argv[3:] or ["base3", "final3", "visx3", "visoff3", "nolag3", "gctrl"]
    plan = {"base3": (exe_base, []), "final3": (exe_new, []), "visx3": (exe_new, ["MASHED_CAR_ENV=0"]),
            "visoff3": (exe_new, ["MASHED_CAR_VIS=0"]), "nolag3": (exe_new, ["MASHED_CAR_ENV_PREVCAM=0"])}
    for a in which:
        if a == "gctrl":
            for name, exe in (("gctrl_base3", exe_base), ("gctrl_final3", exe_new)):
                with open(LOG / f"run_{name}.txt", "w") as f:
                    subprocess.run([PY, str(HERE / "run_gctrl.py"), exe, name], stdout=f, stderr=subprocess.STDOUT)
                print((LOG / f"run_{name}.txt").read_text()[-400:], flush=True)
        else:
            exe, knobs = plan[a]
            print("=== arm", a, flush=True)
            run_arm(exe, a, knobs)


if __name__ == "__main__":
    main()
