# Boot heap-corruption tracer — spawn ORIGINAL MASHED.exe suspended, log the
# ntdll heap calls during CRT init, and catch the AV in-process to pin the exact
# failing operation. Boot heap init is finite (not the steady menu hot path), so
# Interceptor here is safe.
#
# Goal: identify which RtlAllocateHeap/RtlFreeHeap call (heap handle, size, flags)
# triggers the 0xC0000005 WRITE to 0x5477 in ntdll!RtlpHeap during the legacy CRT
# __sbh init, so a targeted heap workaround can be chosen.
#
# Usage: py -3.12 re/frida/trace_boot_heap.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const ring = [];
let n = 0;
function note(s){ n++; ring.push(n+': '+s); if (ring.length > 80) ring.shift(); }

const ntdll = Process.findModuleByName('ntdll.dll');
function exp(name){ try { return ntdll ? ntdll.findExportByName(name) : null; } catch(e){ return null; } }

function hookHeap(fn, hIdx, szIdx){
  const a = exp(fn);
  if (!a){ send({kind:'info', msg:'no export '+fn}); return; }
  Interceptor.attach(a, {
    onEnter(args){
      this.h = args[hIdx];
      this.flags = args[1];
      this.sz = (szIdx >= 0) ? args[szIdx] : ptr(0);
      this.tag = fn+' heap='+this.h+' flags='+this.flags+(szIdx>=0?(' sz='+this.sz):'');
      note('ENTER '+this.tag);
    },
    onLeave(r){ note('LEAVE '+fn+' -> '+r); }
  });
}
// RtlAllocateHeap(heap, flags, size) ; RtlFreeHeap(heap, flags, ptr) ;
// RtlReAllocateHeap(heap, flags, ptr, size)
hookHeap('RtlAllocateHeap', 0, 2);
hookHeap('RtlFreeHeap', 0, -1);
hookHeap('RtlReAllocateHeap', 0, 3);

// Also instrument the process-heap creation to learn the real heap base/flags.
const create = exp('RtlCreateHeap');
if (create) Interceptor.attach(create, {
  onEnter(a){ this.flags = a[0]; this.base = a[1]; },
  onLeave(r){ note('RtlCreateHeap flags='+this.flags+' base='+this.base+' -> heap='+r);
              send({kind:'heap', msg:'RtlCreateHeap flags='+this.flags+' -> '+r}); }
});

Process.setExceptionHandler(function(d){
  let regs = '';
  try { const c=d.context; regs = 'eip='+c.eip+' eax='+c.eax+' ebx='+c.ebx+' ecx='+c.ecx+' edx='+c.edx+' esi='+c.esi+' edi='+c.edi; } catch(e){}
  let insn = '';
  try { insn = Instruction.parse(d.context.eip).toString(); } catch(e){ insn='?'; }
  send({kind:'crash', etype:d.type, addr:''+d.address, insn:insn, regs:regs, ring:ring.slice(-40)});
  return false;  // do not swallow — let it crash so we get a faithful picture
});
send({kind:'ready'});
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 20
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"  spawned MASHED pid={pid} (suspended); will kill ONLY this pid")
    sess = dev.attach(pid)
    got = {"crash": None}
    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        k = p.get("kind")
        if k == "info": print("  [info]", p["msg"])
        elif k == "heap": print("  [heap]", p["msg"])
        elif k == "crash":
            got["crash"] = p
            print("\n=== CRASH CAUGHT ===")
            print("  type:", p["etype"], " addr:", p["addr"])
            print("  insn:", p["insn"])
            print("  regs:", p["regs"])
            print("  --- last 40 heap ops before fault ---")
            for r in p["ring"]:
                print("   ", r)
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  resumed; tracing boot heap for up to {seconds}s")
    t = time.time() + seconds
    while time.time() < t:
        if got["crash"]: time.sleep(0.3); break
        if psutil and not psutil.pid_exists(pid): print("  process exited"); break
        time.sleep(0.25)
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    print("\n  done." if got["crash"] else "\n  no crash captured in window.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
