# Identify what ntdll+0x542f0 actually IS in the CURRENT ntdll (the 2026-06-10
# build). The old memory note called it RtlpHeap, but the update may have shifted
# ntdll layout, so the same offset could be a different function -> the whole "heap"
# diagnosis (and every heap-targeted fix) may be aimed at the wrong code.
#
# Resolves the nearest preceding ntdll export to base+0x542f0, tries DebugSymbol,
# and disassembles around the faulting instruction. Spawns MASHED only to get ntdll
# mapped; resolves at agent-load (before the crash) and kills it.
#
# Usage: py -3.12 re/frida/symbolize_ntdll_fault.py [--off 0x542f0]
import os, sys
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

OFF = int(sys.argv[sys.argv.index("--off")+1], 16) if "--off" in sys.argv else 0x542f0

AGENT = r'''
'use strict';
const OFF = __OFF__;
const ntdll = Process.findModuleByName('ntdll.dll');
const target = ntdll.base.add(OFF);
let best = null;
ntdll.enumerateExports().forEach(function(e){
  if (e.type === 'function' && e.address.compare(target) <= 0) {
    if (!best || e.address.compare(best.address) > 0) best = e;
  }
});
let sym = '';
try { sym = DebugSymbol.fromAddress(target).toString(); } catch(e){ sym = '(no DebugSymbol: '+e+')'; }
let dis = [];
try {
  let a = target.sub(0x10);
  for (let i = 0; i < 12; i++){
    const ins = Instruction.parse(a);
    dis.push((a.equals(target) ? '>>> ' : '    ') + a.sub(ntdll.base) + '  ' + ins.toString());
    a = ins.next;
  }
} catch(e){ dis.push('disasm error: '+e); }
send({
  ntdllBase: ''+ntdll.base,
  ntdllVer: ntdll.path,
  target: ''+target,
  nearestExport: best ? (best.name + '  (+0x' + target.sub(best.address).toString(16) + ' past it)') : '(none)',
  debugSym: sym,
  disasm: dis
});
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
        p = m.get("payload", {})
        print(f"ntdll: {p['ntdllVer']}")
        print(f"  base={p['ntdllBase']}  target(+0x{OFF:x})={p['target']}")
        print(f"  nearest export : {p['nearestExport']}")
        print(f"  DebugSymbol    : {p['debugSym']}")
        print("  disasm around fault:")
        for line in p["disasm"]:
            print("    " + line)
        done["v"] = True
    scr = sess.create_script(AGENT.replace("__OFF__", hex(OFF))); scr.on("message", on_msg); scr.load()
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
