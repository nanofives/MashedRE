#!/usr/bin/env python3
"""A8: run the SAME angular-velocity chain (a8_angvel_orig.py) on the PORT's logged
per-wheel forces and per-wheel ld4/le4, and solve for the effective damping input.

Port inputs (motion_diag.log with the [A8-ORIENT] fields): wf= per-wheel force X/Z
(Y assumed 0: the original's Y components are 0-2.4k against 15-80k), wle4/wld4 per
wheel (-> l_60), av=(x,y,z), sp. Lever arms p[-9..-7] and p[-0xb] and +0x5c are NOT
logged; by default the ORIGINAL's record values for the same car (car 0, from
orig_steerR.msd) are used, and that assumption is printed. --k18c overrides the
+0x18c divisor (default 1.0 = the original's value).

Prints pred/next like the original-side tool; if the port's own av is reproduced only
with a different +0x18c or lever set, that names the input that differs.
"""
import argparse, math, re, struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402
import a8_angvel_orig as ao  # noqa: E402

RE = re.compile(r"reseed=(\d).*?steer=([-+\d.]+) gnd=([\d.]+) sp=([\d.]+) horiz=([\d.]+) velH=([-\d.]+) bodyH=([-\d.]+).*?av=\(([-\d.e+]+),([-\d.e+]+),([-\d.e+]+)\) wf=\[([^\]]*)\] wax=\[([^\]]*)\] wle4=\[([^\]]*)\] wld4=\[([^\]]*)\]")


def med(v):
    v = sorted(v); return v[len(v) // 2] if v else float('nan')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--port', default='verify/a8_orient_20260913/motion_diag.log')
    ap.add_argument('--orig', default='verify/a8_steer_20260824/orig_steerR.msd')
    ap.add_argument('--k18c', type=float, default=1.0)
    ap.add_argument('--k5c', type=float, default=None)
    ap.add_argument('--dt', type=float, default=50.0)
    a = ap.parse_args()
    # lever arms / +0x5c from the original's record (median frame)
    _, _, frames = m.load_msd(a.orig)
    f = lambda p, o: struct.unpack_from('<f', p, o)[0]
    # The original's lever arms p[-9..-7] are WORLD-space (they rotate with the body):
    # de-rotate each frame by the record's heading atan2(+0x9dc, +0x9d4) and take the
    # median body-local offset per wheel; the port's lever for a frame is that offset
    # rotated by its logged bodyH.
    loc = [[] for _ in range(4)]; pmb = [[] for _ in range(4)]
    for idx in sorted(frames):
        p = frames[idx]
        if f(p, 0x9e4) < 1500: continue
        h = math.atan2(f(p, 0x9dc), f(p, 0x9d4)); c, s = math.cos(h), math.sin(h)
        for w, b in enumerate(ao.WB):
            x, y, z = f(p, b - 0x24), f(p, b - 0x20), f(p, b - 0x1c)
            loc[w].append((x * c + z * s, y, -x * s + z * c)); pmb[w].append(f(p, b - 0x2c))
    local = [(med([l[0] for l in loc[w]]), med([l[1] for l in loc[w]]), med([l[2] for l in loc[w]])) for w in range(4)]
    wmass = [med(pmb[w]) for w in range(4)]
    p = frames[sorted(frames)[len(frames) // 2]]
    k5c = a.k5c if a.k5c is not None else f(p, 0x5c)
    print(f"ASSUMED from the original's record: body-local lever offsets = {[tuple(round(c,3) for c in l) for l in local]}, p[-0xb] = {[round(x,4) for x in wmass]}, +0x5c = {k5c:.6e}; +0x18c = {a.k18c}; dt = {a.dt}")

    def lever_at(bodyH):
        c, s = math.cos(bodyH), math.sin(bodyH)
        return [((lx * c - lz * s, ly, lx * s + lz * c), wm) for (lx, ly, lz), wm in zip(local, wmass)]
    rows = []
    for line in open(a.port, errors='replace'):
        mm = RE.search(line)
        if not mm:
            continue
        rows.append(dict(rs=int(mm[1]), steer=float(mm[2]), gnd=float(mm[3]), sp=float(mm[4]), bodyH=float(mm[7]), av=(float(mm[8]), float(mm[9]), float(mm[10])),
                         wf=[float(x) for x in mm[11].split(',')], wle4=[float(x) for x in mm[13].split(',')], wld4=[float(x) for x in mm[14].split(',')]))
    out = []
    for r, q in zip(rows, rows[1:]):
        if q['rs'] or r['sp'] < 1500 or r['steer'] < 0.9 or r['gnd'] < 3.5:
            continue
        l = [0.0, 0.0, 0.0]; l60 = 0.0
        lever = lever_at(r['bodyH'])
        for w in range(4):
            F = (r['wf'][2 * w], 0.0, r['wf'][2 * w + 1])
            l60 += r['wld4'][w] * min(r['wle4'][w], 1024.0)
            if not (ao.KSPEEDMIN < math.hypot(F[0], F[2])):
                continue
            (d0, d1, d2), wm = lever[w]
            inv = 1.0 / wm; d0, d1, d2 = d0 * inv, d1 * inv, d2 * inv
            dot = d2 * F[2] + d1 * F[1] + d0 * F[0]
            c8, c4, c0 = d0 * dot, d1 * dot, d2 * dot
            ac, a8, a4 = F[0] - c8, F[1] - c4, F[2] - c0
            x84, x80, x7c = a4 * c4 - a8 * c0, ac * c0 - a4 * c8, a8 * c8 - ac * c4
            if 0.0 < dot: x84, x80, x7c = -x84, -x80, -x7c
            m15 = math.sqrt((ac * wm) ** 2 + (a8 * wm) ** 2 + (a4 * wm) ** 2)
            m16 = math.sqrt(x84 * x84 + x80 * x80 + x7c * x7c)
            if ao.KSPEEDMIN < m16:
                rr = (m15 * ao.KNORMACCUM) / m16
                l[0] -= x84 * rr; l[1] -= x80 * rr; l[2] -= x7c * rr
        grip = (l60 / a.k18c) * r['sp']
        omk = ao.damping_k(grip)
        adt = a.dt * k5c * ao.KANGMUL
        pred = (r['av'][1] + l[1] * adt) * omk
        # effective (1-k) that would reproduce the observed next av
        eff = q['av'][1] / (r['av'][1] + l[1] * adt) if abs(r['av'][1] + l[1] * adt) > 1e-9 else float('nan')
        out.append(dict(sp=r['sp'], av=r['av'][1], nxt=q['av'][1], l74=l[1], grip=grip, omk=omk, pred=pred, eff=eff, l60=l60))
    print(f"PORT: {len(out)} consecutive steady pairs;  l_60 median {med([o['l60'] for o in out]):.0f}  grip median {med([o['grip'] for o in out]):.3e}  (1-k) {med([o['omk'] for o in out]):.4f}  l_74 median {med([o['l74'] for o in out]):.4e}")
    print(f"  av.y this {med([o['av'] for o in out]):.4f} next {med([o['nxt'] for o in out]):.4f}  pred {med([o['pred'] for o in out]):.4f}  pred/next median {med([o['pred']/o['nxt'] for o in out if abs(o['nxt'])>1e-6]):.3f}")
    e = [o['eff'] for o in out if o['eff'] == o['eff'] and 0 < o['eff'] < 1.5]
    print(f"  effective (1-k) implied by the port's own av evolution: median {med(e):.4f}  (chain says {med([o['omk'] for o in out]):.4f})")
    for lo, hi in ((1500, 1800), (1800, 2100), (2100, 2400)):
        s = [o for o in out if lo <= o['sp'] < hi]
        if len(s) < 10: continue
        print(f"    {lo}-{hi}: n={len(s):>3} av.y {med([o['av'] for o in s]):.3f} pred {med([o['pred'] for o in s]):.3f} ratio {med([o['pred']/o['nxt'] for o in s if abs(o['nxt'])>1e-6]):.3f} grip {med([o['grip'] for o in s]):.2e} 1-k {med([o['omk'] for o in s]):.4f} eff {med([o['eff'] for o in s if 0<o['eff']<1.5]):.4f}")


if __name__ == '__main__':
    main()
