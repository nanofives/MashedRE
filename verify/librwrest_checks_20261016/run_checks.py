"""Standalone side + gates for the librwrest_checks arms (PREREG_librwrest_checks.md).

    py -3.12 run_checks.py <gameroot> <arm> [<arm> ...]

For each arm with an original capture in orig_<arm>/ (run_orig_arms.py):
  1. w1_check.py prep   -> orig_<arm>/law_in.txt, law_rec.json
  2. run_stand.py       -> stand_<arm>/ (mashed_re.log, law_out.txt), the standalone on the arm's track
                           with MASHED_ROUND_COLOURS = the colours the original capture recorded
  3. w1_check.py compare-> orig_<arm>/W1_report.txt
Then l1_check.py over all arms -> L1_report.txt (L1-b skipped for T2: the standalone has no setting
for the mode word 0x0067e9fc).
<gameroot> is a private MASHED_ROOT (a robocopy of the main checkout's original/, outside the repo).
"""
import json
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXE = HERE.parent.parent / "mashedmod" / "build" / "mashed_re.exe"
TRACK = {"T": "Training", "A": "Arctic", "C": "City"}


def run(cmd):
    print("$", " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run([str(c) for c in cmd], cwd=str(HERE)).returncode


def main():
    gameroot = sys.argv[1]
    arms = sys.argv[2:]
    l1_specs = []
    rc_all = 0
    for arm in arms:
        cp = HERE / f"orig_{arm}" / "trace.csv.carparts.json"
        if not cp.exists():
            print(f"{arm}: no original capture ({cp})")
            rc_all = 1
            continue
        liv = json.loads(cp.read_text())["livery"]
        cols = ",".join(str(c["colour"]) for c in liv["cars"])
        run(["py", "-3.12", "w1_check.py", "prep", cp, HERE / f"orig_{arm}" / "law_in.txt",
             HERE / f"orig_{arm}" / "law_rec.json"])
        run(["py", "-3.12", "run_stand.py", EXE, HERE / f"stand_{arm}", "--track", TRACK[arm[0]],
             "--colours", cols, "--law", HERE / f"orig_{arm}" / "law_in.txt", "--frames", "90",
             "--gameroot", gameroot])
        rc = run(["py", "-3.12", "w1_check.py", "compare", cp, HERE / f"orig_{arm}" / "law_rec.json",
                  HERE / f"stand_{arm}" / "law_out.txt", "-o", HERE / f"orig_{arm}" / "W1_report.txt"])
        rc_all |= rc
        spec = f"{arm}=orig_{arm}/trace.csv.carparts.json"
        if arm != "T2":
            spec += f",stand_{arm}/mashed_re.log"
        l1_specs.append(spec)
    if l1_specs:
        rc_all |= run(["py", "-3.12", "l1_check.py", *l1_specs, "-o", HERE / "L1_report.txt"])
    return rc_all


if __name__ == "__main__":
    sys.exit(main())
