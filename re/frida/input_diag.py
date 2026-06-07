# Input-API diagnostic — OBSERVATION ONLY. Determines HOW MASHED reads the keyboard.
# Probes (counts only, no writes): user32 GetAsyncKeyState / GetKeyboardState /
# RawInput, and DirectInput8Create / DirectInputCreateA presence. Reports which fire.
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const C = {};
function bump(n){ C[n]=(C[n]||0)+1; }
// frida 17: static Module.getExportByName is gone — use module instance .findExportByName
function tryHook(mod, exp){
  try {
    const m = Process.findModuleByName(mod);
    if (!m) return false;
    const p = m.findExportByName(exp);
    if (!p) return false;
    Interceptor.attach(p,{onEnter(){bump(exp);}});
    send({kind:'info',msg:'hooked '+exp+' in '+mod});
    return true;
  } catch(e){ send({kind:'err',msg:'tryHook '+exp+' '+e}); return false; }
}
const mods = Process.enumerateModules().filter(m=>/input|user32|hid|dinput/i.test(m.name)).map(m=>m.name);
send({kind:'info', msg:'input-ish modules: '+mods.join(', ')});
// keyboard read APIs in user32
['GetAsyncKeyState','GetKeyboardState','GetKeyState','GetRawInputData','MapVirtualKeyA','ToAscii'].forEach(e=>tryHook('user32.dll',e));
// DirectInput entries in both proxy + real
['DINPUT8.dll','dinput8_real.DLL'].forEach(function(mod){
  ['DirectInput8Create','DirectInputCreateA','DirectInputCreateEx'].forEach(e=>tryHook(mod,e));
});
// also peek window-message pump (menus often read WM_KEYDOWN via PeekMessage/GetMessage)
['PeekMessageA','GetMessageA','PeekMessageW','GetMessageW'].forEach(e=>tryHook('user32.dll',e));
send({kind:'ready'});
setInterval(function(){ send({kind:'counts', c:C}); }, 2000);
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 18
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid); last = {}
    def on_msg(m,d):
        if m.get("type")=="error": print("  agent error:", m.get("description")); return
        p=m.get("payload",{})
        if p.get("kind")=="info": print("  [diag]", p["msg"])
        elif p.get("kind")=="counts": last.clear(); last.update(p["c"])
    scr=sess.create_script(AGENT); scr.on("message",on_msg); scr.load(); dev.resume(pid)
    print(f"  resumed; probing input APIs {seconds}s")
    t=time.time()+seconds
    while time.time()<t:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        time.sleep(0.5)
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    print(f"\n=== input API call counts ===\n  {last or '(none fired)'}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
