"""U-9191 leg 3: is car 1's body-heading residual GENERATED at the matched instant or
ACCUMULATED before it?

Pre-registration: verify/d3_leg3_20261005/PREREG_LEG3.md. READ-ONLY on committed captures
plus one new port capture; no injection, no original-side run.

Legs 1-2b (verify/d3_headattrib_20261005) established that car 1's heading differs from the
original's by a median 0.8918 deg at matched position, matched commanded steer and matched
speed, and that staleness, command and speed are each excluded. They could NOT say whether
that offset is produced at the sampled call or inherited, because a body heading is an
INTEGRAL of past yaw rate and matched position/command/speed is not matched HISTORY.

This tool reads the yaw rate DIRECTLY (record +0x9c0) on both sides and never differences it,
and it measures the heading DIRECTLY (atan2 of +0x9dc/+0x9d4) instead of inverting it out of
`signed_err` -- the inversion U-9192 records as an algebraic identity.

PAIRING is on the record's own world position +0x958/+0x960 on BOTH sides, not on the AI's
own_x/own_z, so it cannot be contaminated by any difference between what the AI believes its
position is and where the physics record puts it. R defaults to ai_posmatch.py's 0.12.

THE VALIDITY FLOOR IS IN THE UNITS OF THE CLAIM. U-9190 was filed because sa_headwatch.py
bounded a POSITION while claiming an ANGLE. Here the floor is the ORIGINAL's own
frame-to-frame variability of +0x9c0 -- how far the quantity moves inside one sampling step of
the coarser side -- so it bounds what the per-frame-vs-per-call granularity alone could
manufacture, in yaw-rate units, on the same field. Pairing the original against a second
original capture was DELIBERATELY REJECTED as a floor: the original is deterministic over this
window, so that floor would be exactly 0, which is the construction-zero trap U-9188's player
floor fell into.

Usage:
  py -3.12 re/tools/ai_yawrate.py --msd <orig.msd> --port <port.aistep.csv>
           [--car 1] [--radius 0.12] [--min-n 30] [--json]
"""
import argparse
import csv
import json
import math
import statistics
import struct
import sys

OFF = {"x": 0x958, "z": 0x960, "fx": 0x9d4, "fz": 0x9dc,
       "wx": 0x9bc, "wy": 0x9c0, "wz": 0x9c4}
R_DEFAULT = 0.12          # ai_posmatch.py:24
MIN_N = 30                # same floor legs 1-2b used
KA1_CORR = 0.95           # PREREG section 3
BAND_LO, BAND_HI = 0.45, 1.80   # PREREG G-DIRECT: 2x either side of 0.8918
FLOOR_X = 10.0            # PREREG G-FLOOR
GEN_FRAC, ACC_FRAC = 0.20, 0.05  # PREREG G-RATE


def read_msd(path):
    """MSD1 frames -> list of dicts. Same parse as re/tools/statediff/msd_fields.py."""
    b = open(path, "rb").read()
    assert b[:4] == b"MSD1", "not MSD1"
    rec, _base, _ = struct.unpack_from("<III", b, 4)
    out, off = [], 16
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from("<I", b, off)
        p = b[off + 4: off + 4 + rec]
        d = {k: struct.unpack_from("<f", p, o)[0] for k, o in OFF.items()}
        d["frame"] = fi
        out.append(d)
        off += 4 + rec
    return out


def load_port(path, car, windowed=True):
    """Port rows for one car.

    PREREG section 5c: the pairing key is own_x/own_z, NOT rec_958/rec_960 -- the port
    writes +0x958/+0x960 as identically 0.0 (it keeps position in a.pos[], not in the
    record), so the registered key does not exist on this side. The substitute is measured
    against the original's own .msd-to-aistep join: phase offset median 0.000000, and one
    frame of slip costs a median 0.0000 deg of heading against a 0.8918 deg claim.

    `windowed` applies ai_posmatch.window -- the first 220 calls from the first c4 != 0 --
    so the population is the same one legs 1-2b scored.
    """
    with open(path, newline="", encoding="utf-8", errors="replace") as fh:
        rows = [r for r in csv.DictReader(fh)]
    if windowed:
        from ai_posmatch import window
        rows, _ = window(rows, car)
    out = []
    for r in rows:
            if r.get("v") != str(car) or r.get("own_x", "") == "":
                continue
            try:
                out.append({"frame": int(r["frame"]), "seq": int(r["seq"]),
                            "c0": r["c0"], "c1": r["c1"], "sp": float(r["rec_9e4"]),
                            "x": float(r["own_x"]), "z": float(r["own_z"]),
                            "fx": float(r["rec_9d4"]), "fz": float(r["rec_9dc"]),
                            "wx": float(r["rec_9bc"]), "wy": float(r["rec_9c0"]),
                            "wz": float(r["rec_9c4"])})
            except (ValueError, KeyError):
                continue
    return out


def head(d):
    """Body heading in degrees, measured directly from the record's forward row."""
    return math.degrees(math.atan2(d["fz"], d["fx"]))


def wrap180(a):
    while a > 180.0:
        a -= 360.0
    while a <= -180.0:
        a += 360.0
    return a


def pearson(xs, ys):
    n = len(xs)
    if n < 3:
        return None
    mx, my = sum(xs) / n, sum(ys) / n
    sx = math.sqrt(sum((x - mx) ** 2 for x in xs))
    sy = math.sqrt(sum((y - my) ** 2 for y in ys))
    if sx == 0.0 or sy == 0.0:
        return None
    return sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / (sx * sy)


def ka1(msd):
    """Is +0x9c0 the yaw rate? A single scale k must fit d(heading) = k * wy.

    k absorbs dt and the unit scale, so no dt is needed. Frames where the car has not
    moved at all are excluded: both sides are then 0 and would inflate the correlation
    for free (the atan2(0,0)=0 trap recorded in U-9188's own third self-correction).
    """
    dh, wy = [], []
    for a, b in zip(msd, msd[1:]):
        if a["fx"] == 0.0 and a["fz"] == 0.0:
            continue
        if b["fx"] == 0.0 and b["fz"] == 0.0:
            continue
        d = wrap180(head(b) - head(a))
        if d == 0.0 and a["wy"] == 0.0:
            continue
        dh.append(d)
        wy.append(a["wy"])
    if len(dh) < 3:
        return {"status": "insufficient frames", "n": len(dh)}
    corr = pearson(wy, dh)
    num = sum(w * h for w, h in zip(wy, dh))
    den = sum(w * w for w in wy)
    k = (num / den) if den else None
    resid = None
    if k is not None:
        r = sorted(abs(h - k * w) for w, h in zip(wy, dh))
        resid = {"median": r[len(r) // 2], "p90": r[int(0.9 * (len(r) - 1))], "max": r[-1]}
    return {"status": "ok", "n": len(dh), "corr": corr, "k": k, "resid_deg": resid,
            "wx_nonzero": sum(1 for d in msd if d["wx"] != 0.0),
            "wz_nonzero": sum(1 for d in msd if d["wz"] != 0.0),
            "frames": len(msd),
            "pass": (corr is not None and corr >= KA1_CORR)}


def med(x):
    return statistics.median(x) if x else None


def run(msd, port, radius, min_n):
    out = {"R": radius, "min_n": min_n, "prereg": "verify/d3_leg3_20261005/PREREG_LEG3.md",
           "ka1": ka1(msd)}
    # PREREG section 5b (amendment committed at 01ada11e, with G-DIRECT still unrun):
    # KA-1 gates the RATE legs only. G-DIRECT compares atan2(+0x9dc,+0x9d4) and does not
    # use +0x9c0, so it is independent of the field KA-1 tests. A KA-1 FAIL abandons
    # G-FLOOR and G-RATE outright -- no cross-side rate number is produced, here or
    # anywhere -- and leaves G-DIRECT to run on its own pre-fixed band.
    out["rate_legs"] = ("ABANDONED -- KA-1 did not pass; +0x9c0 is not usable as a "
                        "per-frame yaw rate and no cross-side rate number is reported"
                        if not out["ka1"].get("pass") else "eligible")

    # Pair each port AI call to the nearest original .msd frame by record position.
    pairs = []
    for p in port:
        best, bd = None, 1e18
        for o in msd:
            if o["fx"] == 0.0 and o["fz"] == 0.0:
                continue
            dd = math.hypot(o["x"] - p["x"], o["z"] - p["z"])
            if dd < bd:
                bd, best = dd, o
        if best is not None and bd <= radius:
            pairs.append((p, best, bd))
    out["n_matched"] = len(pairs)
    if len(pairs) < min_n:
        out["verdict"] = "NO-VERDICT (matched n=%d < %d)" % (len(pairs), min_n)
        return out

    # G-DIRECT: heading measured directly, no err inversion.
    dh = [abs(wrap180(head(p) - head(o))) for p, o, _ in pairs]
    # G-RATE: yaw rate read directly, never differenced.
    dw = [abs(p["wy"] - o["wy"]) for p, o, _ in pairs]
    ow = [abs(o["wy"]) for _, o, _ in pairs]

    # G-FLOOR: the ORIGINAL's own per-sampling-step variability of the SAME field,
    # restricted to the frames that actually got matched.
    idx = {id(o) for _, o, _ in pairs}
    step = []
    for a, b in zip(msd, msd[1:]):
        if id(a) in idx or id(b) in idx:
            step.append(abs(b["wy"] - a["wy"]))
    floor = med(step)
    dw_med = med(dw)

    out["pair_dist"] = {"median": med([d for _, _, d in pairs]),
                        "max": max(d for _, _, d in pairs)}
    out["g_direct"] = {"n": len(dh), "median_abs_deg": med(dh),
                       "band": [BAND_LO, BAND_HI],
                       "pass": (BAND_LO <= med(dh) <= BAND_HI)}
    if out["rate_legs"] != "eligible":
        out["g_floor"] = out["g_rate"] = out["rate_legs"]
        return out
    out["g_floor"] = {"orig_per_step_median": floor, "cross_side_median": dw_med,
                      "ratio": (dw_med / floor) if floor else None,
                      "required_x": FLOOR_X,
                      "pass": bool(floor and dw_med / floor >= FLOOR_X)}
    orig_med = med(ow)
    frac = (dw_med / orig_med) if orig_med else None
    out["g_rate"] = {"cross_side_median": dw_med, "orig_median_abs_wy": orig_med,
                     "fraction_of_orig": frac,
                     "generated_above": GEN_FRAC, "accumulated_below": ACC_FRAC}
    if not out["g_floor"]["pass"]:
        out["g_rate"]["verdict"] = ("VOID -- below the %gx floor; this is NOT a finding of "
                                    "agreement" % FLOOR_X)
    elif frac is None:
        out["g_rate"]["verdict"] = "VOID -- original median |wy| is zero"
    elif frac >= GEN_FRAC:
        out["g_rate"]["verdict"] = "GENERATED"
    elif frac <= ACC_FRAC:
        out["g_rate"]["verdict"] = "ACCUMULATED"
    else:
        out["g_rate"]["verdict"] = "INCONCLUSIVE"
    return out


def report(o):
    print("U-9191 leg 3 -- PREREG %s" % o["prereg"])
    k = o["ka1"]
    print("\nKA-1  is +0x9c0 the yaw rate?  n=%s corr=%s k=%s"
          % (k.get("n"), None if k.get("corr") is None else round(k["corr"], 6),
             None if k.get("k") is None else round(k["k"], 9)))
    if k.get("resid_deg"):
        r = k["resid_deg"]
        print("      single-scale fit residual deg: median %.3e  p90 %.3e  max %.3e"
              % (r["median"], r["p90"], r["max"]))
    print("      roll/pitch rate non-zero frames: wx %s, wz %s of %s"
          % (k.get("wx_nonzero"), k.get("wz_nonzero"), k.get("frames")))
    print("      KA-1 %s (need corr >= %s)" % ("PASS" if k.get("pass") else "FAIL", KA1_CORR))
    print("      RATE LEGS: %s" % o.get("rate_legs"))
    print("\nmatched pairs %s   pair distance median %s max %s"
          % (o.get("n_matched"), None if not o.get("pair_dist") else round(o["pair_dist"]["median"], 5),
             None if not o.get("pair_dist") else round(o["pair_dist"]["max"], 5)))
    if "verdict" in o:
        print("%s" % o["verdict"])
        return
    d = o["g_direct"]
    print("\nG-DIRECT heading measured DIRECTLY (no err inversion)")
    print("      n=%d median_abs=%.4f deg   band %s   %s"
          % (d["n"], d["median_abs_deg"], d["band"], "PASS" if d["pass"] else "FAIL"))
    if not isinstance(o["g_floor"], dict):
        print("\nG-FLOOR / G-RATE: %s" % o["g_floor"])
        return
    f = o["g_floor"]
    print("\nG-FLOOR floor is in yaw-rate units, NOT a position bound")
    print("      orig per-step median |dwy| = %.6g" % f["orig_per_step_median"])
    print("      cross-side median |dwy|    = %.6g" % f["cross_side_median"])
    print("      ratio %.3f (need >= %g)  %s"
          % (f["ratio"] or 0.0, f["required_x"], "PASS" if f["pass"] else "FAIL"))
    r = o["g_rate"]
    print("\nG-RATE  cross-side median |dwy| = %.6g, orig median |wy| = %.6g -> %.4f of orig"
          % (r["cross_side_median"], r["orig_median_abs_wy"], r["fraction_of_orig"] or 0.0))
    print("      GENERATED if >= %g, ACCUMULATED if <= %g" % (GEN_FRAC, ACC_FRAC))
    print("      VERDICT: %s" % r["verdict"])


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--msd", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--car", type=int, default=1)
    ap.add_argument("--radius", type=float, default=R_DEFAULT)
    ap.add_argument("--min-n", type=int, default=MIN_N)
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args(argv)
    out = run(read_msd(a.msd), load_port(a.port, a.car), a.radius, a.min_n)
    out["msd"], out["port"], out["car"] = a.msd, a.port, a.car
    print(json.dumps(out, indent=1)) if a.json else report(out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
