# Catch the bad EnterCriticalSection on the (log-patched) binary: hook ntdll
# RtlEnterCriticalSection; when the CS pointer is unreadable (garbage ~0x5473),
# dump a backtrace from the intact entry stack (reliable, unlike at the AV) to
# identify the real caller chain of the NEXT boot crash.
#
# Usage: py -3.12 re/frida/probe_badenter.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
function rva(p){ try{ const m=Process.findModuleByAddress(p); return m?(m.name+'+0x'+p.sub(m.base).toString(16)):(''+p); }catch(e){ return ''+p; } }
const ntdll = Process.findModuleByName('ntdll.dll');
let done = 0;
['RtlEnterCriticalSection','RtlpEnterCriticalSectionContended'].forEach(function(n){
  let a=null; try{ a = ntdll.findExportByName(n); }catch(e){}
  if (!a) return;
  Interceptor.attach(a, { onEnter(args){
    let cs;
    try { cs = args[0]; } catch(e){ return; }
    let bad=false; try { cs.add(4).readU32(); bad = (cs.toUInt32() < 0x10000); } catch(e){ bad=true; }
    if (bad && done < 4){
      done++;
      send({kind:'bad', fn:n, cs:''+cs, bt: Thread.backtrace(this.context, Backtracer.ACCURATE).slice(0,16).map(rva)});
    }
  }});
});
send({kind:'ready'});
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
        if k == "ready": print("  [ready]")
        elif k == "bad":
            print(f"\n=== bad EnterCriticalSection: {p['fn']}  cs={p['cs']} ===")
            for f in p["bt"]: print("    " + f)
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  process EXITED"); time.sleep(0.3); break
        time.sleep(0.25)
    try: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
