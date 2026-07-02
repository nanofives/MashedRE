# CAPTURE RELIGHT PARITY (2026-07-02, WS-E vehicle lighting acceptance) — warp the
# ORIGINAL MASHED.exe into a race (scenario_launch plumbing, same defaults as
# capture_player_dynamics.py: Arctic TimeTrial solo car 0 = the standalone race
# demo's car), skip the GO countdown, then hold accel + steer so the player car
# does the same donut as the standalone's MASHED_DEMO_DRIVE. Every shot of the
# window is tagged with the player heading (atan2 of fwd +0x9d4/+0x9dc on the
# player record 0x008815a0) into shots.json, so a standalone capture can be
# matched to the original shot with the closest heading and compared with
# re/tools/imgdiff.py. This is the original-side half of the WS-E lighting
# acceptance gate (re/prior_art/notes/rw_lighting_research_2026-07.md §10).
#
# Spawn+attach (NEVER frida.spawn). Kills ONLY the pid it launches.
# Usage: py -3.12 re/frida/capture_relight_parity.py [--track 3] [--hold 14]
import ctypes, json, os, sys, time, argparse, subprocess
from ctypes import wintypes
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
# Worktrees carry no original/ junction: resolve the game install via
# MASHED_ROOT (same convention as the standalone's asset loader) or the
# .worktrees/<name> -> main-repo layout. Evidence still lands in THIS tree.
GAME = ROOT
if not (GAME / "original" / "MASHED.exe").exists():
    if os.environ.get("MASHED_ROOT"):
        GAME = Path(os.environ["MASHED_ROOT"])
    elif ROOT.parent.name == ".worktrees":
        GAME = ROOT.parent.parent
EXE  = GAME / "original" / "MASHED.exe"
OUTD = ROOT / "verify" / "ws_e_lighting" / "orig_parity"

# --- backbuffer shot via the d3d9 shim's on-demand dump (MASHED_ORIG_BBDUMP_REQ,
# --- same mechanism re/parity/sweep.py uses; PrintWindow yields a white frame
# --- when DWM is not composing the window, e.g. another MASHED overlaps it) ----
REQF = OUTD / "orig_bbdump.req"

def shoot(path, timeout=6.0):
    try:
        target = Path(path).resolve()
        REQF.write_text(str(target) + "\n")
        end = time.time() + timeout
        # the shim polls the req file each Present, dumps, then deletes it
        while time.time() < end and REQF.exists():
            time.sleep(0.1)
        return target.exists()
    except Exception as e:
        print("  [shot] err", e); return False

# --- frida agent: identical warp/drive plumbing to capture_player_dynamics.py --
AGENT = r'''
'use strict';
const IMG = 0x400000;
let M = null;
function modBase(){ if(!M) M = Process.findModuleByName('MASHED.exe'); return M ? M.base : null; }
function ga(addr){ const b = modBase(); return b ? b.add(addr - IMG) : null; }

const PHASE=0x00771968, TRACK_ENG=0x0063ba7c, TRACK_MENU=0x0067f17c, MODE=0x0067e9fc,
      RULE=0x007f0fd0, CAR_P0=0x0067ea98, DIFFICULTY=0x0067ea7c, POWERUPS=0x0067ea80,
      TEAM=0x0067ea64, CARREC=0x008815a0, SPAWN_RVA=0x0046b540,
      ACTIVATE=0x0040e480, RES_RVA=0x00497310;

let spawnFired=0, spawnArmed=false;
function armSpawn(){ if(spawnArmed)return; spawnArmed=true;
  try{ Interceptor.attach(ga(SPAWN_RVA),{onEnter(){spawnFired++;}});}catch(e){send({kind:'err',msg:'armSpawn '+e});}}

let pc1=-1,pu1=0, inputArmed=false;
function armInput(){ if(inputArmed)return; inputArmed=true;
  try{ Interceptor.attach(ga(RES_RVA),{
    onEnter(){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
    onLeave(ret){ if(this.p===0 && this.c===pc1 && Date.now()<pu1) ret.replace(ptr(0xff)); }
  });}catch(e){send({kind:'err',msg:'armInput '+e});}}

const COOK_RVA = 0x00496530;
const BLK0 = 0x007f1038;            // player-0 cooked descriptor block
let gAccel=0, gSteer=0, cookArmed=false;
function armCook(){ if(cookArmed)return; cookArmed=true;
  try{ Interceptor.attach(ga(COOK_RVA),{ onLeave(){ const b=ga(BLK0); if(!b)return;
    b.add(4).writeU8(gAccel?0xff:0);           // accel = block[4]
    // steer = descriptor bytes [0]/[1], mutually exclusive, 0..255 magnitude —
    // the channel A4 FUN_00470670 reads (WS-A8-STEER, VehiclePhysicsRun.cpp;
    // AI writer FUN_00416250 uses the same bytes). The [2]/[3]+[0xe]/[0xf]
    // scheme in capture_player_dynamics.py does NOT turn the car (verified
    // this session: heading stayed 1.57 for 8 s under full "steer").
    b.add(0).writeU8(gSteer>0?0xff:0);
    b.add(1).writeU8(gSteer<0?0xff:0);
  }});}catch(e){send({kind:'err',msg:'armCook '+e});}}

// Log every file the game opens under VEHICLES\ — identifies the player-car
// model piz ground-truth (the CAR_P0 index semantics are unconfirmed).
let vehFiles=[], fileLogArmed=false;
function armFileLog(){          // armed from setup() (menu settled), NOT at
  if (fileLogArmed) return;     // attach — hooking CreateFile mid-boot is a
  fileLogArmed = true;          // crash risk (boot AV seen 2026-07-02).
  const k32 = Process.getModuleByName('kernel32.dll');
  for (const nm of ['CreateFileA','CreateFileW']){
    const f = k32.findExportByName(nm);   // Frida 17: module-scoped lookup
    if (!f) continue;
    const wide = nm.endsWith('W');
    Interceptor.attach(f, { onEnter(args){
      try{
        const p = wide ? args[0].readUtf16String() : args[0].readAnsiString();
        if (p && p.toUpperCase().indexOf('VEHICLES') >= 0) vehFiles.push(p);
      }catch(e){}
    }});
  }
}

rpc.exports = {
  vehfiles:function(){ return vehFiles; },
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
      armSpawn(); armInput(); armCook(); armFileLog(); return 'ok';
    }catch(e){return 'ERR '+e;}
  },
  launch:function(){ try{ ga(PHASE).writeU8(2); return 1; }catch(e){return 'ERR '+e;} },
  press:function(c,ms){ pc1=c; pu1=Date.now()+ms; return 1; },
  drive:function(accel,steer){ gAccel=accel; gSteer=steer; return 1; },
  sample:function(){
    try{ const r=ga(CARREC);
      const fx=r.add(0x9d4).readFloat(), fz=r.add(0x9dc).readFloat();
      return { spawnFired:spawnFired, heading:Math.atan2(fz,fx),
        grounded:r.add(0x9e0).readFloat(), speed:r.add(0x9e4).readFloat() };
    }catch(e){ return {err:''+e}; }
  }
};
send({kind:'ready'});
'''

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--track", type=int, default=3)     # 3 = Arctic (standalone demo track)
    ap.add_argument("--mode", type=int, default=2)      # 2 = TimeTrial (solo, no AI in frame)
    ap.add_argument("--cars", type=int, default=1)
    ap.add_argument("--car", type=int, default=0)       # car 0 = Advantage (standalone default)
    ap.add_argument("--rule", type=int, default=0)
    ap.add_argument("--team", type=int, default=0)
    ap.add_argument("--hold", type=float, default=20.0, help="donut drive seconds")
    ap.add_argument("--shot-every", type=float, default=0.6, help="seconds between donut shots")
    ap.add_argument("--speed-cap", type=float, default=1200.0,
                    help="pulse accel off above this speed (+0x9e4 units) so the car "
                         "pirouettes instead of launching down the straight")
    ap.add_argument("--fps", default="60")
    args = ap.parse_args()
    if not EXE.exists(): print(f"error: {EXE} not found"); return 2
    OUTD.mkdir(parents=True, exist_ok=True)
    try: REQF.unlink()
    except OSError: pass

    env = dict(os.environ); env["MASHED_FPS_CAP"]=str(args.fps); env["MASHED_RE_NO_AUTO_HOOK"]="1"
    env["MASHED_ORIG_BBDUMP_REQ"] = str(REQF.resolve())
    dev = frida.get_local_device()
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env); pid = proc.pid
    print(f"=== capture_relight_parity pid={pid} track={args.track} mode={args.mode} car={args.car} ===")
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
            try: ph=E.phase()
            except Exception: ph=None
            if ph!=last: print(f"    phase={ph} (waiting {label})"); last=ph
            if ph==target: return ph
            time.sleep(0.25)
        print(f"  TIMEOUT {label} (last={last})"); return None

    shots=[]; rc=1
    try:
        if wait_phase(1,40,"menu") is None: raise SystemExit
        time.sleep(0.5)
        print("  [setup]", E.setup({"track":args.track,"mode":args.mode,"cars":args.cars,
              "car":args.car,"rule":args.rule,"team":args.team,"difficulty":-1,"powerups":-1}))
        time.sleep(0.2)
        E.launch()
        if wait_phase(3,40,"race") is None: raise SystemExit
        ci={}
        for _ in range(16):
            time.sleep(0.5); E.press(4,700)
            ci=E.sample()
            if ci.get("spawnFired",0)>0 and ci.get("grounded",0)>0: break
        print(f"  spawned={ci.get('spawnFired')} grounded={ci.get('grounded')}")
        # settle at the grid (no input), then grid reference shot at spawn heading
        time.sleep(1.5)
        ci=E.sample()
        p = OUTD/"orig_grid.bmp"
        if shoot(p):
            shots.append({"file":p.name,"t":0.0,"heading":round(ci.get("heading",0),5),
                          "speed":round(ci.get("speed",0),3)})
            print(f"  [shot] {p.name} heading={ci.get('heading'):.5f}")
        # donut: held steer + SPEED-CAPPED accel (full accel launched the car down
        # the straight into a crash on the first attempt); shot every --shot-every s,
        # each tagged with the live heading
        t0=time.time(); tend=t0+args.hold; nexts=t0+1.0; idx=0
        while time.time()<tend:
            E.press(4, 400)                  # keep GO/countdown released
            ci=E.sample()
            accel = 1 if ci.get("speed", 0) < args.speed_cap else 0
            E.drive(accel, 1)
            if time.time()>=nexts:
                idx+=1; ci=E.sample()
                p = OUTD/f"orig_donut_{idx:02d}.bmp"
                if "err" not in ci and shoot(p):
                    shots.append({"file":p.name,"t":round(time.time()-t0,2),
                                  "heading":round(ci["heading"],5),
                                  "speed":round(ci.get("speed",0),3)})
                    print(f"  [shot] {p.name} t={shots[-1]['t']} heading={ci['heading']:.5f} speed={ci.get('speed',0):.0f}")
                nexts=time.time()+args.shot_every
            time.sleep(0.1)
        E.drive(0,0)
        try:
            vf = E.vehfiles()
            seen = sorted(set(vf))
            print(f"  VEHICLES files opened ({len(vf)} opens):")
            for v in seen: print(f"    {v}")
        except Exception as e:
            print("  vehfiles err", e)
        rc=0
    except SystemExit: pass
    finally:
        try: sess.detach()
        except Exception: pass
        try:
            if (not psutil) or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass

    if shots:
        with open(OUTD/"shots.json","w") as f: json.dump(shots, f, indent=1)
        print(f"  wrote {len(shots)} shots -> {OUTD}\\shots.json")
    else:
        print("  NO shots captured")
    return rc

if __name__ == "__main__":
    sys.exit(main())
