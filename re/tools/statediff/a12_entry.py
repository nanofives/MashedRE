#!/usr/bin/env python3
"""a12_entry.py - D2 attempt 12 step 2. Reduce the ORIGINAL's A6a-ENTRY velocity
(scenario_launch.py --fixup-probe site 2, entry hook on 0x00467650, ESI-filtered)
against the render-tick .msd snapshot from the SAME run.

Registered in verify/d2_sink_20261001/PREREG_2.md (commit 0d5ff8b7). Read-only,
executes no game, changes no source.

Static facts (MASHED.exe.unpatched, re/tools/disasm_va.py, this session):
  0x00467650  SUB ESP,0xe4            A6a entry, record in ESI
  0x00467660  LEA EAX,[ESI+0x9b0]     -> RwV3dLength 0x4c3ac0
  0x00467673  FSTP [ESI+0x9e4]        +0x9e4 := |vel| AT ENTRY
  0x004686cc  MOV  [ESI+0x9e4],EDX    +0x9e4 := post-W1 speed  (the .msd's s_mid)
So the site-2 `speed` column is the PREVIOUS frame's post-W1 speed, which is
exactly msd[f-1].s_mid -- a known-answer join, not an assumption.

Usage:
  a12_entry.py --msd <orig.msd> --csv <orig.msd.fixupprobe.csv> [--portlog <a5g_diag.log>]
"""
import csv, math, sys

from a11_accum import msd_rows, med, BANDS, GROUNDED, load_msd, rf

# section 24.3's ORIGINAL `resid`, the known-answer target for M3
S243_RESID = {(70, 100): -7.8859, (100, 150): -12.9566, (150, 260): -8.5290,
              (260, 500): -4.0489, (500, 1000): -4.3643, (1000, 1500): -7.8728,
              (1500, 2000): -19.4471}


def site2(path):
    out = []
    for r in csv.DictReader(open(path, newline='')):
        if r['site'] != '2':
            continue
        out.append(dict(seq=int(r['seq']), frame=int(r['frame']),
                        vel=(float(r['velx']), float(r['vely']), float(r['velz'])),
                        speed=float(r['speed']), gnd=float(r['gnd'])))
    out.sort(key=lambda x: x['seq'])
    return out


def mag(v):
    return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])


def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    mr = msd_rows(a['msd'])
    # body forward axis +0x9d4/+0x9d8/+0x9dc, needed for M4 and not in msd_rows
    for r, (_fi, p) in zip(mr, load_msd(a['msd'])):
        r['fwd'] = (rf(p, 0x9d4), rf(p, 0x9d8), rf(p, 0x9dc))
    cr = site2(a['csv'])
    print('msd frames %d   site-2 rows %d' % (len(mr), len(cr)))

    # ---------------- G1 ----------------
    g1n = len(cr) >= 1000
    g1r = abs(len(cr) - len(mr)) <= 0.02 * len(mr)
    print('[G1] n>=1000 %s   within +/-2%% of msd frames %s  (|%d-%d| = %d, bar %.1f)'
          % ('PASS' if g1n else 'FAIL', 'PASS' if g1r else 'FAIL',
             len(cr), len(mr), abs(len(cr) - len(mr)), 0.02 * len(mr)))

    # ---------------- G2: discover the shift, do not assume it ----------------
    # csv row k (0-based) should pair with msd row k+d; its `speed` is msd[k+d-1].s_mid.
    best = []
    for d in range(-4, 5):
        rel = []
        for k in range(len(cr)):
            j = k + d - 1
            if 0 <= j < len(mr) and mr[j]['s_mid'] > 1.0:
                rel.append(abs(cr[k]['speed'] - mr[j]['s_mid']) / mr[j]['s_mid'])
        if len(rel) > 100:
            best.append((med(rel), d, len(rel)))
    best.sort()
    print('[G2] shift scan (median rel of site2.speed vs msd[k+d-1].s_mid):')
    for m, d, n in best[:5]:
        print('      d=%+d  median rel %.3e  n=%d' % (d, m, n))
    m0, D, _ = best[0]
    m1 = best[1][0] if len(best) > 1 else float('inf')
    g2 = (m0 <= 1e-5) and (m1 >= 100 * max(m0, 1e-300))
    print('[G2] best d=%+d  %.3e   runner-up %.3e   ratio %.1fx   bar <=1e-5 and >=100x   %s'
          % (D, m0, m1, (m1 / m0) if m0 else float('inf'), 'PASS' if g2 else 'FAIL'))

    # ---------------- build the paired table ----------------
    P = []
    for k in range(len(cr)):
        j = k + D - 1                       # the PREVIOUS msd frame
        jc = k + D                          # this frame's msd row
        if not (0 <= j < len(mr) and 0 <= jc < len(mr)):
            continue
        pv, ev = mr[j]['vel'], cr[k]['vel']
        sp, se = mag(pv), mag(ev)
        P.append(dict(k=k, prev=mr[j], cur=mr[jc], ev=ev, se=se, sp=sp,
                      gndp=mr[j]['gnd'], gndc=mr[jc]['gnd']))

    # ---------------- G3 non-degeneracy ----------------
    live = [t for t in P if t['sp'] > 10.0]
    diff = [t for t in live
            if abs(t['se'] - t['sp']) / t['sp'] > 1e-6]
    frac = len(diff) / len(live) if live else 0.0
    print('[G3] entry vel differs from snapshot vel on %d of %d live frames (%.1f%%)'
          '  bar >=50%%  %s' % (len(diff), len(live), 100 * frac,
                                'PASS' if frac >= 0.5 else 'FAIL'))

    if not (g1n and g1r and g2 and frac >= 0.5):
        print('\n*** R1: a gate FAILED. STOP. No measurement below is used. ***')
        return

    # ---------------- M1 purity ----------------
    spreads = []
    for t in live:
        pv, ev = t['prev']['vel'], t['ev']
        rs = [ev[c] / pv[c] for c in range(3) if abs(pv[c]) > 1e-4]
        if len(rs) >= 2:
            spreads.append(max(rs) - min(rs))
    print('\n[M1] per-component ratio spread  n=%d  median %.3e  p95 %.3e  max %.3e'
          '   bar median<=1e-5 -> %s'
          % (len(spreads), med(spreads), sorted(spreads)[int(0.95 * len(spreads))],
             max(spreads), 'PURE' if med(spreads) <= 1e-5 else 'NOT PURE'))

    # ---------------- bands: M2 / M3 / M4 ----------------
    print('\n=== per band (grounded both frames, banded by |msd[f-1].vel|) ===')
    hdr = ('band            n |   sigma_o     resid_dir   s24.3_resid    rel |'
           '   slip    med_s')
    print(hdr)
    print('-' * len(hdr))
    m3 = []
    for lo, hi in BANDS:
        b = [t for t in P if lo <= t['sp'] < hi
             and t['gndp'] == GROUNDED and t['gndc'] == GROUNDED]
        if len(b) < 10:
            continue
        sig = med([t['se'] / t['sp'] for t in b])
        rd = med([t['se'] - t['sp'] for t in b])
        ref = S243_RESID.get((lo, hi))
        rel = abs(rd - ref) / abs(ref) if ref else float('nan')
        if ref:
            m3.append(('%d-%d' % (lo, hi), rel, len(b)))
        slips = []
        for t in b:
            v, f = t['prev']['vel'], t['prev']['fwd']
            fm = mag(f)
            if fm < 1e-6 or t['sp'] < 1e-6:
                continue
            u = [c / fm for c in f]
            d = v[0] * u[0] + v[1] * u[1] + v[2] * u[2]
            perp = mag(tuple(v[c] - d * u[c] for c in range(3)))
            slips.append(perp / t['sp'])
        print('%-13s %4d | %9.6f %11.4f %12s %6s | %6.3f %8.1f'
              % ('%d-%d' % (lo, hi), len(b), sig, rd,
                 ('%.4f' % ref) if ref else '--',
                 ('%.3f' % rel) if ref else '--',
                 med(slips), med([t['sp'] for t in b])))

    print('\n[M3] known-answer vs section 24.3  bar rel <= 0.25')
    if m3:
        w = max(m3, key=lambda x: x[1])
        for nm, r, n in m3:
            print('      %-10s rel %.3f  n=%d  %s' % (nm, r, n, 'ok' if r <= 0.25 else 'OVER'))
        print('      worst %s %.3f  ->  %s' % (w[0], w[1], 'R2 (agrees)' if w[1] <= 0.25
                                               else 'R3 (DISAGREES)'))


if __name__ == '__main__':
    main(sys.argv[1:])
