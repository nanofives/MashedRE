#!/usr/bin/env python3
"""A8 slip-deficit: the ORIENTATION half, measured per side.

WHY THIS EXISTS
---------------
a8_momentum.py proved the force->velocity half of the loop is applied identically
on both sides (effective dt per frame matches on three regimes). It ends with:
"the orientation half (bodyH) is what is left". Nobody has measured it.

The port rotates the body basis, on the record+0x10 == 0 arm, from a STEER
DIFFERENTIAL torque (BodyOrientationIntegrate.cpp: BodyOrient_OmegaFromSteer,
"yaw on this arm comes from the steer differential, never from A6a") and
deliberately does NOT feed the A6a yaw rate +0x9c0 into the basis. Whether the
ORIGINAL's forward axis (+0x9d4/+0x9dc) follows +0x9c0 or not is a per-frame
self-consistency question between two RECORD FIELDS, answerable from the .msd
capture with no game run, no hook, no Ghidra.

THE MEASURE
-----------
Per consecutive driving frame pair, per side:
    dBodyH   = wrap(bodyH[i+1] - bodyH[i])          measured rotation of the body
    yawrate  = record +0x9c0 (orig) / av.y (port)   A6a angular velocity, world Y
    dVelH    = wrap(velH[i+1] - velH[i])            for the slip-growth comparison
Reported per speed band: median dBodyH, median yawrate, median per-sample ratio
dBodyH/yawrate (an effective dt, same construction as a8_momentum.py), Pearson r
between dBodyH and yawrate, and Pearson r between dBodyH and steer*speed (the
shape the port's steer-arm torque has: grip = speed/1500 clamped, times steer).

  - If the ORIGINAL's dBodyH tracks +0x9c0 with a stable dt and high r, the
    original integrates its body basis FROM the A6a angular velocity, and the
    port's "+0x9c0 deliberately absent" arm is the wrong arm for a driving car.
  - If the original's dBodyH does NOT track +0x9c0 but the port's does not
    either, the arm choice is not the mechanism and the search moves elsewhere.
  - Sign conventions differ per side (D2_REALPHYS_REMEASURE_2026-08-21.md: the
    original's +yawrate turns the atan2(z,x) heading the NEGATIVE way). Ratios
    are reported signed; |r| is what carries the information.

INPUTS: same files as a8_momentum.py. Regime filters are a8_momentum's `pairs()`.
Read-only. Does not execute the game.
"""
import argparse
import math
import re
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

OFF_YAWRATE = 0x9c0
RE_AV = re.compile(r"av=\(([-+0-9.eE]+),([-+0-9.eE]+),([-+0-9.eE]+)\)")


def samples_original(path):
    _, _, frames = m.load_msd(path)
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        vx, vz = m.f32(p, m.OFF_VELX), m.f32(p, m.OFF_VELZ)
        fwdx, fwdz = m.f32(p, m.OFF_FWDX), m.f32(p, m.OFF_FWDZ)
        horiz = math.hypot(vx, vz)
        out.append(dict(
            idx=idx, vx=vx, vz=vz, horiz=horiz,
            velH=math.atan2(vz, vx) if horiz > 1e-3 else 0.0,
            bodyH=math.atan2(fwdz, fwdx),
            yawrate=m.f32(p, OFF_YAWRATE),
            sp=m.f32(p, m.OFF_SPEED), gnd=m.f32(p, m.OFF_GND),
            steer=m.f32(p, m.OFF_STEER0), reseed=0,
        ))
    return out


def samples_port(path):
    rows = m.samples_port(path)
    # re-read av per line (a8_momentum's RE_KV does not capture it)
    avs = {}
    with open(path, "r", errors="replace") as fh:
        for lineno, line in enumerate(fh):
            mm = RE_AV.search(line)
            if mm:
                avs[lineno] = tuple(float(x) for x in mm.groups())
    for r in rows:
        av = avs.get(r["idx"], (0.0, 0.0, 0.0))
        r["yawrate"] = av[1]
        r["av"] = av
    return rows


def pearson(xs, ys):
    n = len(xs)
    if n < 3:
        return float("nan")
    mx, my = sum(xs) / n, sum(ys) / n
    sxx = sum((x - mx) ** 2 for x in xs)
    syy = sum((y - my) ** 2 for y in ys)
    if sxx < 1e-18 or syy < 1e-18:
        return float("nan")
    return sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / math.sqrt(sxx * syy)


def med(v):
    v = sorted(v)
    return v[len(v) // 2] if v else float("nan")


def measure(rows, label, min_speed, grounded, drop_reseed, steer_min, steer_is_deg):
    ps = m.pairs(rows, min_speed, grounded, drop_reseed, steer_min)
    print(f"\n=== {label} ===")
    print(f"  raw samples {len(rows)}   usable consecutive pairs {len(ps)}")
    if not ps:
        print("  NO USABLE PAIRS")
        return {}
    res = {}
    print(f"  {'band':<10} {'n':>4} {'dBodyH/fr':>10} {'yawrate':>10} {'dt=dB/yr':>10} "
          f"{'r(dB,yr)':>9} {'r(dB,st*v)':>10} {'dVelH/fr':>10} {'slip':>8}")
    for lo, hi in m.BANDS:
        sel = [(a, b) for a, b in ps if lo <= a["horiz"] < hi]
        if len(sel) < 8:
            continue
        dB, yr, stv, dV, sl = [], [], [], [], []
        for a, b in sel:
            dB.append(m.wrap(b["bodyH"] - a["bodyH"]))
            yr.append(a["yawrate"])
            st = a["steer"] / 33.9 if steer_is_deg else a["steer"]
            g = min(a["horiz"] / 1500.0, 1.0)
            stv.append(st * g)
            dV.append(m.wrap(b["velH"] - a["velH"]))
            sl.append(abs(m.wrap(a["velH"] - a["bodyH"])))
        rr = [d / y for d, y in zip(dB, yr) if abs(y) > 1e-9]
        print(f"  {lo}-{hi:<5} {len(dB):>4} {med(dB):>10.5f} {med(yr):>10.5f} {med(rr):>10.4f} "
              f"{pearson(dB, yr):>9.3f} {pearson(dB, stv):>10.3f} {med(dV):>10.5f} {med(sl):>8.4f}")
        res[(lo, hi)] = dict(n=len(dB), dB=med(dB), yr=med(yr), dt=med(rr),
                             r_yr=pearson(dB, yr), r_stv=pearson(dB, stv), dV=med(dV), slip=med(sl))
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--orig")
    ap.add_argument("--port")
    ap.add_argument("--min-speed", type=float, default=500.0)
    ap.add_argument("--no-grounded-filter", action="store_true")
    ap.add_argument("--orig-steer-min", type=float, default=None)
    ap.add_argument("--port-steer-min", type=float, default=None)
    args = ap.parse_args()
    if not args.orig and not args.port:
        ap.error("give --orig and/or --port")
    g = not args.no_grounded_filter
    o = p = {}
    if args.orig:
        o = measure(samples_original(args.orig), f"ORIGINAL  {args.orig}",
                    args.min_speed, g, False, args.orig_steer_min, True)
    if args.port:
        p = measure(samples_port(args.port), f"PORT      {args.port}",
                    args.min_speed, g, True, args.port_steer_min, False)
    if o and p:
        print("\n=== ORIENTATION HALF, side by side ===")
        print(f"  {'band':<10} {'dt orig':>9} {'dt port':>9} {'r_yr orig':>10} {'r_yr port':>10} "
              f"{'r_stv orig':>11} {'r_stv port':>11} {'dB/dV orig':>11} {'dB/dV port':>11}")
        for band in m.BANDS:
            if band in o and band in p:
                a, b = o[band], p[band]
                ra = a["dB"] / a["dV"] if abs(a["dV"]) > 1e-9 else float("nan")
                rb = b["dB"] / b["dV"] if abs(b["dV"]) > 1e-9 else float("nan")
                print(f"  {band[0]}-{band[1]:<5} {a['dt']:>9.4f} {b['dt']:>9.4f} {a['r_yr']:>10.3f} {b['r_yr']:>10.3f} "
                      f"{a['r_stv']:>11.3f} {b['r_stv']:>11.3f} {ra:>11.3f} {rb:>11.3f}")
        print("\n  READ IT THIS WAY: a stable dt and |r_yr| near 1 on a side means that side's")
        print("  body rotates FROM its A6a yaw rate (+0x9c0). dB/dV is body rotation per unit")
        print("  velocity rotation: 1.0 = body and velocity turn together (no slip growth).")


if __name__ == "__main__":
    main()
