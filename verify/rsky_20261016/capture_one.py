"""rsky: one new original-side in-race capture with the camera_probe.py that names the car shadow render textures.

    py -3.12 verify\\rsky_20261016\\capture_one.py --track 2 --out <abs dir> [--attempts 3] [--shots 30,70,105]

The same command as verify/capfix_20261016/run_capture.py (camera_probe.py --frame --early --shots-race 30,70,105 --drawrec
--carframes --track N, ABSOLUTE output paths, CWD = this worktree root) but it writes ONLY into --out (capfix's driver wipes its
own output directory on every attempt, so it is not reused). PID hygiene: the probe prints the pid it spawns; only that pid is
killed, and only if its image name is still MASHED.exe. A piz-collision crash (MASHED.exe+0x1509b4 / +0xe66db) is a re-run.
"""
import argparse
import csv
import json
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
PROBE = ROOT / "re" / "frida" / "camera_probe.py"
STEMS = {1: "egypt", 2: "neustein", 4: "highway", 5: "sands", 6: "superg", 7: "rouabout", 8: "storm", 9: "forest",
         10: "dump", 11: "warzone", 3: "arctic"}


def image_name(pid):
    r = subprocess.run(["tasklist", "/FI", f"PID eq {pid}", "/FO", "CSV", "/NH"], capture_output=True, text=True)
    m = re.match(r'"([^"]+)"', r.stdout.strip())
    return m.group(1) if m else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--track", type=int, required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--attempts", type=int, default=3)
    ap.add_argument("--shots", default="30,70,105")
    ap.add_argument("--rows", type=int, default=1600)
    ap.add_argument("--timeout", type=int, default=420)
    a = ap.parse_args()
    d = Path(a.out).resolve()
    d.mkdir(parents=True, exist_ok=True)
    nshots = len([s for s in a.shots.split(",") if s.strip()])
    for attempt in range(1, a.attempts + 1):
        for f in d.iterdir():                       # clean slate of THIS directory only
            if f.is_file() and f.name != "attempts.log" and not f.name.startswith("stdout_failed"):
                f.unlink()
        cmd = [sys.executable, str(PROBE), "--frame", "--early", "--shots-race", a.shots, "--drawrec", "--carframes",
               "--track", str(a.track), "--rows", str(a.rows),
               "--out", str(d / "trace.csv"), "--static-out", str(d / "static.json")]
        t0 = time.time()
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
        why, facts = [], {}
        tr = d / "trace.csv"
        if not tr.exists() or "done ->" not in text:
            why.append("probe did not finish (no 'done ->')")
        else:
            if "SHOT CAPTURE FAILED" in text: why.append("SHOT CAPTURE FAILED")
            if "[agent error]" in text: why.append("agent errors")
            rows = list(csv.DictReader(open(tr)))
            race = [i for i, r in enumerate(rows) if r["ba8c"] == "6"]
            facts.update(rows=len(rows), racing_rows=len(race), race_start=race[0] if race else None)
            shot_rows = [i for i, r in enumerate(rows) if r["shot"] == "1"]
            facts["shot_rows"] = shot_rows
            if len(shot_rows) != nshots: why.append(f"{len(shot_rows)} shots fired, {nshots} requested")
            for k in shot_rows:
                if rows[k]["ba8c"] != "6": why.append(f"shot row {k} is not a racing row")
                for suffix in (".bmp", ".draws.json"):
                    if not (d / f"orig_row_{k:05d}{suffix}").exists(): why.append(f"missing orig_row_{k:05d}{suffix}")
            tk = json.loads((d / "trace.tracks.json").read_text()) if (d / "trace.tracks.json").exists() else []
            facts["piz"] = tk
            if STEMS.get(a.track) and STEMS[a.track] not in " ".join(tk).lower():
                why.append(f"piz opened {tk!r}, expected *{STEMS[a.track]}*")
        ok = not why and rc == 0
        line = (f"{time.strftime('%H:%M:%S')} track {a.track} attempt {attempt} rc={rc} pid={pid} "
                f"{time.time() - t0:5.1f}s {'OK' if ok else 'FAIL ' + '; '.join(why)} {facts}")
        print(line, flush=True)
        with open(d / "attempts.log", "a") as af:
            af.write(line + "\n")
        if ok:
            (d / "summary.json").write_text(json.dumps({"track": a.track, "attempt": attempt, **facts}, indent=1))
            return 0
        (d / f"stdout_failed_attempt{attempt}.txt").write_text(text, encoding="utf-8")
    return 1


if __name__ == "__main__":
    sys.exit(main())
