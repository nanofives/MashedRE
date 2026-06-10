# R6-faithful: harvest the original's handling telemetry from a LIVE race.
#
# statenav.py sibling. Drives the original to a Quick Battle round (same
# navigation flow), then samples the vehicle struct each VehicleControlUpdate
# (0x00470670) tick via Interceptor onEnter (EAX = vehicle base, per
# re/analysis/vehicle_update_d2/00470670.md):
#   +0x190 max-speed constant, +0x1a8 throttle force, +0x9b0..b8 velocity,
#   +0xb0c lateral/slide measure, param_3[0]/[1] = throttle/reverse inputs.
# ~200 calls/s (4 vehicles x 50Hz) — well under the hot-path danger zone.
# Output: log/handling_telemetry.jsonl + fitted constants printed.
#
# Usage: py -3.12 re/frida/harvest_handling.py [--secs 40]
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
OUT = ROOT / "log" / "handling_telemetry.jsonl"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
const RVA_VCU=0x00470670;
let pressCtrl=-1, pressUntil=0;
const S=[]; let t0=0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG; t0=Date.now();
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }});
    Interceptor.attach(abs(RVA_VCU),{ onEnter(args){
      try{
        const v=this.context.eax; const sp=this.context.esp;
        const ctl=sp.add(12).readPointer();   // param_3 (cdecl: ret,p1,p2,p3)
        if(S.length<20000 && (S.length===0 || true)){
          S.push({t:Date.now()-t0,
            thr:ctl.readU8(), rev:ctl.add(1).readU8(),
            vx:v.add(0x9b0).readFloat(), vy:v.add(0x9b4).readFloat(),
            vz:v.add(0x9b8).readFloat(),
            ms:v.add(0x190).readFloat(), tf:v.add(0x1a8).readFloat(),
            sl:v.add(0xb0c).readFloat(),
            id:v.toUInt32()});
        }
      }catch(e){}
    }});
    return DELTA; },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  depth:function(){ try{return abs(RVA_DEPTH).readS32();}catch(e){return -999;} },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  setsel:function(vv){ try{const d=abs(RVA_DEPTH).readS32(); abs(0x0067ed80+(d-1)*0x40).writeS32(vv); return 1;}catch(e){return 0;} },
  drain:function(){ const out=S.slice(); S.length=0; return out; }
};
send({kind:'ready'});
'''


def main():
    secs = int(sys.argv[sys.argv.index("--secs") + 1]) if "--secs" in sys.argv else 40
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    def wait(pred, timeout, what):
        end = time.time() + timeout
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        print(f"timeout: {what} depth={E.depth()} phase={E.phase()}")
        return False

    def press(c, ms=180):
        E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)

    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target: return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0, f"depth{target}"): return True
        return E.depth() >= target

    print("booting...")
    wait(lambda: E.phase() == 3 and E.depth() >= 1, 20, "title")
    time.sleep(1.0)
    confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)   # GTS + modal
    confirm_to(3)                                # Single Player (sel 0)
    E.setsel(1); time.sleep(0.3)                 # Quick Battle
    confirm_to(4, 4)                             # colour select
    confirm_to(5, 4)                             # track select
    press(4); time.sleep(1.5)                    # confirm track -> game mode
    for _ in range(5):                           # start the round
        if E.phase() != 3: break
        press(4); time.sleep(1.5)
    print(f"in race? phase={E.phase()}")
    E.drain()                                    # discard menu-phase samples

    # drive: alternate full-throttle bursts and coast so the series contains
    # accel, top-speed and decay segments (ctrl candidates: statenav's set)
    end = time.time() + secs
    k = 0
    while time.time() < end:
        for c in (0, 4, 1):                      # find the throttle ctrl
            E.press(c, 1400)
            time.sleep(1.5)
        time.sleep(1.5)                          # coast
        k += 1
    rows = E.drain()
    OUT.parent.mkdir(parents=True, exist_ok=True)
    with open(OUT, "w", encoding="utf-8") as f:
        for r in rows:
            f.write(json.dumps(r) + "\n")
    print(f"samples={len(rows)} -> {OUT}")

    # ---- fit ----
    import collections
    by = collections.defaultdict(list)
    for r in rows: by[r["id"]].append(r)
    big = max(by.values(), key=len)
    sp = [ (r["t"], (r["vx"]**2 + r["vy"]**2 + r["vz"]**2) ** 0.5,
            r["thr"], r["ms"], r["tf"], r["sl"]) for r in big ]
    vmax = max(s[1] for s in sp)
    ms_const = sp[0][3]
    # accel slope: largest dv/dt while throttle on and v < 0.5 vmax
    best_a = 0.0
    for i in range(1, len(sp)):
        dt = (sp[i][0] - sp[i-1][0]) / 1000.0
        if dt <= 0: continue
        if sp[i][2] and sp[i][1] < vmax * 0.5:
            a = (sp[i][1] - sp[i-1][1]) / dt
            if 0 < a < 200 and a > best_a: best_a = a
    # drag: decay rate while throttle off and v > 0.3 vmax: v' = -kv
    ks = []
    for i in range(1, len(sp)):
        dt = (sp[i][0] - sp[i-1][0]) / 1000.0
        if dt <= 0 or sp[i][2] or sp[i-1][1] < vmax * 0.3: continue
        dv = sp[i][1] - sp[i-1][1]
        if dv < 0:
            ks.append(-dv / (sp[i-1][1] * dt))
    drag = sorted(ks)[len(ks)//2] if ks else 0.0
    print(f"vehicle samples={len(big)} vmax={vmax:.3f} u/s  "
          f"maxspeed_const(+0x190)={ms_const:.4f}  accel~{best_a:.2f} u/s^2  "
          f"drag~{drag:.3f} /s")
    try: dev.kill(pid)
    except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
