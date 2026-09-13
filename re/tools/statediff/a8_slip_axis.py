#!/usr/bin/env python3
"""A8: slip angle measured against the WHEEL-AXIS heading (the basis the tire model
actually uses, p[0x1f..0x21] of a rear wheel) on both sides, per speed band, plus the
axis-vs-forward phase offset and av.y. This is the like-for-like slip: in the original's
render-tick snapshot the wheel axes are one frame older than +0x9d4 (A8 twenty-sixth
follow-up), so slip vs +0x9d4 and slip vs the axes differ by one frame of rotation.

Usage: py -3.12 re/tools/statediff/a8_slip_axis.py --orig <.msd> --port <motion_diag.log> [more --port ...]
Regime: speed >= 1500, full lock (orig steer0 >= 33 deg, port steer >= 0.9), all grounded,
port reseeds dropped and spike frames (any wheel |F| > 3x its median) +-30 excluded.
Read-only. Does not execute the game.
"""
import argparse, math, re, struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

BANDS = ((1500, 2000), (2000, 2600))


def wrap(a):
    while a > math.pi: a -= 2 * math.pi
    while a < -math.pi: a += 2 * math.pi
    return a


def med(v):
    v = sorted(v); return v[len(v) // 2] if v else float('nan')


def orig_rows(path):
    _, _, frames = m.load_msd(path); f = lambda p, o: struct.unpack_from('<f', p, o)[0]
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        if f(p, 0x9e4) < 1500 or f(p, 0x1a8) < 33.0 or f(p, 0x9e0) < 3.5: continue
        b2 = 0x1a4 + 2 * 0xc4
        fwdH = math.atan2(f(p, 0x9dc), f(p, 0x9d4)); axH = math.atan2(f(p, b2 + 0x84), f(p, b2 + 0x7c)); velH = math.atan2(f(p, 0x9b8), f(p, 0x9b0))
        out.append(dict(sp=f(p, 0x9e4), s_fwd=abs(wrap(velH - fwdH)), s_ax=abs(wrap(velH - axH)), off=wrap(axH - fwdH), avy=f(p, 0x9c0)))
    return out


RE = re.compile(r"reseed=(\d).*?steer=([-+\d.]+) gnd=([\d.]+) sp=([\d.]+) horiz=([\d.]+) velH=([-\d.]+) bodyH=([-\d.]+).*?av=\(([-\d.e+]+),([-\d.e+]+),([-\d.e+]+)\) wf=\[([^\]]*)\] wax=\[([^\]]*)\]")


def port_rows(path):
    rows = []
    for line in open(path, errors='replace'):
        mm = RE.search(line)
        if not mm: continue
        wf = [float(x) for x in mm[11].split(',')]; wax = [float(x) for x in mm[12].split(',')]
        rows.append(dict(rs=int(mm[1]), steer=float(mm[2]), gnd=float(mm[3]), sp=float(mm[5]), velH=float(mm[6]), bodyH=float(mm[7]), avy=float(mm[9]),
                         fm=[math.hypot(wf[2 * w], wf[2 * w + 1]) for w in range(4)], axH=math.atan2(wax[5], wax[4])))
    medf = [med([r['fm'][w] for r in rows if r['sp'] >= 1500]) for w in range(4)]
    bad = set()
    for k, r in enumerate(rows):
        if any(r['fm'][w] > 3 * medf[w] for w in range(4)):
            for j in range(max(0, k - 30), min(len(rows), k + 31)): bad.add(j)
    out = []
    for k, r in enumerate(rows):
        if k in bad or r['rs'] or r['sp'] < 1500 or r['steer'] < 0.9 or r['gnd'] < 3.5: continue
        out.append(dict(sp=r['sp'], s_fwd=abs(wrap(r['velH'] - r['bodyH'])), s_ax=abs(wrap(r['velH'] - r['axH'])), off=wrap(r['axH'] - r['bodyH']), avy=r['avy']))
    return out, len(bad)


def report(rows, label):
    print(f"=== {label}: n={len(rows)}  axis-minus-forward offset median {med([r['off'] for r in rows]):+.4f} rad ===")
    for lo, hi in BANDS:
        s = [r for r in rows if lo <= r['sp'] < hi]
        if len(s) < 8: print(f"  {lo}-{hi}: n={len(s)}"); continue
        print(f"  {lo}-{hi}: n={len(s):>3}  slip vs fwd {med([r['s_fwd'] for r in s]):.4f}  slip vs AXIS {med([r['s_ax'] for r in s]):.4f}  av.y {med([r['avy'] for r in s]):+.3f}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', default='verify/a8_steer_20260824/orig_steerR.msd')
    ap.add_argument('--port', action='append', default=[])
    a = ap.parse_args()
    report(orig_rows(a.orig), 'ORIGINAL ' + a.orig)
    for p in a.port:
        rows, nbad = port_rows(p)
        report(rows, f'PORT {p} (spike/reseed-excluded {nbad} rows)')


if __name__ == '__main__':
    main()
