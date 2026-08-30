# orig_grid_timeseries.py — sample the ORIGINAL's per-car world positions as a
# TIME SERIES from the instant the race phase begins, to decide whether the grid
# formation (staggered diagonal, as read at the earlier ad-hoc capture moment) is
# the real starting grid or merely a few moments into the roll-away.
#
# Reads the authoritative gameplay car array, layout cited by
# re/frida/carpos_source_probe.py (from FUN_0046d4a0 @0x0046d4a0):
#   record base 0x008815a0, stride 0xd04
#   active-index table  0x00881f48 + i*0xd04   (S32)
#   active matrix base  0x00881ec8 + i*0xd04 + act*0x40, pos at +0x30 (3 floats)
#   car count  0x008a94d0 (S32)
#   phase      0x00771968 (S32); race == 3
#
# Navigation is the proven nav_agent.js recipe used by race_draw_burst.py:
# Quick Battle ALWAYS races TRAINING, so no track choice is needed and the grid
# is directly comparable to the standalone's MASHED_TRACK_SEL=12 (TRAINING) grid.
#
# PID HYGIENE: spawns ONE MASHED, tracks that pid, kills ONLY that pid. Never
# blanket-kills by name.
#
# Usage: py -3.12 re/frida/orig_grid_timeseries.py --out <dir> [--dur 6.0] [--dt 0.15]
import argparse, json, os, subprocess, sys, time
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

CARS_JS = r"""
'use strict';
const IMG = 0x00400000;
const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
const SLIDE = m.base.toUInt32() - IMG;
const RECBASE = 0x008815a0, STRIDE = 0xd04;
const MATBASE = 0x00881ec8, IDXOFF = 0x00881f48;
const COUNT = ptr(0x008a94d0 + SLIDE);
const PHASE = ptr(0x00771968 + SLIDE);
function rf(a){ return a.readFloat(); }

// Keep the game live while a terminal has focus (belt-and-braces alongside the
// no_focus_pause on-disk patch): force the active flag and wake WaitMessage.
const ACTIVE_FLAG = ptr(0x0077391c + SLIDE);
try {
  const ptm = new NativeFunction(
    Process.getModuleByName('user32.dll').getExportByName('PostThreadMessageA'),
    'int', ['uint32','uint32','pointer','pointer'], 'stdcall');
  for (const t of Process.enumerateThreads()) ptm(t.id, 0, NULL, NULL);
  setInterval(function(){ try { ACTIVE_FLAG.writeU32(1); } catch(e){} }, 250);
} catch (e) {}

rpc.exports = {
  phase: function(){ try { return PHASE.readS32(); } catch(e){ return -999; } },
  cars: function(){
    let n = 0; try { n = COUNT.readS32(); } catch(e){ n = -1; }
    const nn = (n > 0 && n < 16) ? n : 4;
    const out = [];
    for (let i = 0; i < nn; i++) {
      try {
        const act = ptr(IDXOFF + SLIDE + i*STRIDE).readS32();
        const mm = ptr(MATBASE + SLIDE + i*STRIDE + act*0x40);
        out.push({ car:i, act:act,
                   pos:[rf(mm.add(0x30)), rf(mm.add(0x34)), rf(mm.add(0x38))] });
      } catch (e) { out.push({ car:i, err:String(e) }); }
    }
    return { count:n, cars:out };
  }
};
"""


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--out", default=str(ROOT / "verify" / "grid_orig_ts"))
    ap.add_argument("--dur", type=float, default=6.0,
                    help="seconds to sample after phase==3")
    ap.add_argument("--dt", type=float, default=0.15, help="sample interval")
    args = ap.parse_args()

    out = Path(args.out).resolve()
    out.mkdir(parents=True, exist_ok=True)

    if not EXE.exists():
        sys.exit(f"original MASHED.exe not found at {EXE}")

    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "left-bl")
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"

    dev = frida.get_local_device()
    proc = sess = None
    for _ in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL, creationflags=NPG | DET | ABOVE)
        time.sleep(0.2)
        try: sess = dev.attach(proc.pid); break
        except Exception:
            try: proc.kill()
            except Exception: pass
            time.sleep(1.0)
    if sess is None:
        sys.exit("attach failed after 5 retries")
    print(f"  pid={proc.pid}", flush=True)

    rc = 3
    try:
        scr = sess.create_script(NAV)
        scr.on("message", lambda mm, d: None)
        scr.load(); E = scr.exports_sync; E.init()

        carscr = sess.create_script(CARS_JS)
        carscr.on("message", lambda mm, d: None)
        carscr.load(); C = carscr.exports_sync

        def wait(pred, t):
            end = time.time() + t
            while time.time() < end:
                if pred(): return True
                time.sleep(0.05)
            return False
        def press(c, ms=180): E.press(c, ms); time.sleep(ms/1000.0 + 0.3)
        def confirm_to(target, tries=6):
            for _ in range(tries):
                if E.depth() >= target: return True
                press(4)
                if wait(lambda: E.depth() >= target, 2.0): return True
            return E.depth() >= target

        print("  booting to menu...", flush=True)
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30):
            sys.exit("never reached title")
        time.sleep(1.0)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(1); time.sleep(0.3)          # mode 1 = Quick Battle (TRAINING)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.2)
        for _ in range(6):
            if E.phase() != 3: break
            press(4); time.sleep(1.2)
        # E.phase() is the MENU phase (3 == in menus). The car array PHASE at
        # 0x00771968 is the race phase (3 == racing). Wait on the car-array phase.
        print("  waiting for race phase (car-array 0x00771968 == 3)...", flush=True)
        if not wait(lambda: C.phase() == 3, 20):
            print(f"  NOT in race (car-array phase={C.phase()}) — aborting", flush=True)
            rc = 3
            return rc
        t0 = time.time()
        print(f"  RACE START at t0; sampling {args.dur}s @ {args.dt}s", flush=True)
        samples = []
        while time.time() - t0 < args.dur:
            t = time.time() - t0
            try:
                d = C.cars()
            except Exception as e:
                d = {"err": str(e)}
            samples.append({"t": round(t, 4), **d})
            time.sleep(args.dt)

        # Persist raw + a compact summary.
        (out / "orig_grid_timeseries.json").write_text(
            json.dumps({"exe": str(EXE), "samples": samples}, indent=2))

        def spread(cars):
            pts = [c["pos"] for c in cars if isinstance(c, dict) and "pos" in c]
            mx = 0.0
            for i in range(len(pts)):
                for j in range(i+1, len(pts)):
                    dx = pts[i][0]-pts[j][0]; dy = pts[i][1]-pts[j][1]; dz = pts[i][2]-pts[j][2]
                    mx = max(mx, (dx*dx+dy*dy+dz*dz) ** 0.5)
            return mx

        lines = ["# ORIGINAL car world positions, time series from race start",
                 f"# exe={EXE}", f"# count field (0x008a94d0), phase(0x00771968)==3",
                 "# t  car  act  x  y  z    | max_pairwise_spread"]
        for s in samples:
            cars = s.get("cars", [])
            sp = spread(cars)
            for c in cars:
                if "pos" in c:
                    p = c["pos"]
                    lines.append(f"{s['t']:6.3f}  c{c['car']} act{c['act']} "
                                 f"{p[0]:8.4f} {p[1]:8.4f} {p[2]:8.4f}"
                                 + (f"   spread={sp:.3f}" if c["car"] == 0 else ""))
            lines.append("")
        (out / "orig_grid_timeseries.txt").write_text("\n".join(lines))

        # Print the first and last valid samples for the transcript.
        def show(tag, s):
            print(f"  {tag} t={s['t']:.3f} count={s.get('count')} spread={spread(s.get('cars',[])):.3f}", flush=True)
            for c in s.get("cars", []):
                if "pos" in c:
                    p = c["pos"]
                    print(f"      c{c['car']} act{c['act']} ({p[0]:.4f},{p[1]:.4f},{p[2]:.4f})", flush=True)
        valid = [s for s in samples if any("pos" in c for c in s.get("cars", []))]
        if valid:
            show("FIRST", valid[0])
            show("LAST ", valid[-1])
            rc = 0
        else:
            print("  no valid car samples captured", flush=True)
            rc = 4
    finally:
        try: scr.unload()
        except Exception: pass
        try: carscr.unload()
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        try: dev.kill(proc.pid)
        except Exception: pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
