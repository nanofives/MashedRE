#!/usr/bin/env python3
"""a11_accum.py - reconstruct A6a's FORCE ACCUMULATOR (`accum` = l_b8 / l_b4 /
lin_b0) and its blend fraction `frac = (l_d0 - m78)/l_d0` from the render-tick
record snapshot alone, so the ORIGINAL's value can be measured where the `.msd`
carries no A6a locals.  D2 attempt 11.  Registered in
verify/d2_accum_20261001/PREREG.md.

WHAT IT REPRODUCES (original RVAs, read from original/MASHED.exe.unpatched with
re/tools/disasm_va.py this session; port lines are Integrate2.cpp):

  cross-product friction block #5, per wheel, 4 wheels
      0x0046833a .. 0x00468544      Integrate2.cpp:489-512
      gate        0x0046833e call 0x4c3ac0 (Vec3Mag3 F) + fcomp [0x5cd03c] (1e-4)
      inv = 1/wm  0x00468357 fld [0x5cc320] (1.0) / 0x0046835d fdiv [edi-0x2c]
      off reads   0x00468360 / 0x00468369 / 0x00468372   [edi-0x24/-0x20/-0x1c]
      F   reads   0x00468388 / 0x00468391 / 0x004683bf / 0x004683ca  [edi+0x70..0x78]
      a*wm        0x0046849e / 0x004684a9 / 0x004684b4   fmul [edi-0x2c]
      kNormAccum  0x004684c0 fmul [0x5cea04] = 50.24 (0x4248f5c3)
      l_d0 accum  0x004684cf fadd [esp+0x28] / 0x004684d3 fstp [esp+0x28]
      m16 gate    0x004684dc fcom [0x5cd03c]
      r           0x004684ec fdivr [esp+0xc]
      l_78/74/70  0x004684f0 .. 0x00468530   ([esp+0x78] / [esp+0x7c] / [esp+0x80])
      stride      0x0046853b add edi,0xc4 ; base self+0x1a4
  m78 + frac + the blend
      0x004685b2 .. 0x00468625      Integrate2.cpp:543-565
      m78         0x004685b2 lea ecx,[esp+0x78] / 0x004685b7 call 0x4c3ac0
      l_d0 - m78  0x004685bc fsubr [esp+0x24]
      == 0 test   0x004685c3 fcom [0x5d757c] (0.0) / 0x004685ce jnp 0x468627
      frac        0x004685d0 fdiv [esp+0x20]
      blend       0x004685d4 .. 0x00468621
  W1, the only use of `accum`
      0x0046862d .. 0x004686a2      Integrate2.cpp:630-632
      linTerm     0x0046865b fmul [esi+0x54] / 0x0046865e fmul [0x5cc948] (1/3000)
      speed       0x004686a4 call 0x4c3ac0 ; +0x9e4 store 0x004686cc

INPUTS - record fields only, at the render-tick snapshot phase.  NO hook, NO
probe, NO source change on either side.  Per wheel w, b = 0x1a4 + w*0xc4:
      wm   b-0x2c          off  b-0x24 / b-0x20 / b-0x1c
      F    b+0x70 / b+0x74 / b+0x78
and +0x9b0..0x9b8 (vel), +0x9e4 (s_mid), +0x9e0 (grounded), +0x54,
+0xb14 / +0xb18 / +0xb1c (ctrl).

Usage:
  a11_accum.py selftest --dump <a6a_dump.log> --fric <friction_diag.log>
  a11_accum.py w1       --dump <a6a_dump.log> [--fric <friction_diag.log>]
  a11_accum.py orig     --msd  <capture.msd>
  a11_accum.py compare  --msd  <capture.msd> --dump <a6a_dump.log>
                        [--fric <friction_diag.log>]
"""
import struct, math, sys, re, statistics

import numpy as np

F32 = np.float32

KDT        = struct.unpack('<f', struct.pack('<I', 0x39aec33e))[0]   # 1/3000  @0x5cc948
KSPEEDMIN  = struct.unpack('<f', struct.pack('<I', 0x38d1b717))[0]   # 1e-4    @0x5cd03c
KNORMACCUM = struct.unpack('<f', struct.pack('<I', 0x4248f5c3))[0]   # 50.24   @0x5cea04
DT_FRAME   = 50.000004        # A6a frame dt (frameMs), as a10_gain.py
GROUNDED   = 0x40800000       # +0x9e0 == float 4.0

BANDS = [(70, 100), (100, 150), (150, 260), (260, 500),
         (500, 1000), (1000, 1500), (1500, 2000), (40, 70)]


def mag3(x, y, z):
    """Vec3Mag3 (call 0x4c3ac0), float32 result."""
    return F32(math.sqrt(float(F32(x)) ** 2 + float(F32(y)) ** 2 + float(F32(z)) ** 2))


# --------------------------------------------------------------- the estimator

def estimate(wheels):
    """Replay block #5 + m78 + frac + the blend from the four per-wheel
    (wm, off[3], F[3]) snapshot tuples.  Returns a dict of every intermediate.

    Arithmetic follows the original's widths: every stack slot in
    0x0046833a..0x00468625 is `fstp dword`, i.e. float32 (U-A6A-FLOAT10,
    resolved).  Emulated with numpy float32; the x87 80-bit transient inside a
    single a*b+c step differs by at most 1 ULP and is not material at the
    reported precision.
    """
    Sc = [F32(0.0)] * 3     # l_b8 / l_b4 / l_b0   normal component sum
    Sf = [F32(0.0)] * 3     # l_6c / l_68 / l_64   total force sum
    T  = [F32(0.0)] * 3     # l_78 / l_74 / l_70   torque accumulator
    D0 = F32(0.0)           # l_d0
    nfired = 0
    for wm, off, Fv in wheels:
        wm = F32(wm)
        f0, f1, f2 = F32(Fv[0]), F32(Fv[1]), F32(Fv[2])
        if not (mag3(f0, f1, f2) > F32(KSPEEDMIN)):        # 0x0046833e..0x00468351
            continue
        nfired += 1
        inv = F32(F32(1.0) / wm)                           # 0x00468357 / 0x0046835d
        d0 = F32(F32(off[0]) * inv)
        d1 = F32(F32(off[1]) * inv)
        d2 = F32(F32(off[2]) * inv)
        dot = F32(F32(F32(d2 * f2) + F32(d1 * f1)) + F32(d0 * f0))
        c8, c4, c0 = F32(d0 * dot), F32(d1 * dot), F32(d2 * dot)
        ac, a8, a4 = F32(f0 - c8), F32(f1 - c4), F32(f2 - c0)
        x84 = F32(F32(a4 * c4) - F32(a8 * c0))
        x80 = F32(F32(ac * c0) - F32(a4 * c8))
        x7c = F32(F32(a8 * c8) - F32(ac * c4))
        if F32(0.0) < dot:                                 # 0x004683.. sign flip
            x84, x80, x7c = F32(-x84), F32(-x80), F32(-x7c)
        Sc[0] = F32(Sc[0] + c8); Sc[1] = F32(Sc[1] + c4); Sc[2] = F32(Sc[2] + c0)
        Sf[0] = F32(Sf[0] + f0); Sf[1] = F32(Sf[1] + f1); Sf[2] = F32(Sf[2] + f2)
        m15 = mag3(F32(ac * wm), F32(a8 * wm), F32(a4 * wm))   # 0x0046849e..0x004684bb
        m15k = F32(m15 * F32(KNORMACCUM))                      # 0x004684c0
        D0 = F32(D0 + m15k)                                    # 0x004684cf / d3
        m16 = mag3(x84, x80, x7c)                              # 0x004684d7
        if m16 > F32(KSPEEDMIN):                               # 0x004684dc / ea
            r = F32(m15k / m16)                                # 0x004684ec
            T[0] = F32(T[0] - F32(x84 * r))                    # 0x004684f0..0x00468530
            T[1] = F32(T[1] - F32(x80 * r))
            T[2] = F32(T[2] - F32(x7c * r))
    m78 = mag3(T[0], T[1], T[2])                               # 0x004685b2 / b7
    dif = F32(D0 - m78)                                        # 0x004685bc
    if float(dif) == 0.0:                                      # 0x004685c3 / ce
        frac = F32(0.0)
        acc = [Sc[0], Sc[1], Sc[2]]
    else:
        frac = F32(dif / D0)                                   # 0x004685d0
        e = [F32(Sf[i] - Sc[i]) for i in range(3)]             # 0x004685d4..0x004685fd
        acc = [F32(F32(e[i] * frac) + Sc[i]) for i in range(3)]  # 0x00468601..0x00468621
    return dict(Sc=Sc, Sf=Sf, T=T, ld0=D0, m78=m78, frac=frac, accum=acc,
                cMag=mag3(*Sc), fMag=mag3(*Sf), nfired=nfired)


# ------------------------------------------------------------------- loaders

def load_msd(path):
    """MSD1 capture -> [(frame_idx, payload)] (re/tools/statediff/FORMAT.md)."""
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1', 'not MSD1'
    rec, _base, _r = struct.unpack_from('<III', b, 4)
    off, out = 16, []
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        out.append((fi, b[off + 4: off + 4 + rec]))
        off += 4 + rec
    return out


def rf(p, o):
    return struct.unpack_from('<f', p, o)[0]


def ri(p, o):
    return struct.unpack_from('<i', p, o)[0]


def msd_rows(path):
    """One row per captured frame, the record fields the estimator needs."""
    rows = []
    for fi, p in load_msd(path):
        wheels = []
        for w in range(4):
            b = 0x1a4 + w * 0xc4
            wheels.append((rf(p, b - 0x2c),
                           (rf(p, b - 0x24), rf(p, b - 0x20), rf(p, b - 0x1c)),
                           (rf(p, b + 0x70), rf(p, b + 0x74), rf(p, b + 0x78))))
        rows.append(dict(f=fi,
                         vel=(rf(p, 0x9b0), rf(p, 0x9b4), rf(p, 0x9b8)),
                         s_mid=rf(p, 0x9e4),
                         gnd=ri(p, 0x9e0),
                         ctrl=(rf(p, 0xb14), rf(p, 0xb18), rf(p, 0xb1c)),
                         m54=rf(p, 0x54),
                         wheels=wheels))
    return rows


_W = re.compile(
    r'\|w(\d) fired=(\d+).*?'
    r's\.off=([^ ]+) s\.ax=[^ ]+ s\.p15=[^ ]+ s\.p16=[^ ]+ s\.p1b=[^ ]+ '
    r's\.pm1=[^ ]+ s\.pm3=[^ ]+ s\.pmb=([^ ]+) s\.F=([^ ]+)')
_SNAP = re.compile(
    r'f=(\d+) snap\.vel=([^ ]+) .*?snap\.sp=([^ ]+) snap\.angsp=[^ ]+ snap\.gc=([^ ]+)')


def _v3(s):
    return tuple(float(x) for x in s.split(','))


def dump_rows(path):
    """a6a_dump.log -> one row per frame, built from the `s.` (snapshot) fields
    ONLY, i.e. exactly the channel the original's .msd gives."""
    rows = []
    for ln in open(path, 'r', errors='replace'):
        m = _SNAP.search(ln)
        if not m:
            continue
        ws = _W.findall(ln)
        if len(ws) != 4:
            continue
        wheels = [(float(pmb), _v3(off), _v3(Fv)) for _w, _fi, off, pmb, Fv in ws]
        rows.append(dict(f=int(m.group(1)), vel=_v3(m.group(2)),
                         s_mid=float(m.group(3)),
                         gnd=GROUNDED if abs(float(m.group(4)) - 4.0) < 1e-6 else 0,
                         wheels=wheels))
    return rows


_FD = re.compile(
    r'spd=([-\d.e+]+) ctrl=\(([^)]*)\) accum=\(([^)]*)\) cMag=([-\d.e+]+) '
    r'fMag=([-\d.e+]+) ld0=([-\d.e+]+) m78=([-\d.e+]+) frac=([-\d.e+]+) '
    r'grounded=0x([0-9A-Fa-f]+) linTerm=([-\d.e+]+)')


def fric_rows(path):
    out = []
    for ln in open(path, 'r', errors='replace'):
        m = _FD.search(ln)
        if not m:
            continue
        out.append(dict(spd=float(m.group(1)), ctrl=_v3(m.group(2)),
                        accum=_v3(m.group(3)), cMag=float(m.group(4)),
                        fMag=float(m.group(5)), ld0=float(m.group(6)),
                        m78=float(m.group(7)), frac=float(m.group(8)),
                        gnd=int(m.group(9), 16), linTerm=float(m.group(10))))
    return out


# ------------------------------------------------------------------- helpers

def relerr(a, b):
    d = max(abs(a), abs(b))
    return 0.0 if d == 0.0 else abs(a - b) / d


def q(xs, p):
    if not xs:
        return float('nan')
    s = sorted(xs)
    i = min(len(s) - 1, max(0, int(round(p * (len(s) - 1)))))
    return s[i]


def med(xs):
    return statistics.median(xs) if xs else float('nan')


def dot3(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


# ------------------------------------------------------------------ selftest

def cmd_selftest(a):
    dr, fr = dump_rows(a['dump']), fric_rows(a['fric'])
    print('a6a_dump frames %d   friction_diag lines %d' % (len(dr), len(fr)))
    ests = [estimate(r['wheels']) for r in dr]

    # KA1-a: join uniqueness, on fMag
    print('\n[KA1-a] join scan, median rel-err of fMag by offset '
          '(friction_diag[i+off] vs estimator[i])')
    best = None
    for off in range(-4, 5):
        es = []
        for i, e in enumerate(ests):
            j = i + off
            if 0 <= j < len(fr) and fr[j]['fMag'] > 1.0:
                es.append(relerr(float(e['fMag']), fr[j]['fMag']))
        m = med(es)
        print('  off %+d  n=%5d  median rel-err %.3e' % (off, len(es), m))
        if off == 0:
            best = m
    print('  offset 0 median rel-err = %.3e' % best)

    # KA1-b..d at offset 0, grounded + non-degenerate frames
    pairs = [(e, fr[i]) for i, e in enumerate(ests)
             if i < len(fr) and fr[i]['gnd'] == GROUNDED and fr[i]['fMag'] > 1.0]
    print('\n[KA1-b..d] offset 0, grounded, fMag>1   n=%d' % len(pairs))
    for key, getter in (('cMag', lambda e, t: (float(e['cMag']), t['cMag'])),
                        ('fMag', lambda e, t: (float(e['fMag']), t['fMag'])),
                        ('ld0',  lambda e, t: (float(e['ld0']),  t['ld0'])),
                        ('m78',  lambda e, t: (float(e['m78']),  t['m78']))):
        es = [relerr(*getter(e, t)) for e, t in pairs]
        print('  %-5s median rel-err %.3e   p95 %.3e   max %.3e'
              % (key, med(es), q(es, 0.95), max(es)))
    fe = [abs(float(e['frac']) - t['frac']) for e, t in pairs]
    print('  frac  median abs-err %.3e   p95 %.3e   max %.3e'
          % (med(fe), q(fe, 0.95), max(fe)))
    ae, ce = [], []
    for e, t in pairs:
        p_ = [float(x) for x in e['accum']]
        mp, mt = math.sqrt(sum(x * x for x in p_)), math.sqrt(sum(x * x for x in t['accum']))
        ae.append(relerr(mp, mt))
        ce.append(dot3(p_, t['accum']) / (mp * mt) if mp * mt > 0 else 1.0)
    print('  |accum| median rel-err %.3e   p95 %.3e   max %.3e'
          % (med(ae), q(ae, 0.95), max(ae)))
    print('  accum  median cosine  %.9f   min %.9f' % (med(ce), min(ce)))
    return ests, fr


# ----------------------------------------------------------------------- KA2

def w1_check(rows, accs, linterms, label):
    """pred s_mid(f) = |v_post(f-1) + linTerm*(ctrl(f) + accum(f))| against the
    recorded +0x9e4(f).  Detector-free: reported as the MEDIAN, immune to the
    <=13% fixup contamination (section 23.2's bound)."""
    out = []
    for i in range(1, len(rows)):
        pr, cu = rows[i - 1], rows[i]
        s_prev = math.sqrt(sum(x * x for x in pr['vel']))
        if s_prev <= 1e-3 or cu['gnd'] != GROUNDED:
            continue
        lt, ac, ct = linterms[i], accs[i], cu['ctrl']
        v = [pr['vel'][k] + lt * (ct[k] + ac[k]) for k in range(3)]
        pred = math.sqrt(sum(x * x for x in v))
        out.append(dict(f=cu['f'], s_prev=s_prev, s_mid=cu['s_mid'], pred=pred,
                        err=pred - cu['s_mid'],
                        rel=abs(pred - cu['s_mid']) / s_prev))
    rs = [r['rel'] for r in out]
    print('[KA2-%s] n=%d   median |err|/s_prev %.3e   p75 %.3e   p95 %.3e'
          % (label, len(out), med(rs), q(rs, 0.75), q(rs, 0.95)))
    return out


def cmd_w1(a):
    dr = dump_rows(a['dump'])
    ests = [estimate(r['wheels']) for r in dr]
    # the port's ctrl comes from friction_diag (same A6a call, same line index)
    fr = fric_rows(a['fric'])
    n = min(len(dr), len(fr))
    for i in range(n):
        dr[i]['ctrl'] = fr[i]['ctrl']
    dr = dr[:n]
    lts = [fr[i]['linTerm'] for i in range(n)]
    print('PORT, linTerm from friction_diag (median %.6g)' % med(lts))
    w1_check(dr, [[float(x) for x in ests[i]['accum']] for i in range(n)], lts,
             'est/port')
    w1_check(dr, [list(fr[i]['accum']) for i in range(n)], lts, 'control/port')


# -------------------------------------------------------------------- original

def orig_rows_est(path):
    rows = msd_rows(path)
    ests = [estimate(r['wheels']) for r in rows]
    lts = [DT_FRAME * r['m54'] * KDT for r in rows]
    return rows, ests, lts


def cmd_orig(a):
    rows, ests, lts = orig_rows_est(a['msd'])
    print('ORIGINAL %s   frames %d   median +0x54 %.17g   linTerm %.6g'
          % (a['msd'], len(rows), med([r['m54'] for r in rows]), med(lts)))
    w1_check(rows, [[float(x) for x in e['accum']] for e in ests], lts, 'orig')


# --------------------------------------------------------------------- compare

def _terms(rows, ests, lts):
    """Per-step first-order decomposition of T_rest against the previous
    frame's unit velocity:
        T_rest ~= linTerm*(Sc.u) + frac * linTerm*((Sf-Sc).u) = A + frac*C
    plus the measured T_W1 / T_drive / T_rest from section 23's identity."""
    out = []
    for i in range(1, len(rows)):
        pr, cu = rows[i - 1], rows[i]
        s_prev = math.sqrt(sum(x * x for x in pr['vel']))
        if s_prev <= 1e-3 or cu['gnd'] != GROUNDED:
            continue
        u = [x / s_prev for x in pr['vel']]
        e, lt = ests[i], lts[i]
        Sc = [float(x) for x in e['Sc']]
        Sf = [float(x) for x in e['Sf']]
        tg = [Sf[k] - Sc[k] for k in range(3)]
        frac = float(e['frac'])
        A = lt * dot3(Sc, u)
        C = lt * dot3(tg, u)
        s_mid = cu['s_mid']
        T_W1 = s_mid - s_prev
        T_drive = lt * dot3(cu['ctrl'], u)
        out.append(dict(f=cu['f'], s_from=s_prev, s_mid=s_mid,
                        s_post=math.sqrt(sum(x * x for x in cu['vel'])),
                        A=A, C=C, frac=frac, fracC=frac * C, pred=A + frac * C,
                        T_W1=T_W1, T_drive=T_drive, T_rest=T_W1 - T_drive,
                        ld0=float(e['ld0']), m78=float(e['m78']),
                        cMag=float(e['cMag']), fMag=float(e['fMag']),
                        tanMag=math.sqrt(sum(x * x for x in tg)),
                        nfired=e['nfired']))
    return out


def _band(ts, lo, hi):
    return [t for t in ts if lo <= t['s_from'] < hi]


def cmd_compare(a):
    orows, oests, olts = orig_rows_est(a['msd'])
    ots = _terms(orows, oests, olts)
    drows = dump_rows(a['dump'])
    dests = [estimate(r['wheels']) for r in drows]
    fr = fric_rows(a['fric'])
    n = min(len(drows), len(fr))
    for i in range(n):
        drows[i]['ctrl'] = fr[i]['ctrl']
    drows, dests = drows[:n], dests[:n]
    plts = [fr[i]['linTerm'] for i in range(n)]
    pts = _terms(drows, dests, plts)
    print('ORIGINAL steps %d   PORT steps %d' % (len(ots), len(pts)))

    hdr = ('band            side   n |      A        frac        C      frac*C'
           '      pred     T_rest   |  ld0        m78      m78/ld0   cMag     '
           'tanMag   med_s')
    print('\n' + hdr)
    print('-' * len(hdr))
    for lo, hi in BANDS:
        for name, ts in (('ORIG', ots), ('PORT', pts)):
            b = _band(ts, lo, hi)
            if not b:
                print('%-14s %-5s %3d |  (no samples)' % ('%d-%d' % (lo, hi), name, 0))
                continue
            print('%-14s %-5s %3d | %9.4f %8.5f %9.3f %9.4f %9.4f %9.4f  |'
                  ' %10.4g %10.4g %8.5f %9.4g %9.4g %7.1f'
                  % ('%d-%d' % (lo, hi), name, len(b),
                     med([t['A'] for t in b]), med([t['frac'] for t in b]),
                     med([t['C'] for t in b]), med([t['fracC'] for t in b]),
                     med([t['pred'] for t in b]), med([t['T_rest'] for t in b]),
                     med([t['ld0'] for t in b]), med([t['m78'] for t in b]),
                     med([t['m78'] / t['ld0'] if t['ld0'] else 0 for t in b]),
                     med([t['cMag'] for t in b]), med([t['tanMag'] for t in b]),
                     med([t['s_from'] for t in b])))
    # S5: does the estimator's first-order prediction reproduce the
    # independently-measured T_rest?
    print('\n[S5] pred (=A+frac*C) against measured T_rest, by band')
    for lo, hi in BANDS:
        for name, ts in (('ORIG', ots), ('PORT', pts)):
            b = _band(ts, lo, hi)
            if len(b) < 5:
                continue
            mp, mt = med([t['pred'] for t in b]), med([t['T_rest'] for t in b])
            r = (mp / mt) if mt else float('nan')
            print('  %-10s %-5s n=%3d  pred %9.4f  T_rest %9.4f  ratio %7.4f'
                  % ('%d-%d' % (lo, hi), name, len(b), mp, mt, r))
    return ots, pts


# ------------------------------------------------------------------------ main

def main(argv):
    if not argv:
        print(__doc__)
        return
    cmd, a = argv[0], {}
    i = 1
    while i < len(argv):
        if argv[i].startswith('--'):
            a[argv[i][2:]] = argv[i + 1]
            i += 2
        else:
            i += 1
    {'selftest': cmd_selftest, 'w1': cmd_w1, 'orig': cmd_orig,
     'compare': cmd_compare}[cmd](a)


if __name__ == '__main__':
    main(sys.argv[1:])
