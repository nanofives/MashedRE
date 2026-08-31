# nav_champ_probe.py — DISCOVERY ONLY (not a capture harness). Boots the stock
# original MASHED.exe, installs nav_agent.js, and walks the frontend menus while
# logging depth()/phase()/game_mode and a snapshot of candidate selection globals
# after every press. Purpose: map the mode-3 (championship / challenge cup) flow
# so race_draw_burst.py can be taught to reach a NON-TRAINING race (T-ARCTIC).
#
# This is throwaway probing scaffolding; the real recipe lives in race_draw_burst.py.
# It deliberately adds NO Interceptor beyond nav_agent.js's single return-override.
#
# Usage: py -3.12 re/frida/nav_champ_probe.py [--plan default|champ|scan]
import argparse, os, subprocess, sys, time, shutil
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent


def find_original(root: Path) -> Path:
    c = root / "original" / "MASHED.exe"
    if c.exists():
        return c
    for p in root.parents:
        c2 = p / "original" / "MASHED.exe"
        if c2.exists():
            return c2
    return c


EXE = find_original(ROOT)
ORIG = EXE.parent
NAV = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")
NPG, DET, ABOVE = 0x00000200, 0x00000008, 0x00008000

# Same track watcher as race_draw_burst.py (kept minimal here).
TRACK_JS = r"""
'use strict';
const seen = [];
rpc.exports = {
  tracks: function () { return seen; },
  trackname: function () {
    if (!seen.length) return '';
    return seen[seen.length - 1].replace(/^.*[\\/]/, '').replace(/\.piz$/i, '');
  }
};
try {
  const k32 = Module.load('kernel32.dll');
  [['CreateFileA', false], ['CreateFileW', true]].forEach(function (e) {
    const p = k32.findExportByName(e[0]);
    if (!p || p.isNull()) return;
    Interceptor.attach(p, { onEnter: function (args) {
      try {
        const s = e[1] ? args[0].readUtf16String() : args[0].readAnsiString();
        if (s && /TRACKS[\\/]/i.test(s) && /\.piz$/i.test(s) && seen.indexOf(s) < 0)
          seen.push(s);
      } catch (_) {}
    }});
  });
} catch (ex) {}
"""

# RVAs to snapshot. game_mode = DAT_0067e9fc (exe_main.cpp:1945 "real frontend
# game mode"). depth = 0067e9f8, phase = 0067eca4. Per-depth cursor base 0067ed80
# stride 0x40 (nav_agent.js). Also sample the car-select slot 0067ea98
# (exe_main.cpp:1995) and a small window around it.
SNAP = {
    "game_mode@e9fc": 0x0067e9fc,
    "cur_d1@ed80":    0x0067ed80,
    "cur_d2@edc0":    0x0067edc0,
    "cur_d3@ee00":    0x0067ee00,
    "cur_d4@ee40":    0x0067ee40,
    "cur_d5@ee80":    0x0067ee80,
    "cur_d6@eec0":    0x0067eec0,
    "carsel@ea98":    0x0067ea98,
    "g@ea94":         0x0067ea94,
    "g@ea9c":         0x0067ea9c,
}


SHOT_DIR = ROOT / "verify" / "nav_shots"
REQ = SHOT_DIR / "nav_bbdump.req"


def boot(env_extra=None):
    if not EXE.exists():
        sys.exit(f"original MASHED.exe not found at {EXE}")
    canon = (ROOT / "scripts" / "canonical" / "videocfg_windowed.bin")
    if not canon.exists():
        canon = ORIG.parent / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists():
        shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "left-bl")
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if env_extra:
        env.update(env_extra)
    dev = frida.get_local_device()
    proc = sess = None
    for _ in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL, creationflags=NPG | DET | ABOVE)
        time.sleep(0.2)
        try:
            sess = dev.attach(proc.pid); break
        except Exception:
            try: proc.kill()
            except Exception: pass
            time.sleep(1.0)
    if sess is None:
        sys.exit("attach failed after 5 retries")
    print(f"  pid={proc.pid}")
    return dev, proc, sess


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--plan", default="champ")
    ap.add_argument("--mode-sel", type=int, default=3)
    ap.add_argument("--track-sel", type=int, default=None)
    ap.add_argument("--d5", type=int, default=2)
    args = ap.parse_args()

    shot_env = None
    if args.plan in ("shots", "challsel"):
        SHOT_DIR.mkdir(parents=True, exist_ok=True)
        try: REQ.unlink()
        except OSError: pass
        shot_env = {"MASHED_ORIG_BBDUMP_REQ": str(REQ)}
    dev, proc, sess = boot(shot_env)
    try:
        scr = sess.create_script(NAV)
        scr.on("message", lambda m, d: None)
        scr.load(); E = scr.exports_sync; E.init()
        trackscr = sess.create_script(TRACK_JS)
        trackscr.on("message", lambda m, d: None)
        trackscr.load()
        T = trackscr.exports_sync

        def snapshot():
            vals = E.snap(list(SNAP.values()))
            return dict(zip(SNAP.keys(), vals))

        def log(tag):
            s = snapshot()
            d, ph = E.depth(), E.phase()
            print(f"  [{tag}] depth={d} phase={ph} tracks={T.tracks()}")
            print("       " + "  ".join(f"{k}={v}" for k, v in s.items()))
            return d, ph

        def wait(pred, t):
            end = time.time() + t
            while time.time() < end:
                if pred(): return True
                time.sleep(0.1)
            return False

        def press(c, ms=180):
            E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)

        def shot(name):
            bmp = SHOT_DIR / f"{name}.bmp"
            try: bmp.unlink()
            except OSError: pass
            REQ.write_text(str(bmp) + "\n")
            ok = wait(lambda: bmp.exists() and not REQ.exists(), 8)
            print(f"  SHOT {name}: depth={E.depth()} phase={E.phase()} "
                  f"-> {bmp.name} {'OK' if ok else 'TIMEOUT'}")

        def confirm_to(target, tries=6):
            for _ in range(tries):
                if E.depth() >= target: return True
                press(4)
                if wait(lambda: E.depth() >= target, 2.0): return True
            return E.depth() >= target

        print("  booting to menu...")
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30):
            sys.exit("never reached title")
        time.sleep(1.0)
        log("title")

        # Walk to the mode-select screen (depth 3), same as race_draw_burst.py.
        confirm_to(2); time.sleep(0.4); log("depth2")
        press(4); time.sleep(0.8); log("after-confirm")
        confirm_to(3); log("depth3-mode-select")

        if args.plan == "dumptable":
            # Dump the 13x12 championship/unlock table DAT_007f0a40 (loaded from
            # the original's current gamesave.bin at boot). Row stride 0x30 (12
            # dwords). Per the RE survey col 4 (+0x10) = track-unlock, value 2 =
            # "track available". This is the authoritative current-save state.
            time.sleep(1.0)
            for r in range(13):
                addrs = [0x007f0a40 + r * 0x30 + j * 4 for j in range(12)]
                vals = E.snap(addrs)
                print(f"  row{r:2d}: {vals}")
            # also the vehicle-unlock table 0x007f0e50 (kAreas count rows), cited
            # in CLAUDE.md alongside 007f0a40.
            print("  --- 0x007f0e50 (vehicle unlock, first 13 rows x12) ---")
            for r in range(13):
                addrs = [0x007f0e50 + r * 0x30 + j * 4 for j in range(12)]
                vals = E.snap(addrs)
                print(f"  vrow{r:2d}: {vals}")
            return 0

        if args.plan == "codescan":
            # Which control code moves the depth-3 mode-select cursor? confirm=4
            # works; 11/12 (documented up/down) do NOT. Scan codes and report any
            # that change cur_d3@ee00.
            confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
            confirm_to(3); time.sleep(0.3)
            for c in range(0, 20):
                if c == 4:  # confirm advances depth; skip
                    continue
                before = snapshot()['cur_d3@ee00']
                if E.depth() != 3:
                    print(f"  left depth3 (depth={E.depth()}) before code {c}; stop")
                    break
                press(c); time.sleep(0.2)
                after = snapshot()['cur_d3@ee00']
                if after != before or E.depth() != 3:
                    print(f"  code {c}: cur_d3 {before}->{after} depth={E.depth()} *** MOVED")
                else:
                    print(f"  code {c}: no change")
            return 0

        if args.plan == "navtest":
            # Verify control codes 11 (up) / 12 (down) actually MOVE a selection,
            # on the depth-3 mode-select where multiple items exist. If cur_d3
            # steps 0->1->2 on press(12), the codes work and any screen that does
            # NOT move under them is genuinely stuck (locked), not a bad code.
            confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
            confirm_to(3); time.sleep(0.3)
            print(f"  depth3 start cur_d3={snapshot()['cur_d3@ee00']}")
            for i in range(4):
                press(12); time.sleep(0.2)
                print(f"  after DOWN#{i+1} cur_d3={snapshot()['cur_d3@ee00']}")
            for i in range(2):
                press(11); time.sleep(0.2)
                print(f"  after UP#{i+1}   cur_d3={snapshot()['cur_d3@ee00']}")
            return 0

        if args.plan == "challsel":
            # Reach Challenge Select (real depth 5) and find the global that
            # actually holds the on-screen selection. Diff a broad block of the
            # frontend state region across up/down presses, and shot each state
            # so the label change is visible ground truth.
            SHOT_DIR.mkdir(parents=True, exist_ok=True)
            block = list(range(0x0067e9f0, 0x0067f210, 4))  # broad frontend state
            def snapblock():
                return dict(zip(block, E.snap(block)))
            def diff(a, b):
                return {hex(k): (a[k], b[k]) for k in a if a[k] != b[k]}
            confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
            confirm_to(3)
            E.setsel(0); time.sleep(0.3); press(4); time.sleep(1.0)   # Challenge Cup
            confirm_to(5, 4)
            if E.depth() < 5:
                print(f"  FAILED depth5 (got {E.depth()})"); return 3
            time.sleep(0.5)
            prev = snapblock(); shot("chall_step0")
            for step in range(1, 6):
                press(12)  # down
                time.sleep(0.3)
                cur = snapblock()
                print(f"  DOWN#{step} depth={E.depth()} phase={E.phase()} "
                      f"changed={diff(prev, cur)}")
                shot(f"chall_step{step}")
                prev = cur
            # then a few UP presses
            for step in range(1, 3):
                press(11)  # up
                time.sleep(0.3)
                cur = snapblock()
                print(f"  UP#{step} depth={E.depth()} phase={E.phase()} "
                      f"changed={diff(prev, cur)}")
                prev = cur
            return 0

        if args.plan == "shots":
            time.sleep(0.8); shot("d1_title")
            confirm_to(2); time.sleep(0.4); shot("d2")
            press(4); time.sleep(0.8)
            confirm_to(3); time.sleep(0.4); shot("d3_modeselect")
            E.setsel(0); time.sleep(0.3); shot("d3_cursor0")
            press(4); time.sleep(1.0); shot("d4_playercolour")
            confirm_to(5, 4); time.sleep(0.4); shot("d5_challengeselect")
            # try depth-5 cursor 2 -> depth 6
            E.setsel(2); time.sleep(0.3); shot("d5_cursor2")
            press(4); time.sleep(1.2); shot("d6")
            print(f"  final depth={E.depth()} phase={E.phase()}")
            return 0

        if args.plan == "deep6":
            # Reach depth 5 (Challenge Select), push into the depth-6 sub-screen
            # via depth-5 cursor 2, set the depth-6 cursor to --track-sel, confirm
            # and report the loaded .piz. --d5 chooses which depth-5 row opens the
            # sub-screen (default 2).
            confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
            confirm_to(3)
            E.setsel(0); time.sleep(0.3); press(4); time.sleep(1.0)   # Challenge Cup
            confirm_to(5, 4)
            if E.depth() < 5:
                print(f"  FAILED depth5 (got {E.depth()})"); return 3
            E.setsel(args.d5); time.sleep(0.3); press(4); time.sleep(1.2)
            d6 = E.depth()
            print(f"  after depth5 cursor {args.d5} confirm: depth={d6} phase={E.phase()}")
            if d6 < 6:
                trk = T.trackname()
                print(f"  did NOT reach depth6; loaded='{trk}' phase={E.phase()}")
                return 0
            ts = args.track_sel if args.track_sel is not None else 0
            # log the depth-6 screen: snap + up/down move test
            base6 = snapshot()
            E.setsel(ts); time.sleep(0.3)
            print(f"  depth6 cursor set to {ts}; cur_d6@eec0 now="
                  f"{snapshot()['cur_d6@eec0']}")
            press(4); time.sleep(1.5)
            for _ in range(6):
                if E.phase() != 3: break
                time.sleep(0.6)
            trk = T.trackname()
            print(f"  RESULT d5={args.d5} d6-track-sel={ts} phase={E.phase()} "
                  f"depth={E.depth()} loaded='{trk}' all_piz={T.tracks()}")
            return 0

        if args.plan == "trackmap":
            # Reach the Challenge Select (depth 5) via Challenge Cup (mode 3),
            # set the depth-5 cursor to --track-sel, confirm, and report which
            # .piz loads. Also snap the championship unlock table row for that
            # cursor (0x007f0a40 + row*0x30) so a blocked launch is visible.
            confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
            confirm_to(3)
            E.setsel(0); time.sleep(0.3)          # Challenge Cup (game_mode 3)
            press(4); time.sleep(1.0)             # -> depth 4 (Player Colour)
            confirm_to(5, 4)                      # -> depth 5 (Challenge Select)
            if E.depth() < 5:
                print(f"  FAILED to reach depth 5 (got {E.depth()})"); return 3
            ts = args.track_sel if args.track_sel is not None else 0
            E.setsel(ts); time.sleep(0.3)
            # unlock table row for this cursor: 12 dwords at 0x007f0a40 + ts*0x30
            row = [0x007f0a40 + ts * 0x30 + j * 4 for j in range(12)]
            rowvals = E.snap(row)
            print(f"  depth5 cursor set to {ts}; unlock_row[{ts}] = {rowvals}")
            print(f"  game_mode={snapshot()['game_mode@e9fc']} depth={E.depth()}")
            press(4); time.sleep(1.5)
            for _ in range(5):
                if E.phase() != 3: break
                time.sleep(0.6)
            trk = T.trackname()
            print(f"  RESULT track-sel={ts} phase={E.phase()} depth={E.depth()} "
                  f"loaded='{trk}' all_piz={T.tracks()}")
            return 0

        if args.plan == "challlaunch":
            # The REAL Challenge-Select index is DAT_0067f17c (measured 2026-08-31,
            # branch race/arctic-cap): with cup rows 0-3 unlocked it steps 0->1->2->3
            # on down (code 12) and back on up, capping at 3, and the on-screen
            # highlight + track preview follow it (nav_shots/chall_step{0..3}.bmp).
            # setsel() (writes 0x0067ee80) does NOT drive it. So reach index N by
            # real down-presses, then confirm to launch, and report the .piz loaded.
            n = args.track_sel if args.track_sel is not None else 0
            SEL = 0x0067f17c
            confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
            confirm_to(3)
            E.setsel(0); time.sleep(0.3)          # Challenge Cup (game_mode 3)
            press(4); time.sleep(1.0)             # -> depth 4 (Player Colour)
            confirm_to(5, 4)                      # -> depth 5 (Challenge Select)
            if E.depth() < 5:
                print(f"  FAILED to reach depth 5 (got {E.depth()})"); return 3
            time.sleep(0.5)
            for _ in range(n):
                press(12); time.sleep(0.2)        # down -> next challenge entry
            sel = E.snap([SEL])[0]
            row = [0x007f0a40 + n * 0x30 + j * 4 for j in range(12)]
            print(f"  challenge index 0x67f17c={sel} (wanted {n}); "
                  f"unlock_row[{n}]={E.snap(row)}")
            press(4); time.sleep(1.5)             # confirm -> launch (or depth 6)
            for _ in range(6):
                if E.phase() != 3: break
                press(4); time.sleep(1.2)
            trk = T.trackname()
            print(f"  RESULT chall-idx={n} sel_0x67f17c={sel} phase={E.phase()} "
                  f"depth={E.depth()} loaded='{trk}' all_piz={T.tracks()}")
            return 0

        if args.plan == "one":
            # Set the depth-3 cursor, confirm ONCE, and report the resulting
            # game_mode + depth. Loop this externally over --mode-sel to learn
            # the cursor-index -> game_mode map (game_mode is only written on
            # confirm, so cursor sweeps without confirm read 0).
            E.setsel(args.mode_sel); time.sleep(0.3)
            press(4); time.sleep(1.2)
            d, ph = log(f"confirm cursor={args.mode_sel}")
            # give a race a moment to start loading a track, if this mode does
            for _ in range(3):
                if E.phase() != 3: break
                time.sleep(0.6)
            print(f"  RESULT mode-sel={args.mode_sel} "
                  f"game_mode={snapshot()['game_mode@e9fc']} "
                  f"depth={E.depth()} phase={E.phase()} tracks={T.tracks()}")
            return 0

        if args.plan == "scan":
            # At the mode-select screen, sweep the cursor 0..7 and log game_mode
            # so we learn which cursor index selects championship (mode 3).
            for i in range(8):
                E.setsel(i); time.sleep(0.2)
                log(f"mode-cursor={i}")
            return 0

        # champ plan: pick the mode-sel item, confirm, and explore each newly
        # opened depth with directional nav before confirming deeper.
        E.setsel(args.mode_sel); time.sleep(0.3)
        log(f"mode-select set cursor={args.mode_sel}")
        press(4); time.sleep(1.0)
        d, ph = log("after mode confirm")

        # Explore up to 5 subsequent screens. At each: try down/up to see if the
        # cursor moves (verifies control codes 11/12), then confirm to advance.
        for step in range(5):
            if E.phase() != 3:
                print(f"  LEFT MENU at step {step} (phase={E.phase()})")
                break
            d = E.depth()
            print(f"  --- screen at depth {d} ---")
            base = snapshot()
            press(12); time.sleep(0.2)      # down
            after_down = snapshot()
            moved = {k: (base[k], after_down[k]) for k in base
                     if base[k] != after_down[k]}
            print(f"       DOWN changed: {moved if moved else 'nothing'}")
            press(11); time.sleep(0.2)      # up (restore)
            after_up = snapshot()
            moved2 = {k: (after_down[k], after_up[k]) for k in after_down
                      if after_down[k] != after_up[k]}
            print(f"       UP   changed: {moved2 if moved2 else 'nothing'}")
            # advance
            press(4); time.sleep(1.2)
            log(f"after confirm from depth {d}")

        # Final: did we reach a race, and on which track?
        for _ in range(4):
            if E.phase() != 3: break
            press(4); time.sleep(1.2)
        print(f"  FINAL phase={E.phase()} depth={E.depth()} tracks={T.tracks()}")
        return 0
    finally:
        try: scr.unload()
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        try: dev.kill(proc.pid)
        except Exception: pass


if __name__ == "__main__":
    sys.exit(main())
