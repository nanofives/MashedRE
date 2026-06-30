# CAPTURE PLAYER DYNAMICS (2026-06-30, WS feel-calibration) — warp the ORIGINAL
# MASHED.exe into a race (reusing the scenario_launch path), hold full throttle
# (control 4 via the FUN_00497310 override), and sample the player car's record each
# ~0.2 s to a CSV: speed(+0x9e4), |vel|(+0x9b0..b8), yaw-rate(+0x9c0), heading(atan2 of
# fwd +0x9d4/+0x9dc), grounded(+0x9e0). The accel-curve SHAPE (time to ~63% of top) and
# the steady-state yaw-rate are UNIT-INDEPENDENT, so they calibrate the standalone
# scaffold's spool (kThrottle/kDrag) and turn rate (kSteer) directly — closing the
# input-matched in-race capture deferred in HANDLING_V2_2026-06-10.md.
#
# Spawn+attach (NEVER frida.spawn). Kills ONLY the pid it launches.
# Usage: py -3.12 re/frida/capture_player_dynamics.py [--track 3] [--mode 2] [--hold 16]
#        --steer 0 (none) | 1 (hold steer ctrl `--steerctrl` after --steer-at s)
import os, sys, time, argparse, subprocess, math
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
EXE  = ROOT / "original" / "MASHED.exe"
OUT  = ROOT / "log" / "player_dynamics.csv"

AGENT = r'''
'use strict';
const IMG = 0x400000;
let M = null;
function modBase(){ if(!M) M = Process.findModuleByName('MASHED.exe'); return M ? M.base : null; }
function ga(addr){ const b = modBase(); return b ? b.add(addr - IMG) : null; }

const PHASE=0x00771968, TRACK_ENG=0x0063ba7c, TRACK_MENU=0x0067f17c, MODE=0x0067e9fc,
      RULE=0x007f0fd0, CAR_P0=0x0067ea98, DIFFICULTY=0x0067ea7c, POWERUPS=0x0067ea80,
      PCOUNT=0x008a94d0, TEAM=0x0067ea64, CARREC=0x008815a0, SPAWN_RVA=0x0046b540,
      ACTIVATE=0x0040e480, RES_RVA=0x00497310;

let spawnFired=0, spawnArmed=false;
function armSpawn(){ if(spawnArmed)return; spawnArmed=true;
  try{ Interceptor.attach(ga(SPAWN_RVA),{onEnter(){spawnFired++;}});}catch(e){send({kind:'err',msg:'armSpawn '+e});}}

// (1) raw-resolver override (FUN_00497310): force control 4 (confirm) to skip the
// race-start intro / GO countdown. Does NOT drive the in-race car (the physics reads the
// cooked descriptor, not the raw resolver) — that's what the cook force below is for.
let pc1=-1,pu1=0, inputArmed=false;
function armInput(){ if(inputArmed)return; inputArmed=true;
  try{ Interceptor.attach(ga(RES_RVA),{
    onEnter(){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
    onLeave(ret){ if(this.p===0 && this.c===pc1 && Date.now()<pu1) ret.replace(ptr(0xff)); }
  });}catch(e){send({kind:'err',msg:'armInput '+e});}}

// (2) IN-RACE INJECTOR: the cook FUN_00496530 zeroes player p's descriptor block
// (0x007f1038 + p*0x4c) then writes it from FUN_00497310; the physics (FUN_00467650 / A4
// FUN_00470670) reads block[4]=accel, block[5]=brake, block[0]/[1]=steer. Force player-0's
// block ON LEAVE so it survives the cook. gAccel/gSteer set via rpc.drive.
const COOK_RVA = 0x00496530;
const BLK0 = 0x007f1038;            // player-0 cooked descriptor block
let gAccel=0, gSteer=0, cookArmed=false;
function armCook(){ if(cookArmed)return; cookArmed=true;
  try{ Interceptor.attach(ga(COOK_RVA),{ onLeave(){ const b=ga(BLK0); if(!b)return;
    b.add(4).writeU8(gAccel?0xff:0);                       // accel (block[4] — confirmed)
    // steer = the 3rd/4th directional control (block[2]/[3]); block[0]/[1] are up/down.
    b.add(2).writeU8(gSteer>0?0xff:0);                     // steer A (one way)
    b.add(3).writeU8(gSteer<0?0xff:0);                     // steer B (other way)
    b.add(0x0e).writeU8(gSteer>0?0xff:0);                  // steer-A active flag (block[0xe])
    b.add(0x0f).writeU8(gSteer<0?0xff:0);                  // steer-B active flag (block[0xf])
  }});}catch(e){send({kind:'err',msg:'armCook '+e});}}

rpc.exports = {
  ready:function(){ return modBase()?1:0; },
  phase:function(){ try{return ga(PHASE).readU8();}catch(e){return -1;} },
  setup:function(cfg){
    try{ ga(TRACK_ENG).writeS32(cfg.track); ga(TRACK_MENU).writeS32(cfg.track);
      ga(MODE).writeS32(cfg.mode); ga(RULE).writeS32(cfg.rule); ga(CAR_P0).writeS32(cfg.car);
      ga(TEAM).writeS32(cfg.team);
      if(cfg.difficulty>=0) ga(DIFFICULTY).writeS32(cfg.difficulty);
      if(cfg.powerups>=0) ga(POWERUPS).writeS32(cfg.powerups);
      const e480=new NativeFunction(ga(ACTIVATE),'void',['int','int'],'mscdecl');
      for(let s=0;s<4;s++) e480(s, s===0?1:(s<cfg.cars?2:0));
      armSpawn(); armInput(); armCook(); return 'ok';
    }catch(e){return 'ERR '+e;}
  },
  launch:function(){ try{ ga(PHASE).writeU8(2); return 1; }catch(e){return 'ERR '+e;} },
  press:function(c,ms){ pc1=c; pu1=Date.now()+ms; return 1; },   // raw control (intro skip)
  drive:function(accel,steer){ gAccel=accel; gSteer=steer; return 1; },  // cooked descriptor force
  sample:function(){
    try{ const r=ga(CARREC);
      const vx=r.add(0x9b0).readFloat(), vy=r.add(0x9b4).readFloat(), vz=r.add(0x9b8).readFloat();
      const fx=r.add(0x9d4).readFloat(), fz=r.add(0x9dc).readFloat();
      return { spawnFired:spawnFired, speed:r.add(0x9e4).readFloat(),
        velmag:Math.sqrt(vx*vx+vy*vy+vz*vz),
        avx:r.add(0x9bc).readFloat(), avy:r.add(0x9c0).readFloat(), avz:r.add(0x9c4).readFloat(),
        heading:Math.atan2(fz,fx), grounded:r.add(0x9e0).readFloat() };
    }catch(e){ return {err:''+e}; }
  }
};
send({kind:'ready'});
'''

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--track", type=int, default=3)     # 3 = Arctic (matches standalone demo)
    ap.add_argument("--mode", type=int, default=2)      # 2 = TimeTrial (solo, no AI interference)
    ap.add_argument("--cars", type=int, default=1)
    ap.add_argument("--car", type=int, default=0)
    ap.add_argument("--rule", type=int, default=0)
    ap.add_argument("--team", type=int, default=0)
    ap.add_argument("--hold", type=int, default=16, help="seconds to drive + sample")
    ap.add_argument("--steer", type=int, default=0, help="steer direction after --steer-at: 0=none, +1/-1")
    ap.add_argument("--steer-at", type=float, default=6.0, help="start holding steer at t>= this (s)")
    ap.add_argument("--fps", default="60")
    args = ap.parse_args()
    if not EXE.exists(): print(f"error: {EXE} not found"); return 2

    env = dict(os.environ); env["MASHED_FPS_CAP"]=str(args.fps); env["MASHED_RE_NO_AUTO_HOOK"]="1"
    dev = frida.get_local_device()
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env); pid = proc.pid
    print(f"=== capture_player_dynamics pid={pid} track={args.track} mode={args.mode} ===")
    sess = None
    for _ in range(200):
        try: sess = dev.attach(pid); break
        except Exception: time.sleep(0.1)
    if sess is None:
        print("  error: could not attach")
        try: proc.kill()
        except Exception: pass
        return 3
    def on_msg(m,d):
        if m.get("type")=="error": print("  agent error:", m.get("description")); return
        p=m.get("payload",{})
        if p.get("kind") in ("ready","err"): print("  [agent]", p.get("msg") or "ready")
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    E = scr.exports_sync

    def wait_phase(target, timeout, label):
        end=time.time()+timeout; last=None
        while time.time()<end:
            if psutil and not psutil.pid_exists(pid): print(f"  game exited ({label})"); return None
            try: ph=E.phase()
            except Exception: ph=None
            if ph!=last: print(f"    phase={ph} (waiting {label})"); last=ph
            if ph==target: return ph
            time.sleep(0.25)
        print(f"  TIMEOUT {label} (last={last})"); return None

    rows=[]; rc=1
    try:
        if wait_phase(1,40,"menu")is None: raise SystemExit
        time.sleep(0.5)
        print("  [setup]", E.setup({"track":args.track,"mode":args.mode,"cars":args.cars,
              "car":args.car,"rule":args.rule,"team":args.team,"difficulty":-1,"powerups":-1}))
        time.sleep(0.2)
        E.launch()
        if wait_phase(3,40,"race")is None: raise SystemExit
        # let spawn settle + skip the start intro/GO by pulsing the raw confirm control
        for _ in range(16):
            time.sleep(0.5); E.press(4,700)
            ci=E.sample()
            if ci.get("spawnFired",0)>0 and ci.get("grounded",0)>0: break
        print(f"  spawned={ci.get('spawnFired')} grounded={ci.get('grounded')}")
        # DRIVE: force the cooked descriptor accel (ctrl block[4]) continuously via the
        # cook-onLeave hook; steer (block[0]/[1]) after --steer-at. Keep pulsing the raw
        # confirm so the GO countdown releases (throttle is gated until game-state != 7).
        t0=time.time(); tend=t0+args.hold; steering=0
        while time.time()<tend:
            if psutil and not psutil.pid_exists(pid): print("  game exited mid-capture"); break
            E.press(4, 400)                       # raw confirm pulse (intro/GO release)
            t=time.time()-t0
            steering = args.steer if (args.steer!=0 and t>=args.steer_at) else 0
            E.drive(1, steering)                  # accel ON; steer per schedule
            ci=E.sample()
            if "err" not in ci:
                rows.append((round(t,3), round(ci["speed"],3), round(ci["velmag"],3),
                             round(ci["avx"],5), round(ci["avy"],5), round(ci["avz"],5),
                             round(ci["heading"],5), round(ci["grounded"],1), steering))
            time.sleep(0.2)
        rc=0
    except SystemExit: pass
    finally:
        try: sess.detach()
        except Exception: pass
        try:
            if (not psutil) or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass

    if rows:
        OUT.parent.mkdir(parents=True, exist_ok=True)
        with open(OUT,"w") as f:
            f.write("t,speed,velmag,avx,avy,avz,heading,grounded,steering\n")
            for r in rows: f.write(",".join(str(x) for x in r)+"\n")
        print(f"  wrote {len(rows)} samples -> {OUT}")
        # summary: top speed + spool (63% of max) + max |angvel| per axis + yaw rate from
        # heading delta over the steering window (the ground-truth turn rate).
        sp=[r[1] for r in rows]
        mx=max(sp) if sp else 0; mxv=max(r[2] for r in rows)
        thr=0.632*mx; spool=next((r[0] for r in rows if r[1]>=thr), None)
        axmax=max((abs(r[3]) for r in rows),default=0); aymax=max((abs(r[4]) for r in rows),default=0)
        azmax=max((abs(r[5]) for r in rows),default=0)
        # yaw rate from heading: max |d(heading)/dt| while steering (unwrap the +-pi seam)
        yr=0.0; prev=None
        for r in rows:
            if r[8]!=0 and prev is not None:
                dh=r[6]-prev[6]
                while dh> 3.14159265: dh-=6.28318531
                while dh<-3.14159265: dh+=6.28318531
                dt=r[0]-prev[0]
                if dt>0: yr=max(yr, abs(dh/dt))
            prev=r
        print(f"  top speed(+0x9e4)={mx:.1f}  top |vel|={mxv:.1f}  spool(63%)~{spool}s")
        print(f"  max|angvel| x={axmax:.4f} y={aymax:.4f} z={azmax:.4f}  | yawrate(from heading, steering)={yr:.4f} rad/s")
    else:
        print("  NO samples captured")
    return rc

if __name__ == "__main__":
    sys.exit(main())
