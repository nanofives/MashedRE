"""carlod2: a measurement campaign, one standalone run at a time (never two at once).

    py -3.12 run_all.py ARM@EXE[@OPT[,OPT...]] ...       arm = 9 matched-pose frames (run_arm.py)
    py -3.12 run_all.py gctrl@EXE@NAME ...               G-CTRL fingerprint (run_gctrl.py)

OPT is a KNOB=V environment setting or the runner flag --rows (carlod's MASHED_CAR_BRAKE_ROWS instrument input,
off by default in this lane). A launch that wrote no record (carlod PREREG amendment 2) is re-run once for that
track and the arm's summary is regenerated from the records on disk (run_arm.py --skip-run).
"""
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
LOG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\log\carlod2")
PY = sys.executable


def run_arm(exe, arm, opts):
    base = [PY, str(HERE / "run_arm.py"), exe, arm]
    out = LOG / f"run_{arm}.txt"
    with open(out, "w") as f:
        subprocess.run(base + opts, stdout=f, stderr=subprocess.STDOUT)
    s = (LOG / arm / "summary.txt").read_text()
    missing = sorted(set(re.findall(r"^(\w+)\s+\d+\s+MISSING", s, re.M)))
    if missing:
        with open(out, "a") as f:
            f.write("\nRE-RUN of: %s\n" % missing)
        for t in missing:
            with open(out, "a") as f:
                subprocess.run(base + ["--tracks", t] + opts, stdout=f, stderr=subprocess.STDOUT)
        with open(out, "a") as f:
            subprocess.run(base + ["--skip-run"] + opts, stdout=f, stderr=subprocess.STDOUT)
        s = (LOG / arm / "summary.txt").read_text()
    print(s, flush=True)


def main():
    LOG.mkdir(parents=True, exist_ok=True)
    for spec in sys.argv[1:]:
        parts = spec.split("@")
        if parts[0] == "gctrl":
            exe, name = parts[1], parts[2]
            with open(LOG / f"run_{name}.txt", "w") as f:
                subprocess.run([PY, str(HERE / "run_gctrl.py"), exe, name], stdout=f, stderr=subprocess.STDOUT)
            print((LOG / f"run_{name}.txt").read_text()[-400:], flush=True)
        else:
            arm, exe = parts[0], parts[1]
            opts = parts[2].split(",") if len(parts) > 2 and parts[2] else []
            print("=== arm", arm, flush=True)
            run_arm(exe, arm, opts)


if __name__ == "__main__":
    main()
