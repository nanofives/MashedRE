#!/usr/bin/env python3
"""A8: evaluate the PORT's angular-velocity chain (block #5 torque -> integration ->
grip-clamp #6 damping) on the ORIGINAL's record fields, one frame ahead, and compare
with the av.y (+0x9c0) the original stored on the next frame.

Transcribed from Integrate2.cpp:343-376 (block #5 + integration) and :484-575 (grip and
the #6 tail). Per-wheel ld4/le4 for grip come from a8_wheelvel_orig.py's chain (verified
this session to reproduce the stored per-wheel force within 6-10%).

Recurrence per substep (dt in the original's ms units, kMaxSubstep = 25):
    l_74     = sum over wheels of -x80 * (m15*kNormAccum)/m16          (block #5, y component)
    adt      = dt * (+0x5c) * kAngMul
    av.y    += l_74 * adt                                              (state +0x10 == 0)
    grip     = l_60 / (+0x18c)   [track-id scalings: none for +0x1f0 = -3637120]
    k        = (32768 - grip) * 3.0518e-5, floors 0.1 then 0.5;  av *= (1 - k)
The .msd is a render-tick snapshot; the number of substeps per frame is not recorded, so
1 and 2 substeps (25 ms each) are both reported. Read-only. Does not execute the game.
"""
import math, struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402
import a8_wheelvel_orig as wv  # noqa: E402

KNORMACCUM = wv.bits(0x4248f5c3); KANGMUL = wv.bits(0x35788da7)
K32768 = 32768.0; K1E7 = 1.0e7; K9P9998E8 = wv.bits(0x33d6bf95); K3P0518E5 = wv.bits(0x38000000)
K0P1 = 0.1; KHALF = 0.5; KSPEEDMIN = wv.KSPEEDMIN
WB = wv.WB


def med(v):
    v = sorted(v); return v[len(v) // 2] if v else float('nan')


def frame_terms(p, f):
    """block #5 torque accumulators (l_78,l_74,l_70) from STORED forces, and l_60 from the chain."""
    sp = f(p, 0x9e4); vel = (f(p, 0x9b0), f(p, 0x9b4), f(p, 0x9b8)); bf = (f(p, 0x9d4), f(p, 0x9d8), f(p, 0x9dc))
    av = (f(p, 0x9bc), f(p, 0x9c0), f(p, 0x9c4)); angspd = f(p, 0x9e8)
    rot = any(c < wv.KANGLO or c > wv.KANGHI for c in av)
    mm = wv.rot_matrix(av, wv.K270) if rot else None
    l = [0.0, 0.0, 0.0]; l60 = 0.0
    for w, b in enumerate(WB):
        # --- l_60 (grip input) from the block-#4 chain
        pos = (f(p, b - 0x24), f(p, b - 0x20), f(p, b - 0x1c))
        if rot and mm is not None:
            d = wv.xform(mm, pos); ff = sp * wv.K0P008
            if ff < wv.K8: ff = wv.K8
            ff = ff * angspd * wv.K0P019877 * wv.K360
            le = (d[0] * ff - vel[0], d[1] * ff - vel[1], d[2] * ff - vel[2]); le4 = math.sqrt(sum(c * c for c in le))
        else:
            le4 = sp; le = (-vel[0], -vel[1], -vel[2])
        if KSPEEDMIN < le4 and f(p, 0x9e0) == 4.0:
            inv = 1.0 / le4; s = (le[0] * inv, le[1] * inv, le[2] * inv)
            le4c = wv.K1024 if le4 > wv.K1024 else le4
            ax = (f(p, b + 0x7c), f(p, b + 0x80), f(p, b + 0x84))
            lc0 = bf[2] * s[2] + bf[0] * s[0] + bf[1] * s[1]
            lat = (s[0] - lc0 * ax[0], s[1] - lc0 * ax[1], s[2] - lc0 * ax[2])
            l60 += math.sqrt(sum(c * c for c in lat)) * le4c
        # --- block #5 from the STORED per-wheel force
        F = (f(p, b + 0x70), f(p, b + 0x74), f(p, b + 0x78))
        if not (KSPEEDMIN < math.sqrt(sum(c * c for c in F))):
            continue
        wm = f(p, b - 0x2c); inv = 1.0 / wm
        d0, d1, d2 = f(p, b - 0x24) * inv, f(p, b - 0x20) * inv, f(p, b - 0x1c) * inv
        dot = d2 * F[2] + d1 * F[1] + d0 * F[0]
        c8, c4, c0 = d0 * dot, d1 * dot, d2 * dot
        ac, a8, a4 = F[0] - c8, F[1] - c4, F[2] - c0
        x84, x80, x7c = a4 * c4 - a8 * c0, ac * c0 - a4 * c8, a8 * c8 - ac * c4
        if 0.0 < dot: x84, x80, x7c = -x84, -x80, -x7c
        m15 = math.sqrt((ac * wm) ** 2 + (a8 * wm) ** 2 + (a4 * wm) ** 2)
        m16 = math.sqrt(x84 * x84 + x80 * x80 + x7c * x7c)
        if KSPEEDMIN < m16:
            r = (m15 * KNORMACCUM) / m16
            l[0] -= x84 * r; l[1] -= x80 * r; l[2] -= x7c * r
    return l, l60, av


def damping_k(grip):
    if K32768 < grip:
        k = (K1E7 - grip) * K9P9998E8
        if k < 0.0: k = 0.0
        return 1.0 - (k * K0P1 + k * K0P1)
    k = (K32768 - grip) * K3P0518E5
    if k < K0P1: k = K0P1
    if k < KHALF: k = KHALF
    return 1.0 - k


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else 'verify/a8_steer_20260824/orig_steerR.msd'
    _, _, frames = m.load_msd(path)
    f = lambda p, o: struct.unpack_from('<f', p, o)[0]
    ks = sorted(frames)
    rows = []
    for a, b in zip(ks, ks[1:]):
        p, q = frames[a], frames[b]
        if b != a + 1 or f(p, 0x9e4) < 1500 or f(p, 0x1a8) < 33.0 or f(p, 0x9e0) < 3.5:
            continue
        l, l60, av = frame_terms(p, f)
        # grip = (l_60 / +0x18c) [track-id scalings: none for this track] * speed
        # (Integrate2.cpp:495 `grip = grip * speed`; decomp `local_60 = local_60 * fVar5`)
        grip = (l60 / f(p, 0x18c)) * f(p, 0x9e4)
        one_minus_k = damping_k(grip)
        adt25 = 25.0 * f(p, 0x5c) * KANGMUL
        pred1 = (av[1] + l[1] * adt25) * one_minus_k
        av2 = pred1
        pred2 = (av2 + l[1] * adt25) * one_minus_k
        pred50 = (av[1] + l[1] * 2 * adt25) * one_minus_k
        ss25 = l[1] * adt25 * one_minus_k / (1 - one_minus_k) if one_minus_k < 1 else float('nan')
        rows.append(dict(sp=f(p, 0x9e4), av=av[1], nxt=f(q, 0x9c0), l74=l[1], grip=grip, omk=one_minus_k, adt=adt25, p1=pred1, p2=pred2, p50=pred50, ss=ss25, ss50=2 * ss25))
    print(f"=== {path}: {len(rows)} consecutive steady pairs ===")
    print(f"  grip median {med([r['grip'] for r in rows]):.1f}  (1-k) median {med([r['omk'] for r in rows]):.4f}  adt(25ms) {med([r['adt'] for r in rows]):.3e}  l_74 median {med([r['l74'] for r in rows]):.4e}")
    print(f"  av.y observed: this frame {med([r['av'] for r in rows]):.4f}  next frame {med([r['nxt'] for r in rows]):.4f}")
    for key, lab in (('p1', '1 substep x 25ms'), ('p2', '2 substeps x 25ms'), ('p50', '1 substep x 50ms'), ('ss', 'steady-state dt=25'), ('ss50', 'steady-state dt=50')):
        pv = [r[key] for r in rows]; ratio = [r[key] / r['nxt'] for r in rows if abs(r['nxt']) > 1e-6]
        print(f"  {lab:<30} pred median {med(pv):+.4f}   pred/next median {med(ratio):.3f}  p10 {sorted(ratio)[len(ratio)//10]:.3f} p90 {sorted(ratio)[9*len(ratio)//10]:.3f}")
    print("\n  per speed band (2 substeps):")
    for lo, hi in ((1500, 1800), (1800, 2100), (2100, 2400), (2400, 2800)):
        s = [r for r in rows if lo <= r['sp'] < hi]
        if len(s) < 10: continue
        print(f"    {lo}-{hi}: n={len(s):>3} av.y {med([r['av'] for r in s]):.3f}  pred2 {med([r['p2'] for r in s]):.3f}  ratio {med([r['p2']/r['nxt'] for r in s]):.3f}  grip {med([r['grip'] for r in s]):.0f}  1-k {med([r['omk'] for r in s]):.4f}")


if __name__ == '__main__':
    main()
