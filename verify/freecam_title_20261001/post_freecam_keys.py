# T5c sanity arm — post the FORMER free-fly inputs at the spawned window.
#
# Posts WM_KEYDOWN/WM_KEYUP for W, A, S, D, Q, E, R and a right-button drag
# (WM_RBUTTONDOWN + WM_MOUSEMOVE sweep + WM_RBUTTONUP) to the HWND belonging to
# the PID we spawned. PostMessage only, never SendInput, so no focus is taken
# and no key leaks into another session's game.
#
# READ THE PRE-REGISTRATION BEFORE INTERPRETING THIS: the standalone reads the
# keyboard through DirectInput GetDeviceState (exe_main.cpp:1023), which window
# messages do not write, and this arm additionally runs under
# MASHED_DETERMINISTIC (which suppressed live input by design even before
# a9da810a). So an "identical backbuffer" result here is a null-input-channel
# GREEN: it cannot pass or fail T5. It is recorded for completeness only.
#
# Usage: py -3.12 verify/freecam_title_20261001/post_freecam_keys.py <label> <seconds> [ENV=VAL ...]
import ctypes, json, os, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"
OUT = Path(__file__).resolve().parent

WM_KEYDOWN, WM_KEYUP = 0x0100, 0x0101
WM_MOUSEMOVE, WM_RBUTTONDOWN, WM_RBUTTONUP = 0x0200, 0x0204, 0x0205
MK_RBUTTON = 0x0002
# The exact inputs a9da810a deleted: WASD move, Q/E down/up, R reset-orbit.
KEYS = [ord(c) for c in "WASDQER"]


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


def main():
    label, dur = sys.argv[1], float(sys.argv[2])
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    applied = {}
    for a in sys.argv[3:]:
        k, _, v = a.partition("=")
        env[k] = v
        applied[k] = v

    u = ctypes.windll.user32
    proc = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env)
    pid = proc.pid
    print(f"[{label}] spawned pid={pid}")
    rec = {"label": label, "pid": pid, "env_applied": applied,
           "posts": 0, "drag_sweeps": 0, "hwnd": None}
    t0 = time.time()
    hwnd = None
    try:
        while time.time() - t0 < dur:
            if proc.poll() is not None:
                rec["exit_code"] = proc.returncode
                rec["exited_at"] = round(time.time() - t0, 2)
                break
            if hwnd is None:
                hwnd = find_hwnd(pid)
                if hwnd:
                    rec["hwnd"] = int(hwnd)
                    print(f"[{label}] hwnd={hwnd} at +{time.time()-t0:.2f}s")
            if hwnd:
                for vk in KEYS:                      # hold each key one beat
                    u.PostMessageW(hwnd, WM_KEYDOWN, vk, 0)
                    rec["posts"] += 1
                time.sleep(0.05)
                # right-button look drag, 8 steps across the window
                u.PostMessageW(hwnd, WM_RBUTTONDOWN, MK_RBUTTON, (300 << 16) | 400)
                for i in range(8):
                    lp = ((300 + i * 12) << 16) | (400 + i * 20)
                    u.PostMessageW(hwnd, WM_MOUSEMOVE, MK_RBUTTON, lp)
                u.PostMessageW(hwnd, WM_RBUTTONUP, 0, (460 << 16) | 496)
                rec["drag_sweeps"] += 1
                for vk in KEYS:
                    u.PostMessageW(hwnd, WM_KEYUP, vk, 0)
                    rec["posts"] += 1
            time.sleep(0.05)
    finally:
        if proc.poll() is None:
            proc.kill()                              # only our PID
            proc.wait(timeout=15)
        rec["killed_pid"] = pid
    (OUT / f"{label}.json").write_text(json.dumps(rec, indent=2), encoding="utf-8")
    print(f"[{label}] posted {rec['posts']} key msgs, {rec['drag_sweeps']} drags")


if __name__ == "__main__":
    main()
