#!/usr/bin/env python3
"""Diff the verbatim race-camera port against the original, frame by frame.

Inputs
------
  log/camera_trace_v2.csv   the ORIGINAL's per-frame camera state and its own inputs
                            (re/frida/camera_probe.py)
  log/cam_driver/port_out.csv  the PORT's outputs, produced by driving RaceCamera with
                            those same inputs (re/tools/camera/cam_driver.cpp)

Comparison, and why it is not a naive column diff
-------------------------------------------------
The two sides store the aim field differently, and this is MEASURED, not assumed:

  ORIGINAL  +0x4c..+0x54 is the aim DIRECTION. Deriving elev/azim from it reproduces
            the recorded +0x34/+0x38 to 0.0000 deg (azim) / 0.002 deg (elev); deriving
            them from (tgt - pos) is off by up to 60 deg / 22 deg.
  PORT      tgt_out_ is the look-at POINT (RaceCamera.cpp:404, `tgt` from `mid`).

So the comparable quantity is the LOOK-AT POINT: port `(pdx,pdy,pdz)` against original
`pos + dir`. Comparing the raw columns would report a huge false divergence.

Warm-up
-------
The port starts with `primed_ = false`, so its first Update takes the SNAP path while
the original is mid-race with converged springs (RaceCamera.cpp:335-367). Early rows
are therefore expected to differ. `--warmup N` drops the first N rows; the report also
shows the error trend so convergence (or its absence) is visible rather than assumed.
"""
import argparse
import csv
import math


def load(path):
    with open(path) as f:
        return list(csv.DictReader(f))


def pct(v, q):
    v = sorted(v)
    if not v:
        return float('nan')
    return v[min(len(v) - 1, int(q * len(v)))]


def stats(name, v, unit=''):
    if not v:
        print(f'  {name:<26} (no samples)')
        return
    print(f'  {name:<26} median {pct(v,.5):9.4f}{unit}  p90 {pct(v,.9):9.4f}{unit}'
          f'  max {max(v):9.4f}{unit}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', default='log/camera_trace_v2.csv')
    ap.add_argument('--port', default='log/cam_driver/port_out.csv')
    ap.add_argument('--warmup', type=int, default=60,
                    help='rows to drop while the port spring converges (default 60)')
    a = ap.parse_args()

    o = load(a.orig)
    p = load(a.port)
    if len(o) != len(p):
        print(f'ROW COUNT MISMATCH: orig {len(o)} vs port {len(p)} — refusing to diff')
        return 2
    f = float
    print(f'rows {len(o)}, dropping first {a.warmup} as port spring warm-up\n')

    # regime guard: the port models the STANDARD path only
    modes = {int(f(r['mode'])) for r in o}
    if modes != {0}:
        print(f'*** mode took values {sorted(modes)}; the port implements the standard '
              f'path only. Rows with mode != 0 are NOT comparable.\n')

    dpos, dpt, dzoom, dang = [], [], [], []
    pair_same = pair_swap = pair_diff = 0

    for i, (ro, rp) in enumerate(zip(o, p)):
        if i < a.warmup:
            continue
        opx, opy, opz = f(ro['px']), f(ro['py']), f(ro['pz'])
        odx, ody, odz = f(ro['dx']), f(ro['dy']), f(ro['dz'])
        # the original's look-at POINT
        oax, oay, oaz = opx + odx, opy + ody, opz + odz

        ppx, ppy, ppz = f(rp['ppx']), f(rp['ppy']), f(rp['ppz'])
        pax, pay, paz = f(rp['pdx']), f(rp['pdy']), f(rp['pdz'])

        dpos.append(math.dist((opx, opy, opz), (ppx, ppy, ppz)))
        dpt.append(math.dist((oax, oay, oaz), (pax, pay, paz)))
        dzoom.append(abs(f(ro['zoom']) - f(rp['pzoom'])))

        # aim-direction angle between the two sides, in degrees
        pdx, pdy, pdz = pax - ppx, pay - ppy, paz - ppz
        no = math.sqrt(odx*odx + ody*ody + odz*odz)
        np_ = math.sqrt(pdx*pdx + pdy*pdy + pdz*pdz)
        if no > 1e-6 and np_ > 1e-6:
            c = (odx*pdx + ody*pdy + odz*pdz) / (no * np_)
            dang.append(math.degrees(math.acos(max(-1.0, min(1.0, c)))))

        oa, ob = int(f(ro['pairA'])), int(f(ro['pairB']))
        qa, qb = int(f(rp['ppairA'])), int(f(rp['ppairB']))
        if (oa, ob) == (qa, qb):
            pair_same += 1
        elif (oa, ob) == (qb, qa):
            pair_swap += 1
        else:
            pair_diff += 1

    print('=== POSE ERROR, port vs original ===')
    stats('|eye - eye|', dpos)
    stats('|lookat - lookat|', dpt)
    stats('aim angle between sides', dang, ' deg')
    stats('|zoom - zoom|', dzoom)

    n = pair_same + pair_swap + pair_diff
    print('\n=== MOST-SEPARATED PAIR (FUN_0040e180, the cluster\'s only C4) ===')
    if n:
        print(f'  exact match      {pair_same:5d}  ({100.0*pair_same/n:5.1f}%)')
        print(f'  same pair, swapped order {pair_swap:5d}  ({100.0*pair_swap/n:5.1f}%)')
        print(f'  different pair   {pair_diff:5d}  ({100.0*pair_diff/n:5.1f}%)')

    print('\n=== TREND (is the port converging, or drifting?) ===')
    seg = max(1, len(dpos) // 5)
    for k in range(5):
        s = dpos[k*seg:(k+1)*seg]
        t = dpt[k*seg:(k+1)*seg]
        if s:
            print(f'  fifth {k+1}: |eye| median {pct(s,.5):8.3f}   '
                  f'|lookat| median {pct(t,.5):8.3f}')

    # scale reference so the numbers above can be judged
    sp = []
    for ro in o[a.warmup:]:
        cx = [f(ro[f'c{i}x']) for i in range(4)]
        cz = [f(ro[f'c{i}z']) for i in range(4)]
        sp.append(max(math.hypot(cx[i]-cx[j], cz[i]-cz[j])
                      for i in range(4) for j in range(i+1, 4)))
    print('\n=== SCALE REFERENCE ===')
    stats('max car separation', sp)
    ey = [f(r['py']) for r in o[a.warmup:]]
    stats('original eye height', ey)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
