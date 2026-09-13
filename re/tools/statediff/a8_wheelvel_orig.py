#!/usr/bin/env python3
"""A8: evaluate the PORT's block-#4 wheel-point-velocity chain on the ORIGINAL's record
fields and compare the predicted per-wheel force with the force the original stored.

Transcribed (no re-derivation) from:
  Integrate2.cpp:286-333          block #4, rotation path + lateral/axial terms
  Math/RwMatrixRotate.cpp         axis normalise, angle_deg*pi/180, sin, 1-cos
  Math/RwMatrixRotateInner.cpp    Rodrigues fill, mode 0, translation row zeroed
  Math/RwV3dTransformPointsCPU    d = x*m[0]+y*m[4]+z*m[8], ... (+m[12..14] = 0)
Constants (bit-exact where Integrate2 marks EXACT): k0p008 0x3c03126f, k8 8.0,
k0p019877 0x3ca30eac, k360 360.0, k270 270.0, k1024 1024.0, kSpeedMin 0x38d1b717,
kAngLo/Hi -+1e-5 (0xb727c5ac/0x3727c5ac), k0p0009766, k50, k0p02 (Integrate2 header),
_DAT_0088e5f0 = 3000/(1560/360) = 692.3 (A8_suspdt_formula_20260826.md Q5).

For every steady full-lock frame of the .msd (speed >= 1500, steer >= 33, all grounded)
and every wheel:
  ld4_formula  - the lateral magnitude the port's chain computes from the ORIGINAL's fields
  F_pred       - the block-#4 force (l94 == 0 arm: lateral f5*lat + axial l98 term)
  F_rec        - the force the original stored at p[0x1c..0x1e]
Reports per wheel: median |F_pred|/|F_rec|, median angle(F_pred, F_rec), and the same
with _DAT_0088e5f0 refitted; plus ld4_formula vs |F_wlat|/lbc (the earlier proxy).
Read-only. Does not execute the game.
"""
import math, struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

def bits(u):
    return struct.unpack('<f', struct.pack('<I', u))[0]

K0P008 = bits(0x3c03126f); K8 = 8.0; K0P019877 = bits(0x3ca30eac); K360 = 360.0; K270 = 270.0
K1024 = 1024.0; KSPEEDMIN = bits(0x38d1b717); KANGLO = bits(0xb727c5ac); KANGHI = bits(0x3727c5ac)
K0P0009766 = 0.0009766; K50 = 50.0; K0P02 = 0.02; DEG2RAD = bits(0x3c8efa35)
SUSP = 3000.0 / (1560.0 * bits(0x3b360bc0))
WB = [0x1a4 + w * 0xc4 for w in range(4)]


def rot_matrix(axis, angle_deg):
    n = math.sqrt(axis[0] ** 2 + axis[1] ** 2 + axis[2] ** 2)
    if n == 0.0:
        return None
    x, y, z = axis[0] / n, axis[1] / n, axis[2] / n
    a = angle_deg * DEG2RAD
    s = math.sin(a); omc = 1.0 - math.cos(a)
    mm = [0.0] * 16
    mm[0] = 1.0 - (1.0 - x * x) * omc; mm[1] = z * s + x * y * omc;       mm[2] = z * x * omc - y * s
    mm[4] = x * y * omc - z * s;       mm[5] = 1.0 - (1.0 - y * y) * omc; mm[6] = x * s + y * z * omc
    mm[8] = y * s + z * x * omc;       mm[9] = y * z * omc - x * s;       mm[10] = 1.0 - (1.0 - z * z) * omc
    return mm


def xform(mm, p):
    x, y, z = p
    return (x * mm[0] + y * mm[4] + z * mm[8] + mm[12], x * mm[1] + y * mm[5] + z * mm[9] + mm[13], x * mm[2] + y * mm[6] + z * mm[10] + mm[14])


def med(v):
    v = sorted(v); return v[len(v) // 2] if v else float('nan')


def run(path, susp, min_speed=1500.0, steer_min=33.0, verbose=True):
    _, _, frames = m.load_msd(path)
    f = lambda p, o: struct.unpack_from('<f', p, o)[0]
    per = [dict(ratio=[], ang=[], ld4f=[], ld4proxy=[], ld4body=[], path=[], le4=[]) for _ in range(4)]
    n = 0
    for idx in sorted(frames):
        p = frames[idx]
        sp, st, g = f(p, 0x9e4), f(p, 0x1a8), f(p, 0x9e0)
        if sp < min_speed or st < steer_min or g < 3.5:
            continue
        n += 1
        vel = (f(p, 0x9b0), f(p, 0x9b4), f(p, 0x9b8)); bf = (f(p, 0x9d4), f(p, 0x9d8), f(p, 0x9dc))
        av = (f(p, 0x9bc), f(p, 0x9c0), f(p, 0x9c4)); angspd = f(p, 0x9e8)
        rot = any(c < KANGLO or c > KANGHI for c in av)
        mm = rot_matrix(av, K270) if rot else None
        for w, b in enumerate(WB):
            pos = (f(p, b - 0x24), f(p, b - 0x20), f(p, b - 0x1c))
            if rot and mm is not None:
                d = xform(mm, pos)
                ff = sp * K0P008
                if ff < K8: ff = K8
                ff = ff * angspd * K0P019877 * K360
                le = (d[0] * ff - vel[0], d[1] * ff - vel[1], d[2] * ff - vel[2])
                le4 = math.sqrt(sum(c * c for c in le))
            else:
                le4 = sp; le = (-vel[0], -vel[1], -vel[2])
            if not (KSPEEDMIN < le4):
                continue
            inv = 1.0 / le4
            s = (le[0] * inv, le[1] * inv, le[2] * inv)
            le4c = K1024 if le4 > K1024 else le4
            p15, p16, p1b = f(p, b + 0x54), f(p, b + 0x58), f(p, b + 0x6c)
            ax = (f(p, b + 0x7c), f(p, b + 0x80), f(p, b + 0x84))
            lbc = p15 * p1b * susp * le4c * K0P0009766
            lc0 = bf[2] * s[2] + bf[0] * s[0] + bf[1] * s[1]
            lc = (lc0 * ax[0], lc0 * ax[1], lc0 * ax[2])
            lat = (s[0] - lc[0], s[1] - lc[1], s[2] - lc[2])
            ld4 = math.sqrt(sum(c * c for c in lat))
            # l94 == 0 arm
            l98 = p16 * p1b * susp
            la = [lc[0] * l98, lc[1] * l98, lc[2] * l98]
            f5 = lbc
            f4 = ld4 * sp
            if f4 < K50: f5 = f4 * K0P02 * lbc
            Fp = (lat[0] * f5 + la[0], lat[1] * f5 + la[1], lat[2] * f5 + la[2])
            Fr = (f(p, b + 0x70), f(p, b + 0x74), f(p, b + 0x78))
            np_, nr = math.sqrt(sum(c * c for c in Fp)), math.sqrt(sum(c * c for c in Fr))
            if nr < 1: continue
            cosang = sum(a * c for a, c in zip(Fp, Fr)) / (np_ * nr)
            per[w]['ratio'].append(np_ / nr); per[w]['ang'].append(math.degrees(math.acos(max(-1, min(1, cosang)))))
            per[w]['ld4f'].append(ld4); per[w]['le4'].append(le4); per[w]['path'].append(rot)
            # proxies used in follow-up 25
            u = tuple(c / math.sqrt(sum(x * x for x in vel)) for c in vel); lc0b = sum(a * c for a, c in zip(u, bf))
            latb = tuple(u[i] - lc0b * ax[i] for i in range(3)); per[w]['ld4body'].append(math.sqrt(sum(c * c for c in latb)))
            axn = math.hypot(ax[0], ax[2]); perp = (-ax[2] / axn, ax[0] / axn)
            per[w]['ld4proxy'].append(abs(Fr[0] * perp[0] + Fr[2] * perp[1]) / lbc)
    if verbose:
        print(f"=== {path}: {n} steady frames, susp={susp:.1f} ===")
        print(f"  {'wheel':<6} {'n':>4} {'|Fpred|/|Frec|':>15} {'angle deg':>10} {'ld4_formula':>12} {'ld4_proxy':>10} {'ld4_body':>9} {'le4 raw':>9} {'rot path':>9}")
        for w in range(4):
            d = per[w]
            print(f"  w{w:<5} {len(d['ratio']):>4} {med(d['ratio']):>15.3f} {med(d['ang']):>10.2f} {med(d['ld4f']):>12.3f} {med(d['ld4proxy']):>10.3f} {med(d['ld4body']):>9.3f} {med(d['le4']):>9.0f} {sum(d['path'])/max(1,len(d['path'])):>9.2f}")
    return per


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else 'verify/a8_steer_20260824/orig_steerR.msd'
    per = run(path, SUSP)
    # refit susp from the median ratio of all four wheels
    r = med([x for w in range(4) for x in per[w]['ratio']])
    if r and r == r:
        print(f"\n  overall median |Fpred|/|Frec| = {r:.3f}  ->  refit _DAT_0088e5f0 = {SUSP / r:.1f}")
        run(path, SUSP / r)


if __name__ == '__main__':
    main()
