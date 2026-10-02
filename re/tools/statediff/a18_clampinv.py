#!/usr/bin/env python3
"""a18_clampinv.py - D2 attempt 18, STEP 1B: split `T_post` by INVERTING grip-clamp #6's own
arithmetic, offline, on both sides.

Registered in verify/d2_budget_20261002/PREREG_STEP1B.md BEFORE this file was run.

CLAMP #6, transcribed byte-exact from original/MASHED.exe.unpatched this session
(`py -3.12 re/tools/disasm_va.py 0x468730 0xd0 original/MASHED.exe.unpatched`):

    0x0046874c  fld   [esp+0x20]              the post-W1 speed (frame -208, section 25.3)
    0x00468750  fcomp [0x005d757c]            vs 0.0
    0x00468758  test  ah, 0x44                C2|C3
    0x0046875b  jnp   0x00468970              TAKEN iff speed == 0.0  -> skip the clamp
    0x00468761  cmp   [esi+0x9e0], 0x40800000 grounded == 4.0 ?
    0x0046876b  jne   0x00468970              not all four grounded  -> skip the clamp
    0x00468771  fld [esi+0x9d8] / fmul [esi+0x9b4]      dot = fwd.y*vel.y
    0x0046877d  fld [esi+0x9d4] / fmul [edi]            + fwd.x*vel.x   (EDI = ESI+0x9b0)
    0x00468787  fld [esi+0x9dc] / fmul [esi+0x9b8]      + fwd.z*vel.z
    0x00468795..0x004687d7                              lat = vel - dot*fwd -> [esp+0x10/14/18]
    0x004687db  fmul  [esp+0x20]              G = grip * speed   (grip live in ST0 since 0x004686be)
    0x004687df  fcom  [0x005ce9fc]            vs 32768.0
    0x004687ea  jne   0x0046888f              -> LOW arm
    0x004687f0  fsubr [0x005ce9f8] / fmul [0x005ce9f4]   HIGH arm

Section 26.1: both arms write `vel -= k*lat`, `lat` orthogonal to `fwd`. So with
`s = |lat|/|v|` at the clamp's ENTRY and `a = 1-k`:

    R  = |v'|/|v|   = sqrt(1 - s^2 (2k - k^2))
    s' = |lat'|/|v'| = (1-k) s / R

which inverts in closed form:

    a^2 = s'^2 R^2 / (1 + s'^2 R^2 - R^2)      k = 1 - a      s = s' R / a

Both measurables come from the snapshot alone on both sides: `R = s_post/s_mid`
(`s_mid = +0x9e4` is stored at 0x004686cc immediately BEFORE the clamp; `s_post =
|+0x9b0..b8|` is the snapshot) and `s'` is built from the snapshot's own `vel`/`fwd` by the
original's own expression at 0x00468771..0x004687d7, in the original's x87 association
order.

CONDITIONING (PREREG section 4.1): ill-conditioned as R -> 1. The ORIGINAL's R is 1 - 2e-5,
so its k is reported ONLY as a band with its sensitivity, and the primary reported original
quantity is the EXACT product bound `s^2(2k-k^2) = 1 - R^2`, which needs no inversion and is
reported as a bound on the PRODUCT only.

Usage:
  py -3.12 re/tools/statediff/a18_clampinv.py --orig <msd> --port <motion_diag.log>
      [--orig-release 890] [--port-release 1] [--csv <out.csv>]
"""
import argparse
import math
import os
import statistics
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a18_budget as b18         # noqa: E402  loaders, WINDOWS

# PREREG section 2, gate KA-R2
KAR2_D = (0, 400)
KAR2_FIXUP_RATIO = 1.02
KAR2_FIXUP_MAX_FRAC = 0.05
KAR2_RANGE = (0.9990, 1.0010)
KAR2_MED = (0.9999, 1.0001)
KAR2_FRAC = 0.99
# PREREG section 4.2, gate INV
INV_ROUNDTRIP = 1e-6
INV_FRAC = 0.99
PORT_K_PRIOR = 0.540954          # section 26.3, measured from the port's own act.l60
ORIG_K_FLOOR = 0.1               # section 26.2, the LOW arm's floor
PRIOR_BAND = (100.0, 150.0)      # the band those priors were measured in
# PREREG section 4.3
DISC_S_FACTOR = 1.5
DISC_K_FACTOR = 2.0
# PREREG section 5
U9173_S_PRED = 0.05
# PREREG section 3 (STEP 1): the port's sp=%.2f floor on R
PORT_R_FLOOR = 1.5e-5


def med(xs):
    return statistics.median(xs) if xs else float("nan")


def q(xs, p):
    if not xs:
        return float("nan")
    ys = sorted(xs)
    i = min(len(ys) - 1, max(0, int(round(p * (len(ys) - 1)))))
    return ys[i]


def lat_original_order(fwd, vel):
    """lat = vel - dot(fwd,vel)*fwd, dot in the ORIGINAL's x87 order
    (0x00468771..0x00468793: (fwd.y*vel.y + fwd.x*vel.x) + fwd.z*vel.z)."""
    dot = (fwd[1] * vel[1] + fwd[0] * vel[0]) + fwd[2] * vel[2]
    return (vel[0] - dot * fwd[0], vel[1] - dot * fwd[1], vel[2] - dot * fwd[2]), dot


def invert(R, sp):
    """-> (k, s, R_roundtrip) or None if the system has no real solution here."""
    den = 1.0 + sp * sp * R * R - R * R
    if den <= 0.0:
        return None
    a2 = sp * sp * R * R / den
    if not (0.0 < a2 <= 1.0 + 1e-12):
        return None
    a = math.sqrt(min(a2, 1.0))
    if a <= 0.0:
        return None
    k = 1.0 - a
    s = sp * R / a
    if s > 1.0 + 1e-9:
        return None
    inner = 1.0 - s * s * (2.0 * k - k * k)
    if inner < 0.0:
        return None
    return k, s, math.sqrt(inner)


def enrich(rows, release):
    """-> {d: dict} with R, s' and the inversion, from the snapshot alone."""
    out = {}
    for r in rows:
        vel, fwd, sm, sp_ = r["vel"], r["fwd"], r["s_mid"], r["s_post"]
        if not all(math.isfinite(x) for x in list(vel) + list(fwd) + [sm, sp_]):
            continue
        if sp_ <= 1.0 or sm <= 1.0:
            continue
        lat, dot = lat_original_order(fwd, vel)
        slat = b18.mag3(lat)
        spost = slat / sp_
        R = sp_ / sm
        inv = invert(R, spost)
        d = dict(d=r["frame"] - release, frame=r["frame"], R=R, s_post=spost,
                 prod=1.0 - R * R, speed=sm, gnd=r["gnd"],
                 fwdlen=b18.mag3(fwd), cos=(dot / sp_ if sp_ else float("nan")))
        if inv:
            d["k"], d["s_pre"], d["R_rt"] = inv
            d["rt_err"] = abs(inv[2] - R) / max(abs(R), 1e-30)
        else:
            d["k"] = d["s_pre"] = d["R_rt"] = d["rt_err"] = float("nan")
        out[d["d"]] = d
    return out


# ------------------------------------------------------------------ gate KA-R2

def kar2(en, label, is_gate):
    sel = [r for d, r in en.items() if KAR2_D[0] <= d <= KAR2_D[1]]
    rat = [1.0 / r["R"] for r in sel]          # s_mid/s_post, the published orientation
    fix = [x for x in rat if x > KAR2_FIXUP_RATIO]
    comp = [x for x in rat if x <= KAR2_FIXUP_RATIO]
    ffrac = (len(fix) / len(rat)) if rat else float("nan")
    inr = sum(1 for x in comp if KAR2_RANGE[0] <= x <= KAR2_RANGE[1])
    cfrac = (inr / len(comp)) if comp else 0.0
    mm = med(comp)
    l1 = ffrac <= KAR2_FIXUP_MAX_FRAC
    l2 = cfrac >= KAR2_FRAC
    l3 = KAR2_MED[0] <= mm <= KAR2_MED[1]
    print("  %s: n=%d  fixup pop (ratio > %.2f) %d = %.2f%%  [leg1 bar <= %.0f%%] %s"
          % (label, len(rat), KAR2_FIXUP_RATIO, len(fix), 100.0 * ffrac,
             100.0 * KAR2_FIXUP_MAX_FRAC, "PASS" if l1 else "FAIL"))
    print("      complement n=%d  in [%.4f,%.4f] %d = %.2f%%  [leg2 bar >= %.0f%%] %s"
          % (len(comp), KAR2_RANGE[0], KAR2_RANGE[1], inr, 100.0 * cfrac,
             100.0 * KAR2_FRAC, "PASS" if l2 else "FAIL"))
    print("      complement median %.9f  p25 %.9f  p75 %.9f  [leg3 in [%.4f,%.4f]] %s"
          % (mm, q(comp, 0.25), q(comp, 0.75), KAR2_MED[0], KAR2_MED[1],
             "PASS" if l3 else "FAIL"))
    ok = l1 and l2 and l3
    print("      KA-R2 %s%s" % ("PASS" if ok else "FAIL",
                                "" if is_gate else "   (REPORTED, not a gate -- this side is"
                                                   " the thing under test)"))
    return dict(ok=ok, n=len(rat), fixfrac=ffrac, cfrac=cfrac, med=mm, ncomp=len(comp))


# ------------------------------------------------------------------ gate INV

def inv_gate(en, label):
    sel = [r for r in en.values() if math.isfinite(r["rt_err"])]
    nsol = len(sel)
    ntot = sum(1 for r in en.values())
    good = sum(1 for r in sel if r["rt_err"] <= INV_ROUNDTRIP)
    frac = (good / nsol) if nsol else 0.0
    ok = nsol > 0 and frac >= INV_FRAC
    print("  %s: solved %d/%d  round-trip <= %.0e on %d/%d = %.2f%%  worst %.3e   %s"
          % (label, nsol, ntot, INV_ROUNDTRIP, good, nsol, 100.0 * frac,
             max((r["rt_err"] for r in sel), default=float("nan")),
             "PASS" if ok else "FAIL"))
    return dict(ok=ok, nsol=nsol, ntot=ntot, frac=frac)


def band_k(en, lo, hi, label):
    """k over a SPEED band, for comparison with section 26.3's banded priors."""
    sel = [r for r in en.values() if lo <= r["speed"] <= hi and math.isfinite(r["k"])]
    if not sel:
        print("      %s band %g-%g: n=0" % (label, lo, hi))
        return None
    ks = [r["k"] for r in sel]
    print("      %s band %g-%g: n=%d med speed %.1f  k med %.6f  p10 %.6f  p90 %.6f  "
          "s_pre med %.6f  s' med %.6f  1-R med %.3e"
          % (label, lo, hi, len(sel), med([r["speed"] for r in sel]), med(ks),
             q(ks, 0.10), q(ks, 0.90), med([r["s_pre"] for r in sel]),
             med([r["s_post"] for r in sel]), med([1.0 - r["R"] for r in sel])))
    return dict(n=len(sel), kmed=med(ks), k10=q(ks, 0.10), k90=q(ks, 0.90),
                s_pre=med([r["s_pre"] for r in sel]), s_post=med([r["s_post"] for r in sel]))


def window(eo, ep, lo, hi, tag):
    o = [r for d, r in sorted(eo.items()) if lo <= d <= hi]
    p = [r for d, r in sorted(ep.items()) if lo <= d <= hi]
    if not o or not p:
        print("\n  === d %d..%d (%s) ===  ORIG n=%d PORT n=%d -- NOT COMPARABLE"
              % (lo, hi, tag, len(o), len(p)))
        return None
    print("\n  === d %d..%d (%s) ===  ORIG n=%d med speed %.1f | PORT n=%d med speed %.1f"
          % (lo, hi, tag, len(o), med([r["speed"] for r in o]),
             len(p), med([r["speed"] for r in p])))
    rows = {}
    for nm, ss in (("ORIG", o), ("PORT", p)):
        ksol = [r for r in ss if math.isfinite(r["k"])]
        rows[nm] = dict(
            n=len(ss), nsol=len(ksol), speed=med([r["speed"] for r in ss]),
            R=med([r["R"] for r in ss]), oneR=med([1.0 - r["R"] for r in ss]),
            prod=med([r["prod"] for r in ss]), spost=med([r["s_post"] for r in ss]),
            k=med([r["k"] for r in ksol]), k10=q([r["k"] for r in ksol], 0.10),
            k90=q([r["k"] for r in ksol], 0.90),
            spre=med([r["s_pre"] for r in ksol]),
            spre10=q([r["s_pre"] for r in ksol], 0.10),
            spre90=q([r["s_pre"] for r in ksol], 0.90),
            cos=med([r["cos"] for r in ss]), fwdlen=med([r["fwdlen"] for r in ss]))
    print("      %-5s %4s %4s %10s %11s %11s %10s %10s %10s %9s"
          % ("side", "n", "sol", "med speed", "1-R", "1-R^2", "s' med", "k med",
             "s_pre med", "cos med"))
    for nm in ("ORIG", "PORT"):
        r = rows[nm]
        print("      %-5s %4d %4d %10.1f %11.4e %11.4e %10.6f %10.6f %10.6f %9.6f"
              % (nm, r["n"], r["nsol"], r["speed"], r["oneR"], r["prod"], r["spost"],
                 r["k"], r["spre"], r["cos"]))
    print("      ORIG k band p10..p90 %.6f .. %.6f   s_pre band %.6f .. %.6f"
          % (rows["ORIG"]["k10"], rows["ORIG"]["k90"],
             rows["ORIG"]["spre10"], rows["ORIG"]["spre90"]))
    print("      PORT k band p10..p90 %.6f .. %.6f   s_pre band %.6f .. %.6f"
          % (rows["PORT"]["k10"], rows["PORT"]["k90"],
             rows["PORT"]["spre10"], rows["PORT"]["spre90"]))
    print("      [ORIG 1-R %.3e vs the port's sp quantization floor %.1e -- the ORIGINAL's R"
          % (rows["ORIG"]["oneR"], PORT_R_FLOOR))
    print("       is full-float .msd, the PORT's 1-R %.4e is %.0fx its own floor]"
          % (rows["PORT"]["oneR"], rows["PORT"]["oneR"] / PORT_R_FLOOR))
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--orig-release", type=int, default=890)
    ap.add_argument("--port-release", type=int, default=1)
    ap.add_argument("--csv")
    a = ap.parse_args()

    orows, prows = b18.load_orig(a.orig), b18.load_port(a.port)
    eo = enrich(orows, a.orig_release)
    ep = enrich(prows, a.port_release)
    print("a18_clampinv  clamp #6 0x004687f0..0x0046897b, inversion of section 26.1's law")
    print("ORIG %s  rows %d -> scored %d" % (a.orig, len(orows), len(eo)))
    print("PORT %s  rows %d -> scored %d" % (a.port, len(prows), len(ep)))

    print("\n--- GATE KA-R2 (PREREG_STEP1B section 2) ---")
    g_o = kar2(eo, "ORIGINAL  [THE GATE]", True)
    g_p = kar2(ep, "PORT      [reported]", False)
    print("      published priors, section 23.2: ORIG median 0.999998, 23/1447 = 1.59%% > 1.02")
    print("                                      PORT median 1.172734, 1324/1628 = 81.3%% > 1.02")

    print("\n--- GATE INV leg 1 (round-trip) ---")
    i_o = inv_gate(eo, "ORIGINAL")
    i_p = inv_gate(ep, "PORT    ")

    print("\n--- GATE INV legs 2 and 3 (published priors, band %g-%g) ---"
          % PRIOR_BAND)
    bo = band_k(eo, PRIOR_BAND[0], PRIOR_BAND[1], "ORIG")
    bp = band_k(ep, PRIOR_BAND[0], PRIOR_BAND[1], "PORT")
    leg2 = leg3 = None
    if bp:
        rat = bp["kmed"] / PORT_K_PRIOR if PORT_K_PRIOR else float("nan")
        leg2 = (1.0 / DISC_K_FACTOR) <= rat <= DISC_K_FACTOR
        print("      leg2 PORT k %.6f vs section 26.3's %.6f -> ratio %.4f  (bar: within a"
              " factor %.0f)  %s" % (bp["kmed"], PORT_K_PRIOR, rat, DISC_K_FACTOR,
                                     "PASS" if leg2 else "FAIL"))
    if bo:
        leg3 = bo["k10"] <= ORIG_K_FLOOR <= bo["k90"]
        print("      leg3 ORIG k band [%.6f, %.6f] contains the LOW-arm floor %.1f ?  %s"
              % (bo["k10"], bo["k90"], ORIG_K_FLOOR, "PASS" if leg3 else "FAIL"))

    print("\n--- THE SPLIT, matched d ---")
    tabs = {}
    for lo, hi, tag in b18.WINDOWS:
        tabs[(lo, hi)] = window(eo, ep, lo, hi, tag)

    print("\n--- DISCRIMINATOR (PREREG_STEP1B section 4.3), d 222..250 ---")
    t = tabs[(222, 250)]
    if t:
        so_, sp_ = t["ORIG"]["spre"], t["PORT"]["spre"]
        # the k comparison uses the ORIGINAL band's NEARER edge to the port's value
        ko_lo, ko_hi, kp = t["ORIG"]["k10"], t["ORIG"]["k90"], t["PORT"]["k"]
        near = ko_hi if kp > ko_hi else (ko_lo if kp < ko_lo else kp)
        s_rat = (sp_ / so_) if so_ else float("nan")
        k_rat = (kp / near) if near else float("inf")
        s_moved = not ((1.0 / DISC_S_FACTOR) <= s_rat <= DISC_S_FACTOR)
        k_moved = abs(k_rat) > DISC_K_FACTOR or (near and abs(near / kp) > DISC_K_FACTOR)
        print("  s_pre  ORIG %.6f  PORT %.6f  ratio %.4f   (bar: factor %.1f)  %s"
              % (so_, sp_, s_rat, DISC_S_FACTOR, "MOVED" if s_moved else "within"))
        print("  k      ORIG band %.6f..%.6f (nearer edge %.6f)  PORT %.6f  ratio %.4f"
              "   (bar: factor %.0f)  %s"
              % (ko_lo, ko_hi, near, kp, k_rat, DISC_K_FACTOR,
                 "MOVED" if k_moved else "within"))
        if k_moved and not s_moved:
            print("  --> CARRIER: k  => G = grip*speed at 0x004687db => l_60 (read 0x004686b3,")
            print("      accumulated 0x00468220/0x0046822b from 0x004680fb and 0x0046820f)")
        elif s_moved and not k_moved:
            print("  --> CARRIER: the SLIP s. clamp #6 is innocent.")
        elif s_moved and k_moved:
            print("  --> BOTH moved. Both reported; neither named alone.")
        else:
            print("  --> NEITHER moved beyond its bar. Reported as such.")

    print("\n--- U-9173 PREDICTION (PREREG_STEP1B section 5) ---")
    if t:
        sp_orig = t["ORIG"]["spost"]
        pred = sp_orig < U9173_S_PRED
        print("  ORIGINAL s' at matched d 222..250: %.6f   prediction was < %.2f  -> %s"
              % (sp_orig, U9173_S_PRED, "MET" if pred else "NOT MET"))
        print("  section 26.4 measured 0.729 at band 100-150 (n=45), which section 26.9 calls"
              " OFF-REGIME.")
        bo2 = [r for r in eo.values() if PRIOR_BAND[0] <= r["speed"] <= PRIOR_BAND[1]]
        if bo2:
            print("  ORIGINAL s' in band 100-150 here: %.6f (n=%d, med speed %.1f, med d %d)"
                  % (med([r["s_post"] for r in bo2]), len(bo2),
                     med([r["speed"] for r in bo2]),
                     int(med([r["d"] for r in bo2]))))

    if a.csv:
        import csv as _csv
        keys = ["side", "d", "frame", "speed", "R", "prod", "s_post", "k", "s_pre",
                "rt_err", "cos", "gnd", "fwdlen"]
        with open(a.csv, "w", newline="") as fh:
            w = _csv.DictWriter(fh, fieldnames=keys)
            w.writeheader()
            for side, en in (("O", eo), ("P", ep)):
                for d in sorted(en):
                    if not (180 <= d <= 280):
                        continue
                    row = dict(en[d]); row["side"] = side
                    w.writerow({k: row[k] for k in keys})
        print("\n  wrote %s" % a.csv)


if __name__ == "__main__":
    main()
