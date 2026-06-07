# In-process menu input feeder — SAFE: writes ONLY MASHED's own DirectInput buffer.
#
# Hooks IDirectInputDevice8::GetDeviceState (confirmed keyboard read, input_recon.py)
# and, in onLeave, OR-s a DIK_* byte (0x80) into the 256-byte state buffer AFTER
# dinput8 fills it but BEFORE MASHED reads it. MASHED sees the key as pressed.
# This NEVER calls keybd_event / SendInput / SetForegroundWindow / focus — nothing
# touches the OS input queue or any other window. Cannot affect the host PC.
#
# A time-based schedule taps a sequence (default DOWN,DOWN) with press/gap windows
# so MASHED's per-frame edge detection registers one move per tap.
#
# Usage: py -3.12 re/frida/input_feed.py [--seq down,down,up] [--settle MS] [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
DIK = {"esc":0x01,"enter":0x1C,"up":0xC8,"down":0xD0,"left":0xCB,"right":0xCD,"space":0x39}

AGENT = r'''
'use strict';
let di8Hooked=false, cdHooked=false; const seenDev={};
// schedule injected from python
let SEQ=[], SETTLE=13000, PRESS=130, GAP=260, startT=0, applied=0;
const CNT={};
rpc.exports = {
  setplan:function(seq, settle, press, gap){ SEQ=seq; SETTLE=settle; PRESS=press; GAP=gap; startT=Date.now(); return 1; },
  applied:function(){ return applied; },
  // count nav-handler calls (pre-resume) to prove the menu consumes our fed input
  countthese:function(rvas){ rvas.forEach(function(r){ CNT[r]=0;
    try{ Interceptor.attach(ptr(r),{onEnter(){CNT[r]++;}}); }catch(e){ CNT[r]=-1; } }); return Object.keys(CNT).length; },
  counts:function(){ return CNT; }
};
function curKey(){
  if (!SEQ.length) return -1;
  const t = Date.now() - startT - SETTLE;
  if (t < 0) return -1;
  const period = PRESS + GAP;
  const idx = Math.floor(t / period);
  if (idx >= SEQ.length) return -1;
  const phase = t - idx*period;
  return (phase < PRESS) ? SEQ[idx] : -1;   // pressed only during the PRESS window
}
function hookGDS(vt, devPtr){
  try{
    const gds = vt.add(9*4).readPointer(); const key=gds.toString();
    if (seenDev[key]) return; seenDev[key]=1;
    Interceptor.attach(gds,{
      onEnter(a){ this.cb=a[1].toInt32(); this.buf=a[2]; },
      onLeave(r){
        if (this.cb!==256) return;            // keyboard immediate state only
        const k = curKey();
        if (k>=0){ try{ this.buf.add(k).writeU8(0x80); applied++; }catch(e){} }  // IN-PROCESS buffer only
      }
    });
    send({kind:'info',msg:'feeder armed on GetDeviceState @'+gds});
  }catch(e){ send({kind:'err',msg:'hookGDS '+e}); }
}
function hookCD(vt){
  if (cdHooked) return; cdHooked=true;
  try{ const cd=vt.add(3*4).readPointer();
    Interceptor.attach(cd,{ onEnter(a){this.out=a[2];},
      onLeave(r){ try{ const d=this.out.readPointer(); if(!d.isNull()) hookGDS(d.readPointer(), d); }catch(e){} } });
  }catch(e){ send({kind:'err',msg:'hookCD '+e}); }
}
function hookDI8(){
  if (di8Hooked) return true;
  let ex=null;
  for (const m of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']){
    try{ const mod=Process.findModuleByName(m); if(mod){ ex=mod.findExportByName('DirectInput8Create'); if(ex) break; } }catch(e){}
  }
  if(!ex) return false; di8Hooked=true;
  Interceptor.attach(ex,{ onEnter(a){this.ppv=a[3];},
    onLeave(r){ try{ const di=this.ppv.readPointer(); if(!di.isNull()) hookCD(di.readPointer()); }catch(e){} } });
  return true;
}
hookDI8();
send({kind:'ready'});
'''

def main():
    seq_names = (sys.argv[sys.argv.index("--seq")+1] if "--seq" in sys.argv else "down,down").split(",")
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 13000
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 30
    seq = [DIK[s.strip().lower()] for s in seq_names if s.strip()]
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"]="1"   # stock MASHED, no dev .asi
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m,d):
        if m.get("type")=="error": print("  agent error:", m.get("description")); return
        p=m.get("payload",{})
        if p.get("kind") in ("info","err"): print("  [feed]", p.get("msg"))
    scr=sess.create_script(AGENT); scr.on("message",on_msg); scr.load()
    count_rvas = (sys.argv[sys.argv.index("--count")+1].split(",")) if "--count" in sys.argv else []
    if count_rvas:
        scr.exports_sync.countthese(count_rvas)   # attach BEFORE resume
    scr.exports_sync.setplan(seq, settle, 130, 260)
    dev.resume(pid)
    print(f"  resumed; feeding seq={seq_names} after {settle}ms settle (IN-PROCESS buffer only)")
    t=time.time()+seconds
    while time.time()<t:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        time.sleep(0.5)
    applied = 0; counts = {}
    try: applied = scr.exports_sync.applied()
    except Exception: pass
    if count_rvas:
        try: counts = scr.exports_sync.counts()
        except Exception: pass
    print(f"  buffer-writes applied: {applied}")
    if count_rvas:
        print(f"  nav-handler counts (>0 == fed input drove the menu): {counts}")
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
