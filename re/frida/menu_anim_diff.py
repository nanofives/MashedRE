# Nav-driven diff-original for FUN_004325c0 (menu slide-anim tick) — the
# harness extension ratified 2026-06-10: settled-state menu diffs are
# degenerate (menu-attach ceiling), so each A/B sample REPOPULATES the live
# record table mid-animation via the original FUN_0043d2a0(current, reload)
# and runs original-vs-twin ON THE GAME THREAD with snapshot/restore:
#
#   per sample (inside the replaced 0x004325c0, i.e. the game's own call):
#     1. FUN_0043d2a0(cur_screen, 2)   -> fresh mid-anim records
#     2. S0 = table bytes              (30 x 0x34 @ 0x00898ac0)
#     3. ra = original tick; S1 = table
#     4. restore S0
#     5. rb = MenuAnimTickTwin (.asi); S2 = table
#     6. GREEN iff ra==rb && S1==S2; leave S1 (world as-if-original)
#
# Usage: py -3.12 re/frida/menu_anim_diff.py   (expects fresh build deployed)
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
ASI = ORIG / "mashed_re_dev.asi"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const TBL=0x00898ac0, TBL_LEN=30*0x34;
const RVA_TICK=0x004325c0, RVA_NAV=0x0043d2a0;
const RVA_DEPTH=0x0067e9f8, RVA_SCREEN=0x0067ed7c, RVA_PHASE=0x0067eca4;
let twin=null, nav=null, results=[], want=0, origPtr=null;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(asiPath){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    const asi=Module.load(asiPath);
    const tw=asi.findExportByName('MenuAnimTickTwin');
    if(!tw) throw new Error('twin export missing');
    twin=new NativeFunction(tw,'int',[]);
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push1:function(){ nav(1,0); return abs(RVA_DEPTH).readS32(); },
  arm:function(n){
    want=n; results=[];
    origPtr=Interceptor.replaceFast(abs(RVA_TICK), new NativeCallback(function(){
      const orig=new NativeFunction(origPtr,'int',[]);
      if(results.length>=want) return orig();
      const depth=abs(RVA_DEPTH).readS32();
      const scr=abs(RVA_SCREEN+(depth-1)*0x40).readS32();
      nav(scr,2);                                  // reload -> mid-anim records
      const s0=abs(TBL).readByteArray(TBL_LEN);
      const ra=orig();
      const s1=abs(TBL).readByteArray(TBL_LEN);
      abs(TBL).writeByteArray(s0);
      const rb=twin();
      const s2=abs(TBL).readByteArray(TBL_LEN);
      const a=new Uint8Array(s1), b=new Uint8Array(s2);
      let diff=-1;
      for(let i=0;i<TBL_LEN;i++){ if(a[i]!==b[i]){ diff=i; break; } }
      results.push({ra:ra, rb:rb, diff:diff, scr:scr});
      abs(TBL).writeByteArray(s1);                 // leave as-if-original
      return ra;
    },'int',[]));
    return 1;
  },
  done:function(){ return results.length; },
  report:function(){
    Interceptor.revert(abs(RVA_TICK));
    return JSON.stringify(results);
  }
};
'''


def main():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init(str(ASI))
    dev.resume(pid)
    E = scr.exports_sync

    print("waiting for menu...")
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    print("push screen 1 (coverage: push-transition records), depth ->",
          E.push1())
    print("arming 20 nav-driven A/B samples...")
    E.arm(20)
    end = time.time() + 20
    while time.time() < end and E.done() < 20:
        time.sleep(0.3)
    rows = json.loads(E.report())
    n_green = sum(1 for r in rows if r["diff"] == -1 and r["ra"] == r["rb"])
    print(f"samples={len(rows)} GREEN={n_green}")
    for r in rows[:6]:
        print(" ", r)
    out = ROOT / "log" / "diff_menu_anim_tick_navdriven.json"
    out.write_text(json.dumps({"samples": rows, "green": n_green}, indent=1))
    print("->", out)
    try: dev.kill(pid)
    except Exception: pass
    return 0 if rows and n_green == len(rows) else 1


if __name__ == "__main__":
    sys.exit(main())
