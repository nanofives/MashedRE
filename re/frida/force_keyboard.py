# Runtime FORCE-KEYBOARD-ONLY (Frida variant of patch_mashed_force_keyboard.py).
# Zeroes DAT_00772fac (the joystick count at 0x772fac) so the per-frame joypad poll loop
# FUN_00495fe0 iterates 0 devices and never calls the crashing poll FUN_00495870.
#
# Timing: the enum FUN_00496040 sets the count at boot. For a clean keyboard-only state the
# binary patch is preferred (it runs before FUN_00498510 assigns devices). This attach
# variant zeroes the count to STOP THE POLL even if attached after boot (the device may stay
# assigned but is never polled -> no crash). Hooks FUN_00496040 too so it stays 0 if the
# enum re-runs. By explicit PID; never kills.
#
# Usage: py -3.12 re/frida/force_keyboard.py --pid <PID>
import sys, time, argparse
import frida

AGENT = r'''
'use strict';
const m = Process.findModuleByName('MASHED.exe');
const D = m.base.sub(0x400000);              // ASLR delta (0 if based at 0x400000)
const CNT = D.add(0x772fac);                 // DAT_00772fac (joystick count)
const ENUM = D.add(0x496040);                // FUN_00496040 (joypad enumerator)
function zero(){ try{ const before = CNT.readU32(); if(before!==0){ CNT.writeU32(0); send({kind:'zeroed', before:before}); } }catch(e){} }
zero();                                       // immediate (already enumerated case)
try{ Interceptor.attach(ENUM, { onLeave(){ zero(); } }); send({kind:'hooked'}); }catch(e){ send({kind:'err', msg:''+e}); }
send({kind:'ready', count:CNT.readU32()});
'''

def main():
    ap = argparse.ArgumentParser(); ap.add_argument("--pid", type=int, required=True)
    ap.add_argument("--seconds", type=int, default=30)
    args = ap.parse_args()
    dev = frida.get_local_device(); sess = dev.attach(args.pid)
    def on(m, d):
        if m.get("type") == "error": print("  err", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "ready": print(f"  armed; joystick count now = {p.get('count')}")
        elif p.get("kind") == "zeroed": print(f"  forced joystick count {p.get('before')} -> 0 (keyboard-only)")
        elif p.get("kind") == "hooked": print("  hooked FUN_00496040 (keeps count 0)")
        elif p.get("kind") == "err": print("  agent err:", p.get("msg"))
    scr = sess.create_script(AGENT); scr.on("message", on); scr.load()
    print(f"  holding {args.seconds}s (joystick poll suppressed)...")
    time.sleep(args.seconds)
    sess.detach()
    return 0

if __name__ == "__main__":
    sys.exit(main())
