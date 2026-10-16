"""W1 negative control (PREREG_librwrest_checks.md section 5 addition, run after the arm results):
the comparator must FAIL when the replay inputs are wrong. Takes an arm's replay input, perturbs
units (+1 tick), the steer means (+0.05 deg) and the body vector va.x (+0.01), runs the standalone
replay on it, and compares against the ORIGINAL's recorded state of the unperturbed arm.

    py -3.12 negctl.py <gameroot> <arm>
Expected: W1-a and W1-b both FAIL (angles shift by spin/3000 deg, hub by the steer, body by 0.4 deg).
"""
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXE = HERE.parent.parent / "mashedmod" / "build" / "mashed_re.exe"
TRACK = {"T": "Training", "A": "Arctic", "C": "City"}


def main():
    gameroot, arm = sys.argv[1], sys.argv[2]
    src = HERE / f"orig_{arm}" / "law_in.txt"
    dst = HERE / f"orig_{arm}" / "law_in_neg.txt"
    out = []
    for ln in src.read_text().splitlines():
        t = ln.split()
        t[2] = str(int(t[2]) + 1)                         # units
        # field layout: row car units a0..a3 spin0..3 steer0..3 va0..2 vb0..2 (0-based 0,1,2,3-6,7-10,11-14,15-17,18-20)
        for k in range(11, 15):
            t[k] = repr(float(t[k]) + 0.05)
        t[15] = repr(float(t[15]) + 0.01)                 # va.x
        out.append(" ".join(t))
    dst.write_text("\n".join(out) + "\n")
    cols = None
    import json
    liv = json.loads((HERE / f"orig_{arm}" / "trace.csv.carparts.json").read_text())["livery"]
    cols = ",".join(str(c["colour"]) for c in liv["cars"])
    subprocess.run(["py", "-3.12", str(HERE / "run_stand.py"), str(EXE), str(HERE / f"stand_{arm}_neg"),
                    "--track", TRACK[arm[0]], "--colours", cols, "--law", str(dst), "--frames", "90",
                    "--gameroot", gameroot], cwd=str(HERE))
    rc = subprocess.run(["py", "-3.12", str(HERE / "w1_check.py"), "compare",
                         str(HERE / f"orig_{arm}" / "trace.csv.carparts.json"),
                         str(HERE / f"orig_{arm}" / "law_rec.json"),
                         str(HERE / f"stand_{arm}_neg" / "law_out.txt"),
                         "-o", str(HERE / f"orig_{arm}" / "W1_negctl_report.txt")], cwd=str(HERE)).returncode
    print(f"negative control compare exit {rc} (1 = comparator FAILED as it must)")
    return 0 if rc == 1 else 3


if __name__ == "__main__":
    sys.exit(main())
