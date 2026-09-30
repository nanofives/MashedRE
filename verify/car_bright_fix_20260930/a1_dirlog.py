# A1 (acceptance, CAR_BRIGHTNESS_2026-09-30.md): log the track directional-light
# world direction the standalone computes, for every kAreas[] index.
#
# For each requested MASHED_TRACK_SEL it launches the pinned exe, navigates the
# frontend into a race with the same enter-tap sequence the investigation session
# used, and polls ./mashed_re.log for the `WS-E lights:` line that
# TrackRenderer.cpp:1257 emits at track load. It then kills ONLY the pid it
# spawned. The per-track log slice is written to <outdir>/<idx>_<name>.log.
#
# kAreas[] index -> name is mashedmod/src/mashed_re/Race/GameFlow.cpp:37-51.
#
# Every launch is muted, carries MASHED_TITLE, uses MASHED_WIN_POS=left-bl, and
# never uses MASHED_NAV_DEMO.
#
# Usage: py -3.12 verify/car_bright_fix_20260930/a1_dirlog.py <exe> <outdir> [idx,...]
import ctypes, os, re, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LOG = ROOT / "mashed_re.log"
CARLOG = ROOT / "mashed_re_carlight.log"

AREAS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
         "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]

# The enter taps that walk Main Menu -> Single Player -> Challenge Cup ->
# Challenge Select -> launch. Verbatim from the investigation session's recipe.
KEYS = [4.0, 5.5, 7.0, 8.5, 10.0, 11.5, 13.0, 16.0, 17.5, 19.0]
TIMEOUT = 34.0


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


def tap_enter(pid):
    h = find_hwnd(pid)
    if not h:
        return
    u = ctypes.windll.user32
    u.SetForegroundWindow(h)
    time.sleep(0.12)
    sc = u.MapVirtualKeyW(0x0D, 0)
    u.keybd_event(0x0D, sc, 0, 0)
    time.sleep(0.07)
    u.keybd_event(0x0D, sc, 2, 0)


def run_one(exe, idx, outdir):
    name = AREAS[idx]
    for p in (LOG, CARLOG):
        if p.exists():
            p.unlink()
    env = dict(os.environ)
    env["MASHED_TRACK_SEL"] = str(idx)
    env["MASHED_DBG_CARLIGHT"] = "1"
    env["MASHED_MUTE"] = "1"
    env["MASHED_WIN_POS"] = "left-bl"
    env["MASHED_TITLE"] = f"car-bright A1 dir {idx}={name}"
    env.pop("MASHED_NAV_DEMO", None)
    proc = subprocess.Popen([str(exe)], cwd=str(ROOT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    pid = proc.pid
    t0 = time.time()
    ki = 0
    hit = None
    try:
        while time.time() - t0 < TIMEOUT:
            while ki < len(KEYS) and time.time() - t0 >= KEYS[ki]:
                tap_enter(pid)
                ki += 1
            if proc.poll() is not None:
                break
            if LOG.exists():
                txt = LOG.read_text(errors="replace")
                m = re.search(r"^\s*WS-E lights:.*$", txt, re.M)
                if m:
                    hit = m.group(0).strip()
                    time.sleep(1.2)          # let CARLIGHT land too
                    break
            time.sleep(0.15)
    finally:
        try:
            proc.kill()
        except Exception:
            pass
        time.sleep(0.4)
    car = ""
    if CARLOG.exists():
        for ln in CARLOG.read_text(errors="replace").splitlines():
            if ln.startswith("CARLIGHT"):
                car = ln.strip()
                break
    dst = outdir / f"{idx:02d}_{name}.log"
    body = f"# pid={pid} exe={exe}\n# MASHED_TRACK_SEL={idx} ({name})\n"
    body += (LOG.read_text(errors="replace") if LOG.exists() else "(no log)\n")
    body += "\n#### carlight\n"
    body += (CARLOG.read_text(errors="replace") if CARLOG.exists() else "(none)\n")
    dst.write_text(body)
    return hit, car


def main():
    exe = Path(sys.argv[1]).resolve()
    outdir = Path(sys.argv[2])
    outdir.mkdir(parents=True, exist_ok=True)
    idxs = ([int(v) for v in sys.argv[3].split(",")]
            if len(sys.argv) > 3 else list(range(len(AREAS))))
    if not exe.exists():
        print(f"error: {exe} not found")
        return 2
    print(f"=== a1_dirlog exe={exe}")
    rows = []
    for i in idxs:
        hit, car = run_one(exe, i, outdir)
        print(f"[{i:2d}] {AREAS[i]:<9} {hit or 'NO WS-E LINE'}")
        if car:
            print(f"     {car}")
        rows.append((i, AREAS[i], hit or "", car or ""))
    print("\n=== summary (idx name dir= L=) ===")
    for i, nm, hit, car in rows:
        d = re.search(r"dir=\(([^)]*)\)", hit)
        l = re.search(r"L=\(([^)]*)\)", car)
        print(f"{i:2d} {nm:<9} dir=({d.group(1) if d else '?'})  "
              f"L=({l.group(1) if l else '?'})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
