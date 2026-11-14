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
import shutil
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
    # NOT in the default 13: the new Neustein capture taken with the probe that names the car shadow render textures
    # (capture_one.py --track 2 --out verify/rsky_20261016/cap_neustein), the I-NAME retest of Neustein +105
    20: ("20_NeusteinNamed", "Neustein", HERE / "cap_neustein", None, None),
    # [rres] a new Neustein capture with the BMPs kept (capture_one.py --track 2 --shots 30,70,105), for pixel comparisons of the
    # car shadow; its directory is verify/rres_20261016/cap_neustein_px or $RRES_CAP21
    21: ("21_NeusteinPix", "Neustein", Path(os.environ.get("RRES_CAP21", str(WT / "verify" / "rres_20261016" / "cap_neustein_px"))), None, None),
}
DEFAULT_TRACKS = ",".join(str(t) for t in TRACKS if t < 20)


def shots_of(t):
    d, view, cap, shots, frames = TRACKS[t]
    if shots is None:
        shots = json.loads((cap / "summary.json").read_text())["shot_rows"]
    return shots


def stand_ok(sd: Path, shots):
    return all((sd / "d1r2" / f"stand_row_{k:05d}.draws.json").exists() for k in shots)


def start_key_poster(pid, d, out):
    """[HB-RUN13-LIVE-INPUT] RUN13_POST_KEYS=1: post WM_KEYDOWN/WM_KEYUP (Enter, then Down arrow, alternating every 2 s) to the
    top-level windows owned by THIS pid only (EnumWindows + GetWindowThreadProcessId). No focus change, no SendInput, no image
    name. Posted messages reach the window procedure; they do not change DirectInput device state (exe_main.cpp:1286)."""
    import ctypes
    import threading
    import time
    from ctypes import wintypes
    u = ctypes.windll.user32
    WM_KEYDOWN, WM_KEYUP = 0x0100, 0x0101
    VK_RETURN, VK_DOWN = 0x0D, 0x28
    st = {"stop": threading.Event(), "posted": 0, "hwnds": set()}
    cb_t = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

    def windows():
        found = []

        def cb(h, _):
            p = wintypes.DWORD(0)
            u.GetWindowThreadProcessId(h, ctypes.byref(p))
            if p.value == pid:
                found.append(h)
            return True
        u.EnumWindows(cb_t(cb), 0)
        return found

    def run():
        n = 0
        while not st["stop"].wait(2.0):
            vk = VK_RETURN if n % 2 == 0 else VK_DOWN
            sc = u.MapVirtualKeyW(vk, 0)
            for h in windows():
                st["hwnds"].add(h)
                u.PostMessageW(h, WM_KEYDOWN, vk, 1 | (sc << 16))
                time.sleep(0.05)
                u.PostMessageW(h, WM_KEYUP, vk, 1 | (sc << 16) | (1 << 30) | (1 << 31))
                st["posted"] += 1
            n += 1
    st["thread"] = threading.Thread(target=run, daemon=True)
    st["thread"].start()
    return st


def run_one(exe, root, out, t, knobs, att):
    d, view, cap, _, frames = TRACKS[t]
    shots = shots_of(t)
    # [D3-SUBSTATE-FLIP run 22] + RUN13_PRE_FRAMES (default 800): the replay trace is consumed only from GO, and with
    # MASHED_SUBSTATE default ON phase 3 + state 5 hold ~600-880 frames before it (Egypt: 598 + 112). The stock
    # budget left no room for that, so every Egypt attempt ended before its last shot.
    # [D3-SUBSTATE-FLIP run 23] default back to 0: with MASHED_CAM_REPLAY_P3=1 (below) rows are consumed from the phase-3
    # entry, and HB-RUN13-PHASE3-START measured the flip candidate 2431afb1 on the stock budget. RUN13_PRE_FRAMES=800
    # restores the run 22 padding.
    frames = (frames or (max(shots) + 350)) + int(os.environ.get("RUN13_PRE_FRAMES", "0")) + 150 * (att - 1)
    sd = out / d / "stand"
    sd.mkdir(parents=True, exist_ok=True)
    for f in ("cam_dump.csv", "librw_dump.csv"):
        if (sd / f).exists():
            (sd / f).unlink()
    env = dict(os.environ)
    env.update({
        "MASHED_ROOT": str(root), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": view,
        "MASHED_CAR": "1", "MASHED_ROUND": "1", "MASHED_ROUND_RULE": "0", "MASHED_GAME_MODE": "10",   # [fflip 2026-10-21] recipe F
        "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1",
        "MASHED_DET_FRAMES": str(frames), "MASHED_VERIFY_OUT": str(sd),
        "MASHED_CAM_DUMP": str(sd / "cam_dump.csv"),
        "MASHED_CAM_DUMP_LIBRW": str(sd / "librw_dump.csv"),
        "MASHED_CAM_REPLAY": str((cap / "trace.csv").resolve()),
        "MASHED_CAM_REPLAY_SHOTS": ",".join(str(k) for k in shots),
        "MASHED_PLAYERTRACE_OUT": str(sd / "ptrace.log"),
        "MASHED_CAM_REPLAY_BASIS": "1", "MASHED_CAR_REPLAY": "1", "MASHED_DBG_D3DREC": "1",
        # [HB-RUN13-PHASE3-START] consume trace rows from the port's phase-3 entry (every trace starts there). Inert with
        # MASHED_SUBSTATE off; the knob argument MASHED_CAM_REPLAY_P3=0 turns it off.
        "MASHED_CAM_REPLAY_P3": "1",
    })
    # [HB-RUN13-LIVE-INPUT] The exe reads the live keyboard (DISCL_BACKGROUND, exe_main.cpp:1207) and cooks Enter / joypad
    # button 0 into the ctrl byte 0x007f1042 + i*0x4c every frame (CookCtrlByteAFromKeyboard, exe_main.cpp:3086-3121), which
    # can end phase 3 early (CameraEntries.cpp:405-407). MASHED_NO_R7_INPUT=1 makes that function return at once
    # (exe_main.cpp:3087, R7InputOn() at exe_main.cpp:1114-1117) and keeps no joypad device (exe_main.cpp:1122), so a
    # key typed anywhere on the machine cannot change the table. RUN13_LIVE_INPUT=1 restores the old live-input run.
    # A knob argument (MASHED_NO_R7_INPUT=0) is applied after this and wins.
    if os.environ.get("RUN13_LIVE_INPUT", "") in ("", "0"):
        env["MASHED_NO_R7_INPUT"] = "1"
    for k in knobs:
        a, _, b = k.partition("=")
        env[a] = b if b else "1"
    p = subprocess.Popen([str(exe)], cwd=str(root), env=env)
    print(f"[{d}] attempt {att} pid={p.pid} frames={frames} shots={shots}", flush=True)
    poster = start_key_poster(p.pid, d, out) if os.environ.get("RUN13_POST_KEYS", "") not in ("", "0") else None
    try:
        rc = p.wait(900)
    except subprocess.TimeoutExpired:
        print(f"[{d}] TIMEOUT - stopping pid {p.pid} ONLY", flush=True)
        p.kill()
        rc = p.wait(15)
    print(f"[{d}] exit={rc}", flush=True)
    if poster:
        poster["stop"].set()
        poster["thread"].join(5)
        print(f"[{d}] key poster: {poster['posted']} posts, windows seen {sorted(poster['hwnds'])}", flush=True)
        (sd / "key_poster.txt").write_text(f"posts={poster['posted']}\nhwnds={sorted(poster['hwnds'])}\npid={p.pid}\n")
    # [HB-RUN13-LIVE-INPUT] the private root's mashed_re.log is rewritten by the next track and by the next run; keep this
    # track's copy beside its dumps so an `R7IN byte0xa ... test=0` line (a live input event) can be read after the fact.
    try:
        shutil.copyfile(root / "mashed_re.log", sd / "mashed_re.log")
    except OSError as e:
        print(f"[{d}] mashed_re.log not kept: {e}", flush=True)
    return sd, shots


def cmd_run(a):
    root, out, exe = Path(a.root).resolve(), Path(a.out).resolve(), Path(a.exe).resolve()
    marker = root / "original" / "TOASTART" / "Common" / "Frontend.piz"
    if not marker.exists():
        print(f"REFUSED: {marker} missing. The private root needs original\\ INSIDE it "
              "(else the exe walks up to the shared main root).")
        return 2
    if a.hud:
        a.knobs = list(a.knobs) + ["MASHED_HUD_REPLAY=" + str(WT / "verify" / "rfx_20261016" / "hud_inputs.txt")]
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
        tname = d.split("_", 1)[1].replace("Named", "").replace("Pix", "")   # the track name the cites' "tracks" lists use
        mine = [c for c in all_cites if "tracks" not in c or tname in c["tracks"]]
        cf = out / d / f"cites_used_{a.prefix.strip('_')}.json"
        cf.write_text(json.dumps(mine, indent=1))
        trace = cap / "trace.csv"
        for g in cg.orig_gates(cap, sd / "d1r2"):   # [dflip] peer for the I-NAME render-texture rule
            if g["row"] in shots:
                gate_lines.append(f"orig  {d:<14} {json.dumps(g)}")
                ok_all &= g["I-COVER"] and g["I-FRAME"] and g["I-NAME"]
        sg, _ = cg.stand_gates(sd, trace, cap) if (sd / "d1r2").exists() else ([], 0)
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
    r.add_argument("--tracks", default=DEFAULT_TRACKS); r.add_argument("--attempts", type=int, default=3)
    r.add_argument("--hud", action="store_true",
                   help="[rres] also replay the HUD state (MASHED_HUD_REPLAY=verify/rfx_20261016/hud_inputs.txt): C08's 234 rows are the "
                        "harness not replaying it (RESULT_rres.md)")
    r.add_argument("knobs", nargs="*")
    s = sub.add_parser("score")
    s.add_argument("--out", required=True); s.add_argument("--cite", required=True)
    s.add_argument("--tracks", default=DEFAULT_TRACKS); s.add_argument("--prefix", default="diff_")
    a = ap.parse_args()
    return cmd_run(a) if a.cmd == "run" else cmd_score(a)


if __name__ == "__main__":
    sys.exit(main())
