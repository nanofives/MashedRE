# Boot the ORIGINAL to the menu and hold it open with an rpc nav controller,
# so an outside orchestrator can push/pop screens and take window screenshots
# between steps (side-by-side parity captures vs the standalone).
#
# Writes one line per state change to log/orig_nav_hold.state so the
# orchestrator can poll readiness:  READY <pid> / PUSH <scr> <depth> / ...
# Navigation is driven by a step file: log/orig_nav_hold.cmd — each line is
#   push <scr> | pop | reload | quit
# appended by the orchestrator; this script tails and executes them.
#
# Usage: py -3.12 re/frida/orig_nav_hold.py  (runs until 'quit' or 180s)
import os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
STATE = ROOT / "log" / "orig_nav_hold.state"
CMD   = ROOT / "log" / "orig_nav_hold.cmd"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
let nav=null;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  depth:function(){ return abs(RVA_DEPTH).readS32(); },
  push:function(scr){ nav(scr,0); return abs(RVA_DEPTH).readS32(); },
  pop:function(){ nav(0,1); return abs(RVA_DEPTH).readS32(); },
  reload:function(scr){ nav(scr,2); return abs(RVA_DEPTH).readS32(); }
};
'''


def emit(line):
    with STATE.open("a", encoding="utf-8") as f:
        f.write(line + "\n")
    print(line, flush=True)


def main():
    STATE.parent.mkdir(parents=True, exist_ok=True)
    STATE.write_text("", encoding="utf-8")
    CMD.write_text("", encoding="utf-8")
    dev = frida.get_local_device()
    # Hooks OFF: a bare spawn lets the dinput8 loader install the FULL hook
    # set, which has a known full-install instability — reference captures
    # must be stock behavior anyway.
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    end = time.time() + 30
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    emit(f"READY {pid}")

    seen = 0
    deadline = time.time() + 600
    while time.time() < deadline:
        lines = CMD.read_text(encoding="utf-8").splitlines()
        for ln in lines[seen:]:
            seen += 1
            t = ln.strip().split()
            if not t:
                continue
            try:
                if t[0] == "push":
                    emit(f"PUSH {t[1]} {E.push(int(t[1]))}")
                elif t[0] == "pop":
                    emit(f"POP {E.pop()}")
                elif t[0] == "reload":
                    emit(f"RELOAD {E.reload(int(t[1]))}")
                elif t[0] == "quit":
                    emit("QUIT")
                    try: dev.kill(pid)
                    except Exception: pass
                    return 0
            except Exception as e:
                emit(f"ERR {e}")
        time.sleep(0.25)
    try: dev.kill(pid)
    except Exception: pass
    emit("TIMEOUT")
    return 0


if __name__ == "__main__":
    sys.exit(main())
