"""path1 driver for the dupinstall lane: one run_diff.py spawn per registry key (the warm pool cannot drive
const_return / global4_bool_out / linear_scan_find and loses its session on others, see path1_baseline_s1.log).

    py -3.12 run_path1.py <outdir> <key> [<key> ...]

Each key: up to 2 attempts (a piz-collision crash is a re-run, never a finding -- CLAUDE.md U-9677).
Copies log/diff_<key>.csv into <outdir>. Spawns only through run_diff.py, which owns its own PID.
"""
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
RUN_DIFF = ROOT / "re" / "frida" / "run_diff.py"


EARLY = ROOT / "re" / "frida" / "early_window_leaf_diff.py"
ASI = ROOT / "mashedmod" / "build" / "mashed_re_dev.asi"


def _spawn(script: Path, key: str) -> tuple[str, int]:
    env = dict(os.environ)
    env["MASHED_EW_ASI"] = str(ASI)      # the worktree build, never the shared deployed copy
    try:
        p = subprocess.run([sys.executable, str(script), key], cwd=str(ROOT), env=env,
                           capture_output=True, text=True, timeout=300)
        return (p.stdout or "") + "\n" + (p.stderr or ""), p.returncode
    except subprocess.TimeoutExpired as e:
        txt = (e.stdout or b"").decode("utf-8", "replace") if isinstance(e.stdout, bytes) else (e.stdout or "")
        return txt + "\nTIMEOUT", -1


def one(key: str, out: Path) -> tuple[str, str]:
    last = ""
    for attempt in (1, 2):
        t0 = time.time()
        txt, rc = _spawn(RUN_DIFF, key)
        if "has no handler in diff_template.js" in txt:
            txt, rc = _spawn(EARLY, key)
            txt = "[harness: early_window_leaf_diff.py]\n" + txt
        (out / f"{key}.run{attempt}.txt").write_text(txt, encoding="utf-8")
        m = re.findall(r"(GREEN|RED)[^\n]*", txt)
        verdict = m[-1] if m else "NO-VERDICT"
        last = f"rc={rc} {verdict} ({time.time()-t0:.0f}s)"
        crashed = ("session is gone" in txt) or ("TIMEOUT" in txt) or ("Process crashed" in txt) or not m
        if rc == 0 and not crashed:
            break
        if "RED" in verdict and not crashed and attempt == 2:
            break
    csv = ROOT / "log" / f"diff_{key}.csv"
    if csv.exists():
        shutil.copy2(csv, out / f"diff_{key}.csv")
    return last, verdict


def main() -> int:
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=True)
    rows = []
    for key in sys.argv[2:]:
        status, verdict = one(key, out)
        print(f"{key:36s} {status}", flush=True)
        rows.append((key, status))
    (out / "summary.txt").write_text("\n".join(f"{k}\t{s}" for k, s in rows) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
