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
import argparse, json, math, os, shutil, subprocess, sys, time
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
  // LENS. The standalone's 60 deg vertical FOV is self-declared invented
  // (TrackRenderer.cpp), and it is one of the two named suspects for the
  // residual in the original-vs-standalone pose transplant. RenderWare does not
  // store an angle: the lens is RwCamera::viewWindow, the half-extents at unit
  // distance, so fovy = 2*atan(viewWindow.y).
  //
  // +0x84 of this controller is the RwCamera* -- three independently reversed
  // functions dereference it that way (0x00441700 Camera::SetupFOV,
  // 0x00441760 Camera::Apply, 0x00442a20 Camera::InitWithMatrix). RwCamera
  // offsets are the RW 3.x layout, confirmed against our own reversals:
  // 0x004c1c80 (RwCameraSetViewWindow) writes 0x68/0x6c and the reciprocals at
  // 0x70/0x74, and 0x004c1a70 reads projType 0x14 and the clip planes 0x80/0x84.
  //
  // Also dump the controller inputs to Camera::SetupFOV so the READ can be
  // checked against the STATIC derivation rather than merely believed:
  //   vw.x = ((ctrl[0x6c] * ctrl[0x58]) / ctrl[0x24]) * 0.75
  //   vw.y = 0.75 * ctrl[0x70] * ctrl[0x58]
  // with ctrl[0x58] = 0.6 in race and ctrl[0x24] = 0.75. ctrl[0x6c]/[0x70] are
  // undocumented, which is exactly why they are read here.
  let lens = null;
  try {
    const cp = cam.add(0x84).readPointer();
    if (!cp.isNull()) {
      const g = function (o) { return cp.add(o).readFloat(); };
      lens = { cam_ptr: cp.toString(),
               proj_type: cp.add(0x14).readU32(),
               view_window:  [g(0x68), g(0x6c)],
               recip_window: [g(0x70), g(0x74)],
               view_offset:  [g(0x78), g(0x7c)],
               near_plane: g(0x80), far_plane: g(0x84), fog_plane: g(0x88) };
    }
  } catch (e) { lens = { error: String(e) }; }
  // GROUND TRUTH ORIENTATION. The +0x4c delta is what the DIRECTOR aims at; it
  // is NOT what the renderer uses. Camera::Apply (0x00441760) rebuilds the RW
  // frame from the EULER ANGLES at +0x34 elev / +0x38 azim / +0x3c roll plus the
  // +0x40 position -- it never reads +0x4c. So a pose transplant fed from +0x4c
  // is reconstructing the aim point, not the camera basis, and any mismatch
  // between the two shows up as a wrong vantage with a correct eye position.
  //
  // Read the frame's matrix directly and settle it: camera frame is
  // *(RwCamera + 4), RW 3.x RwFrame has modelling at +0x10 and LTM at +0x50
  // (mashed_qol.cpp:268-271, car_frame_bfs2.py). RwMatrix is right +0x00,
  // up +0x10, at +0x20, pos +0x30.
  let frame = null;
  try {
    const cp = cam.add(0x84).readPointer();
    const fr = cp.add(0x04).readPointer();
    const mat = function (base) {
      const g = function (o) { return base.add(o).readFloat(); };
      return { right: [g(0x00), g(0x04), g(0x08)],
               up:    [g(0x10), g(0x14), g(0x18)],
               at:    [g(0x20), g(0x24), g(0x28)],
               pos:   [g(0x30), g(0x34), g(0x38)] };
    };
    frame = { modelling: mat(fr.add(0x10)), ltm: mat(fr.add(0x50)) };
  } catch (e) { frame = { error: String(e) }; }
  // CAR WORLD POSITIONS, from the render hierarchy rather than any gameplay
  // struct -- this is the space RenderWare actually draws in, which is exactly
  // what the camera-space question needs. Path documented by the 2026-08-01
  // frame BFS (mashed_qol.cpp:283-287, car_frame_bfs2.py):
  //   renderable = *(DAT_0063da18 + i*0x2ac);  frame = *(renderable + 4);
  //   ROOT = *(frame + 0xa0);  world matrix at root+0x10 (modelling) / +0x50 (LTM)
  // RwMatrix pos is +0x30. Projecting one of these with the pose+lens we
  // transplant, and comparing against where that car appears in the capture,
  // decides whether ctrl+0x40 is a world-space eye or something else.
  let cars = [];
  try {
    const tbl = ptr(0x0063da18 + (m.base.toUInt32() - IMG));
    for (let i = 0; i < 4; ++i) {
      try {
        const rend = tbl.add(i * 0x2ac).readPointer();
        if (rend.isNull()) { cars.push(null); continue; }
        const fr0 = rend.add(0x04).readPointer();
        if (fr0.isNull()) { cars.push(null); continue; }
        const root = fr0.add(0xa0).readPointer();
        const use = root.isNull() ? fr0 : root;
        const p = function (o) { return use.add(0x50 + 0x30).add(o).readFloat(); };
        cars.push([p(0), p(4), p(8)]);
      } catch (e) { cars.push(null); }
    }
  } catch (e) { cars = [{ error: String(e) }]; }
  return { eye: [ex, ey, ez],
           dir: [dx, dy, dz],
           at:  [ex + dx, ey + dy, ez + dz],
           lens: lens,
           frame: frame,
           cars: cars,
           euler: { elev: f(0x34), azim: f(0x38), roll: f(0x3c) },
           ctrl: { s58: f(0x58), s24: f(0x24), s6c: f(0x6c), s70: f(0x70) } };
}};
"""


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--out", default=str(ROOT / "verify" / "parity_race" / "orig_race.bmp"))
    ap.add_argument("--settle", type=float, default=3.0,
                    help="seconds in-race before capture (let the scene populate)")
    ap.add_argument("--mode-sel", type=int, default=1, metavar="N",
                    help="menu cursor at the mode screen (depth 3). 1 = Quick "
                         "Battle (default), which always races TRAINING. Use "
                         "another index to reach a mode with a real track "
                         "choice; the loaded track is detected and printed "
                         "either way, so try values and read the result.")
    ap.add_argument("--track-sel", type=int, default=None, metavar="N",
                    help="set the track-select cursor to N before confirming. "
                         "The original's index->track mapping is NOT documented; "
                         "the detected track name is printed either way, so use "
                         "that to find the N you want, then pass it to reproduce. "
                         "Counterpart on the standalone side is MASHED_TRACK_SEL, "
                         "which indexes kAreas[] in Race/GameFlow.cpp.")
    ap.add_argument("--challenge", type=int, default=None, metavar="N",
                    help="Take the Challenge-Cup flow (game_mode 3) instead of "
                         "Quick Battle and launch challenge-select index N. This "
                         "forces the depth-3 mode cursor to 0 (Challenge Cup) and "
                         "reaches entry N at depth 5 by pressing down N times "
                         "(the real challenge index is DAT_0067f17c, which setsel/ "
                         "0x0067ee80 does NOT drive — measured race/arctic-cap "
                         "2026-08-31). Bronze Cup 1 map: 0=TRAINING, 1=EGYPT, "
                         "2=NEUSTEIN, 3=ARCTIC. Requires cup row N unlocked in the "
                         "swapped gamesave (run under run_with_unlocked_save.py).")
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
    # Park the game window on the LEFT monitor, bottom-left corner, so a test run
    # never lands on top of what the user is doing (asked for 2026-08-16).
    # Directional selector on purpose: monitor NUMBERS disagree between Windows
    # Display Settings, EnumDisplayMonitors and Screen.AllScreens on this machine.
    # Caller can override by exporting MASHED_WIN_POS.
    env.setdefault("MASHED_WIN_POS", "left-bl")
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
        # Mode select at depth 3. 1 = Quick Battle, which ALWAYS races TRAINING
        # (measured) -- so it cannot vary the track, and anything needing a
        # second track (e.g. deciding whether the camera's far plane is a
        # constant or track-derived) has to enter through a different mode.
        mode_sel = 0 if args.challenge is not None else args.mode_sel
        E.setsel(mode_sel); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        if args.challenge is not None:
            # Challenge-Cup flow: the depth-5 challenge index is DAT_0067f17c, NOT
            # the per-depth cursor setsel() writes. Reach index N by real down-
            # presses; the on-screen highlight + preview follow it (verified with
            # nav_shots/chall_step{0..3}.bmp on race/arctic-cap). Then fall through
            # to the shared confirm-to-launch loop below.
            time.sleep(0.5)
            for _ in range(args.challenge):
                E.press(12, 180); time.sleep(0.5)
            sel = E.snap([0x0067f17c])[0]
            print(f"  challenge index 0x67f17c={sel} (wanted {args.challenge})")
        # Track select. setsel() writes the cursor at the CURRENT depth, so this
        # must happen after confirm_to(5) and before the confirm that loads.
        elif args.track_sel is not None:
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
                    # Record EVERY .piz seen, not just the resolved name. The
                    # name is seen[-1], which is only trustworthy if the race
                    # actually loaded last; if the game preloads a track (or
                    # opens several), the single name silently mis-reports which
                    # track was raced -- and every same-track comparison built on
                    # it would be wrong without any visible symptom.
                    (out_bmp.parent / "orig_track.txt").write_text(
                        track + "\n#all_piz_opens_in_order:\n"
                        + "".join("#  " + p + "\n" for p in allp))
                    print(f"  ORIGINAL TRACK = {track}   "
                          f"(standalone: MASHED_TRACK_SEL = index of '{track}' in kAreas[])")
                    print(f"  piz opens seen ({len(allp)}): "
                          + ", ".join(p.split('\\')[-1].split('/')[-1] for p in allp))
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
            # DO NOT feed this 6-float form to MASHED_CAM_POSE. It comes from the
            # controller struct DAT_00897fe0 +0x40/+0x4c, which is NOT the world
            # camera, and it cannot express roll (the original's race camera has
            # ~26 deg of it). The faithful value is the 12-float basis printed
            # below as MASHED_CAM_POSE(basis)=, read off the RwCamera FRAME.
            # See mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4029-4045
            # ("this is the part that was wrong for months") and the CARPROJ block
            # below, whose z 4.4-7.5 against the capture's ~18.5-21.8 is the tell.
            # Kept printed because it is what the struct holds and a later reader
            # must be able to re-check the comparison; labelled so nobody pastes it.
            print(f"  campose_ctrl_DISCREDITED_do_not_transplant={pose}")
            # LENS: the measured RwCamera view window, and the fovy it implies.
            # Written as JSON so the raw fields survive alongside the derived
            # angle -- RenderWare stores no angle, so the angle is OUR arithmetic
            # and a later reader must be able to redo it from the halves.
            # Frame-vs-delta cross-check. If the renderer's actual look axis
            # (frame LTM 'at') disagrees with the +0x4c delta we transplant, the
            # transplant is aiming the standalone somewhere the original was not
            # looking, and no lens or sim-moment argument can account for it.
            fr = cp.get("frame") or {}
            eul = cp.get("euler") or {}
            if "ltm" in fr:
                import math as _m
                def _n(v):
                    L = _m.sqrt(sum(c * c for c in v)) or 1.0
                    return [c / L for c in v]
                d_n, a_n = _n(dirv), _n(fr["ltm"]["at"])
                dot = sum(x * y for x, y in zip(d_n, a_n))
                ang = _m.degrees(_m.acos(max(-1.0, min(1.0, dot))))
                pos = fr["ltm"]["pos"]
                dpos = _m.sqrt(sum((a - b) ** 2 for a, b in zip(pos, eye)))
                fr["delta_vs_frame_at_deg"] = ang
                fr["frame_pos_vs_eye_dist"] = dpos
                # THE POSE TO ACTUALLY USE. 12 floats: pos, right, up, at --
                # the camera's full basis, straight off the RwCamera frame.
                # The 6-float eye/at form in orig_campose.txt is kept for the
                # record but is BOTH mis-sourced (controller, not frame) and
                # lossy (no roll); see verify/d1_carproj/RESULT.md.
                L = fr["ltm"]
                basis = ",".join(
                    "%.5f" % v for v in
                    (L["pos"] + L["right"] + L["up"] + L["at"]))
                (out_bmp.parent / "orig_cambasis.txt").write_text(basis + "\n")
                print(f"  MASHED_CAM_POSE={basis}   <-- USE THIS ONE (12-float basis)")
                (out_bmp.parent / "orig_frame.json").write_text(
                    json.dumps({"frame": fr, "euler": eul,
                                "ctrl_eye": eye, "ctrl_dir_0x4c": dirv},
                               indent=2) + "\n")
                print(f"  FRAME ltm.at=({a_n[0]:.3f},{a_n[1]:.3f},{a_n[2]:.3f}) "
                      f"vs 0x4c dir=({d_n[0]:.3f},{d_n[1]:.3f},{d_n[2]:.3f})  "
                      f"ANGLE={ang:.2f}deg")
                print(f"  FRAME ltm.pos=({pos[0]:.3f},{pos[1]:.3f},{pos[2]:.3f}) "
                      f"vs ctrl eye  dist={dpos:.4f}   "
                      f"euler elev={eul.get('elev')} azim={eul.get('azim')} "
                      f"roll={eul.get('roll')}")
            # CAMERA-SPACE TEST. Project each car's world position with the pose
            # we transplant and the lens we measured. If a car lands on the car
            # in the capture, ctrl+0x40/+0x4c ARE the world-space camera and the
            # transplant is correctly sourced; if it lands elsewhere, they are in
            # some other space and every same-view comparison built on them is
            # measuring the wrong place. Left-handed, matching the standalone's
            # MatLookAtLH + MatPerspectiveFovLH.
            cars = cp.get("cars") or []
            lens0 = cp.get("lens") or {}
            if cars and "view_window" in lens0:
                import math as _m
                def _sub(a, b): return [a[i] - b[i] for i in range(3)]
                def _dot(a, b): return sum(a[i] * b[i] for i in range(3))
                def _crs(a, b): return [a[1]*b[2]-a[2]*b[1],
                                        a[2]*b[0]-a[0]*b[2],
                                        a[0]*b[1]-a[1]*b[0]]
                def _nn(v):
                    L = _m.sqrt(_dot(v, v)) or 1.0
                    return [c / L for c in v]
                # Project through the FRAME BASIS when we have one. Deriving the
                # basis from eye/at (below) reconstructs up from world +Y, which
                # throws the camera's roll away -- and it also uses the
                # discredited controller eye. Doing that here is what produced
                # the z 4.4-7.5 spread that disagreed with the capture's
                # ~18.5-21.8 and made these screen coords unusable.
                _ltm = (fr or {}).get("ltm")
                if _ltm:
                    org = _ltm["pos"]
                    rgt = _nn(_ltm["right"])
                    upv = _nn(_ltm["up"])
                    fwd = _nn(_ltm["at"])
                else:
                    org = eye
                    fwd = _nn(_sub(at, eye))
                    rgt = _nn(_crs([0.0, 1.0, 0.0], fwd))
                    upv = _crs(fwd, rgt)
                vwx, vwy = lens0["view_window"]
                W, H = 640, 480
                rows = []
                for i, c in enumerate(cars):
                    if not c or not isinstance(c, list) or len(c) != 3:
                        rows.append(f"    car{i}: (absent)"); continue
                    d = _sub(c, org)
                    z = _dot(d, fwd)
                    if z <= 1e-4:
                        rows.append(f"    car{i}: world=({c[0]:.2f},{c[1]:.2f},"
                                    f"{c[2]:.2f}) BEHIND camera (z={z:.2f})")
                        continue
                    # RenderWare viewWindow IS the half-extent at unit distance,
                    # so ndc = (offset/z)/viewWindow directly -- no extra fov math.
                    nx = (_dot(d, rgt) / z) / vwx
                    ny = (_dot(d, upv) / z) / vwy
                    sx, sy = (nx + 1.0) * 0.5 * W, (1.0 - ny) * 0.5 * H
                    onscreen = (0 <= sx < W and 0 <= sy < H)
                    rows.append(f"    car{i}: world=({c[0]:.2f},{c[1]:.2f},{c[2]:.2f})"
                                f" z={z:6.2f} -> screen=({sx:7.1f},{sy:6.1f})"
                                f" {'ON-SCREEN' if onscreen else 'off-screen'}")
                print("  CARPROJ (FRAME BASIS + lens, 640x480):" if _ltm
                      else "  CARPROJ (eye/at fallback, NO roll, 640x480):")
                for r in rows: print(r)
                (out_bmp.parent / "orig_carproj.txt").write_text(
                    "eye=%r\nat=%r\nviewWindow=%r\n%s\n"
                    % (eye, at, lens0["view_window"], "\n".join(rows)))
            lens, ctrl = cp.get("lens"), cp.get("ctrl") or {}
            if lens and "view_window" in lens:
                vwx, vwy = lens["view_window"]
                lens["fovy_deg"] = 2.0 * math.degrees(math.atan(vwy))
                lens["fovx_deg"] = 2.0 * math.degrees(math.atan(vwx))
                lens["ctrl"] = ctrl
                # Cross-check 1: recipViewWindow must be 1/viewWindow, else the
                # offsets are wrong and every number here is a misread.
                rx, ry = lens["recip_window"]
                lens["recip_ok"] = (abs(rx * vwx - 1.0) < 1e-3 and
                                    abs(ry * vwy - 1.0) < 1e-3)
                # Cross-check 2: against Camera::SetupFOV (0x00441700) computed
                # from the controller fields, with _DAT_005cc950 = 0.75.
                try:
                    px = ((ctrl["s6c"] * ctrl["s58"]) / ctrl["s24"]) * 0.75
                    py = 0.75 * ctrl["s70"] * ctrl["s58"]
                    lens["setupfov_predicted"] = [px, py]
                    lens["setupfov_ok"] = (abs(px - vwx) < 1e-3 and
                                           abs(py - vwy) < 1e-3)
                except Exception:
                    pass
                (out_bmp.parent / "orig_lens.json").write_text(
                    json.dumps(lens, indent=2) + "\n")
                print(f"  LENS viewWindow=({vwx:.4f},{vwy:.4f}) "
                      f"fovy={lens['fovy_deg']:.2f}deg fovx={lens['fovx_deg']:.2f}deg "
                      f"near={lens['near_plane']:.4f} far={lens['far_plane']:.2f} "
                      f"projType={lens['proj_type']}")
                print(f"  LENS ctrl 0x58={ctrl.get('s58')} 0x24={ctrl.get('s24')} "
                      f"0x6c={ctrl.get('s6c')} 0x70={ctrl.get('s70')}  "
                      f"recip_ok={lens['recip_ok']} "
                      f"setupfov_ok={lens.get('setupfov_ok')}")
            else:
                print(f"  LENS unavailable: {lens}")
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
