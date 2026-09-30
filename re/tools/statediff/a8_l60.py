#!/usr/bin/env python3
"""A8/U-9156: reconstruct the ORIGINAL's `l_60` (and therefore `grip*speed`) from a
`--mag-probe` capture of RwV3dLength's arguments.

Registered in re/analysis/D2_REOPEN_2026-09-29.md section 21.7 BEFORE the run; the gate
amendment is section 21.8. Input is `<statediff-out>.magprobe.csv` from
`re/frida/scenario_launch.py --mag-probe <return-addresses>`.

WHY this is a measurement and not a fit. `l_60 = sum ld4 * le4` (Integrate2.cpp:464) and both
factors are the magnitude of a vector passed BY POINTER to RwV3dLength `0x004c3ac0`
(`mov eax,[esp+4]` then it squares `[eax]`/`[eax+4]`/`[eax+8]`). An ENTRY hook therefore reads
the exact vector, so the factors are observed, not inferred. This is the route section 21.7
preferred precisely because the alternative -- the `+0x70/+0x78` wheel-force route -- needs the
`lat*f5 + axis*l98` split (`:469`/`:474`) that a8_wheelfit.py has a WITHDRAWN finding on.

SITE IDENTIFICATION, from signatures read off the code rather than assumed:
  ld4 (:440) = Mag3(lac, la8, la4), built from a UNIT direction minus its projection on a
               unit axis (:432-440), so |v| lies in [0, 1].
  le4 (:425) = Mag3(le0, ldc, ld8), the wheel-point speed. The 1024 cap is applied to the
               LOCAL at :434, i.e. AFTER this call, so the observed argument is UNCAPPED and
               this tool applies `min(le4, 1024)` itself before multiplying -- which is what
               :464 accumulates.
Both are per-wheel and both sit inside conditionals (`le4`'s Mag3 only in the SPIN branch at
:421-425; `ld4` only inside the `kSpeedMin < le4 && +0x9e0 == 4.0` gate at :430), so their
per-frame counts are NOT fixed at 4 and are NOT required to be equal. This tool therefore
reports the per-frame counts and pairs only as many as both sites supply, counting every frame
where they disagree instead of silently truncating.

Frames are delimited by the once-per-frame site that returns from `:633`'s
`speed = Vec3Mag3(v + 0x9b0)`, which runs AFTER the wheel loop -- so a frame's wheel rows
precede its delimiter row. That site is also the self-check: its vector must equal the
record's own `+0x9b0..+0x9b8`.

Usage: py -3.12 re/tools/statediff/a8_l60.py <magprobe.csv> --le4-site <hex> --ld4-site <hex>
                --speed-site <hex> [--band 100 150]
Read-only. Does not execute the game.
"""
import argparse, csv, math


def med(v):
    v = sorted(v)
    return v[len(v) // 2] if v else float('nan')


def mag(r):
    return math.sqrt(r['vx'] ** 2 + r['vy'] ** 2 + r['vz'] ** 2)


K_LE4_CAP = 1024.0        # Integrate2.cpp:434  (k1024)
K_KNEE = 32768.0          # _DAT_005ce9fc, the clamp-#6 arm test @0x0046888f
K_SCALE = 3.0517578125e-05  # _DAT_005ce9f0 = 0x38000000 = 2^-15
K_FLOOR = 0.1             # _DAT_005cc56c, the lateral k floor @0x0046889b


def load(path):
    out = []
    with open(path, newline='') as f:
        for d in csv.DictReader(f):
            out.append(dict(seq=int(d['seq']), site=d['site'],
                            vx=float(d['vx']), vy=float(d['vy']), vz=float(d['vz']),
                            velx=float(d['rec_velx']), velz=float(d['rec_velz']),
                            sp=float(d['rec_speed']), gnd=float(d['gnd']),
                            m18c=float(d['rec_18c'])))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('csv')
    ap.add_argument('--le4-site', required=True)
    ap.add_argument('--ld4-site', required=True)
    ap.add_argument('--speed-site', required=True)
    ap.add_argument('--band', nargs=2, type=float, default=[100.0, 150.0])
    a = ap.parse_args()
    rows = load(a.csv)
    print(f"{a.csv}: {len(rows)} rows, {len(set(r['site'] for r in rows))} distinct sites")

    # --- self-check on the delimiter site (Gate 1) ---
    sc = [r for r in rows if r['site'] == a.speed_site]
    bad = sum(1 for r in sc
              if abs(r['vx'] - r['velx']) > 1e-6 or abs(r['vz'] - r['velz']) > 1e-6)
    print(f"GATE 1 self-check, site {a.speed_site} vs record +0x9b0/+0x9b8: "
          f"{len(sc) - bad}/{len(sc)} match, {bad} mismatched")
    if bad:
        print("  REFUSING: the return-address tagging is wrong; section 21.7 is void.")
        return
    m18c = sorted(set(round(r['m18c'], 6) for r in rows))
    print(f"rec+0x18c distinct values (live): {m18c}")

    # --- group into frames on the delimiter ---
    frames, cur = [], []
    for r in rows:
        if r['site'] == a.speed_site:
            frames.append((cur, r))
            cur = []
        else:
            cur.append(r)
    print(f"frames delimited: {len(frames)}")

    recs, mism = [], 0
    for grp, dl in frames:
        le = [min(mag(r), K_LE4_CAP) for r in grp if r['site'] == a.le4_site]
        ld = [mag(r) for r in grp if r['site'] == a.ld4_site]
        if len(le) != len(ld):
            mism += 1
        k = min(len(le), len(ld))
        if k == 0:
            continue
        l60 = sum(ld[i] * le[i] for i in range(k))
        recs.append(dict(horiz=math.hypot(dl['velx'], dl['velz']), sp=dl['sp'],
                         gnd=dl['gnd'], l60=l60, nw=k,
                         med_ld4=med(ld[:k]), med_le4=med(le[:k])))
    print(f"frames with >=1 paired wheel: {len(recs)}; "
          f"frames where the two site counts DISAGREE: {mism}")

    lo, hi = a.band
    sel = [r for r in recs if lo <= r['horiz'] < hi and r['gnd'] >= 3.5]
    print(f"\n=== band {lo:g}-{hi:g} horizontal, grounded, n={len(sel)} ===")
    if not sel:
        return
    msp = med([r['horiz'] for r in sel])
    ml60 = med([r['l60'] for r in sel])
    # grip == l_60 because +0x18c is 1.0 (measured, section 21.4 and live above)
    gs = med([r['l60'] * r['sp'] for r in sel])
    print(f"  median horizontal speed   {msp:>14.3f}")
    print(f"  median rec+0x9e4 speed    {med([r['sp'] for r in sel]):>14.3f}")
    print(f"  median paired wheels/frame{med([r['nw'] for r in sel]):>14.1f}")
    print(f"  median ld4                {med([r['med_ld4'] for r in sel]):>14.5f}")
    print(f"  median le4 (capped)       {med([r['med_le4'] for r in sel]):>14.3f}")
    print(f"  median l_60               {ml60:>14.3f}   <- HEADLINE")
    print(f"  median grip*speed         {gs:>14.1f}   <- vs the 32768 knee")
    arm = sum(1 for r in sel if r['l60'] * r['sp'] > K_KNEE)
    print(f"  frames above the knee     {arm}/{len(sel)}")
    kk = []
    for r in sel:
        g = r['l60'] * r['sp']
        kk.append(max(K_FLOOR, (K_KNEE - g) * K_SCALE) if g <= K_KNEE else None)
    lowk = [x for x in kk if x is not None]
    if lowk:
        print(f"  low-arm k implied         {med(lowk):>14.5f}   "
              f"(n={len(lowk)}; floor {K_FLOOR})")


if __name__ == '__main__':
    main()
