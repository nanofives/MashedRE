"""Prove no_focus_pause: does MASHED keep rendering while its window is NOT focused?

Metric: the d3d9 shim appends one "present_fps=..." line per second to
log/fps_limiter.txt when MASHED_FPS_LOG=1. Lines accumulating == Present() being
called == frames still being produced. If the main loop is blocked in WaitMessage()
the line count freezes.

Procedure per arm: spawn, let it boot, MINIMIZE the game window (deterministically
unfocused -- no ambiguity about who has foreground), then sample the log.

PROCESS HYGIENE (CLAUDE.md): only the PID we spawned is ever terminated. No
kill-by-name. WaitForExit before reading an exit code.
"""
import ctypes, os, subprocess, sys, time, pathlib
from ctypes import wintypes

ROOT = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EXE = ROOT / "original" / "MASHED.exe"
LOG = ROOT / "log" / "fps_limiter.txt"

user32 = ctypes.WinDLL("user32", use_last_error=True)
EnumWindows = user32.EnumWindows
EnumWindows.argtypes = [ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM), wintypes.LPARAM]
GetWindowThreadProcessId = user32.GetWindowThreadProcessId
IsWindowVisible = user32.IsWindowVisible
ShowWindow = user32.ShowWindow
GetForegroundWindow = user32.GetForegroundWindow
SW_MINIMIZE = 6


def hwnds_for_pid(pid):
    out = []
    @ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    def cb(hwnd, _):
        p = wintypes.DWORD()
        GetWindowThreadProcessId(hwnd, ctypes.byref(p))
        if p.value == pid and IsWindowVisible(hwnd):
            out.append(hwnd)
        return True
    EnumWindows(cb, 0)
    return out


def lines():
    if not LOG.is_file():
        return 0
    return len([l for l in LOG.read_text(errors="replace").splitlines() if "present_fps" in l])


def arm(label, boot=12, observe=15):
    if LOG.is_file():
        LOG.unlink()
    env = dict(os.environ)
    env["MASHED_FPS_LOG"] = "1"
    env["MASHED_WIN_POS"] = "left-bl"
    print(f"\n--- ARM {label}")
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env)
    print(f"    pid {proc.pid}, booting {boot}s")
    time.sleep(boot)
    if proc.poll() is not None:
        proc.wait()
        print(f"    EXITED during boot, code {proc.returncode}")
        return None
    hs = hwnds_for_pid(proc.pid)
    print(f"    windows: {len(hs)}")
    for h in hs:
        ShowWindow(h, SW_MINIMIZE)
    fg = GetForegroundWindow()
    print(f"    minimized; foreground hwnd {fg} is {'THE GAME' if fg in hs else 'NOT the game'}")
    before = lines()
    time.sleep(observe)
    after = lines()
    alive = proc.poll() is None
    proc.terminate()
    try:
        proc.wait(timeout=10)
    except subprocess.TimeoutExpired:
        proc.kill(); proc.wait()
    gained = after - before
    print(f"    fps lines: {before} -> {after}   GAINED WHILE MINIMIZED = {gained} over {observe}s")
    print(f"    process still alive at end: {alive}")
    return gained


if __name__ == "__main__":
    label = sys.argv[1] if len(sys.argv) > 1 else "?"
    g = arm(label)
    print(f"RESULT {label} gained={g}")
