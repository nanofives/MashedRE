#!/usr/bin/env python3
"""A8/U-9156: reduce a `--lat-bracket` capture into the per-stage lateral change.

Registered in re/analysis/D2_REOPEN_2026-09-29.md section 21.3 BEFORE the run. Input is
`<statediff-out>.latbracket.csv` from `re/frida/scenario_launch.py --lat-bracket`, whose
rows are the player record sampled at two ENTRY hooks:

    site 0  A6a entry            0x00467650
    site 1  substep-loop entry   0x004709a0   (called TWICE per frame with 25.0)

Per FUN_00470c70's decode (VehiclePhysicsRun.cpp:1003-1008) the per-frame row pattern is
therefore `0,1,1` repeating, and each frame splits into:

    I_a6a  = lat(sub1) - lat(A6a)        A6a (grip-clamp #6 is at its tail) + A6b
    I_s1   = lat(sub2) - lat(sub1)       the FIRST 25 contact substeps
    I_s2   = lat(next A6a) - lat(sub2)   the SECOND 25, plus the next frame's A4 head
                                         and A5 -- stated, not hidden

`lat = vel . right`, `right = (-fwd.z, +fwd.x)`, each sample on its OWN forward axis: the
same expression section 21.1 registered, so the numbers compose with a8_latinc.py's.

The `0,1,1` pattern is VERIFIED per frame rather than assumed, and any frame that does not
match it is counted and excluded. A run whose pattern does not hold voids section 21.3.

Usage: py -3.12 re/tools/statediff/a8_latbracket.py <latbracket.csv> [--band 70 100]
                                                   [--from-frame N] [--series N M]
Read-only. Does not execute the game.
"""
import argparse, csv, math


def med(v):
    v = sorted(v)
    return v[len(v) // 2] if v else float('nan')


def lat_of(r):
    fx, fz = r['fwdx'], r['fwdz']
    n = math.hypot(fx, fz)
    if n < 1e-12:
        return None
    return (-fz / n) * r['velx'] + (fx / n) * r['velz']


def load(path):
    rows = []
    with open(path, newline='') as f:
        for d in csv.DictReader(f):
            rows.append({k: (int(v) if k in ('seq', 'site') else float(v))
                         for k, v in d.items()})
    return rows


def frames(rows):
    """Group into frames on the verified 0,1,1 pattern. -> (frames, n_malformed)."""
    out, bad, i = [], 0, 0
    while i + 2 < len(rows) + 1:
        if i + 2 >= len(rows):
            break
        a, s1, s2 = rows[i], rows[i + 1], rows[i + 2]
        if (a['site'], s1['site'], s2['site']) != (0, 1, 1):
            bad += 1
            i += 1
            continue
        out.append((a, s1, s2))
        i += 3
    return out, bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('csv')
    ap.add_argument('--band', nargs=2, type=float, default=[70.0, 100.0])
    ap.add_argument('--from-frame', type=int, default=0)
    ap.add_argument('--series', nargs=2, type=int, default=None,
                    help='print the per-frame series for frame range [N, M)')
    a = ap.parse_args()
    rows = load(a.csv)
    fr, bad = frames(rows)
    print(f"{a.csv}: {len(rows)} samples -> {len(fr)} frames on the 0,1,1 pattern, "
          f"{bad} malformed")
    if bad:
        print("  WARNING: the 0,1,1 pattern does not hold everywhere. "
              "Section 21.3's frame-order decode is in question.")

    recs = []
    for k in range(len(fr) - 1):
        a0, s1, s2 = fr[k]
        a1 = fr[k + 1][0]
        if k < a.from_frame:
            continue
        if min(x['gnd'] for x in (a0, s1, s2, a1)) < 3.5:
            continue
        L = [lat_of(x) for x in (a0, s1, s2, a1)]
        if any(x is None for x in L):
            continue
        recs.append(dict(f=k, horiz=math.hypot(a0['velx'], a0['velz']),
                         lat=L[0], i_a6a=L[1] - L[0], i_s1=L[2] - L[1],
                         i_s2=L[3] - L[2], total=L[3] - L[0],
                         avy=a0['avy'], sp=a0['speed']))

    if a.series:
        lo, hi = a.series
        print(f"\n{'f':>5} {'horiz':>8} {'lat':>9} {'I_a6a':>9} {'I_s1':>9} "
              f"{'I_s2':>9} {'total':>9}")
        for r in recs:
            if lo <= r['f'] < hi:
                print(f"{r['f']:>5} {r['horiz']:>8.2f} {r['lat']:>+9.2f} "
                      f"{r['i_a6a']:>+9.3f} {r['i_s1']:>+9.3f} {r['i_s2']:>+9.3f} "
                      f"{r['total']:>+9.3f}")

    lo, hi = a.band
    sel = [r for r in recs if lo <= r['horiz'] < hi]
    print(f"\n=== band {lo:g}-{hi:g} horizontal, grounded, n={len(sel)} "
          f"(of {len(recs)} grounded frames) ===")
    if not sel:
        return
    L = med([abs(r['lat']) for r in sel])
    print(f"  median horiz speed        {med([r['horiz'] for r in sel]):>12.3f}")
    print(f"  median |lat|  (= L)       {L:>12.4f}")
    for key, label in (('i_a6a', 'I_a6a  [A6a + A6b]'),
                       ('i_s1', 'I_s1   [substeps 1-25]'),
                       ('i_s2', 'I_s2   [substeps 26-50 + next A4/A5]'),
                       ('total', 'TOTAL  [whole frame]')):
        v = [r[key] for r in sel]
        print(f"  median {label:<36} {med(v):>+11.4f}"
              f"   |.|/L = {abs(med(v)) / L:>7.4f}"
              f"   ({sum(1 for x in v if x > 0)} pos / {sum(1 for x in v if x < 0)} neg)")
    print(f"  median av.y               {med([r['avy'] for r in sel]):>+12.6f}")


if __name__ == '__main__':
    main()
