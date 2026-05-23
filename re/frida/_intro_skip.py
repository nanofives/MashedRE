"""Mashed intro/splash skipper — keypress-based runtime helper.

Mashed plays multi-second intro/splash screens before reaching the main menu
(company logos, game intro video). Canonical-observation tooling (mass_*.py,
hot_path_behavioral.py) waits a fixed BOOT_WAIT seconds then assumes "main menu
reached", but if BOOT_WAIT is shorter than intro duration, the observation
window is mostly intro, not menu.

This helper sends keypresses to MASHED's window to skip the intro:
  - SPACE / ENTER advance through company-logo screens
  - ESC skips the game intro video

Usage from a Frida-driver script:
    from _intro_skip import skip_intro
    # after frida.spawn + resume + initial boot wait (~5s for window to exist):
    skip_intro(pid, count=8, delay=0.6)
    # then continue with idle_seconds at the (now-actually-displayed) main menu

The keypress is delivered via PostMessage(hwnd, WM_KEYDOWN, vk, 0) +
PostMessage(hwnd, WM_KEYUP, vk, 0). This is non-blocking and doesn't require
the window to be foreground. Tested against MASHED window class "MASHED".

If pywin32 is not installed, falls back to ctypes user32 (always available
on Windows).

Saved here so all current and future canonical-observation tools can
`from _intro_skip import skip_intro` with no extra dependencies.

Related memory: project_mashed_intro_skip.md.
"""

from __future__ import annotations

import time
from typing import Iterable

# Try pywin32 first (cleaner API), fallback to ctypes user32.
try:
    import win32api, win32con, win32gui, win32process
    _USE_PYWIN32 = True
except ImportError:
    _USE_PYWIN32 = False
    import ctypes
    from ctypes import wintypes
    _user32 = ctypes.windll.user32
    _user32.PostMessageW.argtypes = [wintypes.HWND, wintypes.UINT,
                                      wintypes.WPARAM, wintypes.LPARAM]
    _user32.EnumWindows.restype = wintypes.BOOL
    _WM_KEYDOWN = 0x0100
    _WM_KEYUP = 0x0101
    _VK_RETURN = 0x0D
    _VK_SPACE = 0x20
    _VK_ESCAPE = 0x1B


# Mashed's WindowCreate (sub_00499ba0) registers the class "MASHED" and
# creates one top-level window. We find that window by enumerating top-level
# windows owned by our spawned PID.

def _find_window_by_pid_pywin32(pid: int) -> int | None:
    found = []
    def cb(hwnd, _):
        if not win32gui.IsWindowVisible(hwnd):
            return True
        _, wpid = win32process.GetWindowThreadProcessId(hwnd)
        if wpid == pid:
            found.append(hwnd)
        return True
    win32gui.EnumWindows(cb, None)
    return found[0] if found else None


def _find_window_by_pid_ctypes(pid: int) -> int | None:
    EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    found = []
    def cb(hwnd, _):
        if not _user32.IsWindowVisible(hwnd):
            return True
        wpid = wintypes.DWORD()
        _user32.GetWindowThreadProcessId(hwnd, ctypes.byref(wpid))
        if wpid.value == pid:
            found.append(hwnd)
        return True
    _user32.EnumWindows(EnumWindowsProc(cb), 0)
    return found[0] if found else None


def find_mashed_window(pid: int, timeout_seconds: float = 10.0) -> int | None:
    """Find the top-level window owned by the given PID.

    Polls every 0.2s until a window appears or timeout. MASHED creates its
    window during WinMain wrapper → sub_00499ba0 (WindowCreate), so this
    typically becomes findable within 1-3s of spawn-resume.

    Returns the HWND (int) or None on timeout.
    """
    deadline = time.time() + timeout_seconds
    finder = _find_window_by_pid_pywin32 if _USE_PYWIN32 else _find_window_by_pid_ctypes
    while time.time() < deadline:
        hwnd = finder(pid)
        if hwnd:
            return hwnd
        time.sleep(0.2)
    return None


def _send_key(hwnd: int, vk: int) -> None:
    if _USE_PYWIN32:
        win32api.PostMessage(hwnd, win32con.WM_KEYDOWN, vk, 0)
        win32api.PostMessage(hwnd, win32con.WM_KEYUP, vk, 0)
    else:
        _user32.PostMessageW(hwnd, _WM_KEYDOWN, vk, 0)
        _user32.PostMessageW(hwnd, _WM_KEYUP, vk, 0)


def skip_intro(pid: int, count: int = 8, delay: float = 0.6,
               keys: Iterable[int] | None = None) -> bool:
    """Send a sequence of keypresses to MASHED's window to skip the intro.

    Args:
        pid: spawned MASHED process id.
        count: how many keypresses to send (default 8 — empirically enough
            to advance through company logos + skip game intro video).
        delay: seconds between keypresses (default 0.6 — slow enough that
            each screen registers the press, fast enough to skip in <5s).
        keys: VK codes to cycle through. Default: [ESC, SPACE, ENTER]
            cycled. SPACE/ENTER advance logos; ESC skips video.

    Returns:
        True if the window was found and keypresses delivered; False if
        the window could not be found within 10s of looking.

    Call AFTER spawn+resume and AFTER an initial short wait for the window
    to be created (~2-3s). Do NOT call before resume — there's no window yet.
    """
    hwnd = find_mashed_window(pid)
    if hwnd is None:
        return False

    if _USE_PYWIN32:
        default_keys = [win32con.VK_ESCAPE, win32con.VK_SPACE, win32con.VK_RETURN]
    else:
        default_keys = [_VK_ESCAPE, _VK_SPACE, _VK_RETURN]
    keys = list(keys) if keys else default_keys

    for i in range(count):
        vk = keys[i % len(keys)]
        _send_key(hwnd, vk)
        time.sleep(delay)
    return True
