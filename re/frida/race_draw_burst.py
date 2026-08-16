# race_draw_burst.py — capture the ORIGINAL's per-frame 3D draw-call totals in a
# race (P3 parity, original side). The standalone reports loaded 3D geometry via
# MASHED_DBG_DRAWSTREAM3D; this is the comparable original-side signal, captured
# via the d3d9 shim's draw-slot counters (no Frida hot-path hook on the 3D submit
# path — the counting lives in our proxy device, so it is crash-safe).
#
# Drives MASHED into a Quick-Battle race (nav_agent.js recipe, same as
# nav_to_race.py / phys_c4_telemetry.py), lets it settle, then writes the d3d9
# shim's MASHED_ORIG_BBDUMP_REQ request file. The shim dumps the current
# backbuffer; a "<bmp>.draw3d.json" sibling with that frame's draw-call and
# primitive totals is OPTIONAL. When present, compared against the RE's
# MASHED_DBG_DRAWSTREAM3D totals, the camera-INVARIANT metric (total
# primitives/frame) answers: is the RE missing geometry (dark-void cause), or
# is the divergence lighting/material?
#
# SHIM STATUS: the draw-slot counters + draw3d.json writer SHIP as of 2026-08-14
# (mashedmod/src/d3d9_shim/d3d9_shim.cpp, vtable slots 81-84, gated on
# MASHED_ORIG_BBDUMP_REQ so unarmed runs are unaffected). Rebuild the shim with
# mashedmod\build_d3d9_shim.bat if draw3d.json is missing. The totals stay
# OPTIONAL here on purpose: a stale deployed d3d9.dll should degrade to
# "unavailable" (exit 0) rather than fail the capture. Nonzero exits remain
# reserved for missing BMP/campose.
#
# Usage: py -3.12 re/frida/race_draw_burst.py [--out verify/parity_race/orig_race.bmp]
import argparse, os, shutil, subprocess, sys, time
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

# Read the original's RACE CAMERA pose from the global camera struct DAT_00897fe0
# (re/analysis/race_camera/race_camera.md): +0x40 = eye position, +0x4c = look
# target. RW pre-transforms geometry on the CPU so the pose is NOT in any D3D
# matrix — this struct is the source of truth. Feeding eye/at to the RE's
# MASHED_CAM_POSE renders the SAME track from the SAME pose (same-view parity).
# Track identity, read from the ORIGINAL rather than inferred from the menu.
# Same-track capture is a precondition for any pose comparison (U-9039: a pose
# from one track fed to another is meaningless whatever the frame convention),
# and the original's track-select index -> track mapping is not documented
# anywhere. So detect the truth instead of assuming it: watch CreateFileA/W for
# anything under TRACKS\ and report the .piz actually opened. Detection also
# validates --track-sel when that is used.
TRACK_JS = r"""
'use strict';
const seen = [];
let err = '';

// rpc.exports FIRST. If the hook setup below throws, the script still answers —
// otherwise the caller sees only "unable to find method", which says nothing
// about the cause. (That is exactly what happened on the first run of this.)
rpc.exports = {
  tracks: function () { return seen; },
  err: function () { return err; },
  // Basename without extension, e.g. "Egypt" — matches the `piz` column of
  // kAreas[] in mashedmod/src/mashed_re/Race/GameFlow.cpp, which is what
  // MASHED_TRACK_SEL indexes on the standalone side.
  trackname: function () {
    if (!seen.length) return '';
    return seen[seen.length - 1].replace(/^.*[\\/]/, '').replace(/\.piz$/i, '');
  }
};

try {
  // Module.findExportByName is NOT available as a static in Frida 17 (this repo
  // is on 17.9.3). Use the instance method, same idiom as diff_template.js:4818.
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
} catch (ex) { err = String(ex); }
"""

CAM_JS = r"""
'use strict';
rpc.exports = { campose: function () {
  const IMG = 0x00400000;
  const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
  const cam = ptr(0x00897fe0 + (m.base.toUInt32() - IMG));
  const f = function (o) { return cam.add(o).readFloat(); };
  // +0x40..0x48 = eye. +0x4c..0x54 is a DIRECTION DELTA (target - eye), NOT an
  // absolute look-at point -- established 2026-08-16 from the Xbox twin of
  // 0x00446520 (Xbox 0x00198170, tier=const, re/analysis/race_camera/
  // xtwin_00446520_2026-08-15.txt): lines 577-580 compute
  //     local_e8 = fVar6 - param_1[0x10];   ...   param_1[0x13] = local_e8
  // i.e. the field at +0x4c is written as target-minus-eye, with the eye at
  // param_1[0x10..0x12] = bytes +0x40/+0x44/+0x48.
  //
  // This reader previously returned the raw delta and the caller printed it into
  // MASHED_CAM_POSE as though it were a point, so every pose captured before this
  // date fed the standalone a direction where it expects a position (U-9039).
  // Return BOTH: the raw fields for the record, and the resolved at-point.
  const ex = f(0x40), ey = f(0x44), ez = f(0x48);
  const dx = f(0x4c), dy = f(0x50), dz = f(0x54);
  return { eye: [ex, ey, ez],
           dir: [dx, dy, dz],
           at:  [ex + dx, ey + dy, ez + dz] };
}};
"""


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--out", default=str(ROOT / "verify" / "parity_race" / "orig_race.bmp"))
    ap.add_argument("--settle", type=float, default=3.0,
                    help="seconds in-race before capture (let the scene populate)")
    ap.add_argument("--track-sel", type=int, default=None, metavar="N",
                    help="set the track-select cursor to N before confirming. "
                         "The original's index->track mapping is NOT documented; "
                         "the detected track name is printed either way, so use "
                         "that to find the N you want, then pass it to reproduce. "
                         "Counterpart on the standalone side is MASHED_TRACK_SEL, "
                         "which indexes kAreas[] in Race/GameFlow.cpp.")
    args = ap.parse_args()

    out_bmp = Path(args.out).resolve()
    out_bmp.parent.mkdir(parents=True, exist_ok=True)
    out_json = Path(str(out_bmp) + ".draw3d.json")
    req = out_bmp.parent / "orig_bbdump.req"
    for p in (out_bmp, out_json, req):
        try: p.unlink()
        except OSError: pass

    if not EXE.exists():
        sys.exit(f"original MASHED.exe not found at {EXE}")
    if not (ORIG / "d3d9.dll").exists():
        sys.exit(f"d3d9 shim missing at {ORIG/'d3d9.dll'}")
    canon = (ROOT / "scripts" / "canonical" / "videocfg_windowed.bin")
    if not canon.exists():
        canon = ORIG.parent / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists():
        shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))

    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    env["MASHED_ORIG_BBDUMP_REQ"] = str(req)   # arms the shim's draw counters + dump

    dev = frida.get_local_device()
    proc = sess = None
    for _ in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=NPG | DET | ABOVE)
        time.sleep(0.2)
        try: sess = dev.attach(proc.pid); break
        except Exception:
            try: proc.kill()
            except Exception: pass
            time.sleep(1.0)
    if sess is None:
        sys.exit("attach failed after 5 retries")
    print(f"  pid={proc.pid}")

    scr = sess.create_script(NAV)
    scr.on("message", lambda m, d: None)
    scr.load(); E = scr.exports_sync; E.init()

    # Load the track watcher BEFORE navigating, so it sees the load that the
    # track-select confirm triggers.
    trackscr = None
    try:
        trackscr = sess.create_script(TRACK_JS)
        trackscr.on("message", lambda m, d: None)
        trackscr.load()
    except Exception as e:
        print(f"  track watcher failed to load: {e}")

    def wait(pred, t):
        end = time.time() + t
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        return False
    def press(c, ms=180): E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)
    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target: return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0): return True
        return E.depth() >= target

    rc = 3
    try:
        print("  booting to menu...")
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30):
            sys.exit("never reached title")
        time.sleep(1.0)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(1); time.sleep(0.3)               # Quick Battle
        confirm_to(4, 4); confirm_to(5, 4)
        # Track select. setsel() writes the cursor at the CURRENT depth, so this
        # must happen after confirm_to(5) and before the confirm that loads.
        if args.track_sel is not None:
            E.setsel(args.track_sel); time.sleep(0.3)
            print(f"  track-sel cursor set to {args.track_sel} at depth={E.depth()}")
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3: break
            press(4); time.sleep(1.5)
        if E.phase() == 3:
            sys.exit(f"NOT in race (phase={E.phase()}) — aborting")
        print(f"  in race (phase={E.phase()}); settling {args.settle}s")
        time.sleep(args.settle)
        # Which track did the ORIGINAL actually load? Ground truth for same-track
        # capture; write it beside the pose so a later comparison can verify the
        # two sides matched instead of assuming it.
        track = ""
        try:
            if trackscr:
                track = trackscr.exports_sync.trackname() or ""
                allp = trackscr.exports_sync.tracks() or []
                if track:
                    (out_bmp.parent / "orig_track.txt").write_text(track + "\n")
                    print(f"  ORIGINAL TRACK = {track}   "
                          f"(standalone: MASHED_TRACK_SEL = index of '{track}' in kAreas[])")
                else:
                    e = ""
                    try: e = trackscr.exports_sync.err() or ""
                    except Exception: pass
                    print(f"  track not detected (piz opens seen: {len(allp)})"
                          + (f" hook-error: {e}" if e else ""))
        except Exception as e:
            print(f"  track detect failed: {e}")
        # Read the original's race-camera pose (source of truth; D3D matrices are
        # identity under RW). Write it next to the capture for RE same-view replay.
        cam_ok = False
        try:
            camscr = sess.create_script(CAM_JS)
            camscr.on("message", lambda m, d: None)
            camscr.load()
            cp = camscr.exports_sync.campose()
            eye, dirv, at = cp["eye"], cp["dir"], cp["at"]
            # MASHED_CAM_POSE takes eye + an absolute AT POINT, so emit the
            # RESOLVED at (eye+dir) — never the raw +0x4c delta. See CAM_JS.
            pose = ",".join(f"{v:.4f}" for v in list(eye) + list(at))
            (out_bmp.parent / "orig_campose.txt").write_text(pose + "\n")
            # Keep the raw fields alongside: the delta is what the struct actually
            # holds, and a later reader must be able to re-check this resolution.
            (out_bmp.parent / "orig_campose_raw.txt").write_text(
                "eye=%.4f,%.4f,%.4f\n"
                "dir_at_0x4c_delta=%.4f,%.4f,%.4f\n"
                "at_resolved_eye_plus_dir=%.4f,%.4f,%.4f\n"
                % (eye[0], eye[1], eye[2], dirv[0], dirv[1], dirv[2],
                   at[0], at[1], at[2]))
            print(f"  CAMPOSE eye=({eye[0]:.2f},{eye[1]:.2f},{eye[2]:.2f}) "
                  f"dir=({dirv[0]:.2f},{dirv[1]:.2f},{dirv[2]:.2f}) "
                  f"at=({at[0]:.2f},{at[1]:.2f},{at[2]:.2f})")
            print(f"  MASHED_CAM_POSE={pose}")
            cam_ok = True
        except Exception as e:
            print(f"  campose read failed: {e}")
        # Trigger the shim: write the request file; the next Present dumps the
        # backbuffer. draw3d.json is optional (see SHIM LIMITATION above).
        req.write_text(str(out_bmp) + "\n")
        wait(lambda: out_bmp.exists() and not req.exists(), 10)
        if not out_bmp.exists():
            print("  TIMEOUT waiting for backbuffer dump")
            rc = 3
        elif not cam_ok:
            print("  BMP captured but campose read failed — no same-view replay possible")
            rc = 4
        else:
            rc = 0
            if out_json.exists():
                print("  CAPTURED")
                print(out_json.read_text())
            else:
                print("  draw3d totals unavailable — d3d9 shim has no draw counters yet "
                      "(P3 plan); BMP+campose captured OK")
    finally:
        try: scr.unload()
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        try: dev.kill(proc.pid)
        except Exception: pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
