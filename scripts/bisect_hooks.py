# bisect_hooks.py — binary-search a hook-caused runtime failure by REGISTRY INDEX.
#
# Built for U-9025 (2026-07-27): with the full hook set the menu-navigated race wedges at
# race entry; stock completes the round. None of the functions on the wedged thread's stack
# is hooked, so there was nothing to skip by name — hence an index bisect.
#
# SIGNAL: "did the round COMPLETE", never "did it crash". The full set has produced three
# different outcomes (wedge, wedge, clean exit 0) and a crash/no-crash test would have sent
# the search down the wrong branch. Completion is stable: stock 3/3 complete, hooked 0/3.
#
# PROCESS HYGIENE (mandatory, see CLAUDE.md): a wedged run leaves MASHED alive after the
# driver is killed. We snapshot MASHED PIDs before each run and terminate ONLY pids that
# appeared during it. Never a blanket kill by name — other sessions run their own MASHED.
import argparse, os, re, subprocess, sys, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCRATCH = os.environ.get("BISECT_OUT", os.path.join(ROOT, "log"))
STATENAV = os.path.join(ROOT, "re", "frida", "statenav.py")


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


def run_once(lo, hi, tag, round_s, timeout_s):
    """True = round COMPLETED (this hook subset is innocent)."""
    log = os.path.join(SCRATCH, "bisect_%s.log" % tag)
    env = dict(os.environ)
    env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    if lo is not None:
        env["MASHED_HOOK_LO"] = str(lo)
        env["MASHED_HOOK_HI"] = str(hi)
    else:
        env.pop("MASHED_HOOK_LO", None)
        env.pop("MASHED_HOOK_HI", None)

    before = mashed_pids()
    with open(log, "wb") as fh:
        p = subprocess.Popen([sys.executable, "-u", STATENAV, "--hooks",
                              "--round", str(round_s), "--shot-dir", "verify/bisect_%s" % tag],
                             cwd=ROOT, env=env, stdout=fh, stderr=subprocess.STDOUT)
        try:
            p.wait(timeout=timeout_s)
        except subprocess.TimeoutExpired:
            p.kill()
    # kill ONLY pids that appeared during this run
    for pid in mashed_pids() - before:
        kill(pid)
    time.sleep(2)

    text = open(log, "rb").read().decode("utf-8", "replace")
    # STRICT: only a printed FINAL counts. Reaching "round t+30" is NOT completion — the
    # 0x00415e20 width fix produced a run that reached t+30 and then wedged, which a looser
    # predicate would have scored as a pass and sent the search down the wrong branch.
    completed = "FINAL:" in text
    reached_race = "start-attempt" in text
    return completed, reached_race, log


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--total", type=int, required=True)
    ap.add_argument("--round", type=int, default=35)
    ap.add_argument("--timeout", type=int, default=260)
    ap.add_argument("--baseline", action="store_true", help="just verify full-set fails")
    args = ap.parse_args()

    print("== baseline: full hook set ==", flush=True)
    ok, race, log = run_once(0, args.total, "full", args.round, args.timeout)
    print("   full set [0,%d): completed=%s reached_race=%s  (%s)" % (args.total, ok, race, log), flush=True)
    if ok:
        print("   !! full set COMPLETED - the failure is not reproducing; bisect aborted")
        return 1
    if args.baseline:
        return 0

    lo, hi = 0, args.total
    step = 0
    while hi - lo > 1:
        step += 1
        mid = (lo + hi) // 2
        okA, raceA, logA = run_once(lo, mid, "s%d_lo" % step, args.round, args.timeout)
        print("   step %d: [%d,%d) completed=%s race=%s" % (step, lo, mid, okA, raceA), flush=True)
        if not okA:
            hi = mid                      # failure lives in the lower half
            continue
        okB, raceB, logB = run_once(mid, hi, "s%d_hi" % step, args.round, args.timeout)
        print("   step %d: [%d,%d) completed=%s race=%s" % (step, mid, hi, okB, raceB), flush=True)
        if not okB:
            lo = mid
            continue
        print("   !! BOTH halves completed - not a single-hook cause (interaction). "
              "Range [%d,%d)" % (lo, hi), flush=True)
        return 2

    print("\nISOLATED registry index %d" % lo, flush=True)
    man = os.path.join(ROOT, "log", "hook_index_manifest.txt")
    if os.path.exists(man):
        for line in open(man, encoding="utf-8", errors="replace"):
            f = line.split("\t")
            if f and f[0].strip() == str(lo):
                print("   %s" % line.strip(), flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
