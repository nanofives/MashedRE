#!/usr/bin/env python3
"""a10_gain.py - the BETWEEN-CONTACT per-frame speed budget, both sides, from the
render-tick record snapshot only.  Registered in
verify/d2_gain_20261001/PREREG.md (D2 attempt 10).

Terms, per frame f (free-flight frames only; A6a 0x00467650 is the only writer of
+0x9b0 on a non-contact frame, section 22.2/22.4):

    s_mid(f)   = +0x9e4(f)            speed after W1 (Integrate2.cpp:630-632,
                                      original 0x004686cc), before grip-clamp #6
    s_post(f)  = |+0x9b0..b8|(f)      speed after the clamp (0x004687f0..0x0046897b)

    T_W1(f)    = s_mid(f)  - s_post(f-1)
    T_clamp(f) = s_post(f) - s_mid(f)
    T_drive(f) = linTerm * (ctrl(f) . u(f)),  u = v_post(f-1)/s_post(f-1)
    T_rest(f)  = T_W1(f) - T_drive(f)

    T_drive + T_rest + T_clamp == s_post(f) - s_post(f-1)     (identity)

ctrl = (+0xb14, +0xb18, +0xb1c); linTerm = dt * Rf(+0x54) * kDt, kDt = 1/3000
(_DAT_005cc948 = 0x39aec33e).

Usage:
  py -3.12 re/tools/statediff/a10_gain.py --orig <msd> [--port <player_trace.log>]
           [--lin <linTerm>] [--band LO,HI] [--gaps N] [--csv <out.csv>]
"""
import struct, math, sys, csv, re, statistics

KDT = struct.unpack('<f', struct.pack('<I', 0x39aec33e))[0]      # 1/3000 exact
M54 = 0.0010000000474974513                                      # +0x54, measured constant
DT_FRAME = 50.000004                                             # A6a frame dt (frameMs)
LIN_DEFAULT = DT_FRAME * M54 * KDT

CONTACT_RATIO = 1.02        # registered detector threshold
BAND = (150.0, 260.0)       # registered matched-speed band


# ---------------------------------------------------------------- loaders

def load_msd(path):
    """ORIGINAL: MSD1 per-frame vehicle-record snapshot (FORMAT.md)."""
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1', 'not MSD1'
    rec, = struct.unpack_from('<I', b, 4)
    off, rows = 16, []
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        p = b[off + 4: off + 4 + rec]
        vx, vy, vz = struct.unpack_from('<3f', p, 0x9b0)
        rows.append(dict(
            f=fi, vel=(vx, vy, vz),
            s_post=math.sqrt(vx * vx + vy * vy + vz * vz),
            s_mid=struct.unpack_from('<f', p, 0x9e4)[0],
            ctrl=(struct.unpack_from('<f', p, 0xb14)[0],
                  struct.unpack_from('<f', p, 0xb18)[0],
                  struct.unpack_from('<f', p, 0xb1c)[0]),
            fwd=struct.unpack_from('<3f', p, 0x9d4),
            gnd=struct.unpack_from('<f', p, 0x9e0)[0],
            m54=struct.unpack_from('<f', p, 0x54)[0],
        ))
        off += 4 + rec
    return rows


_PT = re.compile(r'(\w+)=(\(?[-\d.eE+,]+\)?)')


def load_pt(path):
    """PORT: player_trace.log (MASHED_PLAYERTRACE). +0xb18 is not in the line; it is
    exactly 0.0 on 2332/2332 original frames and S2 checks the port's own ctrl.y
    against friction_diag."""
    rows = []
    for ln in open(path):
        if not ln.startswith('f='):
            continue
        d = dict(_PT.findall(ln))

        def g(k):
            return float(d[k])

        def v3(k):
            return tuple(float(x) for x in d[k].strip('()').split(','))

        vx, vy, vz = v3('vel')
        rows.append(dict(
            f=int(d['f']), vel=(vx, vy, vz),
            s_post=math.sqrt(vx * vx + vy * vy + vz * vz),
            s_mid=g('v9e4'),
            ctrl=(g('b14'), 0.0, g('b1c')),
            fwd=v3('bodyfwd'), gnd=g('gnd'), m54=M54,
        ))
    return rows


# ---------------------------------------------------------------- budget

def contacts(rows):
    """Registered detector: s_post > 1e-3 and s_mid/s_post > 1.02."""
    return [r['f'] for r in rows
            if r['s_post'] > 1e-3 and r['s_mid'] / r['s_post'] > CONTACT_RATIO]


def steps(rows, lin):
    by = {r['f']: r for r in rows}
    out = []
    for r in rows:
        pr = by.get(r['f'] - 1)
        if pr is None or pr['s_post'] <= 1e-6:
            continue
        s0 = pr['s_post']
        u = tuple(c / s0 for c in pr['vel'])
        t_w1 = r['s_mid'] - s0
        t_cl = r['s_post'] - r['s_mid']
        t_dr = lin * sum(a * b for a, b in zip(r['ctrl'], u))
        out.append(dict(f=r['f'], s_from=s0, s_to=r['s_post'], s_mid=r['s_mid'],
                        dS=r['s_post'] - s0, T_W1=t_w1, T_clamp=t_cl,
                        T_drive=t_dr, T_rest=t_w1 - t_dr, gnd=r['gnd'],
                        ctrl_mag=math.sqrt(sum(c * c for c in r['ctrl']))))
    return out


def gaps_of(cf, st, ngaps):
    """Gap k = steps f in [c_k + 1, c_{k+1} - 1]."""
    byf = {s['f']: s for s in st}
    out = []
    for k in range(min(ngaps, len(cf) - 1)):
        a, b = cf[k], cf[k + 1]
        out.append([byf[f] for f in range(a + 1, b) if f in byf])
    return out


def med(xs):
    return statistics.median(xs) if xs else float('nan')


def report(tag, rows, lin, ngaps, band, csv_out=None):
    cf = contacts(rows)
    st = steps(rows, lin)
    gs = gaps_of(cf, st, ngaps)
    print('=== %s ===  frames %d (%d..%d)  linTerm %.9g' %
          (tag, len(rows), rows[0]['f'], rows[-1]['f'], lin))
    print('  contact frames (%d): %s' % (len(cf), cf[:14]))
    print('  +0x54 distinct: %s' % sorted({r['m54'] for r in rows}))

    print('  gap  frames        n  s_from->s_to      dS    T_drive     T_rest   T_clamp'
          '   (medians per step)')
    for k, g in enumerate(gs):
        if not g:
            print('  %3d  (empty)' % k)
            continue
        print('  %3d  %4d..%-4d %4d  %8.2f->%8.2f %+8.3f  %+9.4f  %+9.4f  %+8.4f'
              % (k, g[0]['f'], g[-1]['f'], len(g), g[0]['s_from'], g[-1]['s_to'],
                 med([s['dS'] for s in g]), med([s['T_drive'] for s in g]),
                 med([s['T_rest'] for s in g]), med([s['T_clamp'] for s in g])))

    allsteps = [s for g in gs for s in g]
    bnd = [s for s in allsteps if band[0] <= s['s_from'] <= band[1]]
    for nm, ss in (('ALL gap steps', allsteps), ('BAND %g-%g' % band, bnd),
                   ('GAP 0', gs[0] if gs else [])):
        if not ss:
            print('  %-16s n=0' % nm)
            continue
        print('  %-16s n=%3d  med s_from %8.2f  dS %+8.4f  T_drive %+9.5f  '
              'T_rest %+9.5f  T_clamp %+8.5f'
              % (nm, len(ss), med([s['s_from'] for s in ss]), med([s['dS'] for s in ss]),
                 med([s['T_drive'] for s in ss]), med([s['T_rest'] for s in ss]),
                 med([s['T_clamp'] for s in ss])))

    # ---- safety
    if allsteps:
        n = len(allsteps)
        s1 = sum(1 for s in allsteps if s['T_clamp'] <= 1e-3 * s['s_from'])
        s4 = sum(1 for s in allsteps if s['gnd'] == 4.0)
        ident = max(abs(s['T_drive'] + s['T_rest'] + s['T_clamp'] - s['dS'])
                    for s in allsteps)
        ctrl0 = sum(1 for s in allsteps if s['ctrl_mag'] <= 0.0)
        print('  S1 T_clamp<=+1e-3*s_from : %d/%d = %.1f%%' % (s1, n, 100.0 * s1 / n))
        print('  S4 grounded==4.0         : %d/%d = %.1f%%' % (s4, n, 100.0 * s4 / n))
        print('  identity max|resid|      : %.3e' % ident)
        print('  steps with |ctrl|==0     : %d' % ctrl0)
    # S3 band sensitivity on this side
    hi = [s for s in st if 1500.0 <= s['s_from'] <= 2000.0]
    lo = [s for s in st if band[0] <= s['s_from'] <= band[1]]
    if hi and lo:
        print('  S3 median T_drive  %g-%g: %+.5f (n=%d)   1500-2000: %+.5f (n=%d)  '
              'ratio %.3f'
              % (band[0], band[1], med([s['T_drive'] for s in lo]), len(lo),
                 med([s['T_drive'] for s in hi]), len(hi),
                 med([s['T_drive'] for s in hi]) / med([s['T_drive'] for s in lo])
                 if med([s['T_drive'] for s in lo]) else float('nan')))
    if csv_out:
        with open(csv_out, 'w', newline='') as fh:
            w = csv.DictWriter(fh, fieldnames=list(allsteps[0].keys()) + ['gap'])
            w.writeheader()
            for k, g in enumerate(gs):
                for s in g:
                    d = dict(s); d['gap'] = k; w.writerow(d)
        print('  wrote %s' % csv_out)
    return dict(contacts=cf, gaps=gs, steps=st)


def main(argv):
    orig = port = csv_out = None
    lin, ngaps, band = LIN_DEFAULT, 9, BAND
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == '--orig': orig = argv[i + 1]; i += 2; continue
        if a == '--port': port = argv[i + 1]; i += 2; continue
        if a == '--lin': lin = float(argv[i + 1]); i += 2; continue
        if a == '--gaps': ngaps = int(argv[i + 1]); i += 2; continue
        if a == '--csv': csv_out = argv[i + 1]; i += 2; continue
        if a == '--band':
            band = tuple(float(x) for x in argv[i + 1].split(',')); i += 2; continue
        raise SystemExit('unknown arg %s' % a)
    print('kDt = %.17g   linTerm default = %.17g = %g * %.17g * %.17g'
          % (KDT, LIN_DEFAULT, DT_FRAME, M54, KDT))
    res = {}
    if orig:
        res['orig'] = report('ORIGINAL ' + orig, load_msd(orig), lin, ngaps, band,
                             csv_out and csv_out.replace('.csv', '_orig.csv'))
    if port:
        res['port'] = report('PORT ' + port, load_pt(port), lin, ngaps, band,
                             csv_out and csv_out.replace('.csv', '_port.csv'))
    if 'orig' in res and 'port' in res:
        print('\n=== CROSS-SIDE, registered decision rule ===')
        for nm, sel in (('GAP 0', lambda r: r['gaps'][0] if r['gaps'] else []),
                        ('BAND %g-%g' % band,
                         lambda r: [s for g in r['gaps'] for s in g
                                    if band[0] <= s['s_from'] <= band[1]]),
                        ('ALL gap steps',
                         lambda r: [s for g in r['gaps'] for s in g])):
            o, p = sel(res['orig']), sel(res['port'])
            if not o or not p:
                print('%-14s  ORIG n=%d  PORT n=%d  -- not comparable' % (nm, len(o), len(p)))
                continue
            dso, dsp = med([s['dS'] for s in o]), med([s['dS'] for s in p])
            w1o = med([s['T_W1'] for s in o])
            resid = abs(dso - dsp)
            print('%s   ORIG n=%d med s_from %.2f | PORT n=%d med s_from %.2f'
                  % (nm, len(o), med([s['s_from'] for s in o]),
                     len(p), med([s['s_from'] for s in p])))
            print('   term        ORIG       PORT       delta   (a) >= %.4f   (b) >= %.4f'
                  % (0.20 * abs(w1o), 0.30 * resid))
            rank = []
            for t in ('T_drive', 'T_rest', 'T_clamp'):
                mo, mp = med([s[t] for s in o]), med([s[t] for s in p])
                d = abs(mp - mo)
                a_ok = d >= 0.20 * abs(w1o)
                b_ok = d >= 0.30 * resid
                rank.append((d, t, mo, mp, a_ok, b_ok))
                print('   %-9s %+10.5f %+10.5f %10.5f   %-5s        %-5s'
                      % (t, mo, mp, d, 'PASS' if a_ok else 'no', 'PASS' if b_ok else 'no'))
            print('   dS        %+10.5f %+10.5f %10.5f   (residual being explained)'
                  % (dso, dsp, resid))
            rank.sort(reverse=True)
            div = [r for r in rank if r[4] and r[5]]
            print('   --> %s' % (('DIVERGING TERM: %s' % div[0][1]) if div
                                 else 'NO term satisfies (a) and (b) in this cut'))


if __name__ == '__main__':
    main(sys.argv[1:])
