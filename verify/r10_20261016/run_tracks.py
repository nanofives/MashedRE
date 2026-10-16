"""r10: standalone matched-pose replay of the 10 capfix tracks (U-9702).

    py -3.12 verify\\r10_20261016\\run_tracks.py [--exe <path>] [--tracks 1,2,4] [--attempts 3] [KNOB ...]

For each track runs d1r1's run_replay3d.py on the capfix trace with the 3 capfix shot rows, writes
verify/r10_20261016/<NN_Track>/stand/ (d1r2/stand_row_<K>.draws.json + .bmp, cam_dump.csv, librw_dump.csv,
ptrace.log) and re-runs a track whose shots are incomplete (piz-collision loads are re-runs, CLAUDE.md U-9677).
MEASUREMENT ONLY: run_replay3d spawns and stops ONLY its own PID. Absolute paths throughout.
"""
import argparse
import json
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
CAPFIX = ROOT / "verify" / "capfix_20261016"
REPLAY = ROOT / "verify" / "d1r1_20261016" / "run_replay3d.py"

# capfix dir -> MASHED_TRACK_VIEW basename (TRACKS/<name>.piz)
TRACKS = {1: ("01_Egypt", "Egypt"), 2: ("02_Neustein", "Neustein"), 4: ("04_Highway", "Highway"),
          5: ("05_Sands", "sands"), 6: ("06_SuperG", "SuperG"), 7: ("07_Roundabout", "rouabout"),
          8: ("08_Storm", "Storm"), 9: ("09_Forest", "Forest"), 10: ("10_Dump", "dump"),
          11: ("11_Warzone", "Warzone")}


def stand_ok(sd: Path, shots):
    return all((sd / "d1r2" / f"stand_row_{k:05d}.draws.json").exists() for k in shots)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", default=str(ROOT / "mashedmod" / "build" / "mashed_re.exe"))
    ap.add_argument("--tracks", default=",".join(str(t) for t in TRACKS))
    ap.add_argument("--attempts", type=int, default=3)
    ap.add_argument("--extra-frames", type=int, default=350)
    ap.add_argument("--out", default=str(HERE))
    ap.add_argument("--cap", default=str(CAPFIX), help="capture root with <NN_Track>/trace.csv + summary.json")
    ap.add_argument("knobs", nargs="*")
    a = ap.parse_args()
    exe = str(Path(a.exe).resolve())
    out_root = Path(a.out).resolve()
    status = {}
    for t in [int(x) for x in a.tracks.split(",")]:
        d, view = TRACKS[t]
        cap = Path(a.cap).resolve() / d
        shots = json.loads((cap / "summary.json").read_text())["shot_rows"]
        sd = out_root / d / "stand"
        frames = max(shots) + a.extra_frames
        ok = False
        for att in range(1, a.attempts + 1):
            cmd = [sys.executable, str(REPLAY), exe, str(sd), "--trace", str(cap / "trace.csv"),
                   "--shots", ",".join(str(k) for k in shots), "--track", view,
                   "--frames", str(frames + 150 * (att - 1))] + a.knobs
            print(f"[{d}] attempt {att}: {' '.join(cmd[2:])}", flush=True)
            r = subprocess.run(cmd, capture_output=True, text=True)
            (out_root / d).mkdir(parents=True, exist_ok=True)
            (out_root / d / f"stand_stdout_a{att}.txt").write_text(r.stdout + "\n--stderr--\n" + r.stderr)
            print(r.stdout.strip().splitlines()[-6:], flush=True)
            if stand_ok(sd, shots):
                ok = True
                break
        status[d] = {"ok": ok, "attempts": att, "shots": shots, "frames": frames}
        print(f"[{d}] {'OK' if ok else 'FAILED'} after {att} attempt(s)", flush=True)
    (out_root / "run_tracks_status.json").write_text(json.dumps(status, indent=1))
    return 0 if all(v["ok"] for v in status.values()) else 1


if __name__ == "__main__":
    sys.exit(main())
