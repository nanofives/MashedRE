#!/usr/bin/env python3
"""a12_split.py - D2 attempt 12 step 3. Split the per-frame speed budget into its
PRE-A6a and IN-A6a halves on BOTH sides, with no estimator in either half.

Registered in verify/d2_sink_20261001/PREREG_3.md (commit a17cf0c0). Read-only,
executes no game, changes no source.

    C - A  ==  (B - A)  +  (C - B)
    A = |snapVel(f-1)|   B = |vel at A6a ENTRY of frame f|   C = s_mid(f) = +0x9e4

ORIGINAL B: scenario_launch --fixup-probe site 2 (entry hook 0x00467650, ESI
filtered); the join to the .msd is proven bit-exact at shift d=+2 (step 2, G2).
PORT B: g_a6aFrame.vel, Integrate2.cpp:235-239, the `act.vel=` field of
a6a_dump.log.

Usage:
  a12_split.py --msd <orig.msd> --csv <orig.fixupprobe.csv>
               --dump <port a6a_dump.log> --fric <port friction_diag.log>
"""
import math, sys

from a11_accum import (msd_rows, dump_rows, fric_rows, estimate, med, dot3,
                       BANDS, GROUNDED, DT_FRAME, KDT)
from a11_drag import dump_act
from a12_entry import site2, mag


def bands_of(rows, key='A'):
    return rows


def build(rows):
    """rows: list of dicts with A,B,C,w,velPrev,velCur,gndPrev,gndCur."""
    for r in rows:
        r['total'] = r['C'] - r['A']
        r['dPre'] = r['B'] - r['A']
        r['dIn'] = r['C'] - r['B']
        if r['w'] is not None and r['entry'] is not None:
            pred = mag(tuple(r['entry'][k] + r['w'][k] for k in range(3)))
            r['residA'] = r['C'] - pred
        else:
            r['residA'] = None
        r['ratio9e4'] = (r['C'] / r['sCur']) if r['sCur'] > 1e-6 else None
        # netTotal is frame-to-frame on the SNAPSHOT velocity alone, so it is
        # immune to where in the frame +0x9e4 is latched. It is the statistic
        # that decides whether the car actually loses speed or whether only the
        # +0x9e4 write order differs.
        r['net'] = r['sCur'] - r['A']
    return rows


def orig_side(msd, csv_path, shift=2):
    mr = msd_rows(msd)
    cr = site2(csv_path)
    ests = [estimate(r['wheels']) for r in mr]
    out = []
    for k in range(len(cr)):
        j, jc = k + shift - 1, k + shift
        if not (0 <= j < len(mr) and 0 <= jc < len(mr)):
            continue
        pv = mr[j]['vel']
        lt = DT_FRAME * mr[jc]['m54'] * KDT
        ac = [float(x) for x in ests[jc]['accum']]
        ct = mr[jc]['ctrl']
        out.append(dict(A=mag(pv), B=mag(cr[k]['vel']), C=mr[jc]['s_mid'],
                        entry=cr[k]['vel'],
                        w=[lt * (ct[i] + ac[i]) for i in range(3)],
                        sCur=mag(mr[jc]['vel']),
                        gndP=mr[j]['gnd'], gndC=mr[jc]['gnd']))
    return build(out)


def port_side(dump, fric):
    dr = dump_rows(dump)
    fr = fric_rows(fric)
    acts = dump_act(dump)
    n = min(len(dr), len(fr), len(acts))
    ests = [estimate(r['wheels']) for r in dr[:n]]
    out = []
    for i in range(1, n):
        if acts[i] is None:
            continue
        lt = fr[i]['linTerm']
        ac = [float(x) for x in ests[i]['accum']]
        ct = fr[i]['ctrl']
        out.append(dict(A=mag(dr[i - 1]['vel']), B=mag(acts[i]), C=dr[i]['s_mid'],
                        entry=acts[i],
                        w=[lt * (ct[k] + ac[k]) for k in range(3)],
                        sCur=mag(dr[i]['vel']),
                        gndP=dr[i - 1]['gnd'], gndC=dr[i]['gnd']))
    return build(out)


def band(rows, lo, hi):
    return [r for r in rows if lo <= r['A'] < hi
            and r['gndP'] == GROUNDED and r['gndC'] == GROUNDED]


def ratio(a, b):
    if b == 0:
        return float('inf') if a else 1.0
    return a / b


def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    O = orig_side(a['msd'], a['csv'])
    P = port_side(a['dump'], a['fric'])
    print('ORIG paired frames %d   PORT paired frames %d' % (len(O), len(P)))

    hdr = ('band          side    n |     A        total      dPre       dIn  '
           '   residA   ratio9e4      net')
    print('\n=== S0..S4 per band (grounded both frames, banded by A) ===')
    print(hdr)
    print('-' * len(hdr))
    rows = {}
    for lo, hi in BANDS:
        bo, bp = band(O, lo, hi), band(P, lo, hi)
        if len(bo) < 10 or len(bp) < 10:
            continue
        rec = {}
        for nm, b in (('ORIG', bo), ('PORT', bp)):
            ra = [r['residA'] for r in b if r['residA'] is not None]
            r9 = [r['ratio9e4'] for r in b if r['ratio9e4'] is not None]
            rec[nm] = dict(n=len(b), A=med([r['A'] for r in b]),
                           total=med([r['total'] for r in b]),
                           dPre=med([r['dPre'] for r in b]),
                           dIn=med([r['dIn'] for r in b]),
                           residA=med(ra), r9=med(r9),
                           net=med([r['net'] for r in b]))
            print('%-13s %-5s %4d | %8.1f %10.4f %9.4f %9.4f %9.4f %9.6f %8.3f'
                  % ('%d-%d' % (lo, hi), nm, len(b), rec[nm]['A'],
                     rec[nm]['total'], rec[nm]['dPre'], rec[nm]['dIn'],
                     rec[nm]['residA'], rec[nm]['r9'], rec[nm]['net']))
        rows[(lo, hi)] = rec

    print('\n=== gates ===')
    tc = []
    for k, rec in rows.items():
        tc.append((k, abs(rec['PORT']['residA'])))
    w = max(tc, key=lambda x: x[1]) if tc else None
    print('[T-C] PORT |residA| <= 0.01 in every band  worst %s %.4f  -> %s'
          % ('%d-%d' % w[0], w[1], 'PASS' if w[1] <= 0.01 else 'FAIL'))

    for gname, key, lo_b, hi_b in (('T-A', 'dIn', 0.5, 2.0),
                                   ('T-B', 'total', 0.5, 2.0),
                                   ('NET (unregistered, reported)', 'net', 0.5, 2.0)):
        bad = []
        print('[%s] %s ORIG/PORT in [%.1f, %.1f]' % (gname, key, lo_b, hi_b))
        for k, rec in rows.items():
            r = ratio(rec['ORIG'][key], rec['PORT'][key])
            ok = lo_b <= r <= hi_b
            if not ok:
                bad.append(k)
            print('      %-11s ORIG %9.4f  PORT %9.4f  ratio %8.3f  n=%d/%d  '
                  'med_s %.1f/%.1f  %s'
                  % ('%d-%d' % k, rec['ORIG'][key], rec['PORT'][key], r,
                     rec['ORIG']['n'], rec['PORT']['n'],
                     rec['ORIG']['A'], rec['PORT']['A'], 'ok' if ok else 'OUT'))
        print('      -> %s (%d band(s) out)' % ('PASS' if not bad else 'FAIL', len(bad)))

    # PREREG_3 section 4 naming bar 3: the candidate's measured per-band magnitude
    # must reproduce the divergence to within a factor of 2, in every band where the
    # divergence exceeds the T-A bound. clampLoss = C - |snapVel(f)| is the velocity
    # the trailing clamp removes AFTER +0x9e4 is latched, per sample.
    print('[BAR-3] clamp loss (C - |snapVel|) per sample, and the net divergence')
    print('      band          n(o)/n(p) |  ORIG loss |  PORT loss | net diverg |'
          ' (P-O)/div')
    for lo, hi in BANDS:
        bo, bp = band(O, lo, hi), band(P, lo, hi)
        if len(bo) < 10 or len(bp) < 10:
            continue
        lo_o = med([r['C'] - r['sCur'] for r in bo])
        lo_p = med([r['C'] - r['sCur'] for r in bp])
        div = med([r['net'] for r in bo]) - med([r['net'] for r in bp])
        print('      %-13s %4d/%-4d | %10.4f | %10.4f | %10.4f | %9.3f'
              % ('%d-%d' % (lo, hi), len(bo), len(bp), lo_o, lo_p, div,
                 (lo_p - lo_o) / div if div else float('nan')))

    print('[T-D] ratio9e4 ORIG vs PORT, |diff| <= 0.01')
    bad = []
    for k, rec in rows.items():
        d = abs(rec['ORIG']['r9'] - rec['PORT']['r9'])
        if d > 0.01:
            bad.append(k)
        print('      %-11s ORIG %9.6f  PORT %9.6f  diff %8.6f  %s'
              % ('%d-%d' % k, rec['ORIG']['r9'], rec['PORT']['r9'], d,
                 'ok' if d <= 0.01 else 'OUT'))
    print('      -> %s (%d band(s) out)' % ('PASS' if not bad else 'FAIL', len(bad)))


if __name__ == '__main__':
    main(sys.argv[1:])
