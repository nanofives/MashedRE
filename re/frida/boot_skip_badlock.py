# Boot workaround: MASHED's boot crash is EnterCriticalSection on a garbage object
# pointer (~0x5453, cs at +0x20) from a CRT static-init-order bug (a global is NULL
# when its object is locked) exposed by the 2026-06-10 Windows update. NOT heap.
#
# Workaround: intercept the Rtl*CriticalSection calls; when the CS pointer is
# unreadable (NULL/garbage), redirect it to a real, initialized DUMMY critical
# section so the call succeeds instead of AV-ing. Single-threaded boot init has no
# lock contention, so substituting a shared dummy is safe enough to get past init.
#
# Reports whether MASHED then reaches the menu (GetDeviceState cb=256).
#
# Usage: py -3.12 re/frida/boot_skip_badlock.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const ntdll = Process.findModuleByName('ntdll.dll');
function e(n){ try{ return ntdll.findExportByName(n); }catch(err){ return null; } }

// one real, initialized critical section to stand in for every bad pointer
const dummy = Memory.alloc(0x40);
const RtlInit = new NativeFunction(e('RtlInitializeCriticalSection'), 'uint32', ['pointer']);
RtlInit(dummy);

let redir = 0;
function bad(p){ try{ p.add(4).readU32(); return (p.toUInt32() < 0x10000); }catch(err){ return true; } }

['RtlEnterCriticalSection','RtlLeaveCriticalSection','RtlInitializeCriticalSection',
 'RtlDeleteCriticalSection','RtlTryEnterCriticalSection'].forEach(function(n){
  const a = e(n); if (!a) return;
  Interceptor.attach(a, { onEnter(args){ if (bad(args[0])){ args[0] = dummy; redir++; } } });
});
const exA = e('RtlInitializeCriticalSectionEx');
if (exA) Interceptor.attach(exA, { onEnter(args){ if (bad(args[0])){ args[0] = dummy; redir++; } } });

// menu-reached detector
let di8=false, cd=false; const seen={}; let gds=0;
function hookGDS(vt){ try{ const g=vt.add(9*4).readPointer(); const k=g.toString(); if(seen[k])return; seen[k]=1;
  Interceptor.attach(g,{ onEnter(a){this.cb=a[1].toInt32();}, onLeave(){ if(this.cb===256){ gds++; if(gds===1) send({kind:'menu'}); } } }); }catch(err){} }
function hookCD(vt){ if(cd)return; cd=true; try{ const c=vt.add(3*4).readPointer();
  Interceptor.attach(c,{ onEnter(a){this.o=a[2];}, onLeave(){ try{ const d=this.o.readPointer(); if(!d.isNull()) hookGDS(d.readPointer()); }catch(err){} } }); }catch(err){} }
function hookDI8(){ if(di8)return true; let ex=null;
  for(const m of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']){ try{ const mod=Process.findModuleByName(m); if(mod){ ex=mod.findExportByName('DirectInput8Create'); if(ex)break; } }catch(err){} }
  if(!ex)return false; di8=true;
  Interceptor.attach(ex,{ onEnter(a){this.p=a[3];}, onLeave(){ try{ const d=this.p.readPointer(); if(!d.isNull()) hookCD(d.readPointer()); }catch(err){} } }); return true; }
if(!hookDI8()){ const iv=setInterval(function(){ if(hookDI8()) clearInterval(iv); },50); }

Process.setExceptionHandler(function(d){
  if (d.type === 'system') return false;
  send({kind:'av', at:''+d.context.eip, esi:''+d.context.esi});
  return false;
});
rpc.exports = { stat:function(){ return {redir:redir, gds:gds}; } };
send({kind:'ready', msg:'badlock workaround armed; dummy CS @'+dummy});
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 30
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"  spawned MASHED pid={pid}")
    sess = dev.attach(pid)
    menu = {"v": False}
    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {}); k = p.get("kind")
        if k == "ready": print("  [ready]", p["msg"])
        elif k == "menu": print("  [MENU REACHED] GetDeviceState(cb=256) fired"); menu["v"] = True
        elif k == "av": print(f"  [AV] still crashed at {p['at']} esi={p['esi']}")
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  resumed; watching {seconds}s for menu...")
    alive = True; t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  process EXITED"); alive = False; break
        if menu["v"]: break
        time.sleep(0.5)
    try:
        st = scr.exports_sync.stat() if alive else None
        if st: print(f"  CS redirections: {st['redir']}   GDS calls: {st['gds']}")
    except Exception: pass
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    print(f"\n=== menu_reached={menu['v']} ===")
    return 0

if __name__ == "__main__":
    sys.exit(main())
