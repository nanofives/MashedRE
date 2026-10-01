#!/usr/bin/env python3
"""a10_clampcost.py - DETECTOR-FREE cross-side measurement of what happens to the
linear speed between A6a's `+0x9e4` store (Integrate2.cpp:636, original 0x004686cc,
immediately BEFORE grip-clamp #6 at 0x004687f0) and the render-tick snapshot.

    ratio(f) = s_mid(f) / s_post(f) = +0x9e4(f) / |+0x9b0..b8|(f)

On a non-contact frame the only thing between those two points is grip-clamp #6
(W2/W3), because A6a 0x00467650 and VehicleContactFixup 0x0046ef70 are the only
writers of +0x9b0 per frame (section 22.2/22.4) and the fixup does not run. So
ratio > 1 on a non-contact frame is exactly the clamp's speed cost.

This needs NO contact detector and NO gap alignment: the fixup touches at most
N_fixup frames, so any quantile below 1 - N_fixup/N_frames is immune to it. That is
what makes the MEDIAN usable here (attempt 10's registered 1.02-ratio detector
failed S6 on the port precisely because the port's clamp swamps it).

Usage:
  py -3.12 re/tools/statediff/a10_clampcost.py --orig <msd> --port <player_trace.log>
           [--nfix-orig 23] [--nfix-port 212]
"""
import sys, statistics
sys.path.insert(0, __file__.rsplit('\\', 1)[0] if '\\' in __file__ else '.')
from a10_gain import load_msd, load_pt, LIN_DEFAULT

BANDS = [(40, 70), (70, 100), (100, 150), (150, 260), (260, 500),
         (500, 1000), (1000, 1500), (1500, 2000), (2000, 2600)]


def q(xs, p):
    xs = sorted(xs)
    return xs[min(len(xs) - 1, int(p * len(xs)))] if xs else float('nan')


def side(tag, rows, nfix):
    r = [x for x in rows if x['s_post'] > 1e-3]
    rat = [x['s_mid'] / x['s_post'] for x in r]
    cost = [x['s_post'] - x['s_mid'] for x in r]          # <= 0 when the clamp bites
    frac = 1.0 - len(r) and 0
    contam = nfix / float(len(r))
    print('%s  frames %d (s_post>1e-3), fixups %d -> contamination %.1f%%'
          % (tag, len(r), nfix, 100 * contam))
    print('   ratio s_mid/s_post : med %.6f  p25 %.6f  p75 %.6f  p%.0f %.6f  max %.3f'
          % (statistics.median(rat), q(rat, .25), q(rat, .75),
             100 * (1 - contam), q(rat, 1 - contam), max(rat)))
    print('   speed cost (units) : med %+.5f  p25 %+.5f  p75 %+.5f  min %+.3f'
          % (statistics.median(cost), q(cost, .25), q(cost, .75), min(cost)))
    print('   frames ratio>1.02  : %d (%.1f%%)   ratio>1.002 : %d (%.1f%%)'
          % (sum(1 for x in rat if x > 1.02), 100.0 * sum(1 for x in rat if x > 1.02) / len(r),
             sum(1 for x in rat if x > 1.002), 100.0 * sum(1 for x in rat if x > 1.002) / len(r)))
    print('   band        n    med ratio   med cost   med pct-lost')
    out = {}
    for lo, hi in BANDS:
        b = [x for x in r if lo <= x['s_post'] < hi]
        if not b:
            print('   %4d-%-4d %4d        --         --        --' % (lo, hi, 0)); continue
        br = [x['s_mid'] / x['s_post'] for x in b]
        bc = [x['s_post'] - x['s_mid'] for x in b]
        pct = [100.0 * (1.0 - x['s_mid'] / x['s_post']) for x in b]
        out[(lo, hi)] = (len(b), statistics.median(br), statistics.median(bc),
                         statistics.median(pct))
        print('   %4d-%-4d %4d   %9.6f  %+9.4f   %+8.3f%%'
              % (lo, hi, len(b), statistics.median(br), statistics.median(bc),
                 statistics.median(pct)))
    return out


def drive_band(tag, rows):
    """T_drive = linTerm*(ctrl.u) per step, by speed band, NO gap selection."""
    by = {x['f']: x for x in rows}
    st = []
    for x in rows:
        p = by.get(x['f'] - 1)
        if not p or p['s_post'] <= 1e-6:
            continue
        u = tuple(c / p['s_post'] for c in p['vel'])
        st.append((p['s_post'],
                   LIN_DEFAULT * sum(a * b for a, b in zip(x['ctrl'], u)),
                   x['s_mid'] - p['s_post']))
    print('%s  T_drive / T_W1 by FROM-speed band, all steps (no gap selection)' % tag)
    print('   band        n    med T_drive   med T_W1   med T_rest')
    o = {}
    for lo, hi in BANDS:
        b = [s for s in st if lo <= s[0] < hi]
        if not b:
            print('   %4d-%-4d %4d          --         --         --' % (lo, hi, 0)); continue
        d = statistics.median(s[1] for s in b)
        w = statistics.median(s[2] for s in b)
        rr = statistics.median(s[2] - s[1] for s in b)
        o[(lo, hi)] = (len(b), d, w, rr)
        print('   %4d-%-4d %4d   %+10.4f  %+9.4f  %+9.4f' % (lo, hi, len(b), d, w, rr))
    return o


def main(argv):
    orig = port = None
    nfo, nfp = 23, 212
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == '--orig': orig = argv[i + 1]; i += 2; continue
        if a == '--port': port = argv[i + 1]; i += 2; continue
        if a == '--nfix-orig': nfo = int(argv[i + 1]); i += 2; continue
        if a == '--nfix-port': nfp = int(argv[i + 1]); i += 2; continue
        raise SystemExit('unknown arg ' + a)
    ro, rp = load_msd(orig), load_pt(port)
    print('=== grip-clamp #6 SPEED COST, detector-free ===')
    co = side('ORIGINAL', ro, nfo)
    print()
    cp = side('PORT    ', rp, nfp)
    print('\n   band        ORIG ratio  PORT ratio   ORIG %lost  PORT %lost   n(o)  n(p)')
    for k in BANDS:
        if k in co and k in cp:
            print('   %4d-%-4d  %10.6f  %10.6f   %+9.3f%%  %+9.3f%%  %5d %5d'
                  % (k[0], k[1], co[k][1], cp[k][1], co[k][3], cp[k][3], co[k][0], cp[k][0]))
    print()
    do = drive_band('ORIGINAL', ro)
    print()
    dp = drive_band('PORT    ', rp)
    print('\n   band        ORIG T_drive  PORT T_drive   ratio   ORIG T_rest  PORT T_rest'
          '   n(o)  n(p)')
    for k in BANDS:
        if k in do and k in dp:
            print('   %4d-%-4d  %+11.4f  %+11.4f  %6.3f  %+11.4f  %+11.4f  %5d %5d'
                  % (k[0], k[1], do[k][1], dp[k][1],
                     dp[k][1] / do[k][1] if do[k][1] else float('nan'),
                     do[k][3], dp[k][3], do[k][0], dp[k][0]))


if __name__ == '__main__':
    main(sys.argv[1:])
