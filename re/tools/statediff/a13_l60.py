#!/usr/bin/env python3
"""a13_l60.py - D2 attempt 13 step 1. Runs the gates and the measurement
registered in verify/d2_l60_20261001/PREREG_STEP1.md. Read-only; it launches
nothing.

Four gates, each of which STOPS the section if it fails:
  G1  capture integrity + the known-answer self-check on the mag-probe's
      return-address tagging (site 004686a9's vector == the record's own
      +0x9b0/+0x9b8). Re-verified here, NOT inherited from section 21.9.
  G2  pairing by COUNT, not by median: the full per-frame distribution of
      004680fb and 0046820f rows plus the disagreement count.
  G3  known-answer on the PORT channel: act.l60 * act.speed == act.grip.
  G4  the horizontal projection is legitimate: fwdx^2 + fwdz^2 ~= 1.

Then, per speed band with n and median speed on every row:
  ORIGINAL  l_60 = sum over paired wheels of mag@0046820f * min(mag@004680fb,1024)
            and s = |v_h - (v_h.f_h) f_h| / |v_h| at the first --fixup-probe
            site-1 sample after each site-2 row.
  PORT      act.l60, act.grip, act.kvel, the act.arm histogram, and the same s
            from snap.vel / snap.bf.

Usage:
  a13_l60.py --mag <read1.msd.magprobe.csv> --fixup <orig.fixupprobe.csv>
             --dump <port a6a_dump.log>
"""
import collections
import csv
import math
import re
import sys

from a11_accum import med, BANDS

SITES = ('00467673', '00467685', '004680fb', '0046820f', '00468343',
         '004684c0', '004684dc', '004685bc', '004686a9')
FRAME_DELIM = '00467673'
LE4_SITE = '004680fb'
LD4_SITE = '0046820f'
SELFCHECK_SITE = '004686a9'
LE4_CAP = 1024.0          # 0x0046816c writes 0x44800000 into frame -228


def mag3(x, y, z):
    return math.sqrt(x * x + y * y + z * z)


def lateral_frac(vx, vy, vz, fx, fz):
    """s = |v - (v.f)f| / |v| using the HORIZONTAL forward axis (no fwdy in the
    probe CSV; G4 checks fx^2+fz^2 ~= 1, so fy is negligible)."""
    vm = mag3(vx, vy, vz)
    if vm <= 1e-9:
        return None
    d = vx * fx + vz * fz
    lx, ly, lz = vx - d * fx, vy, vz - d * fz
    return mag3(lx, ly, lz) / vm


def band_of(s):
    for lo, hi in BANDS:
        if lo <= abs(s) < hi:
            return (lo, hi)
    return None


def hdr(t):
    print('\n' + t)
    print('-' * len(t))


# ----------------------------------------------------------------- G1 / G2

def orig_l60(path):
    rows = list(csv.DictReader(open(path, newline='')))
    cnt = collections.Counter(r['site'] for r in rows)
    hdr('G1  mag-probe capture integrity  (%s)' % path)
    print('  rows %d' % len(rows))
    missing = [s for s in SITES if cnt[s] == 0]
    for s in SITES:
        print('    site %s  n=%6d' % (s, cnt[s]))
    ok1 = not missing and cnt[SELFCHECK_SITE] >= 1000

    good = bad = 0
    for r in rows:
        if r['site'] != SELFCHECK_SITE:
            continue
        vx, vz = float(r['vx']), float(r['vz'])
        rx, rz = float(r['rec_velx']), float(r['rec_velz'])
        scale = max(abs(rx), abs(rz), 1e-6)
        if abs(vx - rx) / scale <= 1e-5 and abs(vz - rz) / scale <= 1e-5:
            good += 1
        else:
            bad += 1
    frac = good / max(1, good + bad)
    print('  known-answer: site %s vector == record +0x9b0/+0x9b8 on '
          '%d of %d rows (%.4f)' % (SELFCHECK_SITE, good, good + bad, frac))
    print('  G1 %s  (all nine sites populated=%s, n@selfcheck=%d, frac=%.4f)'
          % ('PASS' if (ok1 and frac >= 0.99) else 'FAIL',
             not missing, cnt[SELFCHECK_SITE], frac))
    if not (ok1 and frac >= 0.99):
        sys.exit('G1 FAILED - section void, stopping as registered')

    # ---- G2: frames delimited by 00467673, counts reported in full
    frames, cur = [], None
    for r in rows:
        if r['site'] == FRAME_DELIM:
            if cur is not None:
                frames.append(cur)
            cur = dict(sp=float(r['rec_speed']), gnd=float(r['gnd']),
                       m18c=float(r['rec_18c']), le=[], ld=[])
        elif cur is not None:
            if r['site'] == LE4_SITE:
                cur['le'].append(mag3(float(r['vx']), float(r['vy']), float(r['vz'])))
            elif r['site'] == LD4_SITE:
                cur['ld'].append(mag3(float(r['vx']), float(r['vy']), float(r['vz'])))
    if cur is not None:
        frames.append(cur)

    hdr('G2  pairing, by count')
    print('  frames delimited by %s: %d' % (FRAME_DELIM, len(frames)))
    print('  le4 (%s) per frame: %s' % (LE4_SITE,
          sorted(collections.Counter(len(f['le']) for f in frames).items())))
    print('  ld4 (%s) per frame: %s' % (LD4_SITE,
          sorted(collections.Counter(len(f['ld']) for f in frames).items())))
    dis = sum(1 for f in frames if len(f['le']) != len(f['ld']))
    print('  frames where the two counts DISAGREE: %d of %d' % (dis, len(frames)))
    print('  frames with zero ld4 rows: %d'
          % sum(1 for f in frames if not f['ld']))
    print('  G2 reported (no bar; the distribution is the result)')

    for f in frames:
        n = min(len(f['le']), len(f['ld']))
        f['n'] = n
        f['l60'] = sum(min(f['le'][i], LE4_CAP) * f['ld'][i] for i in range(n))
        f['m18c_ok'] = abs(f['m18c'] - 1.0) < 1e-6
    return frames


# ----------------------------------------------------------------- G4 + s

def orig_s(path):
    rows = []
    for r in csv.DictReader(open(path, newline='')):
        try:
            rows.append(dict(seq=int(r['seq']), site=int(r['site']),
                             vx=float(r['velx']), vy=float(r['vely']),
                             vz=float(r['velz']), sp=float(r['speed']),
                             gnd=float(r['gnd']), fx=float(r['fwdx']),
                             fz=float(r['fwdz'])))
        except (ValueError, KeyError):
            continue
    rows.sort(key=lambda x: x['seq'])

    # first site-1 row after each site-2 row, before the next site-2 -- the
    # same first-after-A6a pairing a12_clamp.py uses, with its coverage count.
    follow, nmiss = [], 0
    for i, r in enumerate(rows):
        if r['site'] != 2:
            continue
        for j in range(i + 1, min(i + 8, len(rows))):
            if rows[j]['site'] == 2:
                break
            if rows[j]['site'] == 1:
                follow.append(rows[j])
                break
        else:
            nmiss += 1

    hdr('G4  the horizontal projection, and the first-after-A6a coverage  (%s)'
        % path)
    print('  probe rows %d   site-2 rows with a following site-1: %d   '
          'without: %d' % (len(rows), len(follow), nmiss))
    use = [r for r in follow if abs(r['gnd'] - 4.0) < 1e-6
           and mag3(r['vx'], r['vy'], r['vz']) > 1e-6]
    unit = [r['fx'] ** 2 + r['fz'] ** 2 for r in use]
    ok = sum(1 for u in unit if 0.99 <= u <= 1.01)
    frac = ok / max(1, len(unit))
    b100 = [r for r in use if band_of(mag3(r['vx'], r['vy'], r['vz'])) == (100, 150)]
    print('  grounded nonzero rows %d   fwdx^2+fwdz^2 in [0.99,1.01] on %d '
          '(%.4f)   n in band 100-150: %d' % (len(use), ok, frac, len(b100)))
    passed = frac >= 0.99 and len(b100) >= 100
    print('  G4 %s' % ('PASS' if passed else 'FAIL'))
    if not passed:
        print('  !! s_o does NOT clear the gate, so the registered decision '
              'rule D1/D2/D3 does NOT execute.')
        print('     The s table is still PRINTED below, labelled as an '
              'OBSERVATION. It is not a')
        print('     decision input and nothing in this section is branched on '
              'it. The gate is not amended.')
    for r in use:
        r['s'] = lateral_frac(r['vx'], r['vy'], r['vz'], r['fx'], r['fz'])
        r['m'] = mag3(r['vx'], r['vy'], r['vz'])
    return [r for r in use if r['s'] is not None], passed


# ----------------------------------------------------------------- G3 + port

PAT = re.compile(
    r'snap\.vel=([^ ]+) .*?snap\.bf=([^ ]+) .*?snap\.gc=([-\d.e+]+)'
    r'.*?act\.l60=([-\d.e+]+) act\.m18c=([-\d.e+]+) act\.speed=([-\d.e+]+) '
    r'act\.grip=([-\d.e+]+) act\.kvel=([-\d.e+]+) act\.kav=([-\d.e+]+) '
    r'act\.arm=(-?\d+) act\.clamp=(\d+)')


def port_rows(path):
    out = []
    for ln in open(path, 'r', errors='replace'):
        m = PAT.search(ln)
        if not m:
            continue
        v = [float(x) for x in m.group(1).split(',')]
        f = [float(x) for x in m.group(2).split(',')]
        out.append(dict(v=v, f=f, gc=float(m.group(3)), l60=float(m.group(4)),
                        m18c=float(m.group(5)), speed=float(m.group(6)),
                        grip=float(m.group(7)), kvel=float(m.group(8)),
                        kav=float(m.group(9)), arm=int(m.group(10)),
                        clamp=int(m.group(11))))
    hdr('G3  the PORT channel is direct, known-answer  (%s)' % path)
    ran = [r for r in out if r['clamp'] == 1]
    good = 0
    for r in ran:
        pred = r['l60'] * r['speed']
        if abs(pred - r['grip']) <= 1e-5 * max(abs(r['grip']), 1e-6):
            good += 1
    frac = good / max(1, len(ran))
    print('  rows %d   clamp ran on %d   act.l60*act.speed == act.grip on %d '
          '(%.4f)' % (len(out), len(ran), good, frac))
    print('  G3 %s' % ('PASS' if frac >= 0.99 else 'FAIL'))
    if frac < 0.99:
        sys.exit('G3 FAILED - the port comparison is void, stopping as registered')
    for r in out:
        r['m'] = mag3(*r['v'])
        d = r['v'][0] * r['f'][0] + r['v'][1] * r['f'][1] + r['v'][2] * r['f'][2]
        lat = [r['v'][i] - d * r['f'][i] for i in range(3)]
        r['s'] = (mag3(*lat) / r['m']) if r['m'] > 1e-9 else None
    return out


# ----------------------------------------------------------------- report

def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    frames = orig_l60(a['mag'])
    orows, g4 = orig_s(a['fixup']) if 'fixup' in a else (None, False)
    prows = port_rows(a['dump']) if 'dump' in a else None

    hdr('ORIGINAL  l_60 and grip*speed, per band')
    t = ('band          n | med speed | med ld4  med le4(cap) |     med l_60 '
         '|  med l_60*spd | wheels/frame')
    print(t)
    print('-' * len(t))
    gb = [f for f in frames if abs(f['gnd'] - 4.0) < 1e-6 and f['n'] > 0]
    for lo, hi in BANDS:
        b = [f for f in gb if lo <= f['sp'] < hi]
        if len(b) < 10:
            continue
        ld = med([x for f in b for x in f['ld']])
        le = med([min(x, LE4_CAP) for f in b for x in f['le']])
        print('%-13s %4d | %9.1f | %8.5f %12.3f | %12.3f | %13.1f | %4.1f'
              % ('%d-%d' % (lo, hi), len(b), med([f['sp'] for f in b]), ld, le,
                 med([f['l60'] for f in b]),
                 med([f['l60'] * f['sp'] for f in b]),
                 med([float(f['n']) for f in b])))
    bad18c = sum(1 for f in gb if not f['m18c_ok'])
    print('  frames where +0x18c != 1.0: %d of %d' % (bad18c, len(gb)))

    if orows is not None:
        hdr('ORIGINAL  s = |lateral| / |v|, first sample after A6a returns'
            + ('' if g4 else '   [OBSERVATION ONLY - G4 FAILED, not a '
                             'decision input]'))
        t = 'band          n | med speed |     med s |     p05 s |     p95 s'
        print(t)
        print('-' * len(t))
        for lo, hi in BANDS:
            b = [r for r in orows if lo <= r['m'] < hi]
            if len(b) < 10:
                continue
            ss = sorted(r['s'] for r in b)
            print('%-13s %4d | %9.1f | %9.6f | %9.6f | %9.6f'
                  % ('%d-%d' % (lo, hi), len(b), med([r['m'] for r in b]),
                     med(ss), ss[int(0.05 * len(ss))], ss[int(0.95 * len(ss))]))

    if prows is not None:
        hdr('PORT  act.l60 / act.grip / act.kvel / arm histogram, per band')
        t = ('band          n | med speed |   med l60 |  med grip |  med kVel '
             '| arm hi/lo/none | clampRan |     med s')
        print(t)
        print('-' * len(t))
        gp = [r for r in prows if abs(r['gc'] - 4.0) < 1e-6 and r['m'] > 1e-6]
        for lo, hi in BANDS:
            b = [r for r in gp if lo <= r['m'] < hi]
            if len(b) < 10:
                continue
            print('%-13s %4d | %9.1f | %9.3f | %9.1f | %9.6f | %4d/%4d/%4d | '
                  '%8d | %9.6f'
                  % ('%d-%d' % (lo, hi), len(b), med([r['m'] for r in b]),
                     med([r['l60'] for r in b]), med([r['grip'] for r in b]),
                     med([r['kvel'] for r in b]),
                     sum(1 for r in b if r['arm'] == 1),
                     sum(1 for r in b if r['arm'] == 0),
                     sum(1 for r in b if r['arm'] == -1),
                     sum(1 for r in b if r['clamp']),
                     med([r['s'] for r in b if r['s'] is not None])))

    # ---- the registered decision rule
    if orows is None or not g4:
        hdr('THE REGISTERED DECISION RULE  (PREREG_STEP1.md section 7)')
        print('  D1 / D2 / D3 are NOT EVALUATED: G4 FAILED, so `s_o` is not a '
              'measured quantity')
        print('  under the registered bar. The gate is not amended and no '
              'branch is taken.')
        return
    b = [r for r in orows if 100 <= r['m'] < 150]
    s_o = med([r['s'] for r in b])
    hdr('THE REGISTERED DECISION RULE  (PREREG_STEP1.md section 7)')
    print('  s_o = median s on the ORIGINAL in band 100-150 = %.6f  '
          '(n=%d, med speed %.1f)' % (s_o, len(b), med([r['m'] for r in b])))
    print('  1.25 * s_o = %.6f   D1 bar <= 0.0112   D2 bar s_o >= 0.03'
          % (1.25 * s_o))
    if 1.25 * s_o <= 0.0112:
        print('  >>> D1 FIRES: the no-op is the LATERAL, not k. '
              'Section 25.3 inference UNFOUNDED.')
    elif s_o >= 0.03:
        kmax = 2.0e-5 / (s_o * s_o)
        G = 1e7 - kmax / 0.2 * 1e7
        print('  >>> D2 FIRES: k <= %.3e, below the LOW arm floor 0.1, so the '
              'HIGH arm is forced' % kmax)
        print('      and G = grip*|v| >= %.6g, i.e. grip >= %.1f at speed '
              '%.1f' % (G, G / med([r['m'] for r in b]), med([r['m'] for r in b])))
    else:
        print('  >>> D3: between the bars. Report only, author nothing.')


if __name__ == '__main__':
    main(sys.argv[1:])
