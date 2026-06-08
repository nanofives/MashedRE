# In-process menu input feeder v2 — writes the COOKED frontend edge flags directly.
#
# SAFE: writes ONLY MASHED's own global byte flags via Frida. Never calls keybd_event /
# SendInput / PostMessage / SetForegroundWindow — nothing touches the OS input queue or
# any other window. Cannot affect the host PC.
#
# Why v2: the v1 DI-buffer feed (input_feed.py) wrote the GetDeviceState 256-byte buffer
# (the correct raw stage), but the frontend's control resolver FUN_00497310 gates keyboard
# on the active player's device-type==2 AND the injected DIK matching the player's mapped
# scancode. v2 bypasses that by writing the cooked per-player edge flags the frontend tick
# reads directly. See re/analysis/frontend_input_nav_trace/TRACE.md for the full chain.
#
# Chain (verified, MASHED.exe @ image base 0x00400000):
#   FUN_004967e0 (per-frame input mgr) -> FUN_00496530(player) cooks edge flags
#     player P edge flags: 0x007f1044 + P*0x4c  (+0=left +1=right +2=up +3=down)
#   FUN_0043dfd0 (frontend tick) reads those flags -> moves cursor
#   selection index = *(int*)(0x0067ed40 + DAT_0067e9f8*0x40)   (DAT_0067e9f8 @ 0x0067e9f8)
#
# Hook point: FUN_004967e0 onLeave -> after cooking, set/clear player-0 edge flag (tap),
# and sample the selection index to prove the menu navigated.
#
# Usage: py -3.12 re/frida/input_feed_cooked.py [--seq down,down,down] [--settle MS]
#        [--seconds N] [--player P] [--count 0x..,..]
import ctypes
import os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

def find_hwnd_for_pid(target_pid):
    user32 = ctypes.windll.user32
    found = []
    proto = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    def cb(hwnd, _):
        pid = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        if pid.value == target_pid and user32.IsWindowVisible(hwnd):
            n = user32.GetWindowTextLengthW(hwnd)
            if n:
                buf = ctypes.create_unicode_buffer(n+1); user32.GetWindowTextW(hwnd, buf, n+1)
                if "MASHED" in buf.value.upper(): found.append(hwnd)
        return True
    user32.EnumWindows(proto(cb), 0)
    return found[0] if found else None

def shoot(pid, path):
    """Capture the MASHED window via PrintWindow(PW_RENDERFULLCONTENT) — works even when the
    window is occluded/not foreground (unlike a screen grab). No focus change."""
    try:
        from PIL import Image
        hwnd = find_hwnd_for_pid(pid)
        if not hwnd: print("  [shot] no hwnd"); return False
        user32 = ctypes.windll.user32; gdi32 = ctypes.windll.gdi32
        r = wintypes.RECT(); user32.GetClientRect(hwnd, ctypes.byref(r))
        w, h = r.right - r.left, r.bottom - r.top
        if w <= 0 or h <= 0: print("  [shot] bad rect"); return False
        hdc = user32.GetDC(hwnd)
        memdc = gdi32.CreateCompatibleDC(hdc)
        bmp = gdi32.CreateCompatibleBitmap(hdc, w, h)
        gdi32.SelectObject(memdc, bmp)
        PW_RENDERFULLCONTENT = 0x00000002
        ok = user32.PrintWindow(hwnd, memdc, PW_RENDERFULLCONTENT)
        # pull bits via GetDIBits (32bpp top-down)
        class BMPINFOHDR(ctypes.Structure):
            _fields_ = [("biSize", wintypes.DWORD), ("biWidth", wintypes.LONG),
                        ("biHeight", wintypes.LONG), ("biPlanes", wintypes.WORD),
                        ("biBitCount", wintypes.WORD), ("biCompression", wintypes.DWORD),
                        ("biSizeImage", wintypes.DWORD), ("biXPelsPerMeter", wintypes.LONG),
                        ("biYPelsPerMeter", wintypes.LONG), ("biClrUsed", wintypes.DWORD),
                        ("biClrImportant", wintypes.DWORD)]
        bi = BMPINFOHDR(); bi.biSize = ctypes.sizeof(BMPINFOHDR); bi.biWidth = w
        bi.biHeight = -h; bi.biPlanes = 1; bi.biBitCount = 32; bi.biCompression = 0
        buf = (ctypes.c_char * (w * h * 4))()
        gdi32.GetDIBits(memdc, bmp, 0, h, buf, ctypes.byref(bi), 0)
        img = Image.frombuffer("RGBA", (w, h), bytes(buf), "raw", "BGRA", 0, 1).convert("RGB")
        img.save(str(path))
        gdi32.DeleteObject(bmp); gdi32.DeleteDC(memdc); user32.ReleaseDC(hwnd, hdc)
        print(f"  [shot] saved {path} ({w}x{h}, PrintWindow ret={ok})")
        return True
    except Exception as e:
        print(f"  [shot] err {e}"); return False

# control index within a player's 0x4c flag block (byte offset from block base)
CTRL = {"left":0, "right":1, "up":2, "down":3}

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
const RVA_INPUTMGR = 0x004967e0;   // FUN_004967e0 (per-frame input manager)
const RVA_FLAGS    = 0x007f1044;   // player-0 edge-flag block base
const RVA_DEPTH    = 0x0067e9f8;   // DAT_0067e9f8 menu-stack depth
const RVA_SEL      = 0x0067ed40;   // selection table base (col*0x40)

let SEQ = [], SETTLE = 13000, PRESS_FRAMES = 2, GAP_FRAMES = 14, PLAYER = 0;
let ANALOG = false, ANALOG_MAG = 1.0;
let startT = 0, frame = 0, taps = 0;
const CNT = {};
let selTimeline = [];   // [{f, t, depth, ctrl, v}]
let lastSig = null;

function abs(rva){ return ptr(rva + DELTA); }

rpc.exports = {
  setplan: function(seq, settle, press, gap, player, analog, mag){
    SEQ = seq; SETTLE = settle; PRESS_FRAMES = press; GAP_FRAMES = gap; PLAYER = player;
    ANALOG = !!analog; if (mag) ANALOG_MAG = mag;
    startT = Date.now(); return 1; },
  taps: function(){ return taps; },
  timeline: function(){ return selTimeline; },
  countthese: function(rvas){ rvas.forEach(function(r){ const a = parseInt(r,16); CNT[r]=0;
    try{ Interceptor.attach(abs(a),{onEnter(){CNT[r]++;}}); }catch(e){ CNT[r]=-1; } });
    return Object.keys(CNT).length; },
  counts: function(){ return CNT; }
};

// broaden: sample several candidate selection globals so we can see which (if any) moves
const PROBES = [0x0067ed40,0x0067ed44,0x0067ed74,0x0067ed78,0x0067ed7c,0x0067ed80,0x0067ece0,0x008990e4];
function readSel(){
  try {
    const depth = abs(RVA_DEPTH).readS32();
    const v = {};
    PROBES.forEach(function(p){ try { v['0x'+p.toString(16)] = abs(p + depth*0x40).readS32(); } catch(e){ v['0x'+p.toString(16)]=-999; } });
    // signature = depth + all probe values joined, to detect ANY change
    const sig = depth + ':' + PROBES.map(function(p){ return v['0x'+p.toString(16)]; }).join(',');
    return {depth:depth, sel:v['0x67ed40'], sig:sig, v:v};
  } catch(e){ return {depth:-999, sel:-999, sig:'err'}; }
}

function curCtrl(){
  if (!SEQ.length) return -1;
  const t = Date.now() - startT - SETTLE;
  if (t < 0) return -1;
  // schedule by FRAME count instead of wall time for stable edges
  const period = PRESS_FRAMES + GAP_FRAMES;
  const sinceSettle = frame - settleFrame;
  if (sinceSettle < 0) return -1;
  const idx = Math.floor(sinceSettle / period);
  if (idx >= SEQ.length) return -1;
  const phase = sinceSettle - idx*period;
  return (phase < PRESS_FRAMES) ? SEQ[idx] : -1;
}

let settleFrame = -1;
function onMgrLeave(){
  frame++;
  const t = Date.now() - startT;
  if (settleFrame < 0 && t >= SETTLE) settleFrame = frame;
  // DECISIVE TEST: blast the chosen control across ALL representations & players.
  //   edge flag:      0x7f1044 + p*0x4c + ctrl
  //   working buffer: 0x7f1038 + p*0x4c + ctrl   (raw mapped value the cook also writes)
  // ctrl 0=L 1=R 2=U 3=D. Covers whichever byte the directional consumer reads.
  const ctrl = curCtrl();
  if (ctrl >= 0){
    if (ANALOG){
      // directional via the analog stick fields (FUN_00496530 writes these from joy axes):
      //   x = 0x7f104c + p*0x4c, y = 0x7f1050 + p*0x4c (floats). ctrl 0=L 1=R 2=U 3=D.
      const ax = (ctrl===0?-1:ctrl===1?1:0) * ANALOG_MAG;
      const ay = (ctrl===2?-1:ctrl===3?1:0) * ANALOG_MAG;
      for (let p = 0; p < 4; p++){
        try { abs(0x007f104c + p*0x4c).writeFloat(ax); } catch(e){}
        try { abs(0x007f1050 + p*0x4c).writeFloat(ay); } catch(e){}
      }
    } else {
      for (let p = 0; p < 4; p++){
        try { abs(0x007f1044 + p*0x4c + ctrl).writeU8(0xff); } catch(e){}
        try { abs(0x007f1038 + p*0x4c + ctrl).writeU8(0x7f); } catch(e){}
      }
    }
    taps++;
  }
  // sample selection probes, log on ANY change
  const s = readSel();
  if (s.sig !== lastSig){
    selTimeline.push({f:frame, t:t, depth:s.depth, ctrl:ctrl, v:s.v});
    lastSig = s.sig;
  }
}

function hook(){
  try { Interceptor.attach(abs(RVA_INPUTMGR), { onLeave(){ onMgrLeave(); } });
        send({kind:'info', msg:'hooked input mgr @'+abs(RVA_INPUTMGR)}); return true; }
  catch(e){ send({kind:'err', msg:'hook '+e}); return false; }
}
// self-resolve module base (main module is mapped even when spawned suspended) and arm
(function(){
  try {
    const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMGBASE;
    send({kind:'info', msg:'MASHED.exe base='+m.base+' DELTA=0x'+DELTA.toString(16)});
    hook();
  } catch(e){ send({kind:'err', msg:'base '+e}); }
})();
'''

def main():
    seq_names = (sys.argv[sys.argv.index("--seq")+1] if "--seq" in sys.argv else "down,down,down,down").split(",")
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 13000
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 32
    player = int(sys.argv[sys.argv.index("--player")+1]) if "--player" in sys.argv else 0
    count_rvas = sys.argv[sys.argv.index("--count")+1].split(",") if "--count" in sys.argv else []
    seq = [CTRL[s.strip().lower()] for s in seq_names if s.strip()]

    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"   # stock, no dev .asi
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    state = {"base": None}
    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "info": print("  [feed]", p["msg"])
        elif p.get("kind") == "err": print("  [err]", p["msg"])
        elif p.get("kind") == "need_base": state["base"] = True
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()  # arms hook at load

    if count_rvas:
        scr.exports_sync.countthese(count_rvas)
    analog = "--analog" in sys.argv
    mag = float(sys.argv[sys.argv.index("--mag")+1]) if "--mag" in sys.argv else 1.0
    scr.exports_sync.setplan(seq, settle, 2, 14, player, analog, mag)
    dev.resume(pid)
    print(f"  resumed; feeding seq={seq_names} player={player} after {settle}ms "
          f"({'ANALOG axes' if analog else 'digital flags'} mag={mag}, in-process)")

    shot = sys.argv[sys.argv.index("--shot")+1] if "--shot" in sys.argv else None
    shot_at = float(sys.argv[sys.argv.index("--shotat")+1]) if "--shotat" in sys.argv else None
    t = time.time() + seconds
    shot_done = False
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        if shot and not shot_done and (shot_at is None or (time.time() - (t - seconds)) >= shot_at):
            if shot_at is not None:
                shoot(pid, ROOT / shot); shot_done = True
        time.sleep(0.5)
    if shot and not shot_done:
        shoot(pid, ROOT / shot)

    taps = 0; counts = {}; tl = []
    try: taps = scr.exports_sync.taps()
    except Exception: pass
    try: tl = scr.exports_sync.timeline()
    except Exception: pass
    if count_rvas:
        try: counts = scr.exports_sync.counts()
        except Exception: pass
    print(f"\n  flag-writes (taps) applied: {taps}")
    print(f"  selection-probe timeline (on ANY change; ctrl 0=L 1=R 2=U 3=D, -1=none):")
    for e in tl[:80]:
        print(f"    f={e['f']:>5} t={e.get('t',0):>6}ms depth={e['depth']} ctrl={e.get('ctrl')} v={e.get('v')}")
    if count_rvas:
        print(f"  nav-handler counts: {counts}")
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
