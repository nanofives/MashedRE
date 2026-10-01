#!/usr/bin/env python3
"""a12_g.py - D2 attempt 12 step 1. Score the directly-logged A5 Phase-4
diagnostic (a5g_diag.log) against §24.4's BACK-OUT of the same quantities from
the render-tick snapshot channel.

Registered in verify/d2_sink_20261001/PREREG.md (commit 26b859fd). Read-only,
executes no game, changes no source.

The log carries, per A5 call (= per frame):
    sEnt  |+0x9b0..0x9b8| at A5 entry, before Phase 4 writes
    sp    +0x9e4 at A5 entry                 (C1's own speed input)
    g0 g1 g2 G   bytes 0x150/0x154/0x158 and their product
    m54 dt base l70 sigma gnd

a11_drag.py backs sigma out of the snapshot as `1 + resid/s_prev` and forms
`l70G = (1-sigma)/(linTerm*s_mid_prev)`. C1 asserts `l70G == l70*G*half`.

Usage:
  a12_g.py --a5g <a5g_diag.log> --dump <a6a_dump.log> --fric <friction_diag.log>
"""
import math, re, sys

from a11_accum import estimate, dump_rows, fric_rows, med, GROUNDED, BANDS
from a11_drag import steps, dump_act, band

_A5G = re.compile(
    r'i=(\d+) sEnt=([-\d.e+]+) sp=([-\d.e+]+) g0=([-\d.e+]+) g1=([-\d.e+]+) '
    r'g2=([-\d.e+]+) G=([-\d.e+]+) m54=([-\d.e+]+) dt=([-\d.e+]+) '
    r'base=([-\d.e+]+) l70=([-\d.e+]+) sigma=([-\d.e+]+) gnd=0x([0-9A-Fa-f]+)')


def a5g_rows(path):
    out = []
    for ln in open(path, 'r', errors='replace'):
        m = _A5G.search(ln)
        if not m:
            continue
        g = m.groups()
        out.append(dict(i=int(g[0]), sEnt=float(g[1]), sp=float(g[2]),
                        g0=float(g[3]), g1=float(g[4]), g2=float(g[5]),
                        G=float(g[6]), m54=float(g[7]), dt=float(g[8]),
                        base=float(g[9]), l70=float(g[10]),
                        sigma=float(g[11]), gnd=int(g[12], 16)))
    return out


def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    ar = a5g_rows(a['a5g'])
    drows = dump_rows(a['dump'])
    fr = fric_rows(a['fric'])
    n = min(len(drows), len(fr))
    for i in range(n):
        drows[i]['ctrl'] = fr[i]['ctrl']
    dests = [estimate(r['wheels']) for r in drows]

    print('a5g lines %d   dump rows %d   fric rows %d' % (len(ar), len(drows), len(fr)))

    # ---- PAIRING PROOF, not an assumption -------------------------------
    # A5 reads +0x9e4 at entry; that value was written by the PREVIOUS frame's
    # A6a and is exactly the snapshot's s_mid of frame f-1. If the positional
    # pairing is right, a5g[k].sp == dump[k-1].s_mid to float precision.
    m = min(len(ar), len(drows))
    d = [abs(ar[k]['sp'] - drows[k - 1]['s_mid']) for k in range(1, m)]
    rel = [abs(ar[k]['sp'] - drows[k - 1]['s_mid']) / max(1e-6, drows[k - 1]['s_mid'])
           for k in range(1, m)]
    print('[PAIR] a5g[k].sp vs dump[k-1].s_mid  n=%d  median abs %.3e  max abs %.3e'
          '  median rel %.3e' % (len(d), med(d), max(d), med(rel)))
    # the same test one step off, as a control: it must be far WORSE
    d2 = [abs(ar[k]['sp'] - drows[k]['s_mid']) for k in range(0, m)]
    print('[PAIR-CTL] a5g[k].sp vs dump[k].s_mid (wrong offset)  median abs %.3e' % med(d2))

    # ---- the port's own back-out, exactly as a11_drag does it ------------
    pts = steps(drows[:n], dests[:n], [fr[i]['linTerm'] for i in range(n)],
                dump_act(a['dump'])[:n])
    # steps() iterates i in range(1,len) over drows; its row for loop index i
    # describes the gap snapshot(i-1) -> A6a entry at frame i, i.e. A5 call
    # number i+1 in 1-based a5g terms. Re-derive the index by matching f.
    fmap = {r['f']: k for k, r in enumerate(drows)}
    for t in pts:
        k = fmap[t['f']]              # 0-based dump index of the CURRENT frame
        t['a5g'] = ar[k] if k < len(ar) else None

    print('\n=== constants directly logged on the PORT ===')
    for key in ('g0', 'g1', 'g2', 'G', 'l70', 'm54', 'dt'):
        v = sorted({round(r[key], 9) for r in ar})
        print('  %-4s distinct over %d calls: %d   %s' % (key, len(ar), len(v), v[:6]))
    gnds = {}
    for r in ar:
        gnds[r['gnd']] = gnds.get(r['gnd'], 0) + 1
    print('  gnd  %s' % {hex(k): v for k, v in sorted(gnds.items())})

    print('\n=== KA-1 / KA-2 per band (rows with a5g paired, both frames grounded) ===')
    hdr = ('band            n |  sigma_log  sigma_bko     d_abs |   l70G_log  '
           'l70G_bko     rel   |   l70      G      med_s')
    print(hdr)
    print('-' * len(hdr))
    ka1, ka2 = [], []
    for lo, hi in BANDS:
        b = [t for t in band(pts, lo, hi) if t['a5g'] is not None]
        if len(b) < 10:
            continue
        sl = med([t['a5g']['sigma'] for t in b])
        sb = med([t['sigma'] for t in b])
        ll = med([t['a5g']['l70'] * t['a5g']['G'] *
                  (1.0 if t['a5g']['gnd'] == GROUNDED else 0.5) for t in b])
        lb = med([t['l70G'] for t in b if t['l70G'] is not None])
        r = abs(ll - lb) / abs(lb) if lb else float('nan')
        ka1.append((('%d-%d' % (lo, hi)), abs(sl - sb), len(b)))
        ka2.append((('%d-%d' % (lo, hi)), r, len(b)))
        print('%-13s %4d | %10.7f %10.7f %9.2e | %9.6f %9.6f %7.4f | %6.4f %7.5f %8.1f'
              % ('%d-%d' % (lo, hi), len(b), sl, sb, abs(sl - sb), ll, lb, r,
                 med([t['a5g']['l70'] for t in b]),
                 med([t['a5g']['G'] for t in b]),
                 med([t['s_from'] for t in b])))

    print('\n=== gate verdicts ===')
    if ka1:
        w = max(ka1, key=lambda x: x[1])
        print('  KA-1  bar abs <= 1e-3   worst band %-10s %.3e  n=%d   %s'
              % (w[0], w[1], w[2], 'PASS' if w[1] <= 1e-3 else 'FAIL'))
    if ka2:
        w = max(ka2, key=lambda x: x[1])
        print('  KA-2  bar rel <= 0.10   worst band %-10s %.4f    n=%d   %s'
              % (w[0], w[1], w[2], 'PASS' if w[1] <= 0.10 else 'FAIL'))
    print('  bands scored: %d' % len(ka1))


if __name__ == '__main__':
    main(sys.argv[1:])
