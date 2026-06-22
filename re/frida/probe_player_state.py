# Probe the original's frontend player-state globals as seen on the colour/
# ability/team screens (jumped-to default state). FUN_0043a610 (colour-select
# rows) reads, per player slot p in 0..3:
#   order/active  = *(int*)(0x7f1a14 + p*0x10)   (-1 = inactive)
#   car index     = *(int*)(0x7f1a1c + p*0x10)   (FUN_0042fab0 sprite idx 0..9)
#   device        = *(int*)(0x7e96fc + p*0x200)  (1=joypad, 2=keyboard, per FUN_0042bcb0)
# Ground truth for porting the player rows faithfully into mashed_re.exe.
#
# Usage: py -3.12 re/frida/probe_player_state.py [screen]   (default 4)
import os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_NAV=0x0043d2a0, RVA_PHASE=0x0067eca4, RVA_CURSCREEN=0x0067ecb0;
const A_ORDER=0x007f1a14, A_CAR=0x007f1a1c, A_DEV=0x007e96fc;
let nav=null;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); abs(RVA_CURSCREEN).writeS32(scr); },
  readState:function(){
    const o=[],c=[],d=[];
    for(let p=0;p<4;p++){
      o.push(abs(A_ORDER+p*0x10).readS32());
      c.push(abs(A_CAR  +p*0x10).readS32());
      d.push(abs(A_DEV  +p*0x200).readS32());
    }
    // also a few extra game-state globals used by the content screens
    return {order:o, car:c, dev:d,
      gamemode: abs(0x0067e9fc).readS32(),
      track:    abs(0x0067f17c).readS32()};
  }
};
'''


def main():
    scr = int(sys.argv[1]) if len(sys.argv) > 1 else 4
    dev = frida.get_local_device()
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    s = sess.create_script(AGENT); s.on("message", lambda m, d: print("MSG", m)); s.load()
    E = s.exports_sync
    E.init(); dev.resume(pid)
    end = time.time() + 30
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    print("default state (no screen pushed):", E.read_state())
    E.push(scr); time.sleep(1.6)
    print(f"state @screen {scr}:", E.read_state())
    try: dev.kill(pid)
    except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
