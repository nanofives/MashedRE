"""Original-side capture driver for the librwrest_checks arms (PREREG_librwrest_checks.md section 4).

    py -3.12 run_orig_arms.py <arm> [<arm> ...]        arms: T0 A0 C0 T1 A1 C1 T2

Runs re/frida/camera_probe.py --frame --early --carparts for one arm at a time and writes
<this dir>/orig_<arm>/trace.csv (+ .carparts.json, static.json, orig_row_*.bmp). A run counts only if
the carparts JSON holds snapshots and a 4-car livery record; otherwise it is re-run (up to 3 tries:
the original's concurrent-piz-load crash and a missed menu nav are collisions, not findings).
Only the pid camera_probe printed ("spawned pid=N") is ever stopped, and only if it is still alive.
"""
import os
import re
import subprocess
import sys
import json
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent          # worktree root
PROBE = WT / "re" / "frida" / "camera_probe.py"

ARMS = {
    # arm: (track index, extra camera_probe args)
    "T0": (0, []),
    "A0": (3, []),
    "C0": (12, []),
    "T1": (0, ["--colours", "1,2,3,4"]),
    "A1": (3, ["--colours", "5,4,3,2"]),
    "C1": (12, ["--colours", "3,0,5,1"]),
    "T2": (0, ["--livery-mode", "11"]),
}
ROWS = 2200
SHOTS = "100,250,400"


def alive(pid):
    out = subprocess.run(["tasklist", "/FI", f"PID eq {pid}", "/FO", "CSV", "/NH"],
                         capture_output=True, text=True).stdout
    return "MASHED.exe" in out


def run_arm(arm):
    track, extra = ARMS[arm]
    out = HERE / f"orig_{arm}"
    out.mkdir(exist_ok=True)
    trace = out / "trace.csv"
    for stale in list(out.glob("trace.csv*")) + list(out.glob("orig_row_*.bmp")):
        stale.unlink()
    for attempt in range(1, 4):
        cmd = ["py", "-3.12", str(PROBE), "--frame", "--early", "--carparts",
               "--shots-race", SHOTS, "--track", str(track), "--rows", str(ROWS),
               "--out", str(trace), "--static-out", str(out / "static.json")] + extra
        print(f"=== {arm} try {attempt}: {' '.join(cmd[2:])}", flush=True)
        log = out / f"probe_try{attempt}.log"
        t0 = time.time()
        with open(log, "w") as lf:
            p = subprocess.Popen(cmd, cwd=str(WT), stdout=lf, stderr=subprocess.STDOUT)
            try:
                rc = p.wait(900)
            except subprocess.TimeoutExpired:
                p.kill()
                rc = -999
        txt = log.read_text(errors="replace")
        m = re.search(r"spawned pid=(\d+)", txt)
        if m and alive(int(m.group(1))):
            print(f"    probe left pid {m.group(1)} running, stopping it (own pid only)", flush=True)
            subprocess.run(["taskkill", "/PID", m.group(1), "/F"], capture_output=True)
        cp_path = Path(str(trace) + ".carparts.json")
        good = False
        if cp_path.exists():
            cp = json.loads(cp_path.read_text())
            liv = cp.get("livery") or {}
            nrows = len([r for r in cp.get("rows", []) if "s" in r])
            good = nrows > 0 and len(liv.get("cars", [])) == 4 and all("dff" in c for c in liv["cars"])
            print(f"    rc={rc} {time.time() - t0:.0f}s snapshots={nrows} livery={'ok' if good else liv}",
                  flush=True)
        else:
            print(f"    rc={rc} {time.time() - t0:.0f}s NO carparts json (see {log.name})", flush=True)
        if good:
            return True
    print(f"    {arm}: FAILED after 3 tries", flush=True)
    return False


def main():
    arms = sys.argv[1:] or list(ARMS)
    bad = [a for a in arms if a not in ARMS]
    if bad:
        print(f"unknown arms {bad}")
        return 2
    ok = {a: run_arm(a) for a in arms}
    print(json.dumps(ok))
    return 0 if all(ok.values()) else 1


if __name__ == "__main__":
    sys.exit(main())
