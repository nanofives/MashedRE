# sweep.py — whole-game dual-side parity capture orchestrator.
#
# For each scenario in re/parity/scenarios.json, drive BOTH the original
# (original/MASHED.exe, under Frida) and the standalone reimplementation
# (mashedmod/build/mashed_re.exe) to the SAME settled state and capture each
# side's draw stream (+ a backbuffer pixel dump). The per-scenario bundles it
# writes under re/parity/runs/<ts>/<id>/ are the input to re/parity/report.py
# (ranked, emitter-attributed difference report) and re/parity/target_c4.py
# (C4-promotion targeting queue).
#
# This is the automation that replaces hunting differences one-by-one: it
# reuses the proven capture primitives (menu_draw_burst's nav-push + Im2D burst
# agent on the original; MASHED_GOTO / MASHED_DBG_DRAWSTREAM / MASHED_DBG_BBDUMP
# on the standalone) and the on-demand backbuffer-request mechanism the d3d9
# shim exposes (MASHED_ORIG_BBDUMP_REQ) so the original's pixel dump lands at the
# settled moment without fragile absolute-frame coordination.
#
# P0 = settled MENU screens (draw2d + pixel). P1-P3 drivers (boot_frame /
# nav_race / flythrough, draw3d channel) are stubbed: a scenario whose driver is
# not yet implemented is reported SKIPPED, never counted as captured.
#
# PROCESS HYGIENE (mandatory, CLAUDE.md multi-session rule): this script tracks
# and kills ONLY the PIDs it spawns. It never blanket-kills MASHED by name. One
# original-side Frida session at a time (scenarios run serially).
#
# Usage:
#   py -3.12 re/parity/sweep.py [--manifest re/parity/scenarios.json]
#       [--only menu.s1,menu.s7] [--phase menu] [--side both|re|orig]
#       [--out re/parity/runs/<ts>/] [--re-cwd DIR] [--dry-run] [--keep-going]
#
# After a sweep:  py -3.12 re/parity/report.py re/parity/runs/<ts>
import argparse
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

# ----------------------------------------------------------------- paths
ROOT = Path(__file__).resolve().parent.parent.parent          # worktree root


def find_original(root: Path) -> Path:
    """original/ lives in the MAIN checkout; a worktree does not have it.
    Walk ancestors until we find original/MASHED.exe (mirrors run_diff.py)."""
    cand = root / "original" / "MASHED.exe"
    if cand.exists():
        return cand
    for parent in root.parents:
        c2 = parent / "original" / "MASHED.exe"
        if c2.exists():
            return c2
    return cand   # let the later existence check produce a clear error


MASHED_EXE = find_original(ROOT)
ORIG_DIR = MASHED_EXE.parent
MAIN_ROOT = ORIG_DIR.parent                                   # main checkout root
RE_EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"
RE_MAP = ROOT / "mashedmod" / "build" / "mashed_re.map"
CANONICAL_CFG = MAIN_ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"

# Detach each spawned child so a terminal Ctrl-C does not broadcast-kill every
# MASHED in the group; proc.kill() still tears down THIS pid surgically.
_CREATE_NEW_PROCESS_GROUP = 0x00000200
_DETACHED_PROCESS = 0x00000008

# Original-side capture agent — nav push (FUN_0043d2a0 + curscreen write) and a
# K-frame Im2D burst, lifted verbatim from re/frida/menu_draw_burst.py (proven,
# hot-path-safe: the device-draw Interceptor only does work while `collecting`).
ORIG_AGENT = r"""
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_SHELLA=0x0042e3a0, RVA_PHASE=0x0067eca4;
const VTBL=0x007d3ff8, VBUF=0x00898a20, STRIDE=0x1c;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_CURSCREEN=0x0067ecb0;
let nav=null, collecting=0, frames={}, order=[], cur=null, drawHooked=0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); abs(RVA_CURSCREEN).writeS32(scr); return abs(RVA_DEPTH).readS32(); },
  armburst:function(label,k){
    if(!drawHooked){
      const vt=abs(VTBL).readU32();
      const drawFn=ptr(vt).add(0x30).readU32();
      Interceptor.attach(ptr(drawFn), { onEnter(args){
        if(collecting===0 || cur===null) return;
        let verts=args[1]; let n=args[2].toInt32();
        if(n<1||n>64){ n=4; }
        if(verts.isNull()){ verts=abs(VBUF); }
        let h='';
        try{
          const u=new Uint8Array(verts.readByteArray(n*STRIDE));
          for(let i=0;i<u.length;i++){ h+=('0'+u[i].toString(16)).slice(-2); }
        }catch(e){ return; }
        let rets=[];
        try{
          rets=Thread.backtrace(this.context, Backtracer.FUZZY).slice(0,5)
               .map(function(a){ return a.sub(DELTA).toString(); });
        }catch(e){}
        frames[cur].push({v:h, r:rets});
      }});
      drawHooked=1;
    }
    let seen=0;
    const h=Interceptor.attach(abs(RVA_SHELLA), { onEnter(args){
      seen++;
      if(seen<=k){ cur=label+'_f'+(seen-1); frames[cur]=[]; order.push(cur); collecting=1; }
      else { collecting=0; cur=null; h.detach(); }
    }});
    return 1;
  },
  done:function(){ return (collecting===0 && order.length>0) ? order.length : 0; },
  report:function(){ const out={}; order.forEach(function(l){ out[l]=frames[l]; }); return JSON.stringify(out); }
};
"""


# --------------------------------------------------------------- helpers
def log(msg):
    print(msg, flush=True)


def merged(defaults, scenario):
    """Scenario fields override manifest defaults; tolerances merge key-by-key."""
    out = dict(defaults)
    tol = dict(defaults.get("tolerances", {}))
    tol.update(scenario.get("tolerances", {}))
    out.update({k: v for k, v in scenario.items() if k != "tolerances"})
    out["tolerances"] = tol
    return out


def bmp_is_blank(path: Path) -> bool:
    """A backbuffer that is entirely one colour (all-black / all-white) means the
    capture never reached a rendered state — guards the degenerate-GREEN class
    (plan R7). Cheap: sample the BMP pixel bytes, bail as soon as two differ."""
    try:
        from PIL import Image
        im = Image.open(path).convert("RGB")
        first = None
        for px in im.getdata():
            if first is None:
                first = px
            elif px != first:
                return False
        return True
    except Exception:
        return False   # cannot prove blank -> treat as non-blank (do not fail open)


def wait_for_artifact(path: Path, since: float, timeout: float) -> bool:
    """True once `path` exists, is non-empty, and was modified after `since`."""
    end = time.time() + timeout
    while time.time() < end:
        try:
            st = path.stat()
            if st.st_size > 0 and st.st_mtime >= since - 1.0:
                return True
        except OSError:
            pass
        time.sleep(0.3)
    return False


# ----------------------------------------------------------- RE side
def capture_re(sc, bundle: Path, spawned, re_cwd: Path, dry: bool):
    """Drive the standalone to `screen` via MASHED_GOTO, capture draw2d
    (MASHED_DBG_DRAWSTREAM) + pixel (MASHED_DBG_BBDUMP). No Frida: MASHED_GOTO
    parks the screen from boot, so the frame-200 backbuffer is already settled."""
    res = {"draw2d": None, "pixel": None, "ok_draw2d": False, "ok_pixel": False}
    cap = sc.get("capture", [])
    screen = sc["screen"]
    ds_out = re_cwd / "log" / "drawstream_re.json"
    bb_out = re_cwd / "verify" / "dbg_backbuffer.bmp"

    env = dict(os.environ)
    env["MASHED_GOTO"] = str(screen)
    if "draw2d" in cap:
        env["MASHED_DBG_DRAWSTREAM"] = "200:203"
    if "pixel" in cap:
        env["MASHED_DBG_BBDUMP"] = "203"

    if dry:
        log(f"    [re] DRY would spawn {RE_EXE.name} cwd={re_cwd} "
            f"MASHED_GOTO={screen} cap={cap}")
        return res

    if not RE_EXE.exists():
        log(f"    [re] ERROR: {RE_EXE} not built (run mashedmod\\build.bat)")
        return res
    # Remove stale artifacts so we detect THIS run's fresh output.
    for p in (ds_out, bb_out):
        try: p.unlink()
        except OSError: pass
    ds_out.parent.mkdir(parents=True, exist_ok=True)
    bb_out.parent.mkdir(parents=True, exist_ok=True)

    since = time.time()
    proc = subprocess.Popen(
        [str(RE_EXE)], cwd=str(re_cwd), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=_CREATE_NEW_PROCESS_GROUP | _DETACHED_PROCESS)
    spawned.append(proc)
    log(f"    [re] pid={proc.pid} screen={screen}")
    try:
        if "draw2d" in cap and wait_for_artifact(ds_out, since, 45):
            dst = bundle / "re.draw.json"
            shutil.copy2(ds_out, dst)
            try:
                data = json.loads(dst.read_text())
                res["ok_draw2d"] = any(len(v) > 0 for v in data.values())
            except ValueError:
                res["ok_draw2d"] = False
            res["draw2d"] = str(dst)
        if "pixel" in cap and wait_for_artifact(bb_out, since, 45):
            dst = bundle / "re.png"
            shutil.copy2(bb_out, bundle / "re.bmp")
            try:
                from PIL import Image
                Image.open(bundle / "re.bmp").convert("RGB").save(dst)
            except Exception:
                dst = bundle / "re.bmp"
            res["pixel"] = str(dst)
            res["ok_pixel"] = not bmp_is_blank(bundle / "re.bmp")
    finally:
        try: proc.kill()
        except Exception: pass
    return res


# --------------------------------------------------------- original side
def capture_orig(sc, bundle: Path, spawned, dry: bool):
    """Drive the original under Frida to `screen`, capture an Im2D burst
    (draw2d) and request a settled backbuffer dump (pixel) via the d3d9 shim's
    MASHED_ORIG_BBDUMP_REQ on-demand mechanism."""
    res = {"draw2d": None, "pixel": None, "ok_draw2d": False, "ok_pixel": False}
    cap = sc.get("capture", [])
    screen = sc["screen"]
    frames = sc.get("frames", 4)
    settle = sc.get("settle_ms", 2500) / 1000.0
    label = f"scr{screen}"

    if dry:
        log(f"    [orig] DRY would frida-spawn MASHED.exe push={screen} "
            f"frames={frames} settle={settle}s cap={cap}")
        return res
    if not MASHED_EXE.exists():
        log(f"    [orig] ERROR: original MASHED.exe not found at {MASHED_EXE}")
        return res
    shim = ORIG_DIR / "d3d9.dll"
    if not shim.exists():
        log(f"    [orig] ERROR: {shim} missing — run mashedmod\\build_d3d9_shim.bat")
        return res
    # Windowed 800x600 so concurrent sessions don't fight over fullscreen.
    if CANONICAL_CFG.exists():
        shutil.copy2(CANONICAL_CFG, ORIG_DIR / "videocfg.bin")

    import frida
    req_file = bundle / "orig_bbdump.req"
    bb_target = (bundle / "orig.bmp").resolve()
    try: req_file.unlink()
    except OSError: pass

    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if "pixel" in cap:
        env["MASHED_ORIG_BBDUMP_REQ"] = str(req_file.resolve())

    dev = frida.get_local_device()
    pid = dev.spawn(str(MASHED_EXE), cwd=str(ORIG_DIR), env=env)
    # Track the spawned original by pid so the outer cleanup kills only this one.
    spawned.append(_FridaProc(dev, pid))
    sess = dev.attach(pid)
    scr = sess.create_script(ORIG_AGENT)
    scr.on("message", lambda m, d: None)
    scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync
    log(f"    [orig] pid={pid} push={screen}")
    try:
        end = time.time() + 25
        while time.time() < end and E.phase() != 3:
            time.sleep(0.2)
        time.sleep(1.5)
        E.push(screen)
        time.sleep(settle)

        if "draw2d" in cap:
            E.armburst(label, frames)
            end = time.time() + 15
            while time.time() < end and E.done() == 0:
                time.sleep(0.2)
            data = json.loads(E.report())
            dst = bundle / "orig.draw.json"
            dst.write_text(json.dumps(data, indent=1))
            res["draw2d"] = str(dst)
            res["ok_draw2d"] = any(len(v) > 0 for v in data.values())

        if "pixel" in cap:
            req_file.write_text(str(bb_target) + "\n")
            # The shim polls the req file each Present, dumps, then deletes it.
            end = time.time() + 15
            while time.time() < end and req_file.exists():
                time.sleep(0.3)
            if bb_target.exists():
                try:
                    from PIL import Image
                    Image.open(bb_target).convert("RGB").save(bundle / "orig.png")
                    res["pixel"] = str(bundle / "orig.png")
                except Exception:
                    res["pixel"] = str(bb_target)
                res["ok_pixel"] = not bmp_is_blank(bb_target)
    finally:
        try: sess.detach()
        except Exception: pass
        try: dev.kill(pid)
        except Exception: pass
    return res


class _FridaProc:
    """Adapter so the spawned-pid cleanup list can kill frida-spawned originals
    the same way it kills subprocess.Popen children (kill-only-what-we-spawned)."""
    def __init__(self, dev, pid):
        self.dev, self.pid = dev, pid

    def kill(self):
        self.dev.kill(self.pid)


# --------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--manifest", default=str(ROOT / "re" / "parity" / "scenarios.json"))
    ap.add_argument("--only", default=None, help="comma-separated scenario ids")
    ap.add_argument("--phase", default=None, help="comma-separated phases to include")
    ap.add_argument("--side", choices=["both", "re", "orig"], default="both")
    ap.add_argument("--out", default=None, help="run dir (default re/parity/runs/<ts>)")
    ap.add_argument("--re-cwd", default=str(MAIN_ROOT),
                    help="working dir for mashed_re.exe (must reach the game "
                         "assets; default = main checkout root)")
    ap.add_argument("--ts", default=None, help="run timestamp label (default: read "
                    "from env PARITY_TS or a fixed 'latest')")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--keep-going", action="store_true",
                    help="continue after a scenario errors (default: stop)")
    args = ap.parse_args()

    manifest = json.loads(Path(args.manifest).read_text())
    defaults = manifest.get("defaults", {})
    scenarios = manifest.get("scenarios", [])

    if args.only:
        want = set(s.strip() for s in args.only.split(","))
        scenarios = [s for s in scenarios if s["id"] in want]
    if args.phase:
        phases = set(p.strip() for p in args.phase.split(","))
        scenarios = [s for s in scenarios
                     if merged(defaults, s).get("phase") in phases]
    if not scenarios:
        log("no scenarios selected")
        return 2

    # Date.now() is unavailable in workflow scripts but fine here; still allow an
    # explicit --ts / PARITY_TS so re-runs are reproducible and resumable.
    ts = args.ts or os.environ.get("PARITY_TS") or time.strftime("%Y%m%d_%H%M%S")
    run_dir = Path(args.out) if args.out else ROOT / "re" / "parity" / "runs" / ts
    run_dir.mkdir(parents=True, exist_ok=True)
    re_cwd = Path(args.re_cwd)
    log(f"sweep -> {run_dir}  ({len(scenarios)} scenarios, side={args.side})")
    log(f"  original: {MASHED_EXE}")
    log(f"  re exe:   {RE_EXE}  (cwd {re_cwd})")
    if not args.dry_run and "draw2d" in str(defaults.get("capture")) and not RE_MAP.exists():
        log(f"  note: {RE_MAP} absent — report attribution will show raw RVAs only")

    spawned = []
    index = {"ts": ts, "side": args.side, "manifest": args.manifest, "scenarios": {}}
    rc = 0
    try:
        for s in scenarios:
            sc = merged(defaults, s)
            sid = sc["id"]
            bundle = run_dir / sid
            bundle.mkdir(parents=True, exist_ok=True)
            log(f"  [{sid}] phase={sc.get('phase')} det={sc.get('determinism')} "
                f"cap={sc.get('capture')}")
            entry = {"scenario": sc, "re": None, "orig": None, "error": None}
            try:
                if args.side in ("both", "re"):
                    entry["re"] = capture_re(sc, bundle, spawned, re_cwd, args.dry_run)
                if args.side in ("both", "orig"):
                    entry["orig"] = capture_orig(sc, bundle, spawned, args.dry_run)
            except Exception as e:
                entry["error"] = repr(e)
                log(f"    ERROR: {e}")
                rc = 1
                if not args.keep_going and not args.dry_run:
                    index["scenarios"][sid] = entry
                    break
            index["scenarios"][sid] = entry
    finally:
        for p in spawned:
            try: p.kill()
            except Exception: pass
    (run_dir / "index.json").write_text(json.dumps(index, indent=1, default=str))
    log(f"\nwrote {run_dir / 'index.json'}")
    log(f"next: py -3.12 re/parity/report.py {run_dir}")
    return rc


if __name__ == "__main__":
    sys.exit(main())
