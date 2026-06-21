# Why does MASHED exit before the menu under Frida spawn? The heap-init crash is
# passed under Frida (40k+ ops). This probe identifies the actual termination cause:
# resolves the faulting address to module+offset, logs a backtrace at each first-
# chance exception, and hooks the process-exit / fast-fail paths with backtraces.
#
# Usage: py -3.12 re/frida/boot_death_probe.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
function mod(addr){ try{ const m=Process.findModuleByAddress(addr); return m ? (m.name+'+0x'+addr.sub(m.base).toString(16)) : (''+addr); }catch(e){ return ''+addr; } }
function bt(ctx){ try{ return Thread.backtrace(ctx, Backtracer.ACCURATE).slice(0,12).map(mod); }catch(e){ return ['<bt failed '+e+'>']; } }

const ntdll = Process.findModuleByName('ntdll.dll');
function exp(m,n){ try{ const mm=Process.findModuleByName(m); return mm?mm.findExportByName(n):null; }catch(e){ return null; } }

// hook the exit / fast-fail paths with a backtrace so we see what initiated exit.
[['ntdll.dll','RtlExitUserProcess'], ['ntdll.dll','NtTerminateProcess'],
 ['ntdll.dll','RtlReportFatalFailure'], ['ntdll.dll','RtlReportSilentProcessExit'],
 ['ntdll.dll','RtlpHeapHandleError'], ['ntdll.dll','RtlFailFast2'],
 ['kernelbase.dll','TerminateProcess'], ['kernel32.dll','ExitProcess']].forEach(function(p){
  const a = exp(p[0], p[1]);
  if (a) Interceptor.attach(a, { onEnter(args){
    send({kind:'exit', fn:p[0]+'!'+p[1], a0:''+args[0], a1:''+args[1], bt:bt(this.context)});
  }});
});

let exc = 0;
Process.setExceptionHandler(function(d){
  if (d.type === 'system') return false;   // skip DBG_PRINTEXCEPTION_C (OutputDebugString) noise
  exc++;
  if (exc <= 8){
    let insn=''; try{ insn=Instruction.parse(d.context.eip).toString(); }catch(e){ insn='?'; }
    send({kind:'exc', n:exc, etype:d.type, at:mod(d.context.eip), wraddr:''+d.address, insn:insn,
          ctx:'eax='+d.context.eax+' ecx='+d.context.ecx+' edx='+d.context.edx+' esi='+d.context.esi+' edi='+d.context.edi, bt:bt(d.context)});
  }
  return false;  // pass to app SEH
});
send({kind:'ready'});
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 20
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"  spawned MASHED pid={pid}")
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {}); k = p.get("kind")
        if k == "exc":
            print(f"\n  [EXC #{p['n']}] type={p['etype']} at {p['at']}  write->{p['wraddr']}")
            print(f"        insn: {p['insn']}")
            for f in p["bt"]: print(f"          {f}")
        elif k == "exit":
            print(f"\n  [EXIT] {p['fn']}  arg0={p['a0']} arg1={p['a1']}")
            for f in p["bt"]: print(f"          {f}")
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  resumed; watching {seconds}s...")
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  process EXITED"); time.sleep(0.3); break
        time.sleep(0.25)
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
