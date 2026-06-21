# Hook MASHED's generic "lock object" helper (0x4a5f1b) during boot to catch the
# call that enters a critical section on a garbage object pointer (~0x5453), and log
# its caller. Pinpoints the subsystem whose object/global is still NULL at lock time
# (the real boot-crash cause; NOT heap corruption).
#
# Usage: py -3.12 re/frida/trace_lock_helper.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
LOCK_HELPER_RVA = 0x4a5f1b

AGENT = r'''
'use strict';
const m = Process.findModuleByName('MASHED.exe');
const helper = m.base.add(__RVA__ - 0x400000);
function rva(p){ try{ const mm=Process.findModuleByAddress(p); return mm?(mm.name+'+0x'+p.sub(mm.base).toString(16)):(''+p); }catch(e){ return ''+p; } }
const ring = [];
let total = 0;
Interceptor.attach(helper, {
  onEnter(args){
    total++;
    const esp = this.context.esp;
    let arg = -1, caller = ptr(0);
    try { arg = esp.add(4).readU32(); } catch(e){}
    try { caller = esp.readPointer(); } catch(e){}
    ring.push('#'+total+' arg=0x'+(arg>>>0).toString(16)+'  caller='+rva(caller));
    if (ring.length > 30) ring.shift();
  }
});
Process.setExceptionHandler(function(d){
  if (d.type === 'system') return false;   // skip OutputDebugString noise
  let regs=''; try{ regs='eip='+rva(d.context.eip)+' esi='+d.context.esi+' ecx='+d.context.ecx; }catch(e){}
  send({kind:'av', etype:d.type, at:rva(d.context.eip), regs:regs,
        bt: Thread.backtrace(d.context, Backtracer.ACCURATE).slice(0,14).map(rva),
        ring: ring.slice(-14)});
  return false;
});
rpc.exports = { stat:function(){ return {total:total, ring:ring.slice(-20)}; } };
send({kind:'ready', msg:'lock helper hooked @'+helper});
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 15
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error":
            print("agent error:", m.get("description")); return
        p = m.get("payload", {}); k = p.get("kind")
        if k == "ready": print("  [ready]", p["msg"])
        elif k == "av":
            print(f"\n=== ACCESS VIOLATION: {p['etype']} at {p['at']} ===")
            print(f"  {p['regs']}")
            print("  backtrace:")
            for f in p["bt"]: print("    " + f)
            print("  recent lock-helper calls:")
            for r in p["ring"]: print("    " + r)
    scr = sess.create_script(AGENT.replace("__RVA__", hex(LOCK_HELPER_RVA))); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  watching {seconds}s...")
    try: import psutil
    except ImportError: psutil = None
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid):
            print("  process EXITED")
            break
        time.sleep(0.25)
    try:
        st = scr.exports_sync.stat()
        print(f"\n  total lock-helper calls: {st['total']}")
        print("  last lock calls:")
        for r in st["ring"]: print("    " + r)
    except Exception as e:
        print("  (stat unavailable:", e, ")")
    try: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
