# DirectInput keyboard recon — OBSERVATION ONLY (no writes, no OS input).
#
# Confirms the in-process path MASHED reads keyboard state through, as the
# foundation for the safe in-process input feeder (CANONICAL_INPUT_DESIGN.md).
# Walks the COM chain at spawn:
#   DirectInput8Create -> IDirectInput8::CreateDevice(idx 3) -> capture each
#   IDirectInputDevice8 -> IDirectInputDevice8::GetDeviceState(idx 9).
# On GetDeviceState (cbData==256 => keyboard immediate state), it LOGS which
# DIK_* bytes are set (0x80). It NEVER writes the buffer or injects input.
#
# Spawns MASHED WITHOUT the dev .asi (MASHED_RE_NO_AUTO_HOOK=1) — pure stock input.
#
# Usage: py -3.12 re/frida/input_recon.py [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
// DIK codes we care about for menu nav (for readable logging only).
const DIK = {0x01:'ESC',0x1C:'ENTER',0xC8:'UP',0xD0:'DOWN',0xCB:'LEFT',0xCD:'RIGHT',0x39:'SPACE'};
const seenDev = {};
let gdsHooked = 0, createDevHooked = false, di8Hooked = false;
let lastReport = 0;

function hookGetDeviceState(devVtable, devPtr) {
  try {
    const gds = devVtable.add(9*4).readPointer();   // IDirectInputDevice8::GetDeviceState = vtable[9]
    const key = gds.toString();
    if (seenDev[key]) return; seenDev[key] = 1;
    Interceptor.attach(gds, {
      onEnter(a){ this.cb = a[1].toInt32(); this.buf = a[2]; },
      onLeave(ret){
        if (this.cb !== 256) return;               // 256 => keyboard immediate state
        // OBSERVE ONLY: report set keys at most ~2x/sec
        const now = Date.now();
        if (now - lastReport < 500) return; lastReport = now;
        let down = [];
        try { for (let i=0;i<256;i++){ if ((this.buf.add(i).readU8() & 0x80)) down.push(DIK[i]||('0x'+i.toString(16))); } } catch(e){}
        send({kind:'gds', cb:this.cb, gds:gds.toString(), down:down});
      }
    });
    gdsHooked++;
    send({kind:'info', msg:'hooked GetDeviceState @'+gds+' (dev '+devPtr+'); total='+gdsHooked});
  } catch(e){ send({kind:'err', where:'hookGDS', msg:''+e}); }
}

function hookCreateDevice(di8Vtable) {
  if (createDevHooked) return; createDevHooked = true;
  try {
    const cd = di8Vtable.add(3*4).readPointer();    // IDirectInput8::CreateDevice = vtable[3]
    Interceptor.attach(cd, {
      onEnter(a){ this.out = a[2]; },               // a[0]=this a[1]=rguid a[2]=lplpDevice a[3]=punk
      onLeave(ret){
        try { const dev = this.out.readPointer();
              if (!dev.isNull()) hookGetDeviceState(dev.readPointer(), dev); } catch(e){}
      }
    });
    send({kind:'info', msg:'hooked CreateDevice @'+cd});
  } catch(e){ send({kind:'err', where:'hookCD', msg:''+e}); }
}

function tryHookDI8() {
  if (di8Hooked) return true;
  let ex = null;
  for (const m of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']) {   // frida 17 API
    try { const mod = Process.findModuleByName(m); if (mod){ ex = mod.findExportByName('DirectInput8Create'); if (ex) break; } } catch(e){}
  }
  if (!ex) return false;
  di8Hooked = true;
  Interceptor.attach(ex, {
    onEnter(a){ this.ppv = a[3]; },                 // DirectInput8Create(hinst,ver,riid,ppvOut,punk)
    onLeave(ret){
      try { const di8 = this.ppv.readPointer();
            if (!di8.isNull()) hookCreateDevice(di8.readPointer()); } catch(e){}
    }
  });
  send({kind:'info', msg:'hooked DirectInput8Create'});
  return true;
}

// DirectInput8Create may fire very early; retry until the module is present.
if (!tryHookDI8()) {
  const iv = setInterval(function(){ if (tryHookDI8()) clearInterval(iv); }, 50);
}
send({kind:'ready'});
'''

def main():
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 25
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"  # no dev .asi hooks
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    log = []
    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        k = p.get("kind")
        if k == "info": print("  [hook]", p["msg"])
        elif k == "err": print("  [err]", p.get("where"), p.get("msg"))
        elif k == "gds":
            log.append(p)
            print(f"  [GetDeviceState] cb={p['cb']} down={p['down']}")
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    dev.resume(pid)
    print(f"  resumed; observing keyboard reads for {seconds}s (NO writes)")
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  process exited"); break
        time.sleep(0.25)
    gds_calls = len(log)
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    print(f"\n=== recon result: {gds_calls} keyboard GetDeviceState samples observed ===")
    print("  -> path confirmed" if gds_calls > 0 else "  -> NO keyboard GetDeviceState seen (path differs; investigate)")
    return 0

if __name__ == "__main__":
    sys.exit(main())
