# capture_jump_bug.py — player-in-the-loop capture for the DEAD-JUMP bug (v2).
#
# You drive normally, run this, and it logs — as they happen, flushed to
# log/jump_capture.txt — TWO things per event so nothing is missed:
#   (A) EF70KILL: a call to FUN_0046EF70 (wheel contact spring/damper resolver,
#       the hypothesised culprit) that COLLAPSES a leaving/airborne car's
#       horizontal linear velocity (+0x9B0/9B8). before vs after = the smoking gun.
#   (B) TAKEOFF: any grounded->airborne transition, with the render-transform
#       horizontal speed just before/after and the +0x9B0 velocity — a
#       hook-independent witness in case FUN_0046EF70 is NOT the killer.
# Thresholds are deliberately loose so a real dead jump is always captured; we
# identify the dead one from the numbers afterward.
#
# Usage (while a race is running):
#   py -3.12 re/frida/capture_jump_bug.py            # auto-attach if one MASHED
#   py -3.12 re/frida/capture_jump_bug.py --pid 1234 # explicit PID (if several)
# Runs until Ctrl-C. Attach-only; never spawns/kills; refuses to guess among
# multiple MASHED (multi-session safe). Anchored to MASHED.exe BDCAE093...EFD3C0E.
import sys, time, argparse
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
LOG  = ROOT / "log" / "jump_capture.txt"

def find_pid(explicit):
    if explicit: return explicit
    pids = []
    if psutil:
        for p in psutil.process_iter(["pid", "name"]):
            if (p.info["name"] or "").lower() == "mashed.exe":
                pids.append(p.info["pid"])
    else:
        import subprocess
        out = subprocess.run(["tasklist", "/fi", "imagename eq MASHED.exe", "/fo", "csv", "/nh"],
                             capture_output=True, text=True).stdout
        for line in out.splitlines():
            if "MASHED.exe" in line:
                try: pids.append(int(line.split(",")[1].strip('"')))
                except Exception: pass
    if not pids:
        print("No MASHED.exe running. Launch the game and start a race first."); sys.exit(1)
    if len(pids) > 1:
        print(f"Multiple MASHED.exe: {pids}. Re-run with --pid <the one you're playing>."); sys.exit(1)
    return pids[0]

AGENT = r"""
'use strict';
const IMG=0x400000; const B=Process.findModuleByName('MASHED.exe').base;
function ga(r){return B.add(r-IMG);}
function rF(p){ try { return ptr(p).readFloat(); } catch(e){ return NaN; } }
const REC=0x008815a0, MAT=0x00881ec8, STRIDE=0xd04, COUNT=0x008a94d0, PHASE=0x00771968;
function idxOf(recPtr){ const off=recPtr.sub(ga(REC)); const v=parseInt(off.toString());
  return (v>=0 && v%STRIDE===0 && v/STRIDE<8) ? v/STRIDE : -1; }
function rpos(i){ const mr=ga(MAT).add(i*STRIDE); const a=mr.add(0x80).readS32()&1; const m=mr.add(a*0x40);
  return [rF(m.add(0x30)),rF(m.add(0x34)),rF(m.add(0x38))]; }
function hyp(a,b){ return Math.sqrt(a*a+b*b); }
function wheels(rec){ const w=[]; for(let k=0;k<4;k++){ const b=rec.add(0x194+k*0xc4);
  w.push([+rF(rec.add(0x65*4+k*0x31*4+0x1b*4)).toFixed(3),+rF(rec.add(0x65*4+k*0x31*4+0x1c*4)).toFixed(3),
          +rF(rec.add(0x65*4+k*0x31*4+0x1d*4)).toFixed(3), +rF(b).toFixed(2)]); } return w; }

// The dead-jump SIGNATURE (owner-confirmed): "fast into the ramp, stopped dead
// at the lip, dropped straight down." So detect a SUDDEN DECELERATION of render
// motion — a car whose world-space horizontal speed was HIGH and collapses to ~0
// within a couple ticks — regardless of grounded state (the stop happens while
// still contacting the lip, before a clean takeoff). Then log a wide window so we
// see the fast approach, the exact stop tick, and the subsequent fall.
let cTick=0, cStop=0;
const HIST=20;                 // ~1/3 s of context around the stop
const HI=0.08, LO=0.025;       // world u/tick: HI≈racing, LO≈stopped
let ring={}, cooldown={};
Interceptor.attach(ga(0x0047eb30), { onEnter(){ cTick++;
  const cc=ga(COUNT).readS32(); const n=(cc<0?0:(cc>8?8:cc));
  for(let i=0;i<n;i++){ try{
    const rec=ga(REC).add(i*STRIDE); const g=rF(rec.add(0x9e0)); const p=rpos(i);
    const vx=rF(rec.add(0x9b0)), vz=rF(rec.add(0x9b8));
    if(!ring[i]) ring[i]=[]; const r=ring[i];
    const prev=r.length?r[r.length-1]:null;
    const sp=prev?hyp(p[0]-prev.x,p[2]-prev.z):0;   // render speed this tick
    r.push({tick:cTick,g,x:p[0],y:p[1],z:p[2],vx,vz,sp}); if(r.length>HIST) r.shift();
    if(cooldown[i]>cTick) continue;
    if(r.length>=6){
      // current ~stopped, but a recent tick was fast, and dropping quickly
      const cur=r[r.length-1].sp;
      let recentMax=0; for(let j=Math.max(0,r.length-6);j<r.length-1;j++) if(r[j].sp>recentMax) recentMax=r[j].sp;
      if(cur<LO && recentMax>HI){
        cStop++; cooldown[i]=cTick+25;
        send({kind:'STOP', car:i, tick:cTick, recentMax:recentMax, cur:cur, g:g,
          wheels:wheels(rec),
          window:r.map(s=>({t:s.tick,g:s.g,x:+s.x.toFixed(1),y:+s.y.toFixed(1),z:+s.z.toFixed(1),
                            sp:+s.sp.toFixed(3),hv:+hyp(s.vx,s.vz).toFixed(0)}))});
      }
    }
  }catch(e){} }
}});

// heartbeat every ~2s
setInterval(function(){
  let ph=-1; try{ ph=ga(PHASE).readU32(); }catch(e){}
  send({kind:'HB', phase:ph, ticks:cTick, stops:cStop});
}, 2000);
"""

def main():
    ap = argparse.ArgumentParser(); ap.add_argument("--pid", type=int, default=0)
    pid = find_pid(ap.parse_args().pid)
    print(f"Attaching to MASHED.exe pid={pid} ... (drive and jump; Ctrl-C to stop)", flush=True)
    LOG.parent.mkdir(exist_ok=True)
    logf = open(LOG, "a", buffering=1)   # line-buffered
    logf.write(f"\n===== capture session pid={pid} {time.strftime('%Y-%m-%d %H:%M:%S')} =====\n")
    sess = frida.attach(pid)
    n_stop=[0]; last_hb=[None]
    def w(s): print(s, flush=True); logf.write(s+"\n")
    def on_msg(m, data):
        if m.get("type")!="send":
            if m.get("type")=="error": w("  agent error: "+str(m.get("description")))
            return
        p=m["payload"]
        if p["kind"]=="HB":
            hb=(p["phase"],p["ticks"],p["stops"])
            if hb!=last_hb[0]:
                last_hb[0]=hb
                race = "IN-RACE" if p["phase"] in (3,6) else f"phase={p['phase']} (not racing)"
                print(f"  [hb] {race}  ticks={p['ticks']} dead-stops={p['stops']}", flush=True)
        elif p["kind"]=="STOP":
            n_stop[0]+=1
            w(f"\n[DEAD-STOP #{n_stop[0]}] car{p['car']} tick{p['tick']} "
              f"recentMax_hspd={p['recentMax']:.3f} -> cur={p['cur']:.3f}  grounded={p['g']:.1f}")
            w(f"   wheel contact normals/loads: {p['wheels']}")
            w(f"   window (t, g, x,y,z, render-spd, +0x9B0hvel):")
            for s in p["window"]:
                w(f"     t{s['t']} g={s['g']:.0f} ({s['x']},{s['y']},{s['z']}) sp={s['sp']} hv={s['hv']}")
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    print("Ready. DRIVE fast INTO a ramp and let it die. Dead-stops log as they happen.\n", flush=True)
    try:
        while True: time.sleep(0.5)
    except KeyboardInterrupt:
        print(f"\nStopped. dead-stops captured={n_stop[0]} -> {LOG}")
    finally:
        try: sess.detach()
        except Exception: pass
        logf.close()

if __name__ == "__main__":
    main()
