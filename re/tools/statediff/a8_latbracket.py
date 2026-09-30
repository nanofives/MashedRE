#!/usr/bin/env python3
"""A8/U-9156: reduce a `--lat-bracket` capture into the per-stage lateral change.

Registered in re/analysis/D2_REOPEN_2026-09-29.md section 21.3 BEFORE the run. Input is
`<statediff-out>.latbracket.csv` from `re/frida/scenario_launch.py --lat-bracket`, whose
rows are the player record sampled at two ENTRY hooks:

    site 0  A6a entry            0x00467650
    site 2  A6b entry            0x00468980
    site 1  substep-loop entry   0x004709a0   (called TWICE per frame with 25.0)

Per FUN_00470c70's decode (VehiclePhysicsRun.cpp:1003-1008) the per-frame row pattern is
therefore `0,2,1,1` repeating, and each frame splits into:

    I_a6a  = lat(A6b)  - lat(A6a)        A6a ALONE (grip-clamp #6 is at its tail)
    I_a6b  = lat(sub1) - lat(A6b)        A6b alone
    I_s1   = lat(sub2) - lat(sub1)       the FIRST 25 contact substeps
    I_s2   = lat(next A6a) - lat(sub2)   the SECOND 25, plus the next frame's A4 head
                                         and A5 -- stated, not hidden

[D2 section 21.5] A6b's site was added after section 21.4 over-claimed on the two-site
version, where `I_a6a` was really `[A6a + A6b]`. That matters because A6b also writes the
angular velocity, so the `|av|` ratio across the merged interval is NOT a clean fingerprint
of which arm clamp #6 took. `--legacy3` reduces an old two-site capture on the `0,1,1`
pattern so those numbers stay reproducible.

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


def frames(rows, pattern=(0, 2, 1, 1)):
    """Group into frames on the VERIFIED site pattern. -> (frames, n_malformed).

    The pattern is checked per frame rather than assumed; any position that does not match
    is counted as malformed and skipped, so a capture whose stage order differs shows up as
    a nonzero count instead of silently producing plausible numbers.
    """
    n = len(pattern)
    out, bad, i = [], 0, 0
    while i + n <= len(rows):
        grp = rows[i:i + n]
        if tuple(g['site'] for g in grp) != tuple(pattern):
            bad += 1
            i += 1
            continue
        out.append(grp)
        i += n
    return out, bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('csv')
    ap.add_argument('--band', nargs=2, type=float, default=[70.0, 100.0])
    ap.add_argument('--from-frame', type=int, default=0)
    ap.add_argument('--series', nargs=2, type=int, default=None,
                    help='print the per-frame series for frame range [N, M)')
    ap.add_argument('--legacy3', action='store_true',
                    help='reduce a pre-section-21.5 TWO-site capture on the 0,1,1 pattern. '
                         'Its first interval is [A6a + A6b] merged, which is exactly the '
                         'confound section 21.5 corrects -- use only to reproduce section '
                         "21.4's numbers.")
    a = ap.parse_args()
    rows = load(a.csv)
    pat = (0, 1, 1) if a.legacy3 else (0, 2, 1, 1)
    fr, bad = frames(rows, pat)
    ptxt = ','.join(str(x) for x in pat)
    print(f"{a.csv}: {len(rows)} samples -> {len(fr)} frames on the {ptxt} pattern, "
          f"{bad} malformed")
    if bad:
        print(f"  WARNING: the {ptxt} pattern does not hold everywhere. "
              "The frame-order decode is in question.")

    recs = []
    for k in range(len(fr) - 1):
        if a.legacy3:
            a0, s1, s2 = fr[k]
            b0 = None
        else:
            a0, b0, s1, s2 = fr[k]
        a1 = fr[k + 1][0]
        if k < a.from_frame:
            continue
        chain = [a0] + ([] if b0 is None else [b0]) + [s1, s2, a1]
        if min(x['gnd'] for x in chain) < 3.5:
            continue
        L = [lat_of(x) for x in chain]
        if any(x is None for x in L):
            continue
        # av magnitudes across the A6a-only interval (or the merged one on --legacy3)
        mag = lambda x: math.sqrt(x['avx'] ** 2 + x['avy'] ** 2 + x['avz'] ** 2)
        m0, m1 = mag(a0), mag(b0 if b0 is not None else s1)
        rec = dict(f=k, horiz=math.hypot(a0['velx'], a0['velz']),
                   lat=L[0], avy=a0['avy'], sp=a0['speed'],
                   avr=(m1 / m0) if m0 > 1e-12 else float('nan'),
                   total=L[-1] - L[0])
        if b0 is None:
            rec.update(i_a6a=L[1] - L[0], i_a6b=float('nan'),
                       i_s1=L[2] - L[1], i_s2=L[3] - L[2])
        else:
            rec.update(i_a6a=L[1] - L[0], i_a6b=L[2] - L[1],
                       i_s1=L[3] - L[2], i_s2=L[4] - L[3])
        # effective lateral k of the A6a-only interval, the clamp's own quantity
        rec['k_eff'] = (1.0 - (L[1] / L[0])) if abs(L[0]) > 1e-6 else float('nan')
        recs.append(rec)

    if a.series:
        lo, hi = a.series
        print(f"\n{'f':>5} {'horiz':>8} {'lat':>9} {'I_a6a':>9} {'I_a6b':>9} {'I_s1':>9} "
              f"{'I_s2':>9} {'total':>9} {'k_eff':>9} {'avr':>7}")
        for r in recs:
            if lo <= r['f'] < hi:
                print(f"{r['f']:>5} {r['horiz']:>8.2f} {r['lat']:>+9.2f} "
                      f"{r['i_a6a']:>+9.3f} {r['i_a6b']:>+9.3f} {r['i_s1']:>+9.3f} "
                      f"{r['i_s2']:>+9.3f} {r['total']:>+9.3f} {r['k_eff']:>+9.4f} "
                      f"{r['avr']:>7.3f}")

    lo, hi = a.band
    sel = [r for r in recs if lo <= r['horiz'] < hi]
    print(f"\n=== band {lo:g}-{hi:g} horizontal, grounded, n={len(sel)} "
          f"(of {len(recs)} grounded frames) ===")
    if not sel:
        return
    L = med([abs(r['lat']) for r in sel])
    print(f"  median horiz speed        {med([r['horiz'] for r in sel]):>12.3f}")
    print(f"  median |lat|  (= L)       {L:>12.4f}")
    lab = 'I_a6a  [A6a + A6b, MERGED]' if a.legacy3 else 'I_a6a  [A6a ALONE]'
    for key, label in (('i_a6a', lab),
                       ('i_a6b', 'I_a6b  [A6b alone]'),
                       ('i_s1', 'I_s1   [substeps 1-25]'),
                       ('i_s2', 'I_s2   [substeps 26-50 + next A4/A5]'),
                       ('total', 'TOTAL  [whole frame]')):
        v = [x for x in (r[key] for r in sel) if not math.isnan(x)]
        if not v:
            print(f"  median {label:<38} (no samples)")
            continue
        print(f"  median {label:<38} {med(v):>+11.4f}"
              f"   |.|/L = {abs(med(v)) / L:>7.4f}"
              f"   ({sum(1 for x in v if x > 0)} pos / {sum(1 for x in v if x < 0)} neg)")
    # The clamp's own two quantities, per section 21.5's rule.
    ke = sorted(x for x in (r['k_eff'] for r in sel) if not math.isnan(x))
    if ke:
        print(f"  k_eff across A6a: med {med(ke):>+8.5f}  p25 {ke[len(ke) // 4]:>+8.5f}"
              f"  p75 {ke[3 * len(ke) // 4]:>+8.5f}"
              f"   within 0.005 of the 0.1 FLOOR: {sum(1 for x in ke if abs(x - 0.1) < 0.005)}"
              f"/{len(ke)}   |k|<0.005: {sum(1 for x in ke if abs(x) < 0.005)}/{len(ke)}")
    av = sorted(x for x in (r['avr'] for r in sel) if not math.isnan(x))
    if av:
        print(f"  |av| ratio across A6a: med {med(av):>7.4f}"
              f"   <=0.55 (LOW arm forces this): {sum(1 for x in av if x <= 0.55)}/{len(av)}"
              f"   >=0.80: {sum(1 for x in av if x >= 0.80)}/{len(av)}")
    print(f"  median av.y               {med([r['avy'] for r in sel]):>+12.6f}")


if __name__ == '__main__':
    main()
