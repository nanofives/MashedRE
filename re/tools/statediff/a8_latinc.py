#!/usr/bin/env python3
"""A8/U-9156: the per-frame VELOCITY INCREMENT, decomposed on the body forward/right
axes, on both sides, in a low horizontal-speed band.

Registered in re/analysis/D2_REOPEN_2026-09-29.md section 21.1 BEFORE it was run. It
answers section 20.15's fork between the two remaining sub-candidates for the port's
6.7x slip deficit: (1) the wheel-friction impulse that GENERATES lateral velocity, and
(2) the body yaw rate / reverse gate.

Why the increment and not the ratio. Section 20.15 already measured that the port's
per-frame lateral RATIO `lat(n+1)/lat(n)` is 1.0590 against the original's 0.8939, which
refuses an over-strong bleed but says nothing about how much lateral is made each frame.
The increment does, and splitting it by SIGN along the body right axis separates "makes
less" from "makes as much but alternating".

Decomposition (identical expression on both sides, taken on frame n's basis):

    fwd   = unit(rec+0x9d4, rec+0x9dc)      # body forward, horizontal (y dropped)
    right = (-fwd.z, +fwd.x)                # the 90-deg rotation of fwd; a CONVENTION,
                                            # not a claim about the game's handedness
    dvel  = vel(n+1) - vel(n)               # vel = rec+0x9b0 (x) / rec+0x9b8 (z)
    dlat  = dvel . right                    # SIGNED lateral increment  <- the headline
    dfwd  = dvel . fwd                      # SIGNED forward increment

Friction-impulse witness. WheelContactSolver.cpp:279-281 / :286 add the wheel friction
impulse straight into the body velocity: vF(self,0x26c/0x26d/0x26e) are DWORD indices, so
0x26c*4 = 0x9b0, 0x9b4, 0x9b8 -- the record's own velocity. Its source is pf[-2]/pf[-1]/pf[0]
with pf = vFP(self,0x82) = byte 0x208 and a per-wheel stride of 0x31 dwords = 0xc4 bytes, so
the impulse vector is record byte 0x200/0x204/0x208 + w*0xc4.

[UNCERTAIN] that field is a once-per-render-tick SAMPLE of a slot the solver never clears
(only the pf[-0x1c] gate is zeroed, WheelContactSolver.cpp:278/:284). Its value is the last
writer's, so it is a witness, not a per-frame integral. The headline stays dlat.

Both sides are filtered to GROUNDED consecutive frame pairs. "Grounded" is rec+0x9e0 >= 3.5
on the original (the same test a8_slip_axis.py:35 uses) and gnd >= 3.5 on the port.

Usage:
  py -3.12 re/tools/statediff/a8_latinc.py \
      --orig verify/d2_reopen_20260929/orig_solo3.msd --orig-window 981 1120 \
      --port <player_trace.log> --port-window 81 220 [--port ... --port-window ...] \
      [--band 100 200] [--motion <motion_diag.log>]

Read-only. Does not execute the game.
"""
import argparse, math, os, re, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

# Record byte offsets, all cited in the module docstring.
O_VEL_X, O_VEL_Z = 0x9b0, 0x9b8
O_FWD_X, O_FWD_Z = 0x9d4, 0x9dc
O_AVY = 0x9c0
O_SPEED = 0x9e4
O_GND = 0x9e0
O_FRIC = 0x200          # + w*0xc4, the (x,y,z) friction impulse source
WHEEL_STRIDE = 0xc4


def wrap(a):
    while a > math.pi:
        a -= 2 * math.pi
    while a < -math.pi:
        a += 2 * math.pi
    return a


def med(v):
    v = sorted(v)
    return v[len(v) // 2] if v else float('nan')


def f32(p, o):
    return struct.unpack_from('<f', p, o)[0]


def orig_frames(path, lo, hi):
    """-> list of per-frame dicts in frame order, restricted to [lo, hi]."""
    _, _, frames = m.load_msd(path)
    out = []
    for idx in sorted(frames):
        if not (lo <= idx <= hi):
            continue
        p = frames[idx]
        fx, fz = f32(p, O_FWD_X), f32(p, O_FWD_Z)
        vx, vz = f32(p, O_VEL_X), f32(p, O_VEL_Z)
        fr = [(f32(p, O_FRIC + w * WHEEL_STRIDE), f32(p, O_FRIC + 8 + w * WHEEL_STRIDE))
              for w in range(4)]
        out.append(dict(f=idx, fx=fx, fz=fz, vx=vx, vz=vz, gnd=f32(p, O_GND),
                        sp=f32(p, O_SPEED), avy=f32(p, O_AVY), fric=fr))
    return out


PT = re.compile(
    r"^f=(\d+) .*? yaw=([-\d.e+]+) sp=([-\d.e+]+) "
    r"vel=\(([-\d.e+]+),([-\d.e+]+),([-\d.e+]+)\).*?"
    r"av=\(([-\d.e+]+),([-\d.e+]+),([-\d.e+]+)\).*?"
    r"gnd=([-\d.e+]+) bodyfwd=\(([-\d.e+]+),([-\d.e+]+),([-\d.e+]+)\)")


def port_frames(path, lo, hi):
    out = []
    for line in open(path, errors='replace'):
        mm = PT.match(line)
        if not mm:
            continue
        idx = int(mm[1])
        if not (lo <= idx <= hi):
            continue
        out.append(dict(f=idx, fx=float(mm[11]), fz=float(mm[13]),
                        vx=float(mm[4]), vz=float(mm[6]), gnd=float(mm[10]),
                        sp=float(mm[3]), avy=float(mm[8]), fric=None))
    return out


def pairs(rows, band_lo, band_hi):
    """Consecutive grounded pairs whose frame n horizontal speed is in the band."""
    by = {r['f']: r for r in rows}
    out = []
    for r in rows:
        n = by.get(r['f'] + 1)
        if n is None:
            continue
        if r['gnd'] < 3.5 or n['gnd'] < 3.5:
            continue
        h = math.hypot(r['vx'], r['vz'])
        if not (band_lo <= h < band_hi):
            continue
        fn = math.hypot(r['fx'], r['fz'])
        if fn < 1e-12:
            continue
        fx, fz = r['fx'] / fn, r['fz'] / fn
        rx, rz = -fz, fx
        dvx, dvz = n['vx'] - r['vx'], n['vz'] - r['vz']
        bh0 = math.atan2(r['fz'], r['fx'])
        bh1 = math.atan2(n['fz'], n['fx'])
        fn1 = math.hypot(n['fx'], n['fz'])
        rx1, rz1 = (-n['fz'] / fn1, n['fx'] / fn1) if fn1 > 1e-12 else (rx, rz)
        lat0 = r['vx'] * rx + r['vz'] * rz
        lat1 = n['vx'] * rx1 + n['vz'] * rz1
        out.append(dict(f=r['f'], horiz=h, sp=r['sp'],
                        dlat=dvx * rx + dvz * rz, dfwd=dvx * fx + dvz * fz,
                        # [section 21.2] RETENTION, each frame on its OWN basis. This is
                        # the quantity section 20.15's median-over-the-whole-band hid: the
                        # port's lateral collapses in the 5-6 frames after each bounce and
                        # then REGROWS, and a single median over both phases cancels them.
                        ret=(lat1 / lat0) if abs(lat0) > 1e-6 else float('nan'),
                        lat=lat0,
                        fwdv=r['vx'] * fx + r['vz'] * fz,
                        slip=wrap(math.atan2(r['vz'], r['vx']) - bh0),
                        avy=r['avy'], dbh=wrap(bh1 - bh0), fric=r['fric']))
    return out


def report(label, rows):
    print(f"=== {label}: n={len(rows)} ===")
    if not rows:
        return None
    dl = [r['dlat'] for r in rows]
    npos = sum(1 for x in dl if x > 0)
    nneg = sum(1 for x in dl if x < 0)
    mabs = med([abs(x) for x in dl])
    print(f"  median horiz speed     {med([r['horiz'] for r in rows]):>12.3f}")
    print(f"  median rec+0x9e4 speed {med([r['sp'] for r in rows]):>12.3f}")
    print(f"  median |dlat|          {mabs:>12.4f}   <- HEADLINE")
    print(f"  median  dlat (signed)  {med(dl):>+12.4f}")
    print(f"  sign split             {npos:>5} pos / {nneg:<5} neg"
          f"   (sum {sum(dl):+.3f})")
    # [section 21.2] COHERENCE. The magnitude alone cannot tell "makes less lateral" from
    # "makes as much but alternating, so it cancels". |sum| / sum|.| is 1.0 for a
    # perfectly one-signed increment and 0.0 for one that cancels exactly.
    sabs = sum(abs(x) for x in dl)
    coh = abs(sum(dl)) / sabs if sabs else float('nan')
    flips = sum(1 for i in range(1, len(dl))
                if (dl[i] > 0) != (dl[i - 1] > 0))
    print(f"  COHERENCE |sum|/sum|.| {coh:>12.4f}   <- 1 = one-signed, 0 = cancels")
    print(f"  mean dlat per frame    {sum(dl) / len(dl):>+12.4f}")
    print(f"  sign flips / pairs     {flips:>5} / {len(dl) - 1:<5}"
          f"   ({100.0 * flips / max(1, len(dl) - 1):.1f}%)")
    # [section 21.2] The STATE the increment acts on. dlat's sign only means something
    # next to the sign of the lateral velocity already there: a negative dlat on a
    # positive lat is a bleed, a negative dlat on a negative lat is generation.
    lv = [r['lat'] for r in rows]
    print(f"  median  lat  (vel.right) {med(lv):>+10.4f}"
          f"   {sum(1 for x in lv if x > 0)} pos / {sum(1 for x in lv if x < 0)} neg")
    rt = [r['ret'] for r in rows if not math.isnan(r['ret'])]
    print(f"  median  lat RETENTION  {med(rt):>12.4f}"
          f"   (n={len(rt)}; 1 = held, 0 = annihilated)")
    print(f"  median  fwdv (vel.fwd) {med([r['fwdv'] for r in rows]):>+12.4f}")
    print(f"  median  SIGNED slip    {med([r['slip'] for r in rows]):>+12.4f}"
          f"   (median |slip| {med([abs(r['slip']) for r in rows]):.4f})")
    same = sum(1 for r in rows if (r['lat'] > 0) == (r['dlat'] > 0))
    print(f"  dlat AGREES with lat   {same:>5} / {len(rows):<5}"
          f"   ({100.0 * same / len(rows):.1f}% building, rest bleeding)")
    print(f"  median |dfwd|          {med([abs(r['dfwd']) for r in rows]):>12.4f}")
    print(f"  median  dfwd (signed)  {med([r['dfwd'] for r in rows]):>+12.4f}")
    yaw = med([abs(r['dbh']) for r in rows])
    print(f"  median |d bodyH|       {yaw:>12.6f}   <- YAW RATE (finite diff)")
    print(f"  median  d bodyH        {med([r['dbh'] for r in rows]):>+12.6f}")
    print(f"  median  av.y (+0x9c0)  {med([r['avy'] for r in rows]):>+12.6f}")
    if rows[0]['fric'] is not None:
        for w in range(4):
            fm = med([math.hypot(r['fric'][w][0], r['fric'][w][1]) for r in rows])
            print(f"  wheel {w} |fric(0x{O_FRIC + w * WHEEL_STRIDE:03x}+0/8)| median  {fm:>12.4f}")
    return dict(labs=mabs, yaw=yaw, n=len(rows), coh=coh,
                mean=sum(dl) / len(dl))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', default='verify/d2_reopen_20260929/orig_solo3.msd')
    ap.add_argument('--orig-window', nargs=2, type=int, default=[981, 1120])
    ap.add_argument('--port', action='append', default=[])
    ap.add_argument('--port-window', nargs=2, type=int, default=[81, 220])
    ap.add_argument('--band', nargs=2, type=float, default=[100.0, 200.0])
    a = ap.parse_args()
    lo, hi = a.band
    print(f"band {lo:g}-{hi:g} horizontal, grounded consecutive pairs, "
          f"orig frames {a.orig_window[0]}-{a.orig_window[1]}, "
          f"port f={a.port_window[0]}..{a.port_window[1]}\n")
    o = report('ORIGINAL ' + a.orig,
               pairs(orig_frames(a.orig, *a.orig_window), lo, hi))
    for p in a.port:
        print()
        q = report('PORT ' + p, pairs(port_frames(p, *a.port_window), lo, hi))
        if o and q:
            print(f"\n  R = |dlat| port/orig = {q['labs'] / o['labs']:.4f}"
                  f"   yaw ratio = {q['yaw'] / o['yaw']:.4f}"
                  f"   (rule: section 21.1)")
            print(f"  COHERENCE  orig {o['coh']:.4f}  port {q['coh']:.4f}"
                  f"   ratio {q['coh'] / o['coh']:.4f}"
                  f"   |   mean dlat/frame  orig {o['mean']:+.4f}  port {q['mean']:+.4f}")


if __name__ == '__main__':
    main()
