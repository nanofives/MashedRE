# Frontend-faithfulness reference capture: boot the ORIGINAL to the main menu
# and record (a) a 40-frame burst at ~50 ms spacing (animation reference:
# wavy logo, highlight pulses, background motion) and (b) the live RW camera
# world matrix per frame (the camera that renders Frontend.piz/MAIN.BSP — the
# menu's 3D background world), read through the camera-struct chain
# 0x00897fe0+0x84 -> RwCamera -> +4 frame -> +0x10 modelling matrix
# (chain per Camera::Apply 0x00441760, re/analysis/race_camera/race_camera.md).
#
# Outputs: verify/frontend_ref/menu_burst_NN.png + menu_burst_cam.json
#
# Usage: py -3.12 re/frida/menu_burst.py
import ctypes, json, os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
OUT = ROOT / "verify" / "frontend_ref"


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
        return True
    except Exception as e:
        print("  [shot] err", e); return False


AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_PHASE=0x0067eca4, RVA_DEPTH=0x0067e9f8, CAM=0x00897fe0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG; return DELTA; },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  depth:function(){ try{return abs(RVA_DEPTH).readS32();}catch(e){return -999;} },
  cam:function(){ try{
    const rwcam = abs(CAM+0x84).readU32();
    if (!rwcam) return null;
    const frame = ptr(rwcam).add(4).readU32();
    if (!frame) return null;
    const m = ptr(frame).add(0x10);   // RwFrame modelling matrix
    const out = [];
    for (let i=0;i<16;i++) out.push(m.add(i*4).readFloat());
    return JSON.stringify(out);
  }catch(e){return null;} }
};
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

    print("waiting for menu...")
    end = time.time() + 25
    while time.time() < end:
        if E.phase() == 3:
            break
        time.sleep(0.2)
    print(f"phase={E.phase()} depth={E.depth()}; settling 2s...")
    time.sleep(2.0)

    cams = []
    n_ok = 0
    t0 = time.time()
    for i in range(40):
        target = t0 + i * 0.05
        while time.time() < target:
            time.sleep(0.005)
        ok = shoot(pid, OUT / f"menu_burst_{i:02d}.png")
        c = E.cam()
        cams.append({"i": i, "t": round(time.time() - t0, 3),
                     "shot": bool(ok), "cam": json.loads(c) if c else None})
        if ok: n_ok += 1
    (OUT / "menu_burst_cam.json").write_text(json.dumps(cams, indent=1))
    print(f"captured {n_ok}/40 frames + camera matrices -> {OUT}")
    try: dev.kill(pid)
    except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
