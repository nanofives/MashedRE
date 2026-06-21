# Test fix: MASHED's boot crash is fopen() (FUN_004a4541 = _fsopen("mashed.log",..))
# returning a GARBAGE non-null FILE* (0x5453); the log-printf's `if(FILE!=0)` guard
# passes and fputs crashes. Hook the fopen wrapper and, when it returns an unusable
# pointer, force NULL so callers take their null-handling path (the log just skips).
# Then check whether MASHED reaches the menu -> tells us if the log fopen was the
# only boot blocker (=> a tiny binary patch fixes boot) or if other fopens break too.
#
# Usage: py -3.12 re/frida/boot_nullbadfopen.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
FOPEN_RVA = 0x4a4541

AGENT = r'''
'use strict';
const m = Process.findModuleByName('MASHED.exe');
const fopen = m.base.add(__RVA__ - 0x400000);
let calls = 0, nulled = 0;
function usable(p){ try{ p.add(4).readU32(); return p.toUInt32() >= 0x10000; }catch(e){ return false; } }
Interceptor.attach(fopen, {
  onEnter(args){ try{ this.fn = args[0].readUtf8String(); }catch(e){ this.fn='?'; } },
  onLeave(retval){
    calls++;
    const v = retval.toUInt32();
    if (v !== 0 && !usable(ptr(v))){
      nulled++;
      send({kind:'null', fn:this.fn, was:'0x'+v.toString(16)});
      retval.replace(ptr(0));
    } else {
      send({kind:'ok', fn:this.fn, ret:'0x'+v.toString(16)});
    }
  }
});

// menu detector
let di8=false, cd=false; const seen={}; let gds=0;
function hookGDS(vt){ try{ const g=vt.add(9*4).readPointer(); const k=g.toString(); if(seen[k])return; seen[k]=1;
  Interceptor.attach(g,{ onEnter(a){this.cb=a[1].toInt32();}, onLeave(){ if(this.cb===256){ gds++; if(gds===1) send({kind:'menu'}); } } }); }catch(e){} }
function hookCD(vt){ if(cd)return; cd=true; try{ const c=vt.add(3*4).readPointer();
  Interceptor.attach(c,{ onEnter(a){this.o=a[2];}, onLeave(){ try{ const d=this.o.readPointer(); if(!d.isNull()) hookGDS(d.readPointer()); }catch(e){} } }); }catch(e){} }
function hookDI8(){ if(di8)return true; let ex=null;
  for(const mm of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']){ try{ const mod=Process.findModuleByName(mm); if(mod){ ex=mod.findExportByName('DirectInput8Create'); if(ex)break; } }catch(e){} }
  if(!ex)return false; di8=true;
  Interceptor.attach(ex,{ onEnter(a){this.p=a[3];}, onLeave(){ try{ const d=this.p.readPointer(); if(!d.isNull()) hookCD(d.readPointer()); }catch(e){} } }); return true; }
if(!hookDI8()){ const iv=setInterval(function(){ if(hookDI8()) clearInterval(iv); },50); }

Process.setExceptionHandler(function(d){ if(d.type==='system') return false;
  send({kind:'av', at:''+d.context.eip}); return false; });
rpc.exports = { stat:function(){ return {calls:calls, nulled:nulled, gds:gds}; } };
send({kind:'ready'});
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
        if k == "ready": print("  [ready] fopen hook armed")
        elif k == "null": print(f"  [NULLED garbage fopen] '{p['fn']}' was {p['was']} -> NULL")
        elif k == "ok": print(f"  [fopen ok] '{p['fn']}' -> {p['ret']}")
        elif k == "menu": print("  [MENU REACHED]"); menu["v"] = True
        elif k == "av": print(f"  [AV] at {p['at']}")
    scr = sess.create_script(AGENT.replace("__RVA__", hex(FOPEN_RVA))); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  resumed; watching {seconds}s for menu...")
    alive = True; t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  process EXITED"); alive = False; break
        if menu["v"]: break
        time.sleep(0.5)
    try:
        st = scr.exports_sync.stat() if alive else None
        if st: print(f"  fopen calls={st['calls']} nulled={st['nulled']} gds={st['gds']}")
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
