"""librwrest R1: the standalone's per-car shade (rec+0x284, FUN_0041f590) on the start grid, against
the original's live value (rtex's camera_probe.py --matfx snapshots, log/rtex/orig_matfx2_<track>).

    py -3.12 run_shade.py <exe> <out_dir> <Track> [frames]

Runs the dev race (MASHED_TRACK_VIEW, MASHED_CAR, MASHED_ROUND, deterministic) with
MASHED_DBG_CARSHADE, then prints, per car, the standalone's last grid-phase shade and root position
next to the original's (the snapshot rows whose DAT_0063ba8c is 3, constant over rows 100..800, and
the trace's c{i}x/y/z on the same rows). MEASUREMENT ONLY: spawns and stops only its own PID.
"""
import csv
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")


def main():
    exe, out, track = Path(sys.argv[1]).resolve(), Path(sys.argv[2]).resolve(), sys.argv[3]
    frames = int(sys.argv[4]) if len(sys.argv) > 4 else 300
    out.mkdir(parents=True, exist_ok=True)
    shade = out / "carshade.csv"
    if shade.exists():
        shade.unlink()
    env = dict(os.environ)
    env.update({"MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": track,
                "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
                "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
                "MASHED_DET_FRAMES": str(frames), "MASHED_VERIFY_OUT": str(out),
                "MASHED_PLAYERTRACE_OUT": str(out / "ptrace.log"),
                "MASHED_DBG_CARSHADE": str(shade)})
    p = subprocess.Popen([str(exe)], cwd=str(ROOT), env=env)
    print(f"pid={p.pid}", flush=True)
    try:
        rc = p.wait(600)
    except subprocess.TimeoutExpired:
        p.kill(); rc = p.wait(15)
    print(f"exit={rc}")
    rows = list(csv.reader(open(shade))) if shade.exists() else []
    last = {}
    for r in rows:
        last[int(r[1])] = r
    od = ROOT / "log" / "rtex" / f"orig_matfx2_{track.lower()}"
    snaps = json.loads((od / "trace.csv.matfx.json").read_text())
    grid = [s for s in snaps if s.get("ba8c") == 3 and s.get("row", 0) >= 100]
    tr = list(csv.DictReader(open(od / "trace.csv")))
    lines = [f"track {track}: standalone frames logged {len(rows)}; original grid snapshots {len(grid)}"]
    for i in range(4):
        o = [s["s"]["cars"][i]["f284"] for s in grid]
        orow = grid[-1]["row"] if grid else None
        opos = ([float(tr[orow][f"c{i}{a}"]) for a in "xyz"] if orow is not None and orow < len(tr)
                else None)
        sr = last.get(i)
        spos = [float(x) for x in sr[2:5]] if sr else None
        sv = float(sr[5]) if sr else None
        lines.append(f"car {i}: orig shade {min(o) if o else None:.4f}..{max(o) if o else None:.4f} "
                     f"pos {opos} | stand shade {sv} pos {spos}"
                     + (f" | d={sv - o[-1]:+.4f}" if (o and sv is not None) else ""))
    s = "\n".join(lines)
    (out / "SHADE.txt").write_text(s + "\n")
    print(s)


if __name__ == "__main__":
    main()
