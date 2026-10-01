# Window-title probe for the a9da810a verification (2026-10-01).
#
# Spawns mashedmod/build/mashed_re.exe from the repo root with a CLEAN
# environment (every inherited MASHED_* variable is stripped first, so a T2/T3
# label reflects exactly the vars this run passes), finds the window by the PID
# WE spawned (never by name), polls GetWindowTextW, and records every distinct
# title with the time it first appeared. Kills only the spawned PID.
#
# Usage:
#   py -3.12 verify/freecam_title_20261001/title_probe.py <label> <seconds> [ENV=VAL ...]
import ctypes, json, os, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"
OUT = Path(__file__).resolve().parent


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


def get_title(hwnd):
    u = ctypes.windll.user32
    buf = ctypes.create_unicode_buffer(512)
    n = u.GetWindowTextW(hwnd, buf, 512)
    return buf.value if n else ""


def main():
    label = sys.argv[1]
    dur = float(sys.argv[2])
    env_args = sys.argv[3:]

    # Clean slate: no inherited MASHED_* can leak into the label.
    env = {k: v for k, v in os.environ.items() if not k.startswith("MASHED_")}
    applied = {}
    for a in env_args:
        k, _, v = a.partition("=")
        env[k] = v
        applied[k] = v

    proc = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env)
    pid = proc.pid
    print(f"[{label}] spawned pid={pid} env={applied}")

    rec = {
        "label": label,
        "pid": pid,
        "env_applied": applied,
        "duration_s": dur,
        "titles": [],   # [t_seconds, title]
        "hwnd_found_at": None,
    }
    t0 = time.time()
    hwnd = None
    last = None
    try:
        while time.time() - t0 < dur:
            if proc.poll() is not None:
                rec["exited_early_at"] = round(time.time() - t0, 2)
                rec["exit_code"] = proc.returncode
                break
            if hwnd is None:
                hwnd = find_hwnd(pid)
                if hwnd is not None:
                    rec["hwnd_found_at"] = round(time.time() - t0, 2)
                    rec["hwnd"] = int(hwnd)
            if hwnd is not None:
                t = get_title(hwnd)
                if t and t != last:
                    last = t
                    rec["titles"].append([round(time.time() - t0, 2), t])
                    print(f"  +{rec['titles'][-1][0]:6.2f}s  {t}")
            time.sleep(0.2)
    finally:
        if proc.poll() is None:
            proc.kill()          # only the PID we spawned
            proc.wait(timeout=15)
        rec["killed_pid"] = pid

    (OUT / f"{label}.json").write_text(json.dumps(rec, indent=2), encoding="utf-8")
    print(f"[{label}] {len(rec['titles'])} distinct titles -> {label}.json")


if __name__ == "__main__":
    main()
