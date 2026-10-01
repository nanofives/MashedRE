#!/usr/bin/env python3
"""a11_resid.py - DIAGNOSTIC for D2 attempt 11's registered S3/S5 failures.
Read-only, executes no game, changes no source.

a11_accum.py's estimator is verified EXACT against friction_diag.log's verbatim
accum (median rel-err 5e-7, cosine 1.000000000), so `accum` is no longer an
unknown.  That turns section 23's `T_rest` from one lumped term into an exact
three-way split:

    w(f)        = linTerm * (ctrl(f) + accum(f))         W1's whole increment
    predS(f)    = |v_post(f-1) + w(f)|                   what +0x9e4 must hold
    resid(f)    = s_mid(f) - predS(f)                    the UNEXPLAINED part
    curv(f)     = predS(f) - s_prev - linTerm*((ctrl+accum).u)   >= 0 by convexity
    T_accum(f)  = linTerm * (accum(f) . u)

    T_rest(f)  ==  T_accum(f) + curv(f) + resid(f)       (exact, residual ~1e-13)

`curv >= 0` is the subgradient inequality |v+w| >= |v| + w.u, so any band where
median(T_rest) < median(T_accum) is proof that `resid` is non-zero and negative,
i.e. that velocity is leaving the car somewhere OTHER than W1 between the
frame-(f-1) render snapshot and the +0x9e4 store at 0x004686cc.

The PORT's a6a_dump.log carries `act.vel` = A6a's OWN entry velocity
(g_a6aFrame.vel, Integrate2.cpp:239) alongside `snap.vel` = the render-tick
record.  Using act.vel(f) as the base instead of snap.vel(f-1) localises `resid`
to either side of A6a's entry.  The original has no such channel; that asymmetry
is the finding, not a flaw.

Usage:
  a11_resid.py --msd <capture.msd> --dump <a6a_dump.log> --fric <friction_diag.log>
"""
import math, sys, statistics

from a11_accum import (estimate, msd_rows, dump_rows, fric_rows, med, q,
                       dot3, DT_FRAME, KDT, GROUNDED, BANDS)

import re as _re

_ACT = _re.compile(r'act\.vel=([^ ]+)')


def dump_act(path):
    out = []
    for ln in open(path, 'r', errors='replace'):
        m = _ACT.search(ln)
        out.append(tuple(float(x) for x in m.group(1).split(',')) if m else None)
    return out


def split(rows, ests, lts, acts=None):
    out = []
    for i in range(1, len(rows)):
        pr, cu = rows[i - 1], rows[i]
        vprev = pr['vel']
        s_prev = math.sqrt(sum(x * x for x in vprev))
        if s_prev <= 1e-3 or cu['gnd'] != GROUNDED:
            continue
        lt = lts[i]
        ac = [float(x) for x in ests[i]['accum']]
        ct = cu['ctrl']
        u = [x / s_prev for x in vprev]
        w = [lt * (ct[k] + ac[k]) for k in range(3)]
        predS = math.sqrt(sum((vprev[k] + w[k]) ** 2 for k in range(3)))
        s_mid = cu['s_mid']
        row = dict(f=cu['f'], s_from=s_prev, s_mid=s_mid,
                   s_post=math.sqrt(sum(x * x for x in cu['vel'])),
                   T_drive=lt * dot3(ct, u), T_accum=lt * dot3(ac, u),
                   curv=predS - s_prev - dot3(w, u),
                   resid=s_mid - predS, predS=predS)
        row['T_W1'] = s_mid - s_prev
        row['T_rest'] = row['T_W1'] - row['T_drive']
        row['ident'] = row['T_rest'] - (row['T_accum'] + row['curv'] + row['resid'])
        if acts and acts[i] is not None:
            va = acts[i]
            s_act = math.sqrt(sum(x * x for x in va))
            pa = math.sqrt(sum((va[k] + w[k]) ** 2 for k in range(3)))
            row['s_act'] = s_act
            row['dEntry'] = s_act - s_prev          # change BEFORE A6a's entry
            row['residA'] = s_mid - pa              # resid rebased on act.vel
            # axis decomposition of the pre-A6a velocity change
            dv = [va[k] - vprev[k] for k in range(3)]
            row['dvx'], row['dvy'], row['dvz'] = dv
            row['dvMag'] = math.sqrt(sum(x * x for x in dv))
            row['dvLong'] = dot3(dv, u)             # speed-changing part
            row['dvPerp'] = math.sqrt(max(0.0, row['dvMag'] ** 2 - row['dvLong'] ** 2))
        out.append(row)
    return out


def table(name, ts, keys, acts=False):
    hdr = ('band          side   n | ' +
           ' '.join('%10s' % k for k in keys) + '   med_s')
    print('\n' + hdr)
    print('-' * len(hdr))
    for lo, hi in BANDS:
        b = [t for t in ts if lo <= t['s_from'] < hi]
        if not b:
            continue
        print('%-13s %-5s %4d | %s %7.1f'
              % ('%d-%d' % (lo, hi), name, len(b),
                 ' '.join('%10.4f' % med([t[k] for t in b]) for k in keys),
                 med([t['s_from'] for t in b])))


def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    orows = msd_rows(a['msd'])
    oests = [estimate(r['wheels']) for r in orows]
    olts = [DT_FRAME * r['m54'] * KDT for r in orows]
    ots = split(orows, oests, olts)

    drows = dump_rows(a['dump'])
    dests = [estimate(r['wheels']) for r in drows]
    fr = fric_rows(a['fric'])
    n = min(len(drows), len(fr))
    for i in range(n):
        drows[i]['ctrl'] = fr[i]['ctrl']
    acts = dump_act(a['dump'])[:n]
    pts = split(drows[:n], dests[:n], [fr[i]['linTerm'] for i in range(n)], acts)

    print('ORIGINAL steps %d   PORT steps %d' % (len(ots), len(pts)))
    for nm, ts in (('ORIG', ots), ('PORT', pts)):
        ids = [abs(t['ident']) for t in ts]
        print('  %s identity T_rest == T_accum+curv+resid   max |residual| %.3e'
              % (nm, max(ids)))
        neg = sum(1 for t in ts if t['resid'] < 0)
        print('  %s resid<0 on %d/%d = %.1f%%   median resid %.4f'
              % (nm, neg, len(ts), 100.0 * neg / len(ts),
                 med([t['resid'] for t in ts])))
        cneg = sum(1 for t in ts if t['curv'] < -1e-9)
        print('  %s convexity curv<0 on %d/%d (must be 0)' % (nm, cneg, len(ts)))

    keys = ['T_drive', 'T_rest', 'T_accum', 'curv', 'resid']
    print('\n=== the exact split of section 23\'s T_rest ===')
    for nm, ts in (('ORIG', ots), ('PORT', pts)):
        table(nm, ts, keys)

    print('\n=== PORT only: localise resid across A6a\'s entry '
          '(act.vel = Integrate2.cpp:239) ===')
    pa = [t for t in pts if 's_act' in t]
    print('n with act.vel = %d' % len(pa))
    table('PORT', pa, ['dEntry', 'resid', 'residA'])
    print('\n=== PORT only: axis decomposition of the pre-A6a velocity change '
          '(act.vel(f) - snap.vel(f-1)) ===')
    table('PORT', pa, ['dvx', 'dvy', 'dvz', 'dvMag', 'dvLong', 'dvPerp'])

    print('\n=== resid as a share of T_rest, by band ===')
    for lo, hi in BANDS:
        for nm, ts in (('ORIG', ots), ('PORT', pts)):
            b = [t for t in ts if lo <= t['s_from'] < hi]
            if len(b) < 5:
                continue
            mr, mt = med([t['resid'] for t in b]), med([t['T_rest'] for t in b])
            print('  %-10s %-5s n=%4d  resid %9.4f  T_rest %9.4f  share %7.3f'
                  % ('%d-%d' % (lo, hi), nm, len(b), mr, mt,
                     (mr / mt) if mt else float('nan')))


if __name__ == '__main__':
    main(sys.argv[1:])
