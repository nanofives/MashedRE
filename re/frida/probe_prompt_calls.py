# Probe: when does FUN_00432b30 (prompt strip) actually fire, and with what
# args (EAX=mode, [esp+4]=key, [esp+8]=cmp, ESI=idx ptr)? Logs every call with
# a returnAddress, while the controller pushes/reloads screens 1/8/19.
# Usage: py -3.12 re/frida/probe_prompt_calls.py
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_STRIP=0x00432b30;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
let calls=[], nav=null;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    Interceptor.attach(abs(RVA_STRIP), { onEnter(args){
      if(calls.length>400) return;
      calls.push({mode:this.context.eax.toInt32(),
                  key:this.context.esp.add(4).readS32(),
                  cmp:this.context.esp.add(8).readS32(),
                  esi:this.context.esi.toUInt32().toString(16),
                  idx:this.context.esi.readS32(),
                  ret:this.returnAddress.sub(DELTA).toUInt32().toString(16),
                  depth:abs(RVA_DEPTH).readS32()});
    }});
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); return abs(RVA_DEPTH).readS32(); },
  reload:function(scr){ nav(scr,2); return calls.length; },
  pop:function(){ nav(0,1); return abs(RVA_DEPTH).readS32(); },
  count:function(){ return calls.length; },
  mark:function(label){ calls.push({MARK:label}); return calls.length; },
  report:function(){ return JSON.stringify(calls); }
};
'''


def main():
    env = dict(os.environ)
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    print("waiting for menu...")
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    print("boot calls so far:", E.count())
    E.mark("push1"); print("push 1 ->", E.push(1)); time.sleep(1.0)
    print(" calls:", E.count())
    E.mark("reload1"); E.reload(1); time.sleep(0.5)
    print(" after reload1:", E.count())
    E.mark("push8"); print("push 8 ->", E.push(8)); time.sleep(1.0)
    print(" calls:", E.count())
    E.mark("reload8"); E.reload(8); time.sleep(0.5)
    print(" after reload8:", E.count())
    E.mark("push19"); print("push 19 ->", E.push(19)); time.sleep(1.0)
    print(" calls:", E.count())
    E.mark("pop_a"); print("pop ->", E.pop()); time.sleep(0.8)
    E.mark("pop_b"); print("pop ->", E.pop()); time.sleep(0.8)
    E.mark("pop_c"); print("pop ->", E.pop()); time.sleep(0.8)
    print(" after pops:", E.count())
    E.mark("idle2s"); time.sleep(2.0)
    print(" after 2s idle:", E.count())
    rows = json.loads(E.report())
    out = ROOT / "log" / "probe_prompt_calls.json"
    out.write_text(json.dumps(rows, indent=1))
    print("->", out, f"({len(rows)} entries)")
    # condensed view
    for r in rows[:80]:
        print("  ", r)
    try: dev.kill(pid)
    except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
