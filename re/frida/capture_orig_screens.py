# Capture ORIGINAL-side reference backbuffers for a set of frontend screens.
#
# Boots original/MASHED.exe ONCE via Frida (hooks OFF: stock behavior), holds it
# at the menu, then for each requested screen id drives the REAL nav SM
# (FUN_0043d2a0) to that screen and triggers an on-demand backbuffer dump through
# the d3d9 shim (env MASHED_ORIG_BBDUMP_REQ). The dump is the post-Present client
# backbuffer (640x480), the same truth channel the standalone uses via
# MASHED_DBG_BBDUMP — so original vs standalone compare apples-to-apples (no
# window chrome, no DWM scaling).
#
# Usage: py -3.12 re/frida/capture_orig_screens.py 1 2 3 4 6 7 8 15 16 18 19 24 29 30 31 32 33
#        (no args -> the full frontend set below)
import os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
OUTDIR = ROOT / "verify" / "orig_screens"
REQ = ROOT / "log" / "orig_dump.req"

DEFAULT_SET = [1, 2, 3, 4, 6, 7, 8, 15, 16, 18, 19, 24, 29, 30, 31, 32, 33]

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
// Current-screen id global. FUN_00492e90 draws the TITLE layer (logo +
// "press button", FUN_00403050) iff this == 0x21 (title = nav screen 33).
// A bare FUN_0043d2a0 push does NOT update it, leaving the title composited
// over every pushed screen. Writing it after the push is what the real
// screen-change writers (0x0043f386/5b0/7e7) do. (See menu_draw_burst.py.)
const RVA_CURSCREEN=0x0067ecb0;
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
  depth:function(){ return abs(RVA_DEPTH).readS32(); },
  push:function(scr){ nav(scr,0); abs(RVA_CURSCREEN).writeS32(scr); return abs(RVA_DEPTH).readS32(); },
  pop:function(){ nav(0,1); return abs(RVA_DEPTH).readS32(); }
};
'''


def main():
    screens = [int(a) for a in sys.argv[1:]] or DEFAULT_SET
    OUTDIR.mkdir(parents=True, exist_ok=True)
    REQ.parent.mkdir(parents=True, exist_ok=True)
    if REQ.exists():
        REQ.unlink()

    dev = frida.get_local_device()
    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"           # stock behavior
    env["MASHED_ORIG_BBDUMP_REQ"] = str(REQ)       # arm the shim on-demand dump
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    # Wait for the frontend (phase 3 = menu interactive).
    end = time.time() + 30
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    base = E.depth()
    print(f"READY pid={pid} base_depth={base}", flush=True)

    def dump(name, settle=1.7, timeout=4.0):
        out = OUTDIR / f"{name}.bmp"
        if out.exists():
            out.unlink()
        time.sleep(settle)                          # let the slide-in settle
        REQ.write_text(str(out) + "\n", encoding="utf-8")  # shim reads line verbatim
        t = time.time() + timeout
        while time.time() < t and not out.exists():
            time.sleep(0.1)
        ok = out.exists() and out.stat().st_size > 1000
        print(f"  dump {name}: {'OK' if ok else 'MISS'} ({out})", flush=True)
        return ok

    results = {}
    for s in screens:
        # return to base depth for a clean push
        for _ in range(8):
            if E.depth() <= base:
                break
            E.pop(); time.sleep(0.15)
        d = E.push(s)
        time.sleep(0.2)
        print(f"PUSH {s} -> depth {d}", flush=True)
        results[s] = dump(f"s{s}")
        E.pop(); time.sleep(0.2)

    try: dev.kill(pid)
    except Exception: pass

    ok = sum(1 for v in results.values() if v)
    print(f"\nCAPTURED {ok}/{len(screens)}: " +
          " ".join(f"{s}{'*' if results[s] else 'x'}" for s in screens), flush=True)
    return 0 if ok == len(screens) else 1


if __name__ == "__main__":
    sys.exit(main())
