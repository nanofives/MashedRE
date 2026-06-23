# Replay-clock DIAGNOSTIC (passive; no input injection, no menu-driving).
#
# Purpose: prove or kill the hypothesis that the record/replay frame clock
# (GetDeviceState poll count) drifts run-to-run, by measuring three candidate
# clocks side by side during boot -> menu and snapshotting them at the moment
# the menu first becomes interactive:
#   (1) GDS  = IDirectInputDevice8::GetDeviceState(cb=256) call count   [current clock]
#   (2) PRES = FUN_004c1be0 call count  (MASHED present/frame fn)       [render clock?]
#   (3) MENU = FUN_0043c5b0 call count  (menu state machine)            [menu tick?]
#       NAV  = FUN_0043d2a0 call count  (menu nav handler)              [menu-interactive marker?]
#
# Launches MASHED itself at --fps and attaches ASAP during boot (symmetric with
# record_attach/replay_verify --launch). Kills ONLY the pid it launches. Read-only:
# never writes the input buffer. Run twice and diff the menu-start snapshots.
#
# Usage:
#   py -3.12 re/frida/probe_replay_clock.py --label run1 --fps 60 --seconds 30
import os, sys, time, json, argparse, subprocess
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / "original" / "MASHED.exe"
IMG = 0x00400000

# (label, rva) MASHED.exe functions to count as candidate clocks / markers.
MARKERS = [
    ("PRES", 0x004c1be0),   # present / per-rendered-frame (from replay_verify analysis)
    ("MENU", 0x0043c5b0),   # menu state machine (entry table @0x898ac0)
    ("NAV",  0x0043d2a0),   # menu nav handler
]

AGENT = r'''
'use strict';
let GDS=0, t0=0;
const CNT={};            // name -> call count
const FIRST={};          // name -> {gds, t, ...snapshot of all CNT}
let textLo=null, textHi=null;
function inText(p){ return textLo && p.compare(textLo)>=0 && p.compare(textHi)<0; }
function addText(mod){ mod.enumerateRanges('r-x').forEach(function(r){ const e=r.base.add(r.size);
  if(textLo===null||r.base.compare(textLo)<0)textLo=r.base; if(textHi===null||e.compare(textHi)>0)textHi=e; }); }

// ---- GetDeviceState(cb=256) poll clock (vtable-scan resolver, retry until armed) ----
const seen={};
function hookGDS(addr){ const k=addr.toString(); if(seen[k]) return; seen[k]=1;
  Interceptor.attach(addr,{ onEnter(a){ this.cb=a[1].toInt32(); },
                            onLeave(){ if(this.cb===256){ GDS++; if(t0===0) t0=Date.now(); } } }); }
function resolveGDS(){ let armed=0;
  ['dinput8_real.DLL','DINPUT8.dll','dinput8.dll'].forEach(function(mn){
    const mod=Process.findModuleByName(mn); if(!mod) return; addText(mod);
    mod.enumerateRanges('r--').forEach(function(r){
      let a=r.base; const end=r.base.add(r.size).sub(4); let runStart=null, runLen=0;
      function flush(){ if(runStart!==null && runLen>=16){ try{ const g=runStart.add(9*4).readPointer();
        if(inText(g)){ hookGDS(g); armed++; } }catch(e){} } runStart=null; runLen=0; }
      while(a.compare(end)<0){ let p=null; try{ p=a.readPointer(); }catch(e){}
        if(p && inText(p)){ if(runStart===null){runStart=a;runLen=0;} runLen++; } else flush(); a=a.add(4); }
      flush(); }); });
  return armed; }
let N_GDS = resolveGDS();
if (N_GDS === 0){ const iv=setInterval(function(){ const n=resolveGDS(); if(n>0){ N_GDS=n; clearInterval(iv);
  send({kind:'info', msg:'GDS armed late='+n}); } }, 100); }

// ---- MASHED.exe marker counters ----
function snapshot(){ const s={gds:GDS, t:(t0?Date.now()-t0:0)}; for(const k in CNT) s[k]=CNT[k]; return s; }
function armMarkers(markers){
  const mod=Process.findModuleByName('MASHED.exe'); if(!mod){ send({kind:'err', msg:'MASHED.exe not found'}); return; }
  markers.forEach(function(m){ const name=m[0], rva=m[1]; CNT[name]=0;
    try{ Interceptor.attach(mod.base.add(rva - %IMG%), { onEnter(){ CNT[name]++;
      if(!FIRST[name]){ FIRST[name]=snapshot(); send({kind:'first', name:name, snap:FIRST[name]}); } } });
    }catch(e){ send({kind:'err', msg:'arm '+name+' @0x'+rva.toString(16)+': '+e}); } });
}

rpc.exports = {
  armmarkers: function(m){ armMarkers(m); return Object.keys(CNT); },
  sample: function(){ return {gds:GDS, cnt:CNT, t:(t0?Date.now()-t0:0), n_gds:N_GDS}; },
  first: function(){ return FIRST; }
};
send({kind:'ready', msg:'clock-probe armed; GDS vtable hooks='+N_GDS});
'''.replace('%IMG%', hex(IMG))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--label", default="run", help="label for this run (for diffing two runs)")
    ap.add_argument("--fps", default="60")
    ap.add_argument("--seconds", type=int, default=30)
    args = ap.parse_args()

    if not MASHED_EXE.exists():
        print(f"error: {MASHED_EXE} not found"); return 2

    env = dict(os.environ)
    env["MASHED_FPS_CAP"] = str(args.fps)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env)
    pid = proc.pid
    print(f"=== probe_replay_clock [{args.label}]  launched pid={pid} fps={args.fps} ===")
    print("  attaching ASAP during boot...")
    sess = None
    for _ in range(200):
        try: sess = dev.attach(pid); break
        except Exception: time.sleep(0.1)
    if sess is None:
        print("  error: could not attach")
        try: proc.kill()
        except Exception: pass
        return 3

    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("ready", "info", "err"):
            print("  [agent]", p.get("msg"))
        elif p.get("kind") == "first":
            s = p.get("snap", {})
            extras = " ".join(f"{k}={v}" for k, v in s.items() if k not in ("gds", "t"))
            print(f"\n  >>> FIRST {p.get('name'):<5} fired at  gds={s.get('gds')}  t={s.get('t')}ms  [{extras}]")

    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    scr.exports_sync.armmarkers(MARKERS)
    print(f"  observing {args.seconds}s (passive — no input). per-second rates:\n")
    print(f"  {'t_s':>4} | {'GDS':>7} {'PRES':>7} {'MENU':>7} {'NAV':>6} | "
          f"{'dGDS':>5} {'dPRES':>5} {'dMENU':>5}  poll/render")

    prev = {"gds": 0, "PRES": 0, "MENU": 0}
    try:
        for i in range(args.seconds):
            time.sleep(1.0)
            if psutil and not psutil.pid_exists(pid):
                print("\n  game process exited."); break
            try:
                st = scr.exports_sync.sample()
            except Exception:
                continue
            c = st["cnt"]
            gds = st["gds"]; pres = c.get("PRES", 0); menu = c.get("MENU", 0); nav = c.get("NAV", 0)
            dg = gds - prev["gds"]; dp = pres - prev["PRES"]; dm = menu - prev["MENU"]
            ratio = (dg / dp) if dp else 0.0
            print(f"  {i+1:>4} | {gds:>7} {pres:>7} {menu:>7} {nav:>6} | "
                  f"{dg:>5} {dp:>5} {dm:>5}  {ratio:>6.2f}")
            prev = {"gds": gds, "PRES": pres, "MENU": menu}
    except KeyboardInterrupt:
        print("\n  interrupted.")
    finally:
        try: first = scr.exports_sync.first()
        except Exception: first = {}
        try: sess.detach()
        except Exception: pass
        try:
            if (not psutil) or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass

    print(f"\n=== [{args.label}] menu-interactive snapshots (compare across runs) ===")
    for name in ("PRES", "MENU", "NAV"):
        s = first.get(name)
        if s:
            extras = " ".join(f"{k}={v}" for k, v in s.items() if k not in ("gds", "t"))
            print(f"  first {name:<5}: gds={s.get('gds')}  t={s.get('t')}ms  [{extras}]")
        else:
            print(f"  first {name:<5}: (never fired)")
    print("\n  KEY: if 'first MENU' gds differs run-to-run but its PRES/MENU index holds,")
    print("       the poll clock drifts and a render/menu-tick clock is the fix.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
