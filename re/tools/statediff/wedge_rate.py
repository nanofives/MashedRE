# Measure the statediff capture-failure ("wedge") rate over N boots.
#
# WHY THIS EXISTS: README.md records the residual wedge as "~1/6 full-set boots"
# and points at a scratchpad `bisect_wedge.py` that was never preserved. The 1/6
# figure rests on SIX trials (flake_1..6.msd), which is far too small to plan a
# bisection against: 1/6 is consistent with a true rate anywhere from ~3% to
# ~64% at 95%. This driver re-measures with a stated n and reports a confidence
# interval, so a bisection is only paid for if the rate justifies it.
#
# It also separates two failure modes that the 1/6 figure conflates:
#   EMPTY      0 frames      -- never reached phase 3 (the classic wedge)
#   DEGENERATE 1..N frames   -- reached phase 3 then died/stalled early
# fix_ring_all.msd (2 frames) is a same-day POST-fix degenerate capture that the
# 1/6 tally does not include, which is why the distinction is kept explicit.
#
# Usage:
#   py -3.12 re\tools\statediff\wedge_rate.py --runs 16 [--hooks all] [--drive]
#            [--degenerate-below 100] [--out-dir verify\wedge_rate_<date>]
#
# PID hygiene (mandatory, CLAUDE.md): a wedged run leaves MASHED alive after the
# driver exits. We snapshot pids before each boot and kill ONLY pids that
# appeared during it. Never by name -- other sessions' games must survive.
import argparse, json, math, os, subprocess, sys, time
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent.parent
LAUNCH = ROOT / 're' / 'frida' / 'scenario_launch.py'
HDR = 16
REC = 0xD04 + 4          # frame_idx u32 + payload (FORMAT.md)


def msd_frames(p: Path):
    """Frame count from file size. Returns (frames, remainder) or (None, None)."""
    if not p.exists():
        return None, None
    sz = p.stat().st_size
    if sz < HDR:
        return None, None
    body = sz - HDR
    return body // REC, body % REC


ANCHOR_OFF = 0xBF4       # countdown-start witness (NOISE_MASK.md: 0 -> 0x32)
DRIVE_WINDOW = 314       # documented valid drive-diff window past the anchor


def msd_anchor(p: Path):
    """Frame ordinal where the countdown-start witness +0xBF4 goes non-zero.

    THIS is the meaningful usability test, not distinctness. Measured: the
    anchor lands at frame 783 (hooked, today) and 782 (stock, 2026-07-31) --
    within one frame across 20 days and different builds, so it is effectively
    deterministic. Motion (|vel.z| > 1) starts at 898/897. The documented
    comparison window is [anchor, anchor+314] and is where all the driving is;
    everything before the anchor is the stationary pre-countdown phase.

    A capture is only usable if it REACHES the anchor. Returns None if not.
    """
    frames, _ = msd_frames(p)
    if not frames:
        return None
    try:
        import struct
        with p.open('rb') as fh:
            fh.seek(HDR)
            for i in range(frames):
                rec = fh.read(REC)
                if len(rec) < REC:
                    break
                if struct.unpack_from('<I', rec, 4 + ANCHOR_OFF)[0] != 0:
                    return i
    except Exception:
        return None
    return None


def msd_distinct(p: Path):
    """Count DISTINCT payloads in a capture.

    CAUTION -- this is NOT a usability test, and an earlier version of this file
    wrongly treated it as one. The stationary pre-countdown phase genuinely has
    only 4-5 distinct states: slicing a KNOWN-GOOD 1097-frame capture to the
    lengths of the short runs gives distinct=4 at 236, 446 and 735 frames, and 5
    at 783. So "4 distinct" does not mean the state froze -- it means the capture
    ended before the countdown, where 4 is the correct value. Use msd_anchor()
    to decide usability; keep this only as a descriptive statistic.
    """
    frames, _ = msd_frames(p)
    if not frames:
        return 0 if frames == 0 else None
    seen = set()
    try:
        with p.open('rb') as fh:
            fh.seek(HDR)
            while True:
                rec = fh.read(REC)
                if len(rec) < REC:
                    break
                seen.add(hash(rec[4:]))      # skip frame_idx, hash the payload
    except Exception:
        return None
    return len(seen)


def mashed_pids():
    try:
        out = subprocess.run(
            ['powershell', '-NoProfile', '-Command',
             "(Get-Process MASHED -ErrorAction SilentlyContinue).Id -join ','"],
            capture_output=True, text=True, timeout=30).stdout.strip()
        return {int(x) for x in out.split(',') if x.strip().isdigit()}
    except Exception:
        return set()


def kill_only(pids):
    for pid in sorted(pids):
        try:
            subprocess.run(['powershell', '-NoProfile', '-Command',
                            f'Stop-Process -Id {pid} -Force'],
                           capture_output=True, timeout=30)
            print(f'    killed leftover pid={pid}')
        except Exception as e:
            print(f'    could not kill {pid}: {e}')


def wilson(k, n, z=1.96):
    """Wilson score interval -- honest for small n, unlike k/n +- normal approx."""
    if n == 0:
        return (0.0, 1.0)
    p = k / n
    d = 1 + z * z / n
    c = p + z * z / (2 * n)
    h = z * math.sqrt(p * (1 - p) / n + z * z / (4 * n * n))
    return ((c - h) / d, (c + h) / d)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--runs', type=int, default=16)
    ap.add_argument('--hooks', default='all')
    ap.add_argument('--drive', action='store_true', default=True)
    ap.add_argument('--no-drive', dest='drive', action='store_false')
    ap.add_argument('--degenerate-below', type=int, default=100,
                    help='frames strictly below this (and >0) count DEGENERATE')
    ap.add_argument('--idle-min-frames', type=int, default=1000,
                    help='frames required for an IDLE (--no-drive) run to count '
                         'USABLE; the anchor test does not apply there')
    ap.add_argument('--min-window', type=int, default=200,
                    help='frames required inside [anchor, anchor+314] to count '
                         'USABLE (measured: good runs give 301-314)')
    ap.add_argument('--out-dir', default='')
    ap.add_argument('--timeout', type=int, default=240)
    ap.add_argument('--keep-msd', action='store_true',
                    help='keep every .msd (default keeps only failures, to save disk)')
    a = ap.parse_args()

    out = Path(a.out_dir) if a.out_dir else ROOT / 'verify' / f'wedge_rate_{date.today():%Y%m%d}'
    out.mkdir(parents=True, exist_ok=True)

    print(f'wedge-rate: {a.runs} boots, hooks={a.hooks!r}, drive={a.drive}')
    print(f'  out: {out}')
    print(f'  DEGENERATE = 1..{a.degenerate_below - 1} frames, EMPTY = 0 frames\n')

    results = []
    for i in range(1, a.runs + 1):
        msd = out / f'run_{i:02d}.msd'
        before = mashed_pids()
        cmd = [sys.executable if 'python' in sys.executable.lower() else 'py', '-3.12',
               str(LAUNCH), '--hooks', a.hooks, '--statediff-out', str(msd)]
        if sys.executable and 'python' in sys.executable.lower():
            cmd = [sys.executable, str(LAUNCH), '--hooks', a.hooks,
                   '--statediff-out', str(msd)]
        if a.drive:
            cmd.append('--statediff-drive')

        t0 = time.time()
        rc, timed_out, pr = None, False, None
        try:
            pr = subprocess.run(cmd, cwd=str(ROOT), capture_output=True,
                                text=True, timeout=a.timeout)
            rc = pr.returncode
        except subprocess.TimeoutExpired:
            timed_out = True
        dt = time.time() - t0

        # Keep the launcher's own output for every non-full run. Without it you
        # cannot tell "capture stopped because the RACE ENDED" (legitimate --
        # AI won, car destroyed, round over) from "capture stopped because the
        # run BROKE". The first n=18 pass discarded stdout and could not
        # separate those two, which is the main gap in its conclusion.
        if not timed_out and pr is not None:
            (out / f'run_{i:02d}.stdout.txt').write_text(
                (pr.stdout or '') + '\n--- STDERR ---\n' + (pr.stderr or ''),
                encoding='utf-8', errors='replace')

        frames, rem = msd_frames(msd)
        distinct = msd_distinct(msd)
        anchor = msd_anchor(msd)
        # Usability = reached the countdown anchor and holds some of the
        # documented [anchor, anchor+314] window. Frame count and distinctness
        # both mis-classify here (see msd_distinct docstring).
        window = None if anchor is None else max(0, min(frames, anchor + DRIVE_WINDOW) - anchor)
        if frames is None:
            verdict = 'NOFILE'
        elif frames == 0:
            verdict = 'EMPTY'
        elif not a.drive:
            # IDLE scenario: the countdown witness +0xBF4 never fires because
            # nothing ever accelerates, so the anchor test does NOT apply and
            # applying it labelled every healthy idle run a failure. The idle
            # baseline is ~1255 frames / 34 distinct (matches the archived
            # stock_a/b.msd pair), so judge idle runs on frame count alone.
            verdict = 'USABLE' if frames >= a.idle_min_frames else 'SHORT'
        elif anchor is None:
            verdict = 'PRE-ANCHOR'
        elif window < a.min_window:
            verdict = 'SHORT-WINDOW'
        else:
            verdict = 'USABLE'
        if timed_out:
            verdict += '+TIMEOUT'

        leftover = mashed_pids() - before
        if leftover:
            kill_only(leftover)

        results.append({'run': i, 'frames': frames, 'distinct': distinct,
                        'anchor': anchor, 'window': window, 'remainder': rem,
                        'verdict': verdict, 'rc': rc, 'secs': round(dt, 1),
                        'leftover_pids': sorted(leftover), 'msd': msd.name})
        print(f'  run {i:2d}/{a.runs}  {verdict:<14} frames={frames} anchor={anchor} window={window} distinct={distinct}  '
              f'{dt:5.1f}s  rc={rc}{"  LEFTOVER" if leftover else ""}')

        if verdict.startswith('USABLE') and not a.keep_msd:
            try: msd.unlink()
            except Exception: pass

    n = len(results)
    healthy = sum(1 for r in results if r['verdict'].startswith('USABLE'))
    frozen = sum(1 for r in results if r['verdict'].startswith(('PRE-ANCHOR', 'SHORT-WINDOW', 'SHORT')))
    empty = sum(1 for r in results if r['verdict'].startswith('EMPTY'))
    degen = 0
    nofile = sum(1 for r in results if r['verdict'].startswith('NOFILE'))
    bad = n - healthy
    lo, hi = wilson(bad, n)

    summary = {'runs': n, 'usable': healthy, 'frozen': frozen, 'empty': empty,
               'degenerate': degen, 'nofile': nofile, 'failures': bad,
               'failure_rate': round(bad / n, 4) if n else None,
               'wilson95': [round(lo, 4), round(hi, 4)],
               'hooks': a.hooks, 'drive': a.drive,
               'degenerate_below': a.degenerate_below,
               'runs_detail': results}
    (out / 'REPORT.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')

    print(f'\n=== wedge rate over n={n} ===')
    print(f'  USABLE      {healthy}   (>= {a.min_window} frames inside the anchor window)')
    print(f'  PRE/SHORT   {frozen}   (ended before the countdown anchor, or too little window)')
    print(f'  EMPTY       {empty}   (0 frames -- never reached phase 3)')
    print(f'  DEGENERATE  {degen}   (1..{a.degenerate_below - 1} frames)')
    if nofile:
        print(f'  NOFILE      {nofile}')
    print(f'  failure rate {bad}/{n} = {bad / n:.1%}   Wilson 95% [{lo:.1%}, {hi:.1%}]')
    print(f'  wrote {out / "REPORT.json"}')


if __name__ == '__main__':
    main()
