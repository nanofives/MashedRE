# Where does the original render Frontend.piz/MAIN.BSP? (frontend-faithful
# sweep, user-directed investigation 2026-06-10)
#
# Evidence going in: the menu BACKGROUND is frontend.mpg video (F1); MAIN.BSP
# materials are 'main'/'carbase'/'rendergs' -> suspected car-select stage.
# This probe measures, at (a) the main menu and (b) the car-select screen:
#   - DAT_0066d704 (the course/track-view render guard in FUN_004270f0)
#   - fire counts of FUN_00478cd0 (world render, world obj DAT_00646e58)
#   - fire counts of FUN_004270f0 (course-view render dispatch)
#
# Usage: py -3.12 re/frida/mainbsp_probe.py
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
let pressCtrl=-1, pressUntil=0;
let cWorld=0, cCourse=0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }});
    Interceptor.attach(abs(0x00478cd0), { onEnter(args){ cWorld++; } });
    Interceptor.attach(abs(0x004270f0), { onEnter(args){ cCourse++; } });
    return DELTA; },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  counts:function(){ const r={world:cWorld, course:cCourse}; cWorld=0; cCourse=0; return JSON.stringify(r); },
  state:function(){ return JSON.stringify({
    depth: abs(RVA_DEPTH).readS32(), phase: abs(RVA_PHASE).readS32(),
    guard_66d704: abs(0x0066d704).readS32(),
    world_646e58: abs(0x00646e58).readU32(),
    screen: abs(0x0067ed7c + (abs(RVA_DEPTH).readS32()-1)*0x40).readS32() }); },
  setsel:function(v){ try{const d=abs(RVA_DEPTH).readS32(); abs(0x0067ed80+(d-1)*0x40).writeS32(v); return 1;}catch(e){return 0;} }
};
'''


def main():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    def wait_phase3(timeout=25):
        end = time.time() + timeout
        while time.time() < end:
            st = json.loads(E.state())
            if st["phase"] == 3 and st["depth"] >= 1:
                return True
            time.sleep(0.2)
        return False

    def press(c, ms=180):
        E.press(c, ms); time.sleep(ms / 1000.0 + 0.35)

    print("booting...")
    wait_phase3()
    time.sleep(1.5)
    E.counts()  # reset
    time.sleep(3.0)
    print("MAIN MENU:", E.counts(), E.state())

    # walk: confirm -> GTS, select Single Player -> game types -> Quick Battle
    # path toward car select (screen 0x1c per FUN_00432800)
    press(4); time.sleep(0.6)
    press(4); time.sleep(0.6)        # into GTS depth
    E.setsel(1); time.sleep(0.3)     # Quick Battle (per race_refs flow)
    press(4); time.sleep(0.8)
    print("after push:", E.state())
    press(4); time.sleep(0.8)
    print("deeper:", E.state())
    E.counts()
    time.sleep(3.0)
    print("CAR-SELECT-ish:", E.counts(), E.state())
    try: dev.kill(pid)
    except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
