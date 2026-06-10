# R2-close parity sweep: drive the ORIGINAL to the canonical menu screens and
# screenshot each (counterparts to the standalone's NavDemo captures).
#
# statenav.py sibling (same agent: FUN_00497310 input-override 4=confirm/11=up/
# 12=down, DAT_0067e9f8 depth / DAT_0067eca4 phase / DAT_0067ed40 sel polling,
# PrintWindow shots). Plan: title -> GTS main menu -> Options -> Sound, shooting
# each state into verify/parity/.
#
# Usage: py -3.12 re/frida/parity_shots.py
import ctypes, os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"
OUT = ROOT / "verify" / "parity"

def _find_hwnd(pid):
    u = ctypes.windll.user32; found = []
    proto = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    def cb(h, _):
        p = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u.IsWindowVisible(h):
            n = u.GetWindowTextLengthW(h)
            if n:
                b = ctypes.create_unicode_buffer(n + 1); u.GetWindowTextW(h, b, n + 1)
                if "MASHED" in b.value.upper(): found.append(h)
        return True
    u.EnumWindows(proto(cb), 0); return found[0] if found else None

def shoot(pid, path):
    try:
        from PIL import Image
        h = _find_hwnd(pid)
        if not h: return False
        u = ctypes.windll.user32; g = ctypes.windll.gdi32
        r = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(r)); w, ht = r.right, r.bottom
        if w <= 0 or ht <= 0: return False
        hdc = u.GetDC(h); md = g.CreateCompatibleDC(hdc)
        bm = g.CreateCompatibleBitmap(hdc, w, ht); g.SelectObject(md, bm)
        u.PrintWindow(h, md, 2)
        class BH(ctypes.Structure):
            _fields_ = [("biSize", wintypes.DWORD), ("biWidth", wintypes.LONG),
                        ("biHeight", wintypes.LONG), ("biPlanes", wintypes.WORD),
                        ("biBitCount", wintypes.WORD), ("biCompression", wintypes.DWORD),
                        ("biSizeImage", wintypes.DWORD), ("biXPelsPerMeter", wintypes.LONG),
                        ("biYPelsPerMeter", wintypes.LONG), ("biClrUsed", wintypes.DWORD),
                        ("biClrImportant", wintypes.DWORD)]
        bi = BH(); bi.biSize = ctypes.sizeof(BH); bi.biWidth = w; bi.biHeight = -ht
        bi.biPlanes = 1; bi.biBitCount = 32
        buf = (ctypes.c_char * (w * ht * 4))(); g.GetDIBits(md, bm, 0, ht, buf, ctypes.byref(bi), 0)
        Image.frombuffer("RGBA", (w, ht), bytes(buf), "raw", "BGRA", 0, 1).convert("RGB").save(str(path))
        g.DeleteObject(bm); g.DeleteDC(md); u.ReleaseDC(h, hdc)
        print(f"  [shot] {path}"); return True
    except Exception as e:
        print("  [shot] err", e); return False

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4, RVA_SEL=0x0067ed40;
let pressCtrl=-1, pressUntil=0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }});
    return DELTA; },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  depth:function(){ try{return abs(RVA_DEPTH).readS32();}catch(e){return -999;} },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  // The push/pop builder (FUN_0043d2a0) writes the per-depth slot arrays at the
  // PRE-increment depth, so the CURRENT screen's slot index is depth-1.
  // Highlight cursor = the DAT_0067ed80 per-depth family (the slot the nav SM
  // port models as NavSlot.cursor), not the 0x67ed40 family.
  sel:function(){ try{const d=abs(RVA_DEPTH).readS32(); return abs(0x0067ed80+(d-1)*0x40).readS32();}catch(e){return -999;} },
  screenid:function(){ try{const d=abs(RVA_DEPTH).readS32(); return abs(0x0067ed7c+(d-1)*0x40).readS32();}catch(e){return -999;} },
  // Direct cursor write: the FUN_00497310 override moves confirm but not the
  // cursor (movement reads a different input path), so set the per-depth
  // highlight cursor global directly; the draw + select handler read it live.
  setsel:function(v){ try{const d=abs(RVA_DEPTH).readS32(); abs(0x0067ed80+(d-1)*0x40).writeS32(v); return 1;}catch(e){return 0;} }
};
send({kind:'ready'});
'''

class Nav:
    def __init__(s, scr, pid): s.scr = scr; s.pid = pid
    def depth(s): return s.scr.exports_sync.depth()
    def phase(s): return s.scr.exports_sync.phase()
    def sel(s): return s.scr.exports_sync.sel()
    def screenid(s): return s.scr.exports_sync.screenid()
    def alive(s): return (not psutil) or psutil.pid_exists(s.pid)
    def press(s, c, ms=180): s.scr.exports_sync.press(c, ms); time.sleep(ms / 1000.0 + 0.3)
    def wait(s, pred, timeout=8.0, what=""):
        end = time.time() + timeout
        while time.time() < end:
            if not s.alive(): print("   process exited"); return False
            if pred(): return True
            time.sleep(0.1)
        print(f"   wait timeout: {what} (depth={s.depth()} phase={s.phase()})"); return False
    def probe_down(s):
        # Find the control id that moves the menu cursor DOWN, empirically:
        # press candidates and watch the ed80 cursor; depth-guarded.
        for c in (12, 1, 3, 13, 0, 2, 9, 10, 8, 14, 15):
            d0, s0 = s.depth(), s.sel()
            s.press(c, 350)
            time.sleep(0.3)
            if s.depth() != d0:
                print(f"   probe ctrl {c}: depth changed {d0}->{s.depth()}, recovering")
                while s.depth() > d0: s.press(5); time.sleep(0.4)
                continue
            if s.sel() != s0:
                print(f"   probe: DOWN = ctrl {c} (sel {s0}->{s.sel()})")
                return c
        print("   probe: no control moved the cursor")
        return None
    def to_sel(s, target, down_ctrl, tries=12):
        if down_ctrl is None: return s.sel() == target
        for _ in range(tries):
            if s.sel() == target: return True
            s.press(down_ctrl, 350)
            time.sleep(0.2)
        return s.sel() == target
    def confirm_to_depth(s, target, tries=6):
        for _ in range(tries):
            if s.depth() >= target: return True
            s.press(4)
            if s.wait(lambda: s.depth() >= target, 2.0): return True
        return s.depth() >= target

def main():
    OUT.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    nav = Nav(scr, pid)
    print("  booting...")
    nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 20.0, "title up")
    time.sleep(1.0)
    print(f"  title: depth={nav.depth()} phase={nav.phase()} screen={nav.screenid()}")
    shoot(pid, OUT / "orig_title.png")
    # title -> GTS main menu (depth 2) + dismiss the Load-Successful modal
    nav.confirm_to_depth(2); time.sleep(0.6); nav.press(4); time.sleep(1.2)
    print(f"  GTS: depth={nav.depth()} sel={nav.sel()} screen={nav.screenid()}")
    shoot(pid, OUT / "orig_gts.png")
    # GTS -> Options: write the cursor directly (input override only carries
    # confirm), then confirm.
    scr.exports_sync.setsel(2); time.sleep(0.5)
    print(f"  GTS sel -> {nav.sel()}")
    d0 = nav.depth(); nav.confirm_to_depth(d0 + 1)
    time.sleep(0.5)
    print(f"  options: depth={nav.depth()} sel={nav.sel()} screen={nav.screenid()}")
    shoot(pid, OUT / "orig_options.png")
    # Options -> Sound (index 0)
    scr.exports_sync.setsel(0); time.sleep(0.3)
    d0 = nav.depth(); nav.confirm_to_depth(d0 + 1)
    time.sleep(0.5)
    print(f"  sound: depth={nav.depth()} sel={nav.sel()} screen={nav.screenid()}")
    shoot(pid, OUT / "orig_sound.png")
    print("  done; killing")
    try:
        if nav.alive(): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
