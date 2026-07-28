# bisect_hooks_set.py — narrow a hook-caused runtime failure over an ARBITRARY hook SET.
#
# Why this exists (U-9025, 2026-07-28): bisect_hooks.py can only express ONE contiguous
# registry-index range, via MASHED_HOOK_LO/HI. U-9025 is an INTERACTION — [75,150) fails as
# a set while BOTH halves [75,112) and [112,150) complete — so a contiguous-range search
# cannot narrow it: no single range that the bisect would ever try contains a culprit from
# both sides. Narrowing an interaction needs "pin this set, search that set", i.e. an
# arbitrary allowlist.
#
# HookSystem.cpp:189 already provides exactly that: MASHED_HOOK_ONLY is an exact-token
# allowlist matched against the registered NAME or the "0x%08x" RVA token, and it takes
# PRECEDENCE over LO/HI (HookSystem.cpp:150). So no .asi change is needed.
#
# SELECT BY NAME, NOT RVA. log/hook_index_manifest.txt has 1205 entries with 27 DUPLICATE
# RVAs (multiple hooks registered at one address, e.g. the AiControllerAB A/B drivers) but
# ZERO duplicate names. An RVA token would silently co-install more hooks than requested and
# make the search unsound.
#
# GATING-EQUIVALENCE (mandatory, --baseline): this driver changes the mechanism that selects
# hooks (ONLY-list instead of LO/HI). Before any search decision is trusted, the ONLY-list
# form of the failing set must reproduce the SAME failure the LO/HI form produced. If it
# does not, the gate itself perturbed the outcome and every later branch is worthless.
#
# SIGNAL: "did the round COMPLETE" — a printed "FINAL:" and nothing else. Reaching a given
# round tick is NOT completion (bisect_hooks.py:65).
#
# PROCESS HYGIENE (CLAUDE.md): snapshot MASHED pids before each run, terminate ONLY pids
# that appeared during it. Never a blanket kill by name.
import argparse, os, re, subprocess, sys, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCRATCH = os.environ.get("BISECT_OUT", os.path.join(ROOT, "log"))
STATENAV = os.path.join(ROOT, "re", "frida", "statenav.py")
MANIFEST = os.path.join(ROOT, "log", "hook_index_manifest.txt")


def load_manifest():
    """index -> (rva, name). Manifest is TSV: index, rva, installed, name."""
    idx2 = {}
    names = {}
    with open(MANIFEST, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            f = line.rstrip("\n").split("\t")
            if len(f) < 4:
                continue
            i = int(f[0])
            idx2[i] = (f[1], f[3])
            names.setdefault(f[3], []).append(i)
    dups = [n for n, v in names.items() if len(v) > 1]
    if dups:
        sys.exit("ABORT: duplicate hook NAMES in manifest (%d), selection would be "
                 "ambiguous: %s" % (len(dups), dups[:5]))
    return idx2


def parse_spec(spec, idx2):
    """'75-112,139,MenuGroupCount' -> sorted list of indices. Ranges are HALF-OPEN [a,b)."""
    out = []
    for tok in re.split(r"[,\s]+", spec.strip()):
        if not tok:
            continue
        m = re.fullmatch(r"(\d+)-(\d+)", tok)
        if m:
            a, b = int(m.group(1)), int(m.group(2))
            out.extend(range(a, b))
        elif tok.isdigit():
            out.append(int(tok))
        else:
            hit = [i for i, (_, n) in idx2.items() if n == tok]
            if not hit:
                sys.exit("ABORT: no hook named %r in manifest" % tok)
            out.extend(hit)
    missing = [i for i in out if i not in idx2]
    if missing:
        sys.exit("ABORT: indices not in manifest: %s" % missing[:10])
    return sorted(set(out))


def fmt(indices, idx2, limit=6):
    names = [idx2[i][1] for i in indices]
    head = ", ".join(names[:limit])
    return "%d hooks [%s%s]" % (len(indices), head, "" if len(names) <= limit else ", ...")


def run_set(indices, idx2, tag, round_s, timeout_s, use_range=None, stock=False):
    """True = round COMPLETED (this hook set is innocent).

    use_range=(lo,hi) forces the legacy MASHED_HOOK_LO/HI gate instead of the ONLY-list;
    used only by --baseline to prove the two gates agree.
    """
    log = os.path.join(SCRATCH, "bset_%s.log" % tag)
    env = dict(os.environ)
    env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    env.pop("MASHED_HOOK_LO", None)
    env.pop("MASHED_HOOK_HI", None)
    env.pop("MASHED_HOOK_ONLY", None)
    if use_range is not None:
        env["MASHED_HOOK_LO"] = str(use_range[0])
        env["MASHED_HOOK_HI"] = str(use_range[1])
    else:
        if not indices:
            # A sentinel that equals no token installs nothing (HookSystem.cpp:156).
            env["MASHED_HOOK_ONLY"] = "__none__"
        else:
            env["MASHED_HOOK_ONLY"] = ",".join(idx2[i][1] for i in indices)
    # Independent record of what actually installed, so a run can be audited after the fact.
    # HookSystem::ManifestLine APPENDS, so a reused tag would stack this run's lines on top of
    # a previous run's and inflate the count -- observed 2026-07-28 as installed=150 for a
    # 75-hook request when a --measure tag was reused. Delete it first so the count means
    # "installed THIS run".
    manifest_path = os.path.join(SCRATCH, "bset_%s.manifest" % tag)
    try:
        os.remove(manifest_path)
    except OSError:
        pass
    env["MASHED_HOOK_MANIFEST"] = manifest_path

    argv = [sys.executable, "-u", STATENAV,
            "--round", str(round_s), "--shot-dir", "verify/bset_%s" % tag]
    if not stock:
        # statenav sets MASHED_RE_NO_AUTO_HOOK=1 itself when --hooks is absent, so the stock
        # control runs the SAME navigation through the SAME driver with no hooks armed.
        argv.insert(3, "--hooks")

    before = mashed_pids()
    with open(log, "wb") as fh:
        p = subprocess.Popen(argv, cwd=ROOT, env=env, stdout=fh, stderr=subprocess.STDOUT)
        try:
            p.wait(timeout=timeout_s)
        except subprocess.TimeoutExpired:
            p.kill()
    for pid in mashed_pids() - before:
        kill(pid)
    time.sleep(2)

    text = open(log, "rb").read().decode("utf-8", "replace")
    completed = "FINAL:" in text
    reached_race = "start-attempt" in text
    installed = count_installed(env["MASHED_HOOK_MANIFEST"])
    return completed, reached_race, log, installed


def count_installed(path):
    """How many hooks the .asi reports as ACTUALLY installed. Guards against a run that
    proves nothing because the gate selected nothing (or the .asi never armed)."""
    if not os.path.exists(path):
        return None
    n = 0
    with open(path, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            f = line.rstrip("\n").split("\t")
            if len(f) >= 3 and f[2].strip() == "1":
                n += 1
    return n


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


def report(tag, indices, idx2, res):
    ok, race, log, inst = res
    print("   %-14s %-46s completed=%-5s race=%-5s installed=%s  (%s)"
          % (tag, fmt(indices, idx2, 3), ok, race, inst, os.path.basename(log)), flush=True)
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pin", default="", help="index spec ALWAYS installed (half-open ranges)")
    ap.add_argument("--search", required=True, help="index spec to binary-search")
    ap.add_argument("--round", type=int, default=35)
    ap.add_argument("--timeout", type=int, default=260)
    ap.add_argument("--baseline", action="store_true",
                    help="only prove pin+search fails, and that the LO/HI gate agrees")
    ap.add_argument("--measure", type=int, default=0, metavar="N",
                    help="run the IDENTICAL pin+search set N times and report the completion "
                         "rate, then exit. Use this before trusting ANY single-run branch: "
                         "on 2026-07-28 the same 75-hook set (byte-identical installed "
                         "manifests) both wedged and completed, so a 1-run predicate is "
                         "unsound and the search must use a repeat-based one.")
    ap.add_argument("--stock", action="store_true",
                    help="with --measure: run the CONTROL (no hooks armed) through the same "
                         "driver. Required to know whether the wedge is hook-caused at all: "
                         "against a flaky failure, the historical 'stock 3/3 vs hooked 0/3' is "
                         "only about p=0.05, and the hooked set is no longer 0/N.")
    ap.add_argument("--range-equiv", default="",
                    help="'lo:hi' — the contiguous range whose LO/HI result must be "
                         "reproduced by the ONLY-list form of pin+search")
    args = ap.parse_args()

    idx2 = load_manifest()
    pin = parse_spec(args.pin, idx2) if args.pin else []
    search = parse_spec(args.search, idx2)
    overlap = set(pin) & set(search)
    if overlap:
        sys.exit("ABORT: pin and search overlap at %s" % sorted(overlap)[:10])
    full = sorted(set(pin) | set(search))

    print("== U-9025 set search ==", flush=True)
    print("   pin    = %s" % fmt(pin, idx2), flush=True)
    print("   search = %s" % fmt(search, idx2), flush=True)

    if args.measure:
        what = "STOCK (no hooks armed)" if args.stock else "the IDENTICAL set"
        print("\n-- determinism: %d runs of %s --" % (args.measure, what), flush=True)
        done = 0
        for k in range(args.measure):
            res = run_set(full, idx2, "%sm%d" % ("stock" if args.stock else "", k),
                          args.round, args.timeout, stock=args.stock)
            if report("rep %d" % k, [] if args.stock else full, idx2, res):
                done += 1
        print("\nCOMPLETED %d/%d for %s"
              % (done, args.measure,
                 "STOCK" if args.stock else "the identical %d-hook set" % len(full)), flush=True)
        if 0 < done < args.measure:
            print("   => NON-DETERMINISTIC. A single run is NOT a valid pass/fail predicate; "
                  "any bisect branch taken on one run is unsound.", flush=True)
        return 0

    print("\n-- baseline: the whole set must FAIL --", flush=True)
    res = run_set(full, idx2, "base", args.round, args.timeout)
    ok = report("pin+search", full, idx2, res)
    if res[3] is not None and res[3] != len(full):
        print("   !! installed=%s but %d were requested - the gate did not select what was "
              "asked; aborting" % (res[3], len(full)), flush=True)
        return 3
    if ok:
        print("   !! whole set COMPLETED - failure not reproducing; aborting", flush=True)
        return 1

    if args.range_equiv:
        lo, hi = (int(x) for x in args.range_equiv.split(":"))
        print("\n-- gating equivalence: same set via the legacy LO/HI gate --", flush=True)
        res2 = run_set(full, idx2, "rangeq", args.round, args.timeout, use_range=(lo, hi))
        ok2 = report("LO/HI %d:%d" % (lo, hi), full, idx2, res2)
        if ok2:
            print("   !! the LO/HI form COMPLETED while the ONLY form FAILED - the two gates "
                  "disagree on the same set. The gate itself is a variable; stop and "
                  "investigate before trusting any branch.", flush=True)
            return 4
        print("   gates agree (both fail)", flush=True)

    if args.baseline:
        return 0

    # Binary-search `search` with `pin` held installed. Assumes ONE culprit in `search`;
    # if both halves complete, >=2 members of `search` are needed and we say so rather
    # than forcing a single answer.
    cur = list(search)
    step = 0
    while len(cur) > 1:
        step += 1
        mid = len(cur) // 2
        loH, hiH = cur[:mid], cur[mid:]
        print("\n-- step %d: %d candidates --" % (step, len(cur)), flush=True)
        resA = run_set(sorted(pin + loH), idx2, "s%d_lo" % step, args.round, args.timeout)
        okA = report("pin+lo", loH, idx2, resA)
        if not okA:
            cur = loH
            continue
        resB = run_set(sorted(pin + hiH), idx2, "s%d_hi" % step, args.round, args.timeout)
        okB = report("pin+hi", hiH, idx2, resB)
        if not okB:
            cur = hiH
            continue
        print("   !! BOTH halves completed with the pin held - >=2 members of this "
              "%d-element set are jointly required. Sub-search each half against the "
              "other." % len(cur), flush=True)
        print("   remaining: %s" % ", ".join("%d:%s" % (i, idx2[i][1]) for i in cur), flush=True)
        return 2

    i = cur[0]
    print("\nISOLATED index %d  rva=%s  name=%s" % (i, idx2[i][0], idx2[i][1]), flush=True)
    print("   (required together with the pinned set: %s)" % fmt(pin, idx2), flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
