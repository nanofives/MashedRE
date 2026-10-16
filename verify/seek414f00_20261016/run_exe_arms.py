"""Exe arms for PREREG_seek414f00.md section 3, one after the other, on frozen images.

    py -3.12 run_exe_arms.py <tag> <exe> [what ...] [-- KNOB=VALUE ...]     what in: gctrl bscore warzone   (default: all)

<tag>_gctrl.log, <tag>_K<k>.log, <tag>_warzone.log here (run_arm.py prints the sha8 of each dump). The Warzone arm also sets
MASHED_SEEK_TRACE=<this dir>/<tag>_seektrace.txt, the knob-took witness (a base image ignores it).
"""
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
ARM = HERE / "run_arm.py"
KS = [0, 1, 2, 4, 26, 27, 54, 57]


def arm(exe, name, extra, knobs):
    p = subprocess.run([sys.executable, str(ARM), exe, name, *extra, *knobs], capture_output=True, text=True, cwd=str(ROOT))
    (HERE / f"{name}.log").write_text(p.stdout + "\n" + p.stderr, encoding="utf-8")
    print(name, " | ".join(ln.strip() for ln in p.stdout.splitlines() if "sha8" in ln or "exit=" in ln or "MISSING" in ln),
          flush=True)


def main() -> int:
    argv = sys.argv[1:]
    knobs = []
    if "--" in argv:
        i = argv.index("--")
        argv, knobs = argv[:i], argv[i + 1:]
    tag, exe = argv[0], argv[1]
    what = argv[2:] or ["gctrl", "bscore", "warzone"]
    if "gctrl" in what:
        arm(exe, f"{tag}_gctrl", ["--frames", "14400", "--gates", "--ptrace"], knobs)
    if "bscore" in what:
        for k in KS:
            arm(exe, f"{tag}_K{k:02d}", ["--frames", "1200", "--ring-skip", str(k)], knobs)
    if "warzone" in what:
        arm(exe, f"{tag}_warzone", ["--track", "Warzone", "--frames", "3600", "--gates"],
            knobs + [f"MASHED_SEEK_TRACE={HERE / (tag + '_seektrace.txt')}"])
    print("ARMS DONE", tag, flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
