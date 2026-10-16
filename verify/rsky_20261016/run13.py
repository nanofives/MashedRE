"""rsky: private-root matched-pose replay and scoring of the 13 adjudicated tracks.

    py -3.12 verify\\rsky_20261016\\run13.py run   --exe <exe> --root <gameroot> --out <dir> [--tracks 1,2,...] [KNOB ...]
    py -3.12 verify\\rsky_20261016\\run13.py score --out <dir> --cite <cites.json> [--tracks ...] [--prefix diff_]

Same recipe as verify/d1r1_20261016/run_replay3d.py and verify/r10_20261016/{run_tracks,score_tracks}.py (one deterministic round,
MASHED_CAM_REPLAY_BASIS=1, MASHED_CAR_REPLAY=1, MASHED_DBG_D3DREC=1, no opt-in knobs), with ONE change: MASHED_ROOT and the CWD are a
PRIVATE game root (a robocopy of original\\ with original\\ inside), so mashed_re.log, log/librw_race.txt and every CWD-relative file land
there and not in the main checkout that other sessions share. MEASUREMENT ONLY: spawns and stops ONLY its own PID.
"""
import argparse
import importlib.util
import json
import os
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent                                  # this worktree
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")  # holds the gitignored d1r1 captures (log/d1r1)
DIFF = WT / "re" / "tools" / "drawlist_diff.py"

spec = importlib.util.spec_from_file_location("cg", WT / "verify" / "d1r1_20261016" / "check_gates.py")
cg = importlib.util.module_from_spec(spec)
spec.loader.exec_module(cg)

CAPFIX = WT / "verify" / "capfix_20261016"
D1 = MAIN / "log" / "d1r1"
# id -> (dir name, MASHED_TRACK_VIEW basename, original-capture dir, shots (None = capfix summary.json), frames (None = max shot + 350))
TRACKS = {
    1:  ("01_Egypt", "Egypt", CAPFIX / "01_Egypt", None, None),
    2:  ("02_Neustein", "Neustein", CAPFIX / "02_Neustein", None, None),
    3:  ("03_Arctic", "Arctic", D1 / "orig_arctic", [1051, 1091, 1126], 1450),
    4:  ("04_Highway", "Highway", CAPFIX / "04_Highway", None, None),
    5:  ("05_Sands", "sands", CAPFIX / "05_Sands", None, None),
    6:  ("06_SuperG", "SuperG", CAPFIX / "06_SuperG", None, None),
    7:  ("07_Roundabout", "rouabout", CAPFIX / "07_Roundabout", None, None),
    8:  ("08_Storm", "Storm", CAPFIX / "08_Storm", None, None),
    9:  ("09_Forest", "Forest", CAPFIX / "09_Forest", None, None),
    10: ("10_Dump", "dump", CAPFIX / "10_Dump", None, None),
    11: ("11_Warzone", "Warzone", CAPFIX / "11_Warzone", None, None),
    12: ("12_City", "City", D1 / "orig_city", [761, 801, 836], None),
    13: ("13_Training", "training", D1 / "orig_training", [884, 924, 959], None),
}


def shots_of(t):
    d, view, cap, shots, frames = TRACKS[t]
    if shots is None:
        shots = json.loads((cap / "summary.json").read_text())["shot_rows"]
    return shots


def stand_ok(sd: Path, shots):
    return all((sd / "d1r2" / f"stand_row_{k:05d}.draws.json").exists() for k in shots)


def run_one(exe, root, out, t, knobs, att):
    d, view, cap, _, frames = TRACKS[t]
    shots = shots_of(t)
    frames = (frames or (max(shots) + 350)) + 150 * (att - 1)
    sd = out / d / "stand"
    sd.mkdir(parents=True, exist_ok=True)
    for f in ("cam_dump.csv", "librw_dump.csv"):
        if (sd / f).exists():
            (sd / f).unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(root), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": view,
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "4",
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(frames), "MASHED_VERIFY_OUT": str(sd),
        "MASHED_CAM_DUMP": str(sd / "cam_dump.csv"),
        "MASHED_CAM_DUMP_LIBRW": str(sd / "librw_dump.csv"),
        "MASHED_CAM_REPLAY": str((cap / "trace.csv").resolve()),
        "MASHED_CAM_REPLAY_SHOTS": ",".join(str(k) for k in shots),
        "MASHED_PLAYERTRACE_OUT": str(sd / "ptrace.log"),
        "MASHED_CAM_REPLAY_BASIS": "1", "MASHED_CAR_REPLAY": "1", "MASHED_DBG_D3DREC": "1",
    })
    for k in knobs:
        a, _, b = k.partition("=")
        env[a] = b if b else "1"
    p = subprocess.Popen([str(exe)], cwd=str(root), env=env)
    print(f"[{d}] attempt {att} pid={p.pid} frames={frames} shots={shots}", flush=True)
    try:
        rc = p.wait(900)
    except subprocess.TimeoutExpired:
        print(f"[{d}] TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill()
        rc = p.wait(15)
    print(f"[{d}] exit={rc}", flush=True)
    return sd, shots


def cmd_run(a):
    root, out, exe = Path(a.root).resolve(), Path(a.out).resolve(), Path(a.exe).resolve()
    marker = root / "original" / "TOASTART" / "Common" / "Frontend.piz"
    if not marker.exists():
        print(f"REFUSED: {marker} missing. The private root needs original\\ INSIDE it "
              "(else the exe walks up to the shared main root).")
        return 2
    status = {}
    for t in [int(x) for x in a.tracks.split(",")]:
        ok = False
        for att in range(1, a.attempts + 1):
            sd, shots = run_one(exe, root, out, t, a.knobs, att)
            if stand_ok(sd, shots):
                ok = True
                break
        status[TRACKS[t][0]] = {"ok": ok, "attempts": att, "shots": shots}
        print(f"[{TRACKS[t][0]}] {'OK' if ok else 'FAILED'} after {att} attempt(s)", flush=True)
    log = root / "mashed_re.log"
    print(f"private root log: {log} exists={log.exists()}", flush=True)
    (out / "run13_status.json").write_text(json.dumps(status, indent=1))
    return 0 if all(v["ok"] for v in status.values()) else 1


def cmd_score(a):
    out = Path(a.out).resolve()
    all_cites = json.loads(Path(a.cite).read_text())
    lines, gate_lines = [], []
    lines.append(f"{'track':<14}{'row':>6} {'orig':>5} {'matched':>8} {'orig tri':>9} {'m.tri':>7} {'m.tri%':>7} | "
                 f"{'rows':>4} {'cited':>5} {'red':>4} verdict")
    ok_all = True
    for t in [int(x) for x in a.tracks.split(",")]:
        d, view, cap, _, _ = TRACKS[t]
        sd = out / d / "stand"
        shots = shots_of(t)
        tname = d.split("_", 1)[1]
        mine = [c for c in all_cites if "tracks" not in c or tname in c["tracks"]]
        cf = out / d / f"cites_used_{a.prefix.strip('_')}.json"
        cf.write_text(json.dumps(mine, indent=1))
        trace = cap / "trace.csv"
        for g in cg.orig_gates(cap):
            if g["row"] in shots:
                gate_lines.append(f"orig  {d:<14} {json.dumps(g)}")
                ok_all &= g["I-COVER"] and g["I-FRAME"] and g["I-NAME"]
        sg, _ = cg.stand_gates(sd, trace) if (sd / "d1r2").exists() else ([], 0)
        for g in sg:
            gate_lines.append(f"stand {d:<14} {json.dumps(g)}")
            ok_all &= g["I-SYNC"] and g["I-NAME"]
        if len(sg) != len(shots):
            ok_all = False
            gate_lines.append(f"stand {d:<14} MISSING {len(shots) - len(sg)} of {len(shots)} replay records")
        for k in shots:
            fa = cap / f"orig_row_{k:05d}.draws.json"
            fb = sd / "d1r2" / f"stand_row_{k:05d}.draws.json"
            if not fb.exists():
                lines.append(f"{d:<14}{k:>6} UNSCORED (no replay record)")
                continue
            jo = out / d / f"{a.prefix}{k:05d}.json"
            r = subprocess.run([sys.executable, str(DIFF), str(fa), str(fb), "--trace", str(trace),
                                "--cite", str(cf), "--json", str(jo), "--max-rows", "800"],
                               capture_output=True, text=True)
            (out / d / f"{a.prefix}{k:05d}.txt").write_text(r.stdout + r.stderr)
            j = json.loads(jo.read_text())
            rows = j["rows"]
            red = [x for x in rows if "cite" not in x]
            ver = "GREEN" if not rows else ("CITED" if not red else "RED")
            m = re.search(r"matched (\d+) draws \((\d+) tri\)", r.stdout)
            mtri = int(m.group(2)) if m else 0
            lines.append(f"{d:<14}{k:>6} {j['na']:>5} {j['matched']:>8} {j['tri_a']:>9} {mtri:>7} "
                         f"{100 * mtri / max(1, j['tri_a']):>6.1f}% | {len(rows):>4} {len(rows) - len(red):>5} "
                         f"{len(red):>4} {ver}")
    txt = "\n".join(lines) + "\n\nINSTRUMENT GATES: " + ("PASS" if ok_all else "FAIL") + "\n"
    (out / f"scores_{a.prefix.strip('_')}.txt").write_text(txt)
    (out / f"gates_{a.prefix.strip('_')}.txt").write_text("\n".join(gate_lines) + "\n")
    print(txt)
    return 0


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("run")
    r.add_argument("--exe", required=True); r.add_argument("--root", required=True); r.add_argument("--out", required=True)
    r.add_argument("--tracks", default=",".join(str(t) for t in TRACKS)); r.add_argument("--attempts", type=int, default=3)
    r.add_argument("knobs", nargs="*")
    s = sub.add_parser("score")
    s.add_argument("--out", required=True); s.add_argument("--cite", required=True)
    s.add_argument("--tracks", default=",".join(str(t) for t in TRACKS)); s.add_argument("--prefix", default="diff_")
    a = ap.parse_args()
    return cmd_run(a) if a.cmd == "run" else cmd_score(a)


if __name__ == "__main__":
    sys.exit(main())
