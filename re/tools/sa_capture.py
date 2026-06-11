# Capture screenshots of the STANDALONE mashed_re.exe (track-view/race modes).
# Spawns the exe with the given env vars from the repo root, PrintWindow-
# captures at the requested times, then kills it.
#
# Usage: py -3.12 re/tools/sa_capture.py out_prefix t1,t2,... [ENV=VAL ...]
# e.g.:  py -3.12 re/tools/sa_capture.py verify/parity3d/sa_cam 6,12,20 \
#            MASHED_TRACK_VIEW=Arctic MASHED_CAR=1 MASHED_ROUND=1
import ctypes, os, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"


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


VK = {"enter": 0x0D, "down": 0x28, "up": 0x26, "esc": 0x1B}


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


def tap_enter(pid):
    tap_key(pid, "enter")


def main():
    prefix = sys.argv[1]
    times = sorted(float(x) for x in sys.argv[2].split(","))
    env = dict(os.environ)
    enter_at = None
    keys = []
    for kv in sys.argv[3:]:
        k, _, v = kv.partition("=")
        if k == "ENTER_AT":
            enter_at = float(v)
        elif k == "KEYS":
            for item in v.split(","):
                t, _, kn = item.partition(":")
                keys.append((float(t), kn))
            keys.sort()
        else:
            env[k] = v
    proc = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    t0 = time.time()
    tapped = False
    ki = 0
    try:
        for tcap in times:
            while time.time() - t0 < tcap:
                if enter_at is not None and not tapped and \
                        time.time() - t0 >= enter_at:
                    tap_enter(proc.pid)
                    tapped = True
                while ki < len(keys) and time.time() - t0 >= keys[ki][0]:
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
