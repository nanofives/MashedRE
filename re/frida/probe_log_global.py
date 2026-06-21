# On the patched binary, confirm: (1) FUN_00496490 (log-open) now returns 0 at entry,
# (2) the value of DAT_00772fbc (0x772fbc) when FUN_00496400 (log-printf) is entered.
# If it's 0x5453 despite the open being disabled, a writer other than 0x4964bd sets it.
#
# Usage: py -3.12 re/frida/probe_log_global.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const m = Process.findModuleByName('MASHED.exe');
const G = m.base.add(0x772fbc - 0x400000);
function rva(p){ try{ const mm=Process.findModuleByAddress(p); return mm?(mm.name+'+0x'+p.sub(mm.base).toString(16)):(''+p); }catch(e){ return ''+p; } }
let n = 0;
// log-printf entry: read DAT_00772fbc
Interceptor.attach(m.base.add(0x496400 - 0x400000), { onEnter(){
  n++;
  let v=-1; try{ v = G.readU32(); }catch(e){}
  if (n <= 8 || (v>>>0) < 0x10000) send({kind:'printf', n:n, dat:'0x'+(v>>>0).toString(16), caller:rva(this.returnAddress)});
}});
// patched log-open: confirm it returns at entry
Interceptor.attach(m.base.add(0x496490 - 0x400000), {
  onEnter(){ send({kind:'open', msg:'FUN_00496490 entered'}); },
  onLeave(r){ send({kind:'open', msg:'FUN_00496490 returned eax=0x'+r.toUInt32().toString(16)}); }
});
send({kind:'ready', msg:'DAT_00772fbc @'+G});
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
        elif k == "open": print("  [open]", p["msg"])
        elif k == "printf": print(f"  [log-printf #{p['n']}] DAT_00772fbc={p['dat']}  caller={p['caller']}")
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  process EXITED"); break
        time.sleep(0.25)
    try: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
