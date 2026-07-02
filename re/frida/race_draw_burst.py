# race_draw_burst.py — capture the ORIGINAL's per-frame 3D draw-call totals in a
# race (P3 parity, original side). The standalone reports loaded 3D geometry via
# MASHED_DBG_DRAWSTREAM3D; this is the comparable original-side signal, captured
# via the d3d9 shim's draw-slot counters (no Frida hot-path hook on the 3D submit
# path — the counting lives in our proxy device, so it is crash-safe).
#
# Drives MASHED into a Quick-Battle race (nav_agent.js recipe, same as
# nav_to_race.py / phys_c4_telemetry.py), lets it settle, then writes the d3d9
# shim's MASHED_ORIG_BBDUMP_REQ request file. The shim dumps the current
# backbuffer AND a "<bmp>.draw3d.json" sibling with that frame's draw-call and
# primitive totals. Compared against the RE's MASHED_DBG_DRAWSTREAM3D totals, the
# camera-INVARIANT metric (total primitives/frame) answers: is the RE missing
# geometry (dark-void cause), or is the divergence lighting/material?
#
# Requires the instrumented d3d9 shim deployed to original/ (the draw-slot
# counters are only present in the parity build of d3d9_shim.cpp).
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
CAM_JS = r"""
'use strict';
rpc.exports = { campose: function () {
  const IMG = 0x00400000;
  const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
  const cam = ptr(0x00897fe0 + (m.base.toUInt32() - IMG));
  const f = function (o) { return cam.add(o).readFloat(); };
  return [f(0x40), f(0x44), f(0x48), f(0x4c), f(0x50), f(0x54)];
}};
"""


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--out", default=str(ROOT / "verify" / "parity_race" / "orig_race.bmp"))
    ap.add_argument("--settle", type=float, default=3.0,
                    help="seconds in-race before capture (let the scene populate)")
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
    canon = ROOT.parents[-1]  # placeholder; resolved below
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
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3: break
            press(4); time.sleep(1.5)
        if E.phase() == 3:
            sys.exit(f"NOT in race (phase={E.phase()}) — aborting")
        print(f"  in race (phase={E.phase()}); settling {args.settle}s")
        time.sleep(args.settle)
        # Read the original's race-camera pose (source of truth; D3D matrices are
        # identity under RW). Write it next to the capture for RE same-view replay.
        try:
            camscr = sess.create_script(CAM_JS)
            camscr.on("message", lambda m, d: None)
            camscr.load()
            cp = camscr.exports_sync.campose()
            pose = ",".join(f"{v:.4f}" for v in cp)
            (out_bmp.parent / "orig_campose.txt").write_text(pose + "\n")
            print(f"  CAMPOSE eye=({cp[0]:.2f},{cp[1]:.2f},{cp[2]:.2f}) "
                  f"at=({cp[3]:.2f},{cp[4]:.2f},{cp[5]:.2f})")
            print(f"  MASHED_CAM_POSE={pose}")
        except Exception as e:
            print(f"  campose read failed: {e}")
        # Trigger the shim: write the request file; the next Present dumps the
        # backbuffer + the draw3d.json sibling for that frame.
        req.write_text(str(out_bmp) + "\n")
        if wait(lambda: out_json.exists(), 10):
            rc = 0
            print("  CAPTURED")
            print(out_json.read_text())
        else:
            print("  TIMEOUT waiting for draw3d.json (shim may lack draw counters)")
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
