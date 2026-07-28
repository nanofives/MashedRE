# catch_wedge.py — run the menu-navigated race until it WEDGES, then interrogate the live
# process BEFORE anything kills it.
#
# Why (U-9025, 2026-07-28): the wedge is a RACE, not a deterministic interaction. Measured
# completion with a byte-identical 75-hook set: pre-fix 2/6, post-fix 10/12, stock 6/6
# (stock vs pre-fix p=0.03). Because ~1 run in 6 still wedges, CATCHING one is cheap — far
# cheaper than a repeat-predicate bisect, which needs ~3 runs per test x ~14 tests (~4 h) and
# only ever returns a name, never a mechanism.
#
# The existing driver (bisect_hooks_set.py) kills every pid that appeared during a run, so a
# wedged process is destroyed before it can be examined. This one detects the wedge, runs the
# inspector against that explicit pid, saves the report, and only then kills it.
#
# WEDGE DETECTION: the process is alive, its window stops responding, and statenav has not
# printed FINAL. "Not responding" alone is not enough — a loading hitch can trip it — so it
# must persist across consecutive polls.
#
# PROCESS HYGIENE (CLAUDE.md): snapshot pids before launch, only ever touch pids that appeared
# during THIS run, and pass an explicit pid to the inspector. Never "first MASHED by name" —
# other sessions run their own game.
import argparse, os, re, subprocess, sys, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
STATENAV = os.path.join(ROOT, "re", "frida", "statenav.py")
INSPECT = os.path.join(ROOT, "re", "frida", "inspect_wedge.py")
MANIFEST = os.path.join(ROOT, "log", "hook_index_manifest.txt")
OUTDIR = os.path.join(ROOT, "log", "wedge")


def mashed_pids():
    try:
        out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq MASHED.exe", "/FO", "CSV", "/NH"],
                             capture_output=True, text=True, timeout=30).stdout
    except Exception:
        return set()
    return set(int(m) for m in re.findall(r'"MASHED\.exe","(\d+)"', out))


def kill(pid):
    import ctypes
    h = ctypes.windll.kernel32.OpenProcess(1, False, pid)
    if h:
        ctypes.windll.kernel32.TerminateProcess(h, 1)
        ctypes.windll.kernel32.CloseHandle(h)


def alive_and_hung(pid):
    """(alive, hung). Hung = the pid owns a visible window that stops answering messages.

    psutil.pid_exists LIES while Frida holds a handle (the pid stays allocated after exit) —
    memory feedback_psutil_pid_exists_frida — so aliveness is taken from tasklist, and
    hung-ness from IsHungAppWindow on that pid's own windows.
    """
    import ctypes
    from ctypes import wintypes
    if pid not in mashed_pids():
        return False, False
    u32 = ctypes.windll.user32
    hung = {"v": False, "seen": False}
    WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

    def cb(hwnd, _):
        p = wintypes.DWORD()
        u32.GetWindowThreadProcessId(hwnd, ctypes.byref(p))
        if p.value == pid and u32.IsWindowVisible(hwnd):
            hung["seen"] = True
            if u32.IsHungAppWindow(hwnd):
                hung["v"] = True
        return True

    u32.EnumWindows(WNDENUMPROC(cb), 0)
    return True, (hung["seen"] and hung["v"])


def names_for(lo, hi):
    names = []
    with open(MANIFEST, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            f = line.rstrip("\n").split("\t")
            if len(f) >= 4 and lo <= int(f[0]) < hi:
                names.append(f[3])
    return names


def inspect(pid, tag, probe):
    os.makedirs(OUTDIR, exist_ok=True)
    rpt = os.path.join(OUTDIR, "wedge_%s.txt" % tag)
    argv = [sys.executable, "-u", INSPECT, "--pid", str(pid)]
    if probe:
        argv.append("--probe-semaphores")
    r = subprocess.run(argv, cwd=ROOT, capture_output=True, text=True, timeout=300)
    with open(rpt, "w", encoding="utf-8") as fh:
        fh.write(r.stdout + "\n" + r.stderr)
    print(r.stdout, flush=True)
    if r.stderr.strip():
        print("[inspector stderr] " + r.stderr.strip(), flush=True)
    print("   report -> %s" % rpt, flush=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lo", type=int, default=75)
    ap.add_argument("--hi", type=int, default=150)
    ap.add_argument("--attempts", type=int, default=8)
    ap.add_argument("--round", type=int, default=35)
    ap.add_argument("--hang-polls", type=int, default=4,
                    help="consecutive 5s polls of not-responding before declaring a wedge")
    ap.add_argument("--probe-semaphores", action="store_true",
                    help="also probe semaphore state (CONSUMES a count on WAIT_OBJECT_0 - "
                         "diagnostic on a doomed process only)")
    args = ap.parse_args()

    only = ",".join(names_for(args.lo, args.hi))
    n = len(only.split(","))
    print("hook set: indices [%d,%d) -> %d names (selected BY NAME: the manifest has 27 "
          "duplicate RVAs, zero duplicate names)" % (args.lo, args.hi, n), flush=True)

    for attempt in range(args.attempts):
        env = dict(os.environ)
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
        env["MASHED_HOOK_ONLY"] = only
        log = os.path.join(ROOT, "log", "wedge_run%d.log" % attempt)
        before = mashed_pids()
        print("\n== attempt %d/%d ==" % (attempt + 1, args.attempts), flush=True)
        with open(log, "wb") as fh:
            p = subprocess.Popen(
                [sys.executable, "-u", STATENAV, "--hooks", "--round", str(args.round),
                 "--shot-dir", "verify/wedge%d" % attempt],
                cwd=ROOT, env=env, stdout=fh, stderr=subprocess.STDOUT)

            mine, hung_streak, captured = set(), 0, False
            while p.poll() is None:
                time.sleep(5)
                mine |= (mashed_pids() - before)
                for pid in sorted(mine):
                    alive, hung = alive_and_hung(pid)
                    if alive and hung:
                        hung_streak += 1
                        print("   pid %d not responding (%d/%d)"
                              % (pid, hung_streak, args.hang_polls), flush=True)
                        if hung_streak >= args.hang_polls:
                            print("   WEDGE CAUGHT on pid %d - inspecting BEFORE kill" % pid,
                                  flush=True)
                            inspect(pid, "a%d_pid%d" % (attempt, pid), args.probe_semaphores)
                            captured = True
                            break
                    elif alive:
                        hung_streak = 0
                if captured:
                    break

            if captured:
                p.kill()

        for pid in mashed_pids() - before:
            kill(pid)
        time.sleep(2)

        text = open(log, "rb").read().decode("utf-8", "replace")
        if captured:
            print("\nCAPTURED a wedge on attempt %d. Reports in %s" % (attempt + 1, OUTDIR),
                  flush=True)
            return 0
        print("   completed=%s (no wedge this run)" % ("FINAL:" in text), flush=True)

    print("\nno wedge in %d attempts (expected ~1 in 6 post-fix)" % args.attempts, flush=True)
    return 1


if __name__ == "__main__":
    sys.exit(main())
