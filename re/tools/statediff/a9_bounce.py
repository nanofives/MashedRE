#!/usr/bin/env python3
"""a9_bounce.py -- the FIRST-BOUNCE-ALIGNED cross-side comparator registered in
re/analysis/D2_REOPEN_2026-09-29.md section 22.1 (D2 re-close attempt 8).

Nothing in re/tools/ aligned an ORIGINAL .msd against a PORT log on a physical EVENT:
statediff.py --anchor-nonzero is .msd-vs-.msd only, and a8_slip_axis.py --transient bins by
frames-since-steer-onset per side rather than pairing rows. This tool pairs rows.

WHAT IT DOES
  1. Bounce detector, identical on both sides (section 22.1): with h(f) = hypot(vel.x, vel.z)
     and F0 = the first frame with h >= 10,
         B = argmax over f in (F0, F0+200] of 1 - h(f)/h(f-1),  restricted to h(f-1) >= 300.
     d = f - B.
  2. The original-vs-original CONTROL: two captures with identical argv (orig_solo3 /
     orig_solo4) give the run-to-run spread S(C) per channel over d in [-5, +12]. The
     tolerance actually applied is max(3*S(C), floor(C)) with the floors fixed in 22.1.
  3. The aligned C1..C8 table, and the 22.1 scan rule: increasing d, then C1..C8 in order;
     the first (d, C) outside tolerance with every earlier (d', C') inside it is THE FIRST
     DIVERGING TERM.

CHANNELS (22.1). w = wheel, wheel base b = 0x1a4 + w*0xc4.
  C1 steer deflection at the wheels  orig: front-minus-rear of wrap(atan2(ax.z,ax.x) - bodyH)
                                           over b+0x7c / b+0x84, a8_wheelaxis.py's quantity
                                     port: the same, from motion_diag `wax`
  C2 contact set                     DECLARED UNOBSERVABLE, see below
  C3 accumulator +0x144/148/14c      port: player_trace a144
  C4 horizontal speed                hypot(+0x9b0, +0x9b8) both sides
  C5 cos(slip) and signed slip       convention-free from the raw vel / fwd vectors
  C6 angular velocity av.y +0x9c0    port: player_trace av
  C7 body yaw rate d(bodyH)/frame    differenced atan2 of the forward vector
  C8 position (x, z) +0x958/+0x960   port: player_trace pos

C2 IS UNOBSERVABLE ON THE ORIGINAL SIDE, measured not assumed: the 18 contact slots at
+0x4a8 + i*0x40 have key (+0x04) == -1 on 2332 of orig_solo3.msd's 2333 frames, and +0x9ec is
0 on every frame -- the render-tick snapshot (scenario_launch.py's RENDER_TICK 0x004c1be0
phase-3 hook) lands after the substep loop has cleared them. So no port-side slot channel is
added for it; it is reported as unobservable rather than compared.

C5 is computed convention-free because motion_diag's velH/bodyH/slip use atan2(z, x)
(VehiclePhysicsRun.cpp:1137-1138) while the original-side scratch convention is atan2(x, z);
the two differ by a sign on slip and mixing them fakes a mirrored car.

Usage:
  py -3.12 re/tools/statediff/a9_bounce.py --orig <a.msd> [--orig-ctrl <b.msd>]
        [--port-dir <dir with motion_diag.log + player_trace.log>]
        [--dmin -5] [--dmax 12] [--csv <out.csv>]

Read-only. Does not execute the game.
"""
import argparse, csv, math, os, re, struct, sys

WB = [0x1a4 + w * 0xc4 for w in range(4)]

# 22.1 tolerance floors, verbatim.
FLOOR = {
    'C1': ('rel', 0.25),
    'C3x': ('relabs', 0.25, 0.01), 'C3y': ('relabs', 0.25, 0.01),
    'C3z': ('relabs', 0.25, 0.01),
    'C4': ('rel', 0.10),
    'C5cos': ('abs', 0.05), 'C5slip': ('abs', 0.05),
    'C6': ('relabs', 0.25, 0.005),
    'C7': ('relabs', 0.25, 0.002),
    'C8x': ('abs', 0.15), 'C8z': ('abs', 0.15),
}
# The scan order of 22.1: C1, C2 (unobservable, skipped), C3, C4, C5, C6, C7, C8.
ORDER = ['C1', 'C3x', 'C3y', 'C3z', 'C4', 'C5cos', 'C5slip', 'C6', 'C7', 'C8x', 'C8z']


def wrap(a):
    while a > math.pi:
        a -= 2 * math.pi
    while a < -math.pi:
        a += 2 * math.pi
    return a


def f32(p, o):
    return struct.unpack_from('<f', p, o)[0]


def i32(p, o):
    return struct.unpack_from('<i', p, o)[0]


# ---------------------------------------------------------------- channel extraction

def chan(pos, vel, fwd, av, acc, wax, bodyH_prev):
    """Build the channel dict from raw vectors, identically for both sides."""
    h = math.hypot(vel[0], vel[2])
    fh = math.hypot(fwd[0], fwd[2])
    if h > 1e-6 and fh > 1e-6:
        cos = (vel[0] * fwd[0] + vel[2] * fwd[2]) / (h * fh)
        cos = max(-1.0, min(1.0, cos))
        slip = math.atan2(fwd[2] * vel[0] - fwd[0] * vel[2],
                          vel[0] * fwd[0] + vel[2] * fwd[2])
    else:
        cos, slip = float('nan'), float('nan')
    bodyH = math.atan2(fwd[2], fwd[0]) if fh > 1e-9 else float('nan')
    c = {
        'C1': wax, 'C3x': acc[0], 'C3y': acc[1], 'C3z': acc[2],
        'C4': h, 'C5cos': cos, 'C5slip': slip, 'C6': av[1],
        'C7': (wrap(bodyH - bodyH_prev) if bodyH_prev is not None else float('nan')),
        'C8x': pos[0], 'C8z': pos[2],
        '_bodyH': bodyH, '_h': h,
    }
    return c


def wax_front_minus_rear(ax_headings, bodyH):
    d = [math.degrees(wrap(a - bodyH)) for a in ax_headings]
    return (d[0] + d[1]) / 2.0 - (d[2] + d[3]) / 2.0


def load_orig(path):
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1', path + ' is not MSD1'
    rec, _base, _ = struct.unpack_from('<III', b, 4)
    off, frames = 16, {}
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        frames[fi] = b[off + 4: off + 4 + rec]
        off += 4 + rec
    rows, prev = {}, None
    for fi in sorted(frames):
        p = frames[fi]
        pos = (f32(p, 0x958), f32(p, 0x95c), f32(p, 0x960))
        vel = (f32(p, 0x9b0), f32(p, 0x9b4), f32(p, 0x9b8))
        fwd = (f32(p, 0x9d4), f32(p, 0x9d8), f32(p, 0x9dc))
        av = (f32(p, 0x9bc), f32(p, 0x9c0), f32(p, 0x9c4))
        acc = (f32(p, 0x144), f32(p, 0x148), f32(p, 0x14c))
        bh = math.atan2(fwd[2], fwd[0]) if (fwd[0] or fwd[2]) else 0.0
        axh = [math.atan2(f32(p, WB[w] + 0x84), f32(p, WB[w] + 0x7c)) for w in range(4)]
        r = chan(pos, vel, fwd, av, acc, wax_front_minus_rear(axh, bh), prev)
        r['_gnd'] = f32(p, 0x9e0)
        r['_sp'] = f32(p, 0x9e4)
        r['_steer'] = f32(p, 0x1a8)
        r['_nslot'] = sum(1 for s in range(0x12) if i32(p, 0x4a8 + s * 0x40 + 4) != -1)
        r['_c9ec'] = i32(p, 0x9ec)
        rows[fi] = r
        prev = r['_bodyH']
    return rows


PT = re.compile(
    r"f=(\d+) dt=(\S+) pos=\((\S+),(\S+),(\S+)\) yaw=(\S+) sp=(\S+) "
    r"vel=\((\S+),(\S+),(\S+)\).*?a144=\((\S+),(\S+),(\S+)\) av=\((\S+),(\S+),(\S+)\) "
    r"c9ec=(\d+) gnd=(\S+) bodyfwd=\((\S+),(\S+),(\S+)\)")
MD = re.compile(r"reseed=(\d)\b.*?steer=(\S+) gnd=(\S+) sp=(\S+) horiz=(\S+).*?wax=\[([^\]]*)\]")


def load_port(d):
    """player_trace.log carries pos/vel/bodyfwd/av/a144 with its own f= counter;
    motion_diag.log carries wax but has no counter (line ordinal = frame, one line per
    frame for slot 0 -- the assumption a8_slip_axis.py:67 already relies on). They are
    cross-checked on `sp` and the check is printed."""
    pt_path = os.path.join(d, 'player_trace.log')
    md_path = os.path.join(d, 'motion_diag.log')
    md = {}
    n = 0
    for line in open(md_path, errors='replace'):
        m = MD.search(line)
        if not m:
            continue
        n += 1
        w = [float(x) for x in m[6].split(',')]
        md[n] = dict(steer=float(m[2]), gnd=float(m[3]), sp=float(m[4]),
                     horiz=float(m[5]), wax=w, reseed=int(m[1]))
    if not os.path.exists(pt_path):
        return {}, md, 'NO player_trace.log'
    pt = []
    for line in open(pt_path, errors='replace'):
        m = PT.search(line)
        if m:
            pt.append(m)
    # MEASURE the join offset, do not assume it. player_trace prints BEFORE UpdateRace
    # (TrackRenderer.cpp:2937) and motion_diag at the END of VehiclePhysicsRun
    # (VehiclePhysicsRun.cpp:1133), so player_trace f=N sees the state frame N-1's physics
    # left. The shared invariant is the HORIZONTAL speed -- motion_diag's `horiz` is
    # hypot(+0x9b0, +0x9b8) (VehiclePhysicsRun.cpp:1135-1136) and player_trace's `vel` is
    # the same vector. Pick the integer shift k (md line = pt f + k) that minimises the
    # mismatch count, and report both the shift and the residual.
    def mismatches(k):
        bad = 0
        for m in pt:
            hh = math.hypot(float(m[8]), float(m[10]))
            mm = md.get(int(m[1]) + k)
            if mm is None:
                continue
            if abs(mm['horiz'] - hh) > max(0.05, 0.005 * max(hh, 1.0)):
                bad += 1
        return bad
    cand = {k: mismatches(k) for k in range(-3, 4)}
    shift = min(cand, key=lambda k: cand[k])
    mism = cand[shift]
    rows, prev = {}, None
    for m in pt:
        fi = int(m[1])
        pos = (float(m[3]), float(m[4]), float(m[5]))
        vel = (float(m[8]), float(m[9]), float(m[10]))
        acc = (float(m[11]), float(m[12]), float(m[13]))
        av = (float(m[14]), float(m[15]), float(m[16]))
        fwd = (float(m[19]), float(m[20]), float(m[21]))
        bh = math.atan2(fwd[2], fwd[0]) if (fwd[0] or fwd[2]) else 0.0
        mm = md.get(fi + shift)
        if mm:
            axh = [math.atan2(mm['wax'][2 * k + 1], mm['wax'][2 * k]) for k in range(4)]
            wax = wax_front_minus_rear(axh, bh)
            # Alignment cross-check. NOT on `sp`: player_trace prints car_speed_ and
            # motion_diag prints record +0x9e4, which are different quantities. The shared
            # invariant is the HORIZONTAL speed -- motion_diag's `horiz` is
            # hypot(+0x9b0, +0x9b8) (VehiclePhysicsRun.cpp:1135-1136) and player_trace's
            # vel is the same vector, so on a correct join they agree to float print noise.
        else:
            wax = float('nan')
        r = chan(pos, vel, fwd, av, acc, wax, prev)
        r['_gnd'] = float(m[18])
        r['_sp'] = float(m[7])
        r['_steer'] = mm['steer'] if mm else float('nan')
        r['_nslot'] = -1
        r['_c9ec'] = int(m[17])
        rows[fi] = r
        prev = r['_bodyH']
    note = ('player_trace<->motion_diag join: md_line = pt_f %+d, horiz mismatch on %d of %d '
            'frames (per-shift %s)' % (shift, mism, len(rows), cand))
    return rows, md, note


# ---------------------------------------------------------------- detector

def bounce_frame(rows, win=200, hmin_prev=300.0, h_start=10.0):
    ks = sorted(rows)
    f0 = None
    for k in ks:
        if rows[k]['_h'] >= h_start:
            f0 = k
            break
    if f0 is None:
        return None, None
    best, bf = -1.0, None
    for k in ks:
        if not (f0 < k <= f0 + win):
            continue
        prev = rows.get(k - 1)
        if prev is None or prev['_h'] < hmin_prev:
            continue
        loss = 1.0 - rows[k]['_h'] / prev['_h']
        if loss > best:
            best, bf = loss, k
    return bf, (f0, best)


# ---------------------------------------------------------------- tolerance / verdict

def tol_for(c, s, a, b):
    kind = FLOOR[c]
    if kind[0] == 'abs':
        return max(3.0 * s, kind[1]), 'abs'
    if kind[0] == 'rel':
        scale = max(abs(a), abs(b))
        return max(3.0 * s, kind[1] * scale), 'rel'
    scale = max(abs(a), abs(b))
    return max(3.0 * s, kind[1] * scale, kind[2]), 'relabs'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', default='verify/d2_reopen_20260929/orig_solo3.msd')
    ap.add_argument('--orig-ctrl', default='verify/d2_reopen_20260929/orig_solo4.msd')
    ap.add_argument('--port-dir', default=None)
    ap.add_argument('--dmin', type=int, default=-5)
    ap.add_argument('--dmax', type=int, default=12)
    ap.add_argument('--csv', default=None)
    a = ap.parse_args()

    O = load_orig(a.orig)
    bo, io_ = bounce_frame(O)
    print('ORIGINAL %s  frames=%d  drive-start=%s  BOUNCE B=%s  loss=%.4f'
          % (a.orig, len(O), io_[0], bo, io_[1]))
    print('  control 22.1: B_orig must be in [979, 983] -> %s'
          % ('PASS' if bo is not None and 979 <= bo <= 983 else 'FAIL'))
    print('  ORIGINAL contact-slot occupancy (C2): non-empty-slot count histogram over the '
          'whole capture')
    import collections
    hh = collections.Counter(O[k]['_nslot'] for k in O)
    print('    %s   +0x9ec distinct %s   -> C2 UNOBSERVABLE on this side'
          % (dict(sorted(hh.items())), sorted(set(O[k]['_c9ec'] for k in O))))

    # ---- the original-vs-original control
    S = {c: 0.0 for c in ORDER}
    if a.orig_ctrl and os.path.exists(a.orig_ctrl):
        Oc = load_orig(a.orig_ctrl)
        bc, ic = bounce_frame(Oc)
        print('CONTROL  %s  frames=%d  drive-start=%s  BOUNCE B=%s  loss=%.4f'
              % (a.orig_ctrl, len(Oc), ic[0], bc, ic[1]))
        for d in range(a.dmin, a.dmax + 1):
            ra, rb = O.get(bo + d), Oc.get(bc + d)
            if not ra or not rb:
                continue
            for c in ORDER:
                va, vb = ra[c], rb[c]
                if va != va or vb != vb:
                    continue
                S[c] = max(S[c], abs(va - vb))
        print('  S(C) = max |orig_solo3 - orig_solo4| over d in [%d, %d], BEFORE any port row '
              'is read:' % (a.dmin, a.dmax))
        for c in ORDER:
            print('    %-7s %.6g' % (c, S[c]))
    else:
        print('CONTROL  MISSING -- S(C) = 0, tolerances fall back to the 22.1 floors')

    if not a.port_dir:
        return

    P, MDrows, note = load_port(a.port_dir)
    bp, ip = bounce_frame(P)
    print('\nPORT     %s  frames=%d  drive-start=%s  BOUNCE B=%s  loss=%.4f'
          % (a.port_dir, len(P), ip[0] if ip else None, bp, ip[1] if ip else float('nan')))
    print('  %s' % note)
    print('  control 22.1: B_port must be in [80, 84] -> %s'
          % ('PASS' if bp is not None and 80 <= bp <= 84 else 'FAIL'))

    print('\n=== ALIGNED TABLE, d = f - B ===')
    hdr = ('   d |    ORIG h    PORT h |  O cos   P cos |  O slip   P slip |'
           '   O av.y     P av.y |  O dyaw   P dyaw |  O a148  P a148 |  O a14c  P a14c |'
           '  O wax  P wax')
    print(hdr)
    out_rows = []
    for d in range(a.dmin, a.dmax + 1):
        ra, rb = O.get(bo + d), P.get(bp + d)
        if not ra or not rb:
            continue
        print('%4d | %9.2f %9.2f | %+6.4f %+6.4f | %+7.4f %+7.4f | %+9.5f %+9.5f | '
              '%+7.4f %+7.4f | %+6.4f %+6.4f | %+6.4f %+6.4f | %+6.2f %+6.2f'
              % (d, ra['C4'], rb['C4'], ra['C5cos'], rb['C5cos'], ra['C5slip'], rb['C5slip'],
                 ra['C6'], rb['C6'], ra['C7'], rb['C7'], ra['C3y'], rb['C3y'],
                 ra['C3z'], rb['C3z'], ra['C1'], rb['C1']))
        row = dict(d=d, orig_f=bo + d, port_f=bp + d)
        for c in ORDER:
            row['o_' + c] = ra[c]
            row['p_' + c] = rb[c]
        row['o_gnd'] = ra['_gnd']; row['p_gnd'] = rb['_gnd']
        row['o_steer'] = ra['_steer']; row['p_steer'] = rb['_steer']
        out_rows.append(row)

    print('\n=== 22.1 SCAN: increasing d, then C1..C8 in order ===')
    first = None
    for d in range(0, a.dmax + 1):
        ra, rb = O.get(bo + d), P.get(bp + d)
        if not ra or not rb:
            continue
        for c in ORDER:
            va, vb = ra[c], rb[c]
            if va != va or vb != vb:
                continue
            t, kind = tol_for(c, S[c], va, vb)
            bad = abs(va - vb) > t
            if c == 'C1' and bad:
                rel = abs(va - vb) / max(abs(va), abs(vb), 1e-9)
                if rel < 0.25:
                    print('  d=%+d C1 outside tol by %.1f%% -- below 25%%, this is the section '
                          '21.10 front-axis defect; SKIPPED per 22.1' % (d, 100 * rel))
                    continue
            if bad:
                first = (d, c, va, vb, t, kind)
                break
        if first:
            break
    if first:
        d, c, va, vb, t, kind = first
        print('  FIRST DIVERGING TERM:  d = %+d   channel %s' % (d, c))
        print('    ORIGINAL %.8g    PORT %.8g    |delta| %.6g    tol %.6g (%s, 3*S=%.6g)'
              % (va, vb, abs(va - vb), t, kind, 3 * S[c]))
        win = [r for r in out_rows if 0 <= r['d'] <= d]
        hs = sorted(r['o_C4'] for r in win)
        print('    window d=0..%+d   n=%d   ORIGINAL median h %.2f' % (d, len(win), hs[len(hs) // 2]))
    else:
        print('  NOTHING diverges through d=%+d -- reading 7 of 22.1 fires' % a.dmax)

    if a.csv and out_rows:
        with open(a.csv, 'w', newline='') as fh:
            w = csv.DictWriter(fh, fieldnames=list(out_rows[0].keys()))
            w.writeheader()
            w.writerows(out_rows)
        print('\nwrote %s' % a.csv)


if __name__ == '__main__':
    main()
