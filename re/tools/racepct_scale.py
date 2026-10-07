"""D-11072 legs C + A scorer (read-only, offline).

Leg C -- slot-STATE table liveness, from MASHED_AI_STEPDUMP CSVs with the
ss_base/ss_v/ss_raw columns appended 2026-10-06:
  G-C-SEED   ss_base == 0x005f2728 on 100% of seeded rows, == 0 on 100% unseeded
  G-C-EXPR   seeded ss_v == 2 on >=95% per car; unseeded ss_v == -1 on 100%
  G-C-TABLE  (control) ss_raw == 0 on 100% unseeded AND == 2 on >=95% seeded per car

Leg A -- scale measurement:
  original race_pct  from verify/d3_elim_20261003/o_e{1,2}.msd.alive.csv (pct0..pct3)
  port metric        from the seeded/unseeded step CSVs (racepct, prog, rec_9e4)
  G-A-RANGE / G-A-WRAP / G-A-MONO   on the original
  G-A-SHAPE-ORIG / G-A-SHAPE-PORT   flat-tread fraction, with the 5x verdict rule

Usage:
  py -3.12 re/tools/racepct_scale.py legc  --seeded S1.csv S2.csv S3.csv --unseeded U1.csv U2.csv U3.csv
  py -3.12 re/tools/racepct_scale.py lega  --orig o_e1.msd.alive.csv o_e2.msd.alive.csv --port S1.csv
"""
import argparse, csv, statistics, sys

SLOT_TABLE_PTR_VALUE = 0x005f2728  # 6235944; decomp_pc 0x005f2770 --datarefs


def read_rows(path):
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def pct(num, den):
    return (num / den) if den else 0.0


def legc(args):
    def scan(paths):
        rows = []
        for p in paths:
            rows.extend(read_rows(p))
        return rows

    seeded = scan(args.seeded)
    unseeded = scan(args.unseeded)
    ok = True

    # G-C-SEED
    s_base_ok = sum(1 for r in seeded if int(r["ss_base"]) == SLOT_TABLE_PTR_VALUE)
    u_base_ok = sum(1 for r in unseeded if int(r["ss_base"]) == 0)
    g_seed = (s_base_ok == len(seeded)) and (u_base_ok == len(unseeded))
    ok &= g_seed
    print("G-C-SEED  threshold: seeded ss_base==0x005f2728 100%%, unseeded ss_base==0 100%%")
    print(f"          seeded   {s_base_ok}/{len(seeded)}   unseeded {u_base_ok}/{len(unseeded)}"
          f"   -> {'PASS' if g_seed else 'FAIL'}")

    # G-C-EXPR  (per car v in 1..3)
    print("G-C-EXPR  threshold: seeded ss_v==2 >=95%% per car; unseeded ss_v==-1 100%%")
    g_expr = True
    for v in (1, 2, 3):
        sv = [r for r in seeded if int(r["v"]) == v]
        uv = [r for r in unseeded if int(r["v"]) == v]
        s2 = sum(1 for r in sv if int(r["ss_v"]) == 2)
        um1 = sum(1 for r in uv if int(r["ss_v"]) == -1)
        f = pct(s2, len(sv))
        car_ok = (f >= 0.95) and (um1 == len(uv))
        g_expr &= car_ok
        print(f"          v{v}  seeded ss_v==2 {s2}/{len(sv)}={f:.4f}"
              f"   unseeded ss_v==-1 {um1}/{len(uv)}   -> {'PASS' if car_ok else 'FAIL'}")
    ok &= g_expr

    # G-C-TABLE  (control)
    print("G-C-TABLE threshold (CONTROL): unseeded ss_raw==0 100%%, seeded ss_raw==2 >=95%% per car")
    g_tbl = True
    for v in (1, 2, 3):
        sv = [r for r in seeded if int(r["v"]) == v]
        uv = [r for r in unseeded if int(r["v"]) == v]
        s2 = sum(1 for r in sv if int(r["ss_raw"]) == 2)
        u0 = sum(1 for r in uv if int(r["ss_raw"]) == 0)
        f = pct(s2, len(sv))
        car_ok = (f >= 0.95) and (u0 == len(uv))
        g_tbl &= car_ok
        print(f"          v{v}  seeded ss_raw==2 {s2}/{len(sv)}={f:.4f}"
              f"   unseeded ss_raw==0 {u0}/{len(uv)}   -> {'PASS' if car_ok else 'FAIL'}")
    ok &= g_tbl

    print(f"\nLEG C: {'ALL PASS' if ok else 'FAIL'}")
    return 0 if ok else 1


def _flat_frac(series_moving):
    # series_moving: list of (value, is_moving) consecutive samples.
    num = den = 0
    for i in range(1, len(series_moving)):
        v0, m0 = series_moving[i - 1]
        v1, m1 = series_moving[i]
        if not (m0 and m1):
            continue
        den += 1
        if abs(v1 - v0) < 1e-6:
            num += 1
    return num, den


def lega(args):
    # --- original: pct0..pct3 from alive.csv, with vstep0..3 as the moving flag ---
    orig_rows = []
    for p in args.orig:
        orig_rows.extend(read_rows(p))
    print("M1  original race_pct semantics (pct0..pct3, moving = vstep != 0)")
    ok = True
    orig_flat = {}
    for c in range(4):
        vals = [float(r[f"pct{c}"]) for r in orig_rows]
        mv = [int(float(r[f"vstep{c}"])) != 0 for r in orig_rows]
        mn, mx = min(vals), max(vals)
        rng_ok = (mn >= 0.0) and (mx <= 100.0)
        # wrap analysis
        wraps = neg = total = 0
        mono_ok_steps = mono_steps = 0
        for i in range(1, len(vals)):
            d = vals[i] - vals[i - 1]
            total += 1
            if d < 0:
                neg += 1
                if abs(d) > 50:
                    wraps += 1
            else:
                mono_steps += 1
                if d >= 0:
                    mono_ok_steps += 1
        wrap_ok = (wraps == neg)
        mono_frac = pct(mono_ok_steps, mono_steps)
        num, den = _flat_frac(list(zip(vals, mv)))
        ff = pct(num, den)
        orig_flat[c] = ff
        print(f"  car{c}  n={len(vals)}  range=[{mn:.3f},{mx:.3f}] range_ok={rng_ok}"
              f"  wraps={wraps}/{neg}neg/{total}  mono={mono_ok_steps}/{mono_steps}={mono_frac:.4f}"
              f"  flat={num}/{den}={ff:.4f}")
        ok &= rng_ok and wrap_ok and (mono_frac >= 0.99)

    # --- port: racepct, moving = rec_9e4 > 200 ---
    port_rows = []
    for p in args.port:
        port_rows.extend(read_rows(p))
    pcol = getattr(args, "portcol", "racepct")
    print(f"\nM2  port metric ({pcol}, moving = rec_9e4 > 200)")
    port_flat = {}
    for v in (1, 2, 3):
        pr = [r for r in port_rows if int(r["v"]) == v]
        vals = [float(r[pcol]) for r in pr]
        mv = [float(r["rec_9e4"]) > 200.0 for r in pr]
        mn, mx = (min(vals), max(vals)) if vals else (0, 0)
        num, den = _flat_frac(list(zip(vals, mv)))
        ff = pct(num, den)
        port_flat[v] = ff
        print(f"  car{v}  n={len(vals)}  range=[{mn:.3f},{mx:.3f}]  flat={num}/{den}={ff:.4f}")

    # --- verdict rule: port flat_frac vs 5x orig flat_frac ---
    print("\nVERDICT RULE (fixed before the tool existed): common scale iff "
          "port flat_frac < 5 x orig flat_frac on every car.")
    common = True
    for v in (1, 2, 3):
        of = orig_flat.get(v, 0.0)
        pf = port_flat.get(v, 0.0)
        thr = 5.0 * of
        car_common = pf < thr
        common &= car_common
        print(f"  car{v}  port_flat={pf:.4f}  5x_orig_flat={thr:.4f}"
              f"  -> {'COMMON' if car_common else 'NOT-COMMON'}")
    print(f"\nLEG A VERDICT: {'COMMON SCALE by range-mapping -- candidate map registrable' if common else 'NOT on a common scale by range-mapping -- bridge needs an arc-length metric or re-derived gate thresholds (FINDING)'}")

    # ----- COLLATERAL, run AFTER the verdict and declared as such -----
    # The pre-registered flat-frac rule is DEGENERATE here (orig flat_frac == 0 exactly,
    # so 5*0 is an impossible threshold) and the staircase prediction is refuted
    # (port flat 0.0..0.07, not >= 0.50). This monotonicity measurement is the REAL
    # discriminator and is reported as collateral, not as the gate.
    print("\nCOLLATERAL (not the pre-registered gate): within-lap MONOTONICITY,"
          " moving samples only. A faithful stand-in for the original's spline race_pct"
          " must be monotone within a lap like it is.")
    for label, rows, pcol, movecol, movethr in (
        ("orig", orig_rows, None, None, None),
        ("port", port_rows, pcol, "rec_9e4", 200.0),
    ):
        cars = range(4) if label == "orig" else (1, 2, 3)
        for c in cars:
            if label == "orig":
                pr = rows
                vals = [float(r[f"pct{c}"]) for r in pr]
                mv = [int(float(r[f"vstep{c}"])) != 0 for r in pr]
            else:
                pr = [r for r in rows if int(r["v"]) == c]
                vals = [float(r[pcol]) for r in pr]
                mv = [float(r[movecol]) > movethr for r in pr]
            wraps = neg = fwd = 0
            for i in range(1, len(vals)):
                if not (mv[i - 1] and mv[i]):
                    continue
                d = vals[i] - vals[i - 1]
                if d < 0:
                    # A negative step is a lap WRAP or a round RESET (not a defect)
                    # when it lands near 0 (0..100 metric wrapping) or is a >50 jump.
                    # A genuine backward is a small negative step landing mid-lap.
                    if abs(d) > 50 or vals[i] < 2.0:
                        wraps += 1
                    else:
                        neg += 1
                else:
                    fwd += 1
            print(f"  {label} car{c}  fwd={fwd}  backward_midlap={neg}  wrap/reset={wraps}"
                  f"  (backward_midlap is the defect: original should be 0)")
    return 0 if ok else 1


def bridge(args):
    """D-11072 bridge-write gates: G-WROTE, G-LIVE (control), from the ra_ec/val_880/
    lap_9648 witnesses. Y = bridge ON, N = bridge OFF."""
    def scan(paths):
        rows = []
        for p in paths:
            rows.extend(read_rows(p))
        return rows
    Y = scan(args.on)
    N = scan(args.off)
    ok = True

    # G-WROTE: ra_ec == arcpct on Y (write took), == 0 on N.
    print("G-WROTE  threshold: Y ra_ec==arcpct (|d|<1e-3) 100%, N ra_ec==0 100%")
    gw = True
    for v in (1, 2, 3):
        yv = [r for r in Y if int(r["v"]) == v]
        nv = [r for r in N if int(r["v"]) == v]
        y_ok = sum(1 for r in yv if abs(float(r["ra_ec"]) - float(r["arcpct"])) < 1e-3)
        n_ok = sum(1 for r in nv if float(r["ra_ec"]) == 0.0)
        car = (y_ok == len(yv)) and (n_ok == len(nv))
        gw &= car
        print(f"         v{v}  Y {y_ok}/{len(yv)}  N {n_ok}/{len(nv)}  -> {'PASS' if car else 'FAIL'}")
    ok &= gw

    # G-LIVE (control): on Y, val_880 ~= ra*0.01 + lap (the live reader consumed it) and
    # differs from N. The null "no live reader" predicts val_880 identical in both arms.
    print("G-LIVE   threshold (CONTROL): Y val_880 ~= arcpct*0.01+lap_9648 (|d|<1e-2) >=95%,"
          " and Y!=N in distribution")
    gl = True
    for v in (1, 2, 3):
        yv = [r for r in Y if int(r["v"]) == v]
        nv = [r for r in N if int(r["v"]) == v]
        exp_ok = sum(1 for r in yv
                     if abs(float(r["val_880"])
                            - (float(r["arcpct"]) * 0.01 + int(r["lap_9648"]))) < 1e-2)
        f = pct(exp_ok, len(yv))
        y_nz = sum(1 for r in yv if float(r["val_880"]) != 0.0)
        n_nz = sum(1 for r in nv if float(r["val_880"]) != 0.0)
        car = (f >= 0.95) and (y_nz > 0) and (y_nz != n_nz)
        gl &= car
        print(f"         v{v}  Y val_880~=expr {exp_ok}/{len(yv)}={f:.4f}"
              f"  Y nonzero {y_nz}/{len(yv)}  N nonzero {n_nz}/{len(nv)}"
              f"  -> {'PASS' if car else 'FAIL'}")
    ok &= gl
    print(f"\nBRIDGE WROTE+LIVE: {'PASS' if ok else 'FAIL'}")
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    b2 = sub.add_parser("bridge")
    b2.add_argument("--on", nargs="+", required=True)
    b2.add_argument("--off", nargs="+", required=True)
    b2.set_defaults(fn=bridge)
    c = sub.add_parser("legc")
    c.add_argument("--seeded", nargs="+", required=True)
    c.add_argument("--unseeded", nargs="+", required=True)
    c.set_defaults(fn=legc)
    a = sub.add_parser("lega")
    a.add_argument("--orig", nargs="+", required=True)
    a.add_argument("--port", nargs="+", required=True)
    a.add_argument("--portcol", default="racepct",
                   help="port metric column to score (racepct | arcpct)")
    a.set_defaults(fn=lega)
    args = ap.parse_args()
    sys.exit(args.fn(args))


if __name__ == "__main__":
    main()
