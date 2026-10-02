#!/usr/bin/env python3
"""D2 LAUNCH + RECOVERY reducer: best-fit integer lag, peak/trough, the 400-frame
post-trough recovery window, and the +0xb14 drive-force engagement frame.

STANDING TOOL. D2 attempt 14 computed all of this in scratchpad scripts that are gone,
so attempt 15 had to rebuild the definitions from the published numbers. This file exists
so the next attempt reads the same statistic instead of re-deriving it (memory
`score-the-bands-from-endpoint-first`: a non-portable statistic fakes a verdict).

DEFINITIONS, fixed here and validated against attempt 14's published table
(`re/analysis/D2_REOPEN_2026-09-29.md` sections 28.3 and 28.4) with `--selftest`:

  release frame R   the first frame the car is commanded to move. Not derived: passed in,
                    because the two arms witness it differently (ORIG 886 on orig_solo3,
                    PORT 1). `d = frame - R`.
  lag L             P[d - L] is compared against O[d] over d in [lo,hi]. L > 0 means the
                    PORT is EARLY by L frames. err(L) = mean |P[d-L] - O[d]| / O[d].
  peak              max horizontal speed at d >= 0, and its d.
  trough            min horizontal speed AFTER the peak frame, and its d.
  recovery window   the 400 frames strictly after the trough frame.
  b14 engagement    the first d at which +0xb14 is not all-zero.

Usage:
  py -3.12 re/tools/statediff/a8_launch.py --orig <orig>.msd --port <dir>/motion_diag.log
        [--orig-release 886] [--port-release 1] [--lag-lo 16] [--lag-hi 95] [--window 400]
  py -3.12 re/tools/statediff/a8_launch.py --selftest
"""
import argparse, math, os, re, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

RE_B14 = re.compile(r"b14=\[([^\]]*)\]")
RE_SP = re.compile(r"\bsp=([-+0-9.eE]+)")


def orig_series(path):
    """-> (sp[], b14[]) in frame order, from the .msd record.

    The speed channel is the record's own +0x9e4 (the port's `sp=`), NOT hypot(velx,velz).
    They agree to ~0.01 % while rolling but diverge hard at the bounce frame (orig d=95:
    sp 1832.40 vs horiz 219.89), and section 28's published peaks are the `sp` ones.
    """
    _, _, frames = m.load_msd(path)
    h, b = [], []
    for idx in sorted(frames):
        p = frames[idx]
        h.append(struct.unpack_from('<f', p, 0x9e4)[0])
        b.append(tuple(struct.unpack_from('<f', p, o)[0] for o in (0xb14, 0xb18, 0xb1c)))
    return h, b


def port_series(path):
    h, b = [], []
    for line in open(path, errors='replace'):
        mh = RE_SP.search(line)
        mb = RE_B14.search(line)
        if not mh or not mb:
            continue
        h.append(float(mh.group(1)))
        b.append(tuple(float(x) for x in mb.group(1).split(',')))
    return h, b


def med(v):
    v = sorted(v)
    return v[len(v) // 2] if v else float('nan')


def best_lag(P, O, rp, ro, lo, hi, lags):
    out = {}
    for L in lags:
        num, n = 0.0, 0
        for d in range(lo, hi + 1):
            i, j = rp + d - L, ro + d
            if i < 0 or i >= len(P) or j >= len(O) or O[j] == 0.0:
                continue
            num += abs(P[i] - O[j]) / O[j]
            n += 1
        out[L] = (num / n * 100.0 if n else float('nan'), n)
    bestL = min((k for k in out if not math.isnan(out[k][0])), key=lambda k: out[k][0])
    return out, bestL


def peak_trough(H, R, window):
    """peak = the FIRST local maximum at d >= 0 (the launch crest, not the global max:
    the original's global max 2562.89 comes 1000 frames later, well past the bounce the
    D2 launch metric is about). trough = the first local minimum after it."""
    seg = [H[R + d] for d in range(0, len(H) - R)]
    if len(seg) < 3:
        return None
    pd = None
    for d in range(0, len(seg) - 1):
        if seg[d] > seg[d + 1]:
            pd = d
            break
    if pd is None:
        pd = len(seg) - 1
    td = None
    for d in range(pd + 1, len(seg) - 1):
        if seg[d] < seg[d + 1]:
            td = d
            break
    if td is None:
        return dict(peak=seg[pd], peak_d=pd, trough=float('nan'), trough_d=-1,
                    n=0, ge100=0, med=float('nan'), mx=float('nan'))
    win = seg[td + 1: td + 1 + window]
    return dict(peak=seg[pd], peak_d=pd, trough=seg[td], trough_d=td, n=len(win),
                ge100=sum(1 for v in win if v >= 100.0),
                med=med(win), mx=max(win) if win else float('nan'))


def engage_d(B, R):
    for d in range(0, len(B) - R):
        if any(x != 0.0 for x in B[R + d]):
            return d
    return None


def emit(tag, H, B, R, pt):
    print(f"  {tag:<16} frames {len(H):5d}  R={R:5d}  "
          f"peak {pt['peak']:8.2f} at d={pt['peak_d']:4d}  "
          f"trough {pt['trough']:7.2f} at d={pt['trough_d']:4d}")
    print(f"  {'':<16} post-trough n={pt['n']:4d}  >=100: {pt['ge100']:4d} "
          f"({100.0*pt['ge100']/pt['n'] if pt['n'] else float('nan'):5.1f} %)  "
          f"median {pt['med']:8.1f}  max {pt['mx']:8.1f}")
    e = engage_d(B, R)
    print(f"  {'':<16} +0xb14 engages at d={e if e is not None else 'NEVER'}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig')
    ap.add_argument('--port')
    ap.add_argument('--orig-release', type=int, default=886)
    ap.add_argument('--port-release', type=int, default=1)
    ap.add_argument('--lag-lo', type=int, default=16)
    ap.add_argument('--lag-hi', type=int, default=95)
    ap.add_argument('--window', type=int, default=400)
    ap.add_argument('--lags', default='0,14,15,16')
    ap.add_argument('--selftest', action='store_true')
    a = ap.parse_args()

    if a.selftest:
        root = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..')
        a.orig = a.orig or os.path.join(root, 'verify/d2_reopen_20260929/orig_solo3.msd')
        a.port = a.port or os.path.join(root, 'verify/d2_sched_20261001/s1/motion_diag.log')
        print("SELFTEST against D2_REOPEN section 28.3/28.4 (orig_solo3 + attempt 14 s1):")
        print("  expect ORIG peak 1832.40 d=95, trough 85.45 d=101, 397/400, median 1333.9, max 2478.3")
        print("  expect PORT peak 1856.57 d=80, trough 13.40 d=151,   0/400, median   31.5, max   68.7")
        print("  expect PORT best lag L=15 at 1.44 %")
        print()

    O, OB = orig_series(a.orig)
    P, PB = port_series(a.port)
    print("=== LAUNCH + RECOVERY ===")
    emit('ORIGINAL', O, OB, a.orig_release, peak_trough(O, a.orig_release, a.window))
    emit('PORT', P, PB, a.port_release, peak_trough(P, a.port_release, a.window))
    lags = [int(x) for x in a.lags.split(',')]
    tab, bestL = best_lag(P, O, a.port_release, a.orig_release, a.lag_lo, a.lag_hi, lags)
    print(f"  best-fit lag over d={a.lag_lo}..{a.lag_hi} "
          f"(n={tab[bestL][1]}):  " +
          "  ".join(f"L={L}: {v:.2f} %" for L, (v, _) in sorted(tab.items())))
    print(f"  BEST L = {bestL}  at {tab[bestL][0]:.2f} %")


if __name__ == '__main__':
    main()
