# Drive menu input through the LEGITIMATE pipeline by overriding the per-control resolver
# FUN_00497310(player, control) -> 0xff for chosen (player,control). OBSERVATION+in-process
# return-value override only; writes NO globals, no OS input. The game's own cook
# (FUN_00496530) then produces the edge flags / working bytes / edge-pair exactly as for a
# real keypress, so every downstream gate is satisfied naturally.
#
# Two modes:
#   --probe          : log distinct (esp+4, esp+8, ecx, edx) at FUN_00497310 entry to learn
#                      the calling convention + (player,control) index space. No override.
#   --force P:C[,P:C] : force return 0xff for those (player,control) pairs during scheduled
#                      press windows; observe selection index + depth + push/pop.
#
# Usage: py -3.12 re/frida/input_resolver_drive.py --probe [--settle MS] [--seconds N]
#        py -3.12 re/frida/input_resolver_drive.py --force 0:12 --settle 8000 --seconds 18
import ctypes
import os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

def _find_hwnd(target_pid):
    user32 = ctypes.windll.user32; found = []
    proto = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    def cb(hwnd, _):
        pid = wintypes.DWORD(); user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        if pid.value == target_pid and user32.IsWindowVisible(hwnd):
            n = user32.GetWindowTextLengthW(hwnd)
            if n:
                buf = ctypes.create_unicode_buffer(n+1); user32.GetWindowTextW(hwnd, buf, n+1)
                if "MASHED" in buf.value.upper(): found.append(hwnd)
        return True
    user32.EnumWindows(proto(cb), 0)
    return found[0] if found else None

def shoot(pid, path):
    """PrintWindow capture (works when occluded; no focus change)."""
    try:
        from PIL import Image
        hwnd = _find_hwnd(pid)
        if not hwnd: print("  [shot] no hwnd"); return False
        user32 = ctypes.windll.user32; gdi32 = ctypes.windll.gdi32
        r = wintypes.RECT(); user32.GetClientRect(hwnd, ctypes.byref(r))
        w, h = r.right - r.left, r.bottom - r.top
        if w <= 0 or h <= 0: print("  [shot] bad rect"); return False
        hdc = user32.GetDC(hwnd); memdc = gdi32.CreateCompatibleDC(hdc)
        bmp = gdi32.CreateCompatibleBitmap(hdc, w, h); gdi32.SelectObject(memdc, bmp)
        user32.PrintWindow(hwnd, memdc, 0x00000002)
        class BH(ctypes.Structure):
            _fields_ = [("biSize", wintypes.DWORD), ("biWidth", wintypes.LONG),
                        ("biHeight", wintypes.LONG), ("biPlanes", wintypes.WORD),
                        ("biBitCount", wintypes.WORD), ("biCompression", wintypes.DWORD),
                        ("biSizeImage", wintypes.DWORD), ("biXPelsPerMeter", wintypes.LONG),
                        ("biYPelsPerMeter", wintypes.LONG), ("biClrUsed", wintypes.DWORD),
                        ("biClrImportant", wintypes.DWORD)]
        bi = BH(); bi.biSize = ctypes.sizeof(BH); bi.biWidth = w; bi.biHeight = -h
        bi.biPlanes = 1; bi.biBitCount = 32; bi.biCompression = 0
        buf = (ctypes.c_char * (w * h * 4))()
        gdi32.GetDIBits(memdc, bmp, 0, h, buf, ctypes.byref(bi), 0)
        Image.frombuffer("RGBA", (w, h), bytes(buf), "raw", "BGRA", 0, 1).convert("RGB").save(str(path))
        gdi32.DeleteObject(bmp); gdi32.DeleteDC(memdc); user32.ReleaseDC(hwnd, hdc)
        print(f"  [shot] saved {path} ({w}x{h})"); return True
    except Exception as e:
        print(f"  [shot] err {e}"); return False

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
const RVA_RESOLVER = 0x00497310;
const RVA_DEPTH    = 0x0067e9f8;
const RVA_SEL      = 0x0067ed40;
let MODE = 'probe';
let FORCE = {};        // "player:control" -> true
let SETTLE = 8000, t0 = Date.now();
let PRESS_MS = 120, GAP_MS = 380;   // tap timing
const probe = {};      // dedup entry-arg signatures
let probeN = 0;
let forced = 0, lastSwept = -1, lastConfirmSlot = -99;
const selTL = []; let lastSig = null;

function abs(rva){ return ptr(rva + DELTA); }
rpc.exports = {
  setmode: function(mode, force, settle){ MODE = mode; FORCE = force||{}; SETTLE = settle; t0 = Date.now(); return 1; },
  setsweep: function(list, dwell){ if (list && list.length) SWEEP_LIST = list; if (dwell) SWEEP_DWELL = dwell; return SWEEP_LIST.length; },
  probe: function(){ return probe; },
  forced: function(){ return forced; },
  timeline: function(){ return selTL; },
  dumpmap: function(){
    // per-player binding: scancode map (&DAT_007e96c8)[p*0x80 + c] dword @ 0x7e96c8+(p*0x80+c)*4
    // device type (&DAT_007e96fc)[p*0x80] @ 0x7e96fc + p*0x80*4
    const out = {};
    for (let p = 0; p < 4; p++){
      let typ = -1; try { typ = abs(0x007e96fc + p*0x80*4).readS32(); } catch(e){}
      const ctrls = [];
      for (let c = 0; c < 13; c++){ let v=-1; try { v = abs(0x007e96c8 + (p*0x80 + c)*4).readS32(); } catch(e){} ctrls.push(v); }
      out['p'+p] = {type: typ, scancodes: ctrls};
    }
    return out;
  }
};

function readSel(){
  try { const d = abs(RVA_DEPTH).readS32();
        const s = abs(RVA_SEL + d*0x40).readS32();
        const s80 = abs(RVA_SEL + 0x40 + d*0x40).readS32();
        return {d:d, s:s, s80:s80}; }
  catch(e){ return {d:-999,s:-999,s80:-999}; }
}
function sample(extra){
  const v = readSel(); const sig = v.d+':'+v.s+':'+v.s80;
  if (sig !== lastSig){ selTL.push({t:Date.now()-t0, d:v.d, s:v.s, s80:v.s80, why:extra||''}); lastSig = sig; }
}

// tap schedule: which control is "pressed" right now (by wall time after settle)
function pressedNow(){
  const tt = Date.now() - t0 - SETTLE;
  if (tt < 0) return false;
  const period = PRESS_MS + GAP_MS;
  return (tt % period) < PRESS_MS;
}
// sweep: which player-0 control index is under test right now (1.2s dwell each, 0..12),
// tapped within its dwell so edge-detect sees press->release.
let SWEEP_DWELL = 1500;
// control 4 = confirm. Fire it TWICE (advance title->menu, then dismiss the "Load Successful"
// modal), THEN sweep the directional candidates on the real Game Type Select menu.
let SWEEP_LIST = [4,4,0,1,2,3,9,10,11,12];
function sweepControl(){
  const tt = Date.now() - t0 - SETTLE;
  if (tt < 0) return -1;
  const k = Math.floor(tt / SWEEP_DWELL);
  if (k >= SWEEP_LIST.length) return -1;
  // tap within dwell: press 200ms, release 250ms, repeat (clean edges)
  const within = tt - k*SWEEP_DWELL;
  const on = (within % 450) < 200;
  return on ? SWEEP_LIST[k] : -1;
}

(function(){
  try { const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
        DELTA = m.base.toUInt32() - IMGBASE; send({kind:'info', msg:'base='+m.base}); }
  catch(e){ send({kind:'err', msg:'base '+e}); return; }

  Interceptor.attach(abs(RVA_RESOLVER), {
    onEnter(a){
      const sp = this.context.esp;
      let e4=0,e8=0;
      try { e4 = sp.add(4).readS32(); e8 = sp.add(8).readS32(); } catch(e){}
      const ecx = this.context.ecx.toInt32(), edx = this.context.edx.toInt32();
      this.e4=e4; this.e8=e8; this.ecx=ecx; this.edx=edx;
      if (MODE === 'probe' && probeN < 4000){
        const key = e4+'/'+e8+'/'+ecx+'/'+edx;
        if (!probe[key]){ probe[key] = {esp4:e4, esp8:e8, ecx:ecx, edx:edx, n:0}; }
        probe[key].n++; probeN++;
      }
    },
    onLeave(ret){
      if (MODE === 'force'){
        if (FORCE[this.e4+':'+this.e8] && pressedNow()){ ret.replace(ptr(0xff)); forced++; }
      } else if (MODE === 'sweep'){
        const k = sweepControl();
        if (k >= 0 && this.e4 === 0 && this.e8 === k){
          // fire each control exactly ONCE per dwell-slot (one clean step per slot), so a
          // scripted SWEEP_LIST like [4,4,12,12] = confirm,confirm,down,down is deterministic.
          const slot = Math.floor((Date.now()-t0-SETTLE)/SWEEP_DWELL);
          if (slot === lastConfirmSlot) return;
          lastConfirmSlot = slot;
          ret.replace(ptr(0xff)); forced++;
          if (lastSwept !== k){ sample('sweep ctrl '+k); lastSwept = k; } }
      }
    }
  });
  send({kind:'info', msg:'resolver hooked @'+abs(RVA_RESOLVER)});

  // periodic selection sampler
  setInterval(function(){ sample('tick'); }, 60);
})();
'''

def main():
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 8000
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 18
    mode = "probe"; force = {}
    if "--dumpmap" in sys.argv:
        mode = "dumpmap"
    elif "--sweep" in sys.argv:
        mode = "sweep"
    elif "--force" in sys.argv:
        mode = "force"
        for pair in sys.argv[sys.argv.index("--force")+1].split(","):
            p, c = pair.split(":"); force[f"{int(p)}:{int(c)}"] = True
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "info": print("  [drive]", p["msg"])
        elif p.get("kind") == "err": print("  [err]", p["msg"])
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    scr.exports_sync.setmode(mode, force, settle)
    if "--list" in sys.argv:
        lst = [int(x) for x in sys.argv[sys.argv.index("--list")+1].split(",")]
        dwell = int(sys.argv[sys.argv.index("--dwell")+1]) if "--dwell" in sys.argv else 0
        scr.exports_sync.setsweep(lst, dwell)
    dev.resume(pid)
    print(f"  resumed; mode={mode} force={force} settle={settle}ms seconds={seconds}")
    shot = sys.argv[sys.argv.index("--shot")+1] if "--shot" in sys.argv else None
    shotat = float(sys.argv[sys.argv.index("--shotat")+1]) if "--shotat" in sys.argv else None
    start = time.time(); t = start + seconds; shot_done = False
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        if shot and not shot_done and shotat is not None and (time.time()-start) >= shotat:
            shoot(pid, ROOT / shot); shot_done = True
        time.sleep(0.5)
    if shot and not shot_done:
        shoot(pid, ROOT / shot)
    DIK = {0x01:"ESC",0x1c:"ENTER",0x1d:"LCTRL",0x2a:"LSHIFT",0x38:"LALT",0x39:"SPACE",
           0x48:"NUM8",0x4b:"NUM4",0x4d:"NUM6",0x50:"NUM2",
           0xc8:"UP",0xd0:"DOWN",0xcb:"LEFT",0xcd:"RIGHT",0x9c:"NUMENTER",0x0e:"BACKSPACE",
           0x11:"W",0x1f:"S",0x1e:"A",0x20:"D",0x10:"Q",0x12:"E",0xff:"(unset)",0:"(none)"}
    if mode == "dumpmap":
        mp = {}
        try: mp = scr.exports_sync.dumpmap()
        except Exception as e: print("  dumpmap err", e)
        print(f"\n  === per-player input binding (FUN_00497310 scancode map) ===")
        print(f"  control idx -> DIK scancode (name). type: 1=joystick 2=keyboard 0=none")
        for pk in sorted(mp.keys()):
            pv = mp[pk]
            print(f"  {pk}: type={pv['type']}")
            for c, sc in enumerate(pv["scancodes"]):
                nm = DIK.get(sc & 0xff, "")
                print(f"      ctrl[{c:>2}] = 0x{sc & 0xffffffff:08x}  scancode 0x{sc & 0xff:02x} {nm}")
    elif mode == "probe":
        pr = {}
        try: pr = scr.exports_sync.probe()
        except Exception: pass
        rows = sorted(pr.values(), key=lambda r: -r["n"])
        print(f"\n  === FUN_00497310 entry args (distinct), top 40 by freq ===")
        print(f"  {'esp+4':>8} {'esp+8':>8} {'ecx':>8} {'edx':>8}    n")
        for r in rows[:40]:
            print(f"  {r['esp4']:>8} {r['esp8']:>8} {r['ecx']:>8} {r['edx']:>8}    {r['n']}")
    else:
        forced = 0; tl = []
        try: forced = scr.exports_sync.forced()
        except Exception: pass
        try: tl = scr.exports_sync.timeline()
        except Exception: pass
        print(f"\n  forced returns ({mode}): {forced}")
        print(f"  selection timeline (t, depth, sel@d, sel@d+1) on change:")
        for e in tl[:80]:
            print(f"    t={e['t']:>6}ms d={e['d']} s={e['s']} s80={e['s80']} {e['why']}")
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
