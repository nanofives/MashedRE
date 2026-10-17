"""Arm OP of PREREG_pontoon.md: stock MASHED.exe on Warzone with --pontoon-probe (no hooks).

    py -3.12 capture_op.py <tag> [hold_s] [rule]

Runs this worktree's re/frida/scenario_launch.py, MASHED_ROOT = the main checkout (the shared original\\ install; this worktree
has none). scenario_launch spawns and stops only its own pid. Keeps <tag>.msd.pontoon.csv and <tag>.log here; the MSD1 dump
that arms the probe is deleted after the run (its sha8 goes to the log line).
"""
import hashlib
import os
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")


def main() -> int:
    tag = sys.argv[1]
    hold = sys.argv[2] if len(sys.argv) > 2 else "180"
    rule = sys.argv[3] if len(sys.argv) > 3 else "8"
    out = HERE / f"{tag}.msd"
    cmd = [sys.executable, str(WT / "re" / "frida" / "scenario_launch.py"), "--mode", "10", "--cars", "4",
           "--hold", hold, "--statediff-out", str(out), "--pontoon-probe", "20000", "--track", "11", "--rule", rule]
    env = dict(os.environ)
    env["MASHED_ROOT"] = str(MAIN)
    t0 = time.time()
    p = subprocess.run(cmd, cwd=str(WT), env=env, capture_output=True, text=True, timeout=1800)
    sha = ""
    if out.exists():
        sha = hashlib.sha256(out.read_bytes()).hexdigest()[:8]
        out.unlink()
    (HERE / f"{tag}.log").write_text(p.stdout + "\n" + p.stderr + f"\nmsd sha8 {sha}\n", encoding="utf-8")
    tail = [ln for ln in p.stdout.splitlines() if "pontoon-probe" in ln or "VERDICT" in ln or "Traceback" in ln]
    print(f"[{tag}] rc={p.returncode} {time.time() - t0:.0f}s msd {sha}", *tail, sep="\n  ", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
