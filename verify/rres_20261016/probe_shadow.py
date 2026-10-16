"""rres: one original-side in-race capture with the car shadow witness (re/frida/camera_probe.py --shadow-out).

    py -3.12 verify\rres_20261016\probe_shadow.py --track 2 --out <abs dir> [--rows 900] [--attempts 3]

Writes <out>/shadow.json (one sample every 0.5 s: per player the record fields, the chosen RwCamera, the hull atomic, the
FUN_00422570 batch with its UVs, the gates and the light), <out>/trace.csv and <out>/stdout.txt. PID hygiene as
verify/rsky_20261016/capture_one.py: the probe prints the pid it spawns; only that pid is killed, and only while its image
name is still MASHED.exe. A piz-collision crash (MASHED.exe+0x1509b4 / +0xe66db) is a re-run, not a finding.
"""
import argparse
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
PROBE = ROOT / "re" / "frida" / "camera_probe.py"


def image_name(pid):
    r = subprocess.run(["tasklist", "/FI", f"PID eq {pid}", "/FO", "CSV", "/NH"], capture_output=True, text=True)
    m = re.match(r'"([^"]+)"', r.stdout.strip())
    return m.group(1) if m else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--track", type=int, required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--rows", type=int, default=900)
    ap.add_argument("--attempts", type=int, default=3)
    ap.add_argument("--timeout", type=int, default=420)
    a = ap.parse_args()
    d = Path(a.out).resolve()
    d.mkdir(parents=True, exist_ok=True)
    for attempt in range(1, a.attempts + 1):
        cmd = [sys.executable, str(PROBE), "--frame", "--early", "--track", str(a.track), "--rows", str(a.rows),
               "--out", str(d / "trace.csv"), "--static-out", str(d / "static.json"),
               "--shadow-out", str(d / "shadow.json")]
        logf = d / "stdout.txt"
        with open(logf, "w", encoding="utf-8", errors="replace") as lf:
            p = subprocess.Popen(cmd, cwd=str(ROOT), stdout=lf, stderr=subprocess.STDOUT)
            try:
                rc = p.wait(timeout=a.timeout)
            except subprocess.TimeoutExpired:
                p.kill(); rc = "timeout"
        text = logf.read_text(encoding="utf-8", errors="replace")
        m = re.search(r"spawned pid=(\d+)", text)
        pid = int(m.group(1)) if m else None
        if pid and (image_name(pid) or "").lower() == "mashed.exe":
            subprocess.run(["taskkill", "/PID", str(pid), "/F"], capture_output=True)
        ok = (d / "shadow.json").exists() and "[rres] shadow samples" in text
        print(f"attempt {attempt}: rc={rc} pid={pid} ok={ok}")
        if ok:
            return 0
        time.sleep(3)
    return 1


if __name__ == "__main__":
    sys.exit(main())
