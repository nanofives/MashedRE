# Menu lifecycle observer — OBSERVATION ONLY (no writes, no input).
#
# Hooks FUN_0043d2a0 (submenu push/pop builder) and logs (menu_id, mode, t_ms) for every
# menu transition, plus periodic samples of the frontend phase (DAT_0067eca4) and menu-stack
# depth (DAT_0067e9f8), and per-handler call counts. Reveals the exact auto-navigation script
# the frontend runs (incl. the attract-demo takeover) and whether/when the menu is idle.
#
# mode: 0=push submenu, 1=pop-2-levels, 2=reuse.  See frontend_input_nav_trace/TRACE.md.
#
# Usage: py -3.12 re/frida/menu_nav_observe.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
const RVA_PUSHPOP  = 0x0043d2a0;   // FUN_0043d2a0(menu_id, mode)
const RVA_TICK     = 0x0043dfd0;   // FUN_0043dfd0 frontend tick
const RVA_PHASE    = 0x0067eca4;   // DAT_0067eca4 frontend phase (1 fade-in,2 fade-out,3 active)
const RVA_DEPTH    = 0x0067e9f8;   // menu-stack depth
const SUSPECTS = [0x0042aff0,0x0042b180,0x0042b310,0x0042b770,0x0042ee00,0x0042ebe0,0x00430a10];
let t0 = Date.now();
const trans = [];     // menu transitions
const cnt = {};
let lastPhase = null, lastDepth = null;
const phaseLog = [];

function abs(rva){ return ptr(rva + DELTA); }
rpc.exports = {
  trans: function(){ return trans; },
  counts: function(){ return cnt; },
  phaselog: function(){ return phaseLog; }
};

(function(){
  try {
    const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMGBASE;
    send({kind:'info', msg:'base='+m.base});
  } catch(e){ send({kind:'err', msg:'base '+e}); return; }

  // push/pop builder: log (menu_id, mode, t)
  try {
    Interceptor.attach(abs(RVA_PUSHPOP), { onEnter(a){
      // args on stack: [esp+4]=menu_id [esp+8]=mode  (callee-arg read via this.context.esp)
      let id=-1, mode=-1;
      try { const sp = this.context.esp; id = sp.add(4).readS32(); mode = sp.add(8).readS32(); } catch(e){}
      const depth = (function(){ try { return abs(RVA_DEPTH).readS32(); } catch(e){ return -999; } })();
      trans.push({t: Date.now()-t0, id:id, mode:mode, depth:depth});
    }});
  } catch(e){ send({kind:'err', msg:'pushpop '+e}); }

  // count tick + suspects
  try { cnt['tick_0043dfd0']=0; Interceptor.attach(abs(RVA_TICK),{onEnter(){cnt['tick_0043dfd0']++;}}); } catch(e){}
  SUSPECTS.forEach(function(r){ const k='0x'+r.toString(16); cnt[k]=0;
    try{ Interceptor.attach(abs(r),{onEnter(){cnt[k]++;}}); }catch(e){ cnt[k]=-1; } });

  // sample phase+depth every 250ms via a timer
  setInterval(function(){
    let ph=-999, dp=-999;
    try { ph = abs(RVA_PHASE).readS32(); } catch(e){}
    try { dp = abs(RVA_DEPTH).readS32(); } catch(e){}
    if (ph!==lastPhase || dp!==lastDepth){ phaseLog.push({t:Date.now()-t0, phase:ph, depth:dp}); lastPhase=ph; lastDepth=dp; }
  }, 250);

  send({kind:'info', msg:'observer armed'});
})();
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 40
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "info": print("  [obs]", p["msg"])
        elif p.get("kind") == "err": print("  [err]", p["msg"])
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  resumed; observing menu lifecycle for {seconds}s (no input)")
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        time.sleep(0.5)
    trans = []; counts = {}; phlog = []
    try: trans = scr.exports_sync.trans()
    except Exception: pass
    try: counts = scr.exports_sync.counts()
    except Exception: pass
    try: phlog = scr.exports_sync.phaselog()
    except Exception: pass
    print(f"\n  === phase/depth timeline (phase: 1=fadein 2=fadeout 3=active) ===")
    for e in phlog[:80]:
        print(f"    t={e['t']:>6}ms phase={e['phase']} depth={e['depth']}")
    print(f"\n  === menu transitions (FUN_0043d2a0: id, mode 0=push/1=pop2/2=reuse) ===")
    for e in trans[:80]:
        print(f"    t={e['t']:>6}ms id=0x{e['id']&0xffffffff:x} mode={e['mode']} depth={e['depth']}")
    print(f"\n  === handler counts ===\n    {counts}")
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
