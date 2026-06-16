# WS-B2/B3 — original-side contact-array baseline capture.
#
# Snapshots the per-car contact state + terrain-batch globals that the ported
# contact solvers (Collision/CarWorldContacts.cpp, CarCarContacts.cpp) read /
# write, so the eventual installed-hook scenario diff has a ground-truth anchor.
#
# NO sustained Interceptor (the contact functions are per-frame hot paths that
# destabilise MASHED in ~6s — CLAUDE.md "Frida overhead on hot paths"). Instead
# we POLL the runtime game-mode on a timer and take a one-shot memory snapshot
# the moment a race (game-mode in {6,7,10,0xb}) is detected, then stop.
#
# Vehicle record base DAT_008815a0, stride 0xd04, 16 slots (vehicle.md / WS-A1).
# Contact fields (byte offsets from a car's base):
#   +0x004  active flag (DAT_008815a4[car*0x341])
#   +0x4ac  18-slot contact scan array (stride 0x40; first int = slot id, -1=off)
#   +0x4c0  18 contact records (stride 0x40 ... read first 4 here)
#   +0x9b0  linear velocity vec3
#   +0x9e0  grounded-wheel count (float)
#   +0x9e4  speed
#   +0x9ec  active-contact-count writeback
# Globals: DAT_0088e60c terrain entry count; DAT_0088e650 active contact count.
# Game-mode getter FUN_0040e350 (returns the runtime mode the solvers gate on).
#
# Usage: py -3.12 re/frida/wsb_contact_baseline.py [seconds]   (default 60)
import os, sys, time, json
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
OUT = ROOT / "log" / "wsb_contact_baseline.json"

AGENT = r'''
'use strict';
const IMG=0x00400000; let D=0;
const RVA_MODE=0x0040e350;
const VBASE=0x008815a0, VSTRIDE=0xd04, NCARS=16;
const G_TERRCNT=0x0088e60c, G_ACTIVECNT=0x0088e650;
let fnMode=null;
function abs(r){return ptr(r+D);}
function f(p){return abs(p).readFloat();}
function i(p){return abs(p).readS32();}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    D=m.base.toUInt32()-IMG;
    fnMode=new NativeFunction(abs(RVA_MODE),'int',[]);
    return D;
  },
  mode:function(){ return fnMode(); },
  snap:function(){
    const cars=[];
    for(let c=0;c<NCARS;c++){
      const b=VBASE+c*VSTRIDE;
      if(i(b+0x4)===0) continue;            // inactive slot
      // 18-slot contact scan: collect active slot ids
      const scan=[];
      for(let s=0;s<0x12;s++){
        const id=i(b+0x4ac+s*0x40);
        if(id!==-1) scan.push({slot:s,id:id});
      }
      cars.push({
        car:c,
        vel:[f(b+0x9b0),f(b+0x9b4),f(b+0x9b8)],
        grounded:f(b+0x9e0),
        speed:f(b+0x9e4),
        activeContacts:i(b+0x9ec),
        scan:scan
      });
    }
    return JSON.stringify({
      mode: fnMode(),
      terrainEntryCount: i(G_TERRCNT),
      tickActiveContacts: i(G_ACTIVECNT),
      cars: cars
    });
  }
};
'''

RACE_MODES = {6, 7, 10, 0xb}


def main():
    secs = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    dev = frida.get_local_device()
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    print(f"[*] spawning {EXE}")
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    s = sess.create_script(AGENT)
    s.on("message", lambda m, d: print("MSG", m))
    s.load()
    E = s.exports_sync
    E.init(); dev.resume(pid)

    captured = None
    end = time.time() + secs
    last_mode = None
    while time.time() < end:
        try:
            m = E.mode()
        except Exception as e:
            print("[!] mode() failed (process gone?):", e); break
        if m != last_mode:
            print(f"[t={int(time.time()-(end-secs))}s] game-mode = {m}")
            last_mode = m
        if m in RACE_MODES:
            time.sleep(0.5)  # let one substep run
            series = []
            for k in range(8):              # short time series to catch transient slots
                try:
                    series.append(json.loads(E.snap()))
                except Exception as e:
                    print("[!] snap() failed:", e); break
                time.sleep(0.25)
            captured = {"snapshots": series}
            break
        time.sleep(0.5)

    # Fallback: capture whatever state exists at timeout (documents the menu-state
    # baseline if no race was reached).
    if captured is None:
        try:
            captured = json.loads(E.snap())
            captured["_note"] = "no race reached; menu/idle-state snapshot"
        except Exception:
            pass

    if captured is not None:
        OUT.write_text(json.dumps(captured, indent=2))
        print(f"[+] baseline written -> {OUT}")
        print(json.dumps(captured, indent=2)[:2000])
    else:
        print("[!] no snapshot captured")

    try: dev.kill(pid)
    except Exception: pass
    return 0 if captured is not None else 1


if __name__ == "__main__":
    sys.exit(main())
