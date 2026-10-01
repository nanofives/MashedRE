#!/usr/bin/env python3
"""a11_drag.py - test the C1 hypothesis for the pre-A6a velocity scale measured by
a11_resid.py.  Registered in verify/d2_accum_20261001/PREREG_C.md.  Read-only,
executes no game, changes no source.

MEASURED (a11_resid.py, committed 37220f4d): between the frame-(f-1) render-tick
snapshot and A6a's entry at frame f, the port multiplies +0x9b0..0x9b8 by a
single scalar (per-component ratio spread 3.8e-08 median / 1.0e-07 max on
1628/1628 frames), with zero perpendicular part, and the resulting speed loss is
~4.1e-6 * speed^2 per frame.

C1, the one ungated in-window candidate that produces that shape.  A5
`VehicleWheelForceIntegrate`, RVA 0x0046ddb0, Phase 4,
mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:86-90 and :164-168, read
verbatim this session.  A5 is called at VehicleControl.cpp:207, A6a at :215, so
it is in-window:

    fVar4 = vF(self,0x279)                      # +0x9e4  = s_mid(f-1)
    fVar4 = vF(0x54)*vF(0x55)*vF(0x56) * fVar4  # bytes 0x150/0x154/0x158
    if self[0x278] != 0x40800000: fVar4 *= 0.5  # +0x9e0 grounded gate
    fVar4 = fVar4 * vF(self,0x15) * dt * kDt    # byte 0x54, dt*kDt
    ...
    fVar4 = 1.0 - local_70 * fVar4              # :164
    if fVar4 < 0 or 1 < fVar4: fVar4 = 0        # :165
    vel *= fVar4                                # :166-168  PURE SCALAR

The gravity add at :169-174 is exactly zero (g_gravScale = 0,
ForceIntegratorStubs.cpp:24-25), so the change stays collinear - which is what
the measured dvPerp = 0.0000 requires.

`local_70` (Phases 2-3, :93-161) is an A5 local and is NOT a record field, so it
is backed out:

    sigma     = 1 + resid / s_post(f-1)         # resid is model-free
    K         = G * m54 * dt * kDt,  G = f(0x150)*f(0x154)*f(0x158), m54 = f(0x54)
    local_70  = (1 - sigma) / (K * s_mid(f-1))

FALSIFICATION.  `local_70`'s construction has NO speed term at all: drafting
proximity (:94-120), player count (:123-125), the race-timer ramp (:127-134),
contact counts (:136-152), RubberBandGrip/Gate (:154-160).  On a solo run with
steer held, none of those varies with speed.  So if C1 is the whole of the
pre-A6a scalar, the backed-out `local_70` must be (a) inside [0, 2.0] - its
construction cannot leave that range - and (b) near-constant across speed bands.
A 200x drift across bands refutes C1, or refutes G.

Usage:
  a11_drag.py --msd <capture.msd> --dump <a6a_dump.log> --fric <friction_diag.log>
"""
import math, re, sys, statistics

from a11_accum import (estimate, msd_rows, dump_rows, fric_rows, med, dot3,
                       DT_FRAME, KDT, GROUNDED, BANDS, load_msd, rf, ri)

_ACT = re.compile(r'act\.vel=([^ ]+)')

# record fields C1 reads, as BYTE offsets (vF(v,i) on an int* is byte 4*i)
B_G = (0x150, 0x154, 0x158)      # vF(0x54) / vF(0x55) / vF(0x56)
B_M54 = 0x54                     # vF(0x15)
B_SP = 0x9e4                     # vF(0x279)


def msd_rows_g(path):
    """msd_rows plus C1's own inputs."""
    rows = msd_rows(path)
    for (fi, p), r in zip(load_msd(path), rows):
        assert fi == r['f']
        r['G'] = rf(p, B_G[0]) * rf(p, B_G[1]) * rf(p, B_G[2])
        r['g0'], r['g1'], r['g2'] = (rf(p, b) for b in B_G)
    return rows


_SG = re.compile(r'f=(\d+) ')


def dump_act(path):
    out = []
    for ln in open(path, 'r', errors='replace'):
        m = _ACT.search(ln)
        out.append(tuple(float(x) for x in m.group(1).split(',')) if m else None)
    return out


def steps(rows, ests, lts, acts=None, _unused=None):
    out = []
    for i in range(1, len(rows)):
        pr, cu = rows[i - 1], rows[i]
        s_prev = math.sqrt(sum(x * x for x in pr['vel']))
        if s_prev <= 1e-3 or cu['gnd'] != GROUNDED or pr['gnd'] != GROUNDED:
            continue
        lt, ct = lts[i], cu['ctrl']
        ac = [float(x) for x in ests[i]['accum']]
        w = [lt * (ct[k] + ac[k]) for k in range(3)]
        predS = math.sqrt(sum((pr['vel'][k] + w[k]) ** 2 for k in range(3)))
        resid = cu['s_mid'] - predS
        sig = 1.0 + resid / s_prev
        G = pr.get('G')
        s_mid_prev = pr['s_mid']
        # lt == linTerm == m54 * dt * kDt, the SAME product C1's :90 forms.
        # l70G is computable on BOTH sides; G (and hence local_70) only where the
        # record is available, i.e. the original's .msd.
        den = lt * s_mid_prev
        l70G = ((1.0 - sig) / den) if den > 1e-12 else None
        l70 = (l70G / G) if (l70G is not None and G) else None
        row = dict(f=cu['f'], s_from=s_prev, resid=resid, sigma=sig,
                   G=G, l70G=l70G, s_mid_prev=s_mid_prev, l70=l70, lt=lt,
                   g0=pr.get('g0'), g1=pr.get('g1'), g2=pr.get('g2'))
        if acts and acts[i] is not None:
            sa = math.sqrt(sum(x * x for x in acts[i]))
            row['sigma_true'] = sa / s_prev
        out.append(row)
    return out


def band(ts, lo, hi):
    return [t for t in ts if lo <= t['s_from'] < hi]


def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    orows = msd_rows_g(a['msd'])
    oests = [estimate(r['wheels']) for r in orows]
    ots = steps(orows, oests, [DT_FRAME * r['m54'] * KDT for r in orows])

    drows = dump_rows(a['dump'])
    dests = [estimate(r['wheels']) for r in drows]
    fr = fric_rows(a['fric'])
    n = min(len(drows), len(fr))
    for i in range(n):
        drows[i]['ctrl'] = fr[i]['ctrl']
    # the port's record is not in the dump as raw bytes; G comes from the
    # ORIGINAL-format fields the dump does not carry, so the port's G is taken
    # from its own record via the same byte offsets in player_trace-less form:
    # motion_diag's gt[] is the gear TABLE, not these three slots, so the port's
    # G must be supplied explicitly or left None.
    pts = steps(drows[:n], dests[:n], [fr[i]['linTerm'] for i in range(n)],
                dump_act(a['dump'])[:n])

    print('ORIGINAL steps %d   PORT steps %d' % (len(ots), len(pts)))

    # KA4: sigma from resid against sigma from act.vel (port only)
    pa = [t for t in pts if 'sigma_true' in t]
    d = [abs(t['sigma'] - t['sigma_true']) for t in pa]
    print('[KA4] port sigma(resid) vs sigma(act.vel)  n=%d  median abs diff %.3e'
          '  p95 %.3e  max %.3e'
          % (len(d), med(d), sorted(d)[int(0.95 * len(d))], max(d)))

    print('\n=== C1 back-out, both sides. l70G = (1-sigma)/(linTerm*s_mid_prev) ===')
    hdr = ('band          side    n |    sigma  s_mid_prev     resid      l70*G'
           '        G     local_70   med_s')
    print(hdr)
    print('-' * len(hdr))
    for lo, hi in BANDS:
        for nm, ts in (('ORIG', ots), ('PORT', pts)):
            b = band(ts, lo, hi)
            if not b:
                continue
            lg = [t['l70G'] for t in b if t['l70G'] is not None]
            l7 = [t['l70'] for t in b if t['l70'] is not None]
            Gs = [t['G'] for t in b if t['G'] is not None]
            print('%-13s %-5s %4d | %8.6f %11.2f %9.4f %10.4g %8.4g %10.4g %7.1f'
                  % ('%d-%d' % (lo, hi), nm, len(b),
                     med([t['sigma'] for t in b]),
                     med([t['s_mid_prev'] for t in b]),
                     med([t['resid'] for t in b]),
                     med(lg) if lg else float('nan'),
                     med(Gs) if Gs else float('nan'),
                     med(l7) if l7 else float('nan'),
                     med([t['s_from'] for t in b])))

    print('\n=== gate tests ===')
    for nm, ts, key in (('ORIG l70G', ots, 'l70G'), ('PORT l70G', pts, 'l70G'),
                        ('ORIG local_70', ots, 'l70')):
        vals = []
        for lo, hi in BANDS:
            b = band(ts, lo, hi)
            if len(b) < 10:
                continue
            v = [t[key] for t in b if t[key] is not None]
            if v:
                vals.append(med(v))
        if not vals:
            print('  %-14s no band with n>=10' % nm)
            continue
        print('  %-14s bands=%d  min %.4g  max %.4g  max/min %.4g   in [0,2]? %s'
              % (nm, len(vals), min(vals), max(vals), max(vals) / min(vals),
                 'YES' if (max(vals) <= 2.0 and min(vals) >= 0.0) else 'NO'))

    print('\n=== ORIGINAL: are G\'s three fields constant? ===')
    for k in ('g0', 'g1', 'g2', 'G'):
        v = sorted({round(t[k], 6) for t in ots if t[k] is not None})
        print('  %-3s distinct values over %d steps: %d   %s'
              % (k, len(ots), len(v), v[:8]))


if __name__ == '__main__':
    main(sys.argv[1:])
