"""Run one in-process A/B driver of Ai/AiControllerAB.cpp in a live race and keep only what THIS run appended to its log.

    py -3.12 ab_run.py <outdir> <tag> <driver> <logname> [scenario_launch args ...]

e.g. py -3.12 ab_run.py OUT track0 AbAiBank ai_ab_bank.log --track 0 --cars 4 --hold 120

The drivers (MASHED_AI_AB=1, exactly one selected by MASHED_HOOK_ONLY=<driver>) run the PORT, roll the state back, run
the ORIGINAL, and compare the whole write surface byte for byte (AiControllerAB.cpp header). The log lives in the game's
cwd, original\\ of the MAIN checkout, which other sessions share, so the file is never deleted or truncated here: its size
is read before the run and only the bytes appended after it are kept.

scenario_launch.py spawns and kills only its own pid. It needs the .asi under test deployed in original\\ (window.py does that).
Prints the last summary line and, if any, the count of MISMATCH lines.
"""
import os
import re
import subprocess
import sys
import time
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
ORIG = MAIN / "original"


def main() -> int:
    out, tag, driver, logname = Path(sys.argv[1]), sys.argv[2], sys.argv[3], sys.argv[4]
    extra = sys.argv[5:]
    out.mkdir(parents=True, exist_ok=True)
    log = ORIG / logname
    before = log.stat().st_size if log.exists() else 0
    env = dict(os.environ)
    env["MASHED_ROOT"] = str(MAIN)
    env["MASHED_AI_AB"] = "1"
    cmd = [sys.executable, str(WT / "re" / "frida" / "scenario_launch.py"), *extra, "--hooks", driver]
    t0 = time.time()
    p = subprocess.run(cmd, cwd=str(WT), env=env, capture_output=True, text=True, timeout=900)
    (out / f"{driver}.{tag}.launch.txt").write_text(p.stdout + "\n" + p.stderr, encoding="utf-8")
    new = b""
    if log.exists():
        with log.open("rb") as f:
            f.seek(before)
            new = f.read()
    (out / f"{driver}.{tag}.log").write_bytes(new)
    text = new.decode("utf-8", "replace")
    lines = [ln for ln in text.splitlines() if ln.strip()]
    summ = [ln for ln in lines if "calls=" in ln]
    mism = [ln for ln in lines if "MISMATCH" in ln]
    print(f"[{driver}/{tag}] rc={p.returncode} {time.time()-t0:.0f}s  new log bytes={len(new)}  "
          f"summary lines={len(summ)}  MISMATCH lines={len(mism)}")
    if summ:
        print("  last summary:", summ[-1])
    for ln in mism[:5]:
        print("  ", ln)
    refused = ORIG / "ai_ab_refused.log"
    if refused.exists() and refused.stat().st_mtime > t0:
        print("  REFUSED log touched during this run:", refused.read_text(errors="replace")[-200:])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
