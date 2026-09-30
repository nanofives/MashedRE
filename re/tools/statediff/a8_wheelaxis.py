#!/usr/bin/env python3
"""A8/U-9156: the FOUR wheel forward axes against the body forward axis, both sides.

Registered in re/analysis/D2_REOPEN_2026-09-29.md section 21.9 as the next command. Section
21.9 measured the original's `l_60` directly and found the 1.84x `grip*speed` deficit is
carried by `ld4` (2.71x) rather than `le4` (1.20x). `ld4` is the sine of the angle between
the wheel-point velocity direction and the WHEEL FORWARD AXIS (`Integrate2.cpp:432-440`), and
of those two inputs only the velocity is slip-coupled. **The axis is not**: it is set by A5's
steering, so comparing it cross-side is the non-circular half of the question.

The discriminator is the FRONT pair. `Integrate2.cpp:692-695`'s own `[G7-AXDIAG]` comment
states it: "if the front pair (w0/w1) is identical to the rear pair while steer is applied,
A5's rotation never reached these slots." A rear wheel does not steer, so the offset reported
by `a8_slip_axis.py` -- which reads wheel 2 -- cannot answer this.

ORIGINAL: `.msd`, wheel base `0x1a4 + w*0xc4`, axis x at `+0x7c` and z at `+0x84` (the same
          fields `a8_wheelfit.py` documents), body forward at `+0x9d4`/`+0x9dc`, steer angle
          at `+0x1a8`, grounded at `+0x9e0`.
PORT:     `motion_diag.log`'s `[A8-ORIENT]` line -- `wax=[x0,z0,x1,z1,x2,z2,x3,z3]`, `bodyH`,
          `steer`, `gnd`.

Reported per wheel: the median signed `wrap(axisHeading - bodyHeading)` in DEGREES, with `n`
and the median speed beside it, plus the FRONT-MINUS-REAR difference, which is the steer
deflection actually present in the slots A6a reads.

Usage: py -3.12 re/tools/statediff/a8_wheelaxis.py --orig <.msd> [--port <motion_diag.log>]
Read-only. Does not execute the game.
"""
import argparse, math, os, re, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

WB = [0x1a4 + w * 0xc4 for w in range(4)]


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


def orig_rows(path, steer_min, band):
    _, _, frames = m.load_msd(path)
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        if f32(p, 0x9e0) < 3.5 or f32(p, 0x1a8) < steer_min:
            continue
        h = math.hypot(f32(p, 0x9b0), f32(p, 0x9b8))
        if band and not (band[0] <= h < band[1]):
            continue
        bh = math.atan2(f32(p, 0x9dc), f32(p, 0x9d4))
        ax = [wrap(math.atan2(f32(p, WB[w] + 0x84), f32(p, WB[w] + 0x7c)) - bh)
              for w in range(4)]
        out.append(dict(f=idx, horiz=h, sp=f32(p, 0x9e4), steer=f32(p, 0x1a8), ax=ax))
    return out


RE = re.compile(r"steer=([-+\d.]+) gnd=([\d.]+) sp=([\d.]+) horiz=([\d.]+) "
                r"velH=([-\d.]+) bodyH=([-\d.]+).*?wax=\[([^\]]*)\]")


def port_rows(path, steer_min, band):
    out = []
    for line in open(path, errors='replace'):
        mm = RE.search(line)
        if not mm:
            continue
        if float(mm[2]) < 3.5 or float(mm[1]) < steer_min:
            continue
        h = float(mm[4])
        if band and not (band[0] <= h < band[1]):
            continue
        w = [float(x) for x in mm[7].split(',')]
        if len(w) < 8:
            continue
        bh = float(mm[6])
        ax = [wrap(math.atan2(w[2 * k + 1], w[2 * k]) - bh) for k in range(4)]
        out.append(dict(horiz=h, sp=float(mm[3]), steer=float(mm[1]), ax=ax))
    return out


def report(label, rows):
    print(f"=== {label}: n={len(rows)} ===")
    if not rows:
        return
    print(f"  median horiz speed {med([r['horiz'] for r in rows]):>10.2f}"
          f"   median steer {med([r['steer'] for r in rows]):>+9.3f}")
    deg = [[math.degrees(r['ax'][w]) for r in rows] for w in range(4)]
    for w in range(4):
        d = deg[w]
        print(f"  wheel {w} axis-minus-body  median {med(d):>+9.3f} deg"
              f"   p25 {sorted(d)[len(d)//4]:>+9.3f}   p75 {sorted(d)[3*len(d)//4]:>+9.3f}")
    fr = med([(deg[0][i] + deg[1][i]) / 2.0 - (deg[2][i] + deg[3][i]) / 2.0
              for i in range(len(rows))])
    print(f"  FRONT pair minus REAR pair  median {fr:>+9.3f} deg   <- the steer deflection "
          f"actually present in the slots A6a reads")
    ident = sum(1 for i in range(len(rows))
                if abs(deg[0][i] - deg[2][i]) < 1e-6 and abs(deg[1][i] - deg[3][i]) < 1e-6)
    print(f"  frames where FRONT == REAR bitwise  {ident}/{len(rows)}"
          f"   <- Integrate2.cpp:692's failure mode")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', default='verify/d2_reopen_20260929/orig_solo3.msd')
    ap.add_argument('--port', action='append', default=[])
    ap.add_argument('--orig-steer-min', type=float, default=33.0)
    ap.add_argument('--port-steer-min', type=float, default=0.9)
    ap.add_argument('--band', nargs=2, type=float, default=None)
    a = ap.parse_args()
    b = tuple(a.band) if a.band else None
    print(f"band {b if b else 'ALL'}, grounded, steer held\n")
    report('ORIGINAL ' + a.orig, orig_rows(a.orig, a.orig_steer_min, b))
    for p in a.port:
        print()
        report('PORT ' + p, port_rows(p, a.port_steer_min, b))


if __name__ == '__main__':
    main()
