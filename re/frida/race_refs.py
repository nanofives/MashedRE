# Reconciliation: capture ORIGINAL in-race reference frames for the 3D
# divergence ledger. statenav/harvest_handling sibling: drive to Quick Battle
# (Arctic is the default track on a fresh save) and screenshot moments of the
# round into verify/parity3d/.
#
# Usage: py -3.12 re/frida/race_refs.py
import ctypes, os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
OUT = ROOT / "verify" / "parity3d"

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
            _fields_ = [("a", wintypes.DWORD), ("b", wintypes.LONG), ("c", wintypes.LONG),
                        ("d", wintypes.WORD), ("e", wintypes.WORD), ("f", wintypes.DWORD),
                        ("g", wintypes.DWORD), ("h2", wintypes.LONG), ("i", wintypes.LONG),
                        ("j", wintypes.DWORD), ("k", wintypes.DWORD)]
        bi = BH(); bi.a = ctypes.sizeof(BH); bi.b = w; bi.c = -ht; bi.d = 1; bi.e = 32
        buf = (ctypes.c_char * (w * ht * 4))()
        g.GetDIBits(md, bm, 0, ht, buf, ctypes.byref(bi), 0)
        Image.frombuffer("RGBA", (w, ht), bytes(buf), "raw", "BGRA", 0, 1)\
             .convert("RGB").save(str(path))
        g.DeleteObject(bm); g.DeleteDC(md); u.ReleaseDC(h, hdc)
        print(f"  [shot] {path}"); return True
    except Exception as e:
        print("  [shot] err", e); return False

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
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
  setsel:function(v){ try{const d=abs(RVA_DEPTH).readS32(); abs(0x0067ed80+(d-1)*0x40).writeS32(v); return 1;}catch(e){return 0;} }
};
send({kind:'ready'});
'''


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    def wait(pred, timeout, what):
        end = time.time() + timeout
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        print(f"timeout: {what} depth={E.depth()} phase={E.phase()}")
        return False

    def press(c, ms=180):
        E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)

    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target: return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0, f"d{target}"): return True
        return E.depth() >= target

    print("booting...")
    wait(lambda: E.phase() == 3 and E.depth() >= 1, 20, "title")
    time.sleep(1.0)
    confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
    confirm_to(3)
    E.setsel(1); time.sleep(0.3)        # Quick Battle
    confirm_to(4, 4)
    confirm_to(5, 4)
    press(4); time.sleep(1.5)
    for _ in range(5):
        if E.phase() != 3: break
        press(4); time.sleep(1.5)
    print(f"in race? phase={E.phase()}")
    time.sleep(1.0)
    shoot(pid, OUT / "orig_race_t01.png")
    # let the AI round play; capture several moments
    for i, dwell in enumerate((4, 4, 5, 6), start=2):
        time.sleep(dwell)
        shoot(pid, OUT / f"orig_race_t{i:02d}.png")
    try: dev.kill(pid)
    except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
