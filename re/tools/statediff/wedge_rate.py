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


def msd_distinct(p: Path):
    """Count DISTINCT payloads in a capture.

    This -- not the frame count -- is the discriminating signal. Measured over
    n=18 (verify/wedge_rate2_20260820) the outcome is categorical with no
    overlap: a usable drive capture has 268-271 distinct payloads, a FROZEN one
    has exactly 4 no matter whether it captured 8 frames or 735, and a wedged
    one has 0. A FROZEN capture passes every check the protocol had (non-empty,
    healthy-looking frame count) while containing almost no state evolution to
    diff -- so it can diff GREEN against another FROZEN capture and be recorded
    as evidence of correctness. Frame count cannot see that.
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
    ap.add_argument('--min-distinct', type=int, default=100,
                    help='distinct payloads below this count FROZEN (measured: '
                         'usable=268-271, frozen=exactly 4)')
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
        if frames is None:
            verdict = 'NOFILE'
        elif frames == 0:
            verdict = 'EMPTY'
        elif distinct is not None and distinct < a.min_distinct:
            # Static state -- unusable regardless of how many frames it holds.
            verdict = 'FROZEN'
        elif frames < a.degenerate_below:
            verdict = 'DEGENERATE'
        else:
            verdict = 'USABLE'
        if timed_out:
            verdict += '+TIMEOUT'

        leftover = mashed_pids() - before
        if leftover:
            kill_only(leftover)

        results.append({'run': i, 'frames': frames, 'distinct': distinct,
                        'remainder': rem,
                        'verdict': verdict, 'rc': rc, 'secs': round(dt, 1),
                        'leftover_pids': sorted(leftover), 'msd': msd.name})
        print(f'  run {i:2d}/{a.runs}  {verdict:<18} frames={frames} distinct={distinct}  '
              f'{dt:5.1f}s  rc={rc}{"  LEFTOVER" if leftover else ""}')

        if verdict.startswith('USABLE') and not a.keep_msd:
            try: msd.unlink()
            except Exception: pass

    n = len(results)
    healthy = sum(1 for r in results if r['verdict'].startswith('USABLE'))
    frozen = sum(1 for r in results if r['verdict'].startswith('FROZEN'))
    empty = sum(1 for r in results if r['verdict'].startswith('EMPTY'))
    degen = sum(1 for r in results if r['verdict'].startswith('DEGENERATE'))
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
    print(f'  USABLE      {healthy}   (state evolving, >= {a.min_distinct} distinct payloads)')
    print(f'  FROZEN      {frozen}   (static state -- looks healthy by frame count, is not)')
    print(f'  EMPTY       {empty}   (0 frames -- never reached phase 3)')
    print(f'  DEGENERATE  {degen}   (1..{a.degenerate_below - 1} frames)')
    if nofile:
        print(f'  NOFILE      {nofile}')
    print(f'  failure rate {bad}/{n} = {bad / n:.1%}   Wilson 95% [{lo:.1%}, {hi:.1%}]')
    print(f'  wrote {out / "REPORT.json"}')


if __name__ == '__main__':
    main()
