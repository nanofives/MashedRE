# sa_capture.py, but PINNED to verify/car_bright_20260930/bin/mashed_re.exe.
#
# This session must not run mashedmod\build\mashed_re.exe (a parallel child holds
# the build slot and is rebuilding it), so the EXE constant that
# re/tools/sa_capture.py hardcodes at :13 is overridden here. Everything else is
# a verbatim copy of re/tools/sa_capture.py as of 2026-09-30 so the capture
# behaviour is identical to every earlier standalone capture in verify/.
#
# Usage: py -3.12 verify/car_bright_20260930/sa_capture_pinned.py out_prefix t1,t2,... [ENV=VAL ...]
#   extra pseudo-args: ENTER_AT=<sec>   KEYS=<t>:<name>,<t>:<name>,...
#   SA_EXE=<path> overrides the pinned exe (default: the pinned one).
import ctypes, os, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
PINNED = ROOT / "verify" / "car_bright_20260930" / "bin" / "mashed_re.exe"


def find_hwnd(pid):
    u = ctypes.windll.user32
    found = []
    proto = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    def cb(h, _):
        p = wintypes.DWORD()
        u.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u.IsWindowVisible(h):
            found.append(h)
        return True
    u.EnumWindows(proto(cb), 0)
    return found[0] if found else None


def shoot(pid, path):
    from PIL import Image
    h = find_hwnd(pid)
    if not h:
        print("  [shot] no window"); return False
    u = ctypes.windll.user32; g = ctypes.windll.gdi32
    r = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(r))
    w, ht = r.right, r.bottom
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
    print(f"  [shot] {path}")
    return True


VK = {"enter": 0x0D, "down": 0x28, "up": 0x26, "esc": 0x1B, "left": 0x25,
      "right": 0x27, "space": 0x20}


def tap_key(pid, name):
    h = find_hwnd(pid)
    if not h: return
    u = ctypes.windll.user32
    u.SetForegroundWindow(h)
    time.sleep(0.12)
    vk = VK.get(name, 0x0D)
    sc = u.MapVirtualKeyW(vk, 0)
    u.keybd_event(vk, sc, 0, 0)
    time.sleep(0.07)
    u.keybd_event(vk, sc, 2, 0)


def main():
    prefix = sys.argv[1]
    times = sorted(float(x) for x in sys.argv[2].split(","))
    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "left-bl")
    env.setdefault("MASHED_MUTE", "1")
    exe = PINNED
    enter_at = None
    keys = []
    for kv in sys.argv[3:]:
        k, _, v = kv.partition("=")
        if k == "ENTER_AT":
            enter_at = float(v)
        elif k == "SA_EXE":
            exe = Path(v)
        elif k == "KEYS":
            for item in v.split(","):
                t, _, kn = item.partition(":")
                keys.append((float(t), kn))
            keys.sort()
        else:
            env[k] = v
    if not exe.exists():
        print(f"error: {exe} not found"); return 2
    print(f"=== sa_capture_pinned exe={exe}")
    Path(prefix).parent.mkdir(parents=True, exist_ok=True)
    proc = subprocess.Popen([str(exe)], cwd=str(ROOT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(f"=== pid={proc.pid} (this session kills ONLY this pid)")
    t0 = time.time()
    tapped = False
    ki = 0
    try:
        for tcap in times:
            while time.time() - t0 < tcap:
                if enter_at is not None and not tapped and \
                        time.time() - t0 >= enter_at:
                    tap_key(proc.pid, "enter")
                    tapped = True
                while ki < len(keys) and time.time() - t0 >= keys[ki][0]:
                    print(f"  [key] t={keys[ki][0]} {keys[ki][1]}")
                    tap_key(proc.pid, keys[ki][1])
                    ki += 1
                if proc.poll() is not None:
                    print(f"exe exited early (rc={proc.returncode})")
                    return 1
                time.sleep(0.05)
            shoot(proc.pid, Path(f"{prefix}_t{int(tcap):02d}.png"))
    finally:
        try: proc.kill()
        except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
