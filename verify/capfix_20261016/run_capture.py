"""capfix: original-side in-race capture for the tracks with no matched-pose capture (U-9702).

For each track runs the U-9702 command

    camera_probe.py --frame --early --shots-race 30,70,105 --drawrec --carframes --track N

with ABSOLUTE output paths (the cause of the "trace ends 30 racing rows in, no shot fires" failure was
a relative request-file path, see RESULT_capfix.md), validates the result and re-runs a failed attempt
(MASHED.exe+0x1509b4 / +0xe66db piz-collision crashes are re-runs, CLAUDE.md U-9677).

PID hygiene: the probe prints the pid it spawns; only that pid is ever killed, and only if its image
name is still MASHED.exe.

Usage (from the repo/worktree root):
    py -3.12 verify\\capfix_20261016\\run_capture.py [--tracks 1,2,4] [--attempts 3] [--shots 30,70,105]
"""
import argparse, csv, json, re, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
OUT = Path(__file__).resolve().parent
PROBE = ROOT / "re" / "frida" / "camera_probe.py"

# engine track index -> (dir name, expected piz file stem as it appears in original\TOASTART\TRACKS)
TRACKS = {1: ("Egypt", "egypt"), 2: ("Neustein", "neustein"), 4: ("Highway", "highway"),
          5: ("Sands", "sands"), 6: ("SuperG", "superg"), 7: ("Roundabout", "rouabout"),
          8: ("Storm", "storm"), 9: ("Forest", "forest"), 10: ("Dump", "dump"),
          11: ("Warzone", "warzone"), 3: ("Arctic", "arctic")}
DEFAULT = [1, 2, 4, 5, 6, 7, 8, 9, 10, 11]


def image_name(pid):
    r = subprocess.run(["tasklist", "/FI", f"PID eq {pid}", "/FO", "CSV", "/NH"],
                       capture_output=True, text=True)
    m = re.match(r'"([^"]+)"', r.stdout.strip())
    return m.group(1) if m else None


def kill_own(pid):
    if pid is None: return
    if (image_name(pid) or "").lower() == "mashed.exe":
        subprocess.run(["taskkill", "/PID", str(pid), "/F"], capture_output=True)


def validate(d, nshots, text):
    """Return (ok, reasons, facts)."""
    why, facts = [], {}
    tr = d / "trace.csv"
    if not tr.exists() or "done ->" not in text:
        return False, ["probe did not finish (no 'done ->')"], facts
    if "SHOT CAPTURE FAILED" in text: why.append("SHOT CAPTURE FAILED")
    if "[agent error]" in text: why.append("agent errors")
    rows = list(csv.DictReader(open(tr)))
    race = [i for i, r in enumerate(rows) if r["ba8c"] == "6"]
    facts["rows"] = len(rows); facts["racing_rows"] = len(race)
    facts["race_start"] = race[0] if race else None
    shot_rows = [i for i, r in enumerate(rows) if r["shot"] == "1"]
    facts["shot_rows"] = shot_rows
    if len(shot_rows) != nshots: why.append(f"{len(shot_rows)} shots fired, {nshots} requested")
    for k in shot_rows:
        if rows[k]["ba8c"] != "6": why.append(f"shot row {k} is not a racing row (ba8c={rows[k]['ba8c']})")
        for suffix in (".bmp", ".draws.json"):
            if not (d / f"orig_row_{k:05d}{suffix}").exists(): why.append(f"missing orig_row_{k:05d}{suffix}")
    tk = json.loads((d / "trace.tracks.json").read_text()) if (d / "trace.tracks.json").exists() else []
    facts["piz"] = tk
    return not why, why, facts


def one(idx, args):
    name, stem = TRACKS[idx]
    d = OUT / f"{idx:02d}_{name}"
    d.mkdir(parents=True, exist_ok=True)
    nshots = len([s for s in args.shots.split(",") if s.strip()])
    for attempt in range(1, args.attempts + 1):
        for f in d.iterdir():              # a clean slate per attempt: no stale shots from an earlier one
            if f.is_file() and f.name != "attempts.log" and not f.name.startswith("stdout_failed"): f.unlink()
        cmd = [sys.executable, str(PROBE), "--frame", "--early", "--shots-race", args.shots, "--drawrec",
               "--carframes", "--track", str(idx), "--rows", str(args.rows),
               "--out", str(d / "trace.csv"), "--static-out", str(d / "static.json")]
        t0 = time.time()
        logf = d / "stdout.txt"
        pid = None
        with open(logf, "w", encoding="utf-8", errors="replace") as lf:
            p = subprocess.Popen(cmd, cwd=str(ROOT), stdout=lf, stderr=subprocess.STDOUT)
            try:
                rc = p.wait(timeout=args.timeout)
            except subprocess.TimeoutExpired:
                p.kill(); rc = "timeout"
        text = logf.read_text(encoding="utf-8", errors="replace")
        m = re.search(r"spawned pid=(\d+)", text)
        pid = int(m.group(1)) if m else None
        kill_own(pid)
        ok, why, facts = validate(d, nshots, text)
        if ok and rc not in (0,): ok, why = False, [f"rc={rc}"]
        piz = " ".join(facts.get("piz") or [])
        if ok and stem not in piz.lower(): ok, why = False, [f"piz opened {piz!r}, expected *{stem}*"]
        line = (f"{time.strftime('%H:%M:%S')} {idx:2d} {name:10s} attempt {attempt} rc={rc} pid={pid} "
                f"{time.time() - t0:5.1f}s {'OK' if ok else 'FAIL ' + '; '.join(why)} {facts}")
        print(line, flush=True)
        with open(d / "attempts.log", "a") as af: af.write(line + "\n")
        if ok:
            (d / "summary.json").write_text(json.dumps({"track": idx, "name": name, "attempt": attempt, **facts}, indent=1))
            return True
        # keep the failed attempt's stdout for the record
        (d / f"stdout_failed_attempt{attempt}.txt").write_text(text, encoding="utf-8")
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tracks", default=",".join(map(str, DEFAULT)))
    ap.add_argument("--attempts", type=int, default=3)
    ap.add_argument("--shots", default="30,70,105")
    ap.add_argument("--rows", type=int, default=1600,
                    help="trace cap; 1200 (the default) can cut a long phase 3 before the last shot")
    ap.add_argument("--timeout", type=float, default=420.0)
    args = ap.parse_args()
    res = {}
    for t in [int(x) for x in args.tracks.split(",") if x.strip()]:
        res[t] = one(t, args)
    print("RESULT", {TRACKS[t][0]: ("OK" if ok else "FAIL") for t, ok in res.items()})
    return 0 if all(res.values()) else 1


if __name__ == "__main__":
    sys.exit(main())
