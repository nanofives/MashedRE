"""Run the baseline arms on a frozen exe, one after the other, logging to <tag>_*.log in this directory.

    py -3.12 run_baselines.py <exe> <tag> [what ...]      what in: gctrl bscore warzone cam camN0   (default: all)

`cam` replays the d1r2 original trace (verify/d1r2_20261016/orig2_trace_v3.csv) through run_replay.py and keeps cam_dump.csv /
librw_dump.csv under cam_<tag>/ with their sha8; camN0 is the same run again (the no-op control).
"""
import hashlib
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
ARM = HERE / "run_arm.py"
KS = [0, 1, 2, 4, 26, 27, 54, 57]


def run(cmd, log: Path) -> str:
    p = subprocess.run(cmd, capture_output=True, text=True, cwd=str(ROOT))
    log.write_text(p.stdout + "\n" + p.stderr, encoding="utf-8")
    return p.stdout


def arm(exe, name, extra):
    return run([sys.executable, str(ARM), exe, name, *extra], HERE / f"{name}.log")


def main() -> int:
    exe, tag = sys.argv[1], sys.argv[2]
    what = sys.argv[3:] or ["gctrl", "bscore", "warzone", "cam", "camN0"]
    if "gctrl" in what:
        arm(exe, f"{tag}_gctrl", ["--frames", "14400", "--gates", "--ptrace"])
        print("gctrl", flush=True)
    if "bscore" in what:
        for k in KS:
            arm(exe, f"{tag}_K{k:02d}", ["--frames", "1200", "--ring-skip", str(k)])
            print("bscore K", k, flush=True)
    if "warzone" in what:
        arm(exe, f"{tag}_warzone", ["--track", "Warzone", "--frames", "3600", "--gates"])
        print("warzone", flush=True)
    for nm in ("cam", "camN0"):
        if nm in what:
            out = HERE / f"{nm}_{tag}"
            run([sys.executable, str(ROOT / "verify" / "d1r2_20261016" / "run_replay.py"), exe, str(out),
                 "--trace", str(ROOT / "verify" / "d1r2_20261016" / "orig2_trace_v3.csv"), "--frames", "3000"],
                HERE / f"{nm}_{tag}.log")
            for f in ("cam_dump.csv", "librw_dump.csv"):
                q = out / f
                if q.exists():
                    print(nm, f, hashlib.sha256(q.read_bytes()).hexdigest()[:8], sum(1 for _ in q.open()), flush=True)
    print("BASELINES DONE", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
