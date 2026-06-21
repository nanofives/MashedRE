# Boot probe: does MASHED reach the menu under Frida spawn with the ntdll heap
# functions hooked? trace_boot_heap.py showed the legacy-CRT heap-init corruption
# (0x5477) does NOT reproduce when RtlAllocateHeap/RtlFreeHeap are Interceptor-
# hooked (40k+ heap ops succeed). This tests whether that perturbation gets MASHED
# all the way to the menu (GetDeviceState cb=256 polling) — i.e. a usable boot path
# for the Frida-spawn recorder.
#
# Menu-reached signal = IDirectInputDevice8::GetDeviceState(cb=256) fires. First-
# chance exceptions are counted but NOT swallowed (app SEH handles them).
#
# Usage: py -3.12 re/frida/boot_probe_heaphook.py [--seconds N] [--no-heaphook]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const HEAPHOOK = __HEAPHOOK__;
const ntdll = Process.findModuleByName('ntdll.dll');
function exp(n){ try { return ntdll ? ntdll.findExportByName(n) : null; } catch(e){ return null; } }
let heapOps = 0, gdsCalls = 0, excCount = 0;

if (HEAPHOOK) {
  ['RtlAllocateHeap','RtlFreeHeap','RtlReAllocateHeap'].forEach(function(fn){
    const a = exp(fn); if (a) Interceptor.attach(a, { onEnter(){ heapOps++; } });
  });
}

// menu-reached detector: DirectInput8Create -> CreateDevice[3] -> GetDeviceState[9]
let di8=false, cd=false; const seen={};
function hookGDS(vt){ try{ const g=vt.add(9*4).readPointer(); const k=g.toString(); if(seen[k])return; seen[k]=1;
  Interceptor.attach(g,{ onEnter(a){ this.cb=a[1].toInt32(); }, onLeave(){ if(this.cb===256){ gdsCalls++;
    if(gdsCalls===1) send({kind:'menu', msg:'GetDeviceState(cb=256) fired — MENU REACHED'}); } } });
  }catch(e){} }
function hookCD(vt){ if(cd)return; cd=true; try{ const c=vt.add(3*4).readPointer();
  Interceptor.attach(c,{ onEnter(a){this.o=a[2];}, onLeave(){ try{ const d=this.o.readPointer(); if(!d.isNull()) hookGDS(d.readPointer()); }catch(e){} } }); }catch(e){} }
function hookDI8(){ if(di8)return true; let ex=null;
  for(const m of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']){ try{ const mod=Process.findModuleByName(m); if(mod){ ex=mod.findExportByName('DirectInput8Create'); if(ex)break; } }catch(e){} }
  if(!ex)return false; di8=true;
  Interceptor.attach(ex,{ onEnter(a){this.p=a[3];}, onLeave(){ try{ const d=this.p.readPointer(); if(!d.isNull()) hookCD(d.readPointer()); }catch(e){} } }); return true; }
if(!hookDI8()){ const iv=setInterval(function(){ if(hookDI8()) clearInterval(iv); },50); }

Process.setExceptionHandler(function(d){
  excCount++;
  if (excCount <= 3){ let r=''; try{ r='eip='+d.context.eip+' ecx='+d.context.ecx+' eax='+d.context.eax; }catch(e){}
    send({kind:'exc', msg:'first-chance #'+excCount+' type='+d.type+' addr='+d.address+' '+r}); }
  return false;   // let the app's SEH handle it
});

rpc.exports = { stat:function(){ return {heapOps:heapOps, gdsCalls:gdsCalls, excCount:excCount}; } };
send({kind:'ready', msg:'heaphook='+HEAPHOOK});
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 25
    heaphook = "false" if "--no-heaphook" in sys.argv else "true"
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"  spawned MASHED pid={pid}  heaphook={heaphook}  (kill ONLY this pid)")
    sess = dev.attach(pid)
    menu = {"reached": False}
    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {}); k = p.get("kind")
        if k == "ready": print("  [ready]", p["msg"])
        elif k == "menu": print("  [MENU]", p["msg"]); menu["reached"] = True
        elif k == "exc": print("  [exc]", p["msg"])
    scr = sess.create_script(AGENT.replace("__HEAPHOOK__", heaphook))
    scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  resumed; watching {seconds}s for menu (GetDeviceState cb=256)...")
    t = time.time() + seconds; alive = True
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  process EXITED"); alive = False; break
        time.sleep(0.5)
    stat = None
    try:
        if alive: stat = scr.exports_sync.stat()
    except Exception: pass
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    print(f"\n=== result: menu_reached={menu['reached']} alive_at_end={alive} ===")
    if stat: print(f"  heapOps={stat['heapOps']} gdsCalls={stat['gdsCalls']} firstChanceExc={stat['excCount']}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
