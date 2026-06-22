# Probe the original's game-setup state globals as seen on the Game Mode screens
# (nav screen 18 = MP battle, 24 = SP championship). FUN_0043af10 (option rows
# with -> values) + FUN_004368e0 (content body) read these. Ground truth for
# porting the value text + preview faithfully.
#   gamemode  0x0067e9fc      track     0x0067f17c
#   flags: af10=0x0067e7f0  439210=0x0067e7b8  434720=0x0067e7c8
#   settings: ea74 game-type, ea78, ea7c diff, ea80 powerups, ea88 length,
#             ea90, ea94 vehicle, ea98/9c/a0 opponents, eaac
# Usage: py -3.12 re/frida/probe_gamemode_state.py [scr ...]   (default 18 24)
import os, sys, time
import frida
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_NAV=0x0043d2a0, RVA_PHASE=0x0067eca4, RVA_CURSCREEN=0x0067ecb0;
let nav=null;
function abs(r){return ptr(r+DELTA);}
const G={gamemode:0x0067e9fc, track:0x0067f17c, screen:0x0067e9f8,
  f_af10:0x0067e7f0, f_439210:0x0067e7b8, f_434720:0x0067e7c8, f_colour:0x0067e7a8,
  ea74:0x0067ea74, ea78:0x0067ea78, ea7c:0x0067ea7c, ea80:0x0067ea80,
  ea88:0x0067ea88, ea90:0x0067ea90, ea94:0x0067ea94, ea98:0x0067ea98,
  ea9c:0x0067ea9c, eaa0:0x0067eaa0, eaac:0x0067eaac};
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG; nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']); return DELTA; },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); abs(RVA_CURSCREEN).writeS32(scr); },
  read:function(){ const o={}; for(const k in G) o[k]=abs(G[k]).readS32(); return o; }
};
'''

def main():
    screens = [int(a) for a in sys.argv[1:]] or [18, 24]
    dev = frida.get_local_device()
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    s = sess.create_script(AGENT); s.on("message", lambda m, d: print("MSG", m)); s.load()
    E = s.exports_sync; E.init(); dev.resume(pid)
    end = time.time() + 30
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    print("default:", E.read())
    for scr in screens:
        E.push(scr); time.sleep(1.6)
        print(f"@scr {scr}:", E.read())
    try: dev.kill(pid)
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
