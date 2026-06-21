# Disassemble MASHED.exe live at given RVAs and, for absolute [mem] operands, read
# the current dword there. Used to see what pointer the CRT pushes to
# EnterCriticalSection at the boot crash site (0x4a5f49 caller).
#
# Usage: py -3.12 re/frida/disasm_at.py 0x4a5f20:0x4a5f4a 0x496430:0x49644c
import os, sys, re
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

RANGES = [a for a in sys.argv[1:] if ":" in a] or ["0x4a5f20:0x4a5f4a", "0x496430:0x49644c"]

AGENT = r'''
'use strict';
const RANGES = __RANGES__;
const m = Process.findModuleByName('MASHED.exe');
const out = [];
RANGES.forEach(function(r){
  const lo = ptr(r[0]), hi = ptr(r[1]);
  out.push('--- '+r[0]+' .. '+r[1]+'  (MASHED base '+m.base+') ---');
  let a = m.base.add(ptr(r[0]).sub(0x400000));
  const end = m.base.add(ptr(r[1]).sub(0x400000));
  let guard = 0;
  while (a.compare(end) < 0 && guard++ < 64){
    let line, nxt;
    try { const ins = Instruction.parse(a); line = ins.toString(); nxt = ins.next; }
    catch(e){ line = '<'+e+'>'; nxt = a.add(1); }
    const rva = a.sub(m.base).add(0x400000);
    let extra = '';
    // resolve absolute [0xXXXXXXXX] operands -> read dword
    const mm = /\[(0x[0-9a-f]+)\]/.exec(line);
    if (mm){
      try { const p = ptr(mm[1]); const v = p.readU32();
            extra = '   ; ['+mm[1]+'] = 0x'+v.toString(16); } catch(e){ extra=' ; (read fail)'; }
    }
    out.push('  0x'+rva.toString(16)+'  '+line+extra);
    a = nxt;
  }
});
send({lines: out});
'''

def main():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    done = {"v": False}
    def on_msg(m, d):
        if m.get("type") == "error":
            print("agent error:", m.get("description")); done["v"]=True; return
        for line in m.get("payload", {}).get("lines", []):
            print(line)
        done["v"] = True
    rj = "[" + ",".join("['%s','%s']" % tuple(r.split(":")) for r in RANGES) + "]"
    scr = sess.create_script(AGENT.replace("__RANGES__", rj)); scr.on("message", on_msg); scr.load()
    import time
    t = time.time() + 8
    while time.time() < t and not done["v"]:
        time.sleep(0.2)
    try: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
