"""Original-side capture arms for PREREG_seek414f00.md section 1 (stock MASHED.exe, no hooks).

    py -3.12 capture.py [arm ...]        arms: O1 O2 O3 OC (default all)

Each arm runs re/frida/scenario_launch.py with --seek-probe from this worktree, MASHED_ROOT = the main checkout (the shared
original\\ install). scenario_launch spawns and stops only its own pid. Outputs <arm>.msd.seekobj.csv / <arm>.msd.seek.csv and
<arm>.log here; the MSD1 statediff dump itself (<arm>.msd) is only the vehicle to arm the probe and is deleted after the run.
"""
import os
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
ARMS = {
    "O1": ["--track", "11", "--rule", "0"],
    "O2": ["--track", "11", "--rule", "4"],
    "O3": ["--track", "11", "--rule", "8"],
    "OC": ["--track", "0", "--rule", "0"],
}


def main() -> int:
    arms = sys.argv[1:] or list(ARMS)
    for a in arms:
        out = HERE / f"{a}.msd"
        cmd = [sys.executable, str(WT / "re" / "frida" / "scenario_launch.py"), "--mode", "10", "--cars", "4",
               "--hold", "120", "--statediff-out", str(out), "--seek-probe", "20000", *ARMS[a]]
        env = dict(os.environ)
        env["MASHED_ROOT"] = str(MAIN)
        t0 = time.time()
        p = subprocess.run(cmd, cwd=str(WT), env=env, capture_output=True, text=True, timeout=900)
        (HERE / f"{a}.log").write_text(p.stdout + "\n" + p.stderr, encoding="utf-8")
        if out.exists():
            out.unlink()
        tail = [ln for ln in p.stdout.splitlines() if "seek-probe" in ln or "VERDICT" in ln]
        print(f"[{a}] rc={p.returncode} {time.time() - t0:.0f}s", *tail, sep="\n  ", flush=True)
    print("CAPTURE DONE", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
