#!/usr/bin/env python3
"""U-9191: which omega ARM does each side take, and is car 1's 0.9866 deg heading
residual GENERATED at the matched instant or ACCUMULATED before it?

Pre-registration: verify/d3_arm_20261005/PREREG_ARM.md (committed unrun at 5f3d2bdd,
amended unrun at 87531150). READ-ONLY on committed captures plus one new port capture;
no injection, no Frida, no original-side run.

WHY THIS TOOL IS NOT ai_yawrate.py. Leg 3 (ai_yawrate.py) read +0x9c0 as the yaw rate.
It is the yaw TORQUE -- re/analysis/vehicle_promote_c2/0046e9e0.md:27, committed
2026-05-12, confirmed by verify/d3_omega_20261005/RESULT_RATEFIELD.md. The angular
velocity is +0x144/+0x148/+0x14c. ai_yawrate.py is left UNMODIFIED so its committed
numbers stay reproducible; every shared primitive is imported from it, so the two tools
cannot drift. +0x9c0 is NOT read as a rate anywhere below.

THE ARM COMES FIRST. FUN_0046e9e0 forks its omega source on ESI[4] = record +0x10:
non-zero keeps the torque seed, zero rebuilds omega from the steer chain. Comparing the
+0x144 accumulator across sides without establishing the arm would repeat exactly the
torque-vs-rate error of 2026-10-05, so G-ARM runs first and the pre-registration makes
it gate the accumulator leg.

Usage:
  py -3.12 re/tools/ai_armrate.py --msd <orig.msd> --orig-aistep <orig.aistep.csv>
           --port <port.aistep.csv> [--inert-ref <L1.csv>] [--car 1]
           [--radius 0.12] [--min-n 30] [--json]
"""
import argparse
import csv
import json
import math
import os
import statistics
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ai_yawrate import wrap180, R_DEFAULT, MIN_N          # noqa: E402
from ai_posmatch import window                            # noqa: E402

# --- pre-registered constants, PREREG_ARM.md. NOT TO BE MOVED. ---------------
ARM_PASS_FRAC = 0.950      # section 2: G-ARM PASS needs +0x10 == 0 on >= 95.0% of frames
ARM_FAIL_FRAC = 0.050      # section 2: FAIL if non-zero on > 5.0%
PORTARM_DATA = 0.50        # section 3b: MISMATCH-DATA if rec_10 != 0 on >= 50% of rows
PORTARM_UPSTREAM = 0.95    # section 3b: MISMATCH-UPSTREAM if rec_10 == 0 on >= 95%
ONSET_LO = 0.25            # section 4 G-ONSET, degrees
ONSET_HI = 0.50            # section 4 G-ONSET, degrees (legs 1-2b's own FAIL threshold)
ONSET_RATIO = 2.0          # section 4 G-ONSET
ONSET_VOID_MEDIAN = 0.025  # section 4: 10x below ONSET_LO, the smallest compared number
SPAN_ALIAS = 90.0          # section 4 G-SPAN VOID: max per-step wrap180 increment, degrees
SPAN_ACC = 0.50            # section 4 G-SPAN, degrees
SPAN_INH = 0.25            # section 4 G-SPAN, degrees

# record offsets. EVERY ONE grepped against re/analysis/**/0046e9e0.md before use
# (PREREG_ARM.md section 1). +0x10 is deliberately NOT given a semantic name.
OFF = {"x": 0x958, "z": 0x960, "fx": 0x9d4, "fz": 0x9dc,
       "a144": 0x144, "a148": 0x148, "a14c": 0x14c}
OFF_ARM = 0x10

# the 42 columns verify/d3_leg3_20261005/L1.csv carries, i.e. everything that existed
# before this session appended four more. G-INERT compares exactly these.
INERT_KEY = ("frame", "seq", "v")


def read_msd(path):
    """MSD1 -> list of dicts, floats plus the raw u32 at +0x10.

    Same parse as re/tools/statediff/msd_fields.py and ai_yawrate.read_msd; the arm
    word is added because ai_yawrate has no integer reader.
    """
    b = open(path, "rb").read()
    assert b[:4] == b"MSD1", "not MSD1"
    rec, base, _ = struct.unpack_from("<III", b, 4)
    out, off = [], 16
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from("<I", b, off)
        p = b[off + 4: off + 4 + rec]
        d = {k: struct.unpack_from("<f", p, o)[0] for k, o in OFF.items()}
        d["arm_u32"], = struct.unpack_from("<I", p, OFF_ARM)
        d["frame"] = fi
        out.append(d)
        off += 4 + rec
    return out, rec, base


def load_rows(path):
    with open(path, newline="", encoding="utf-8", errors="replace") as fh:
        return [r for r in csv.DictReader(fh)]


def port_window(rows, car):
    """The registered population: ai_posmatch.window -- the first 220 calls from the
    first c4 != 0 -- for one car, keeping only rows that carry a position."""
    w, _ = window(rows, car)
    out = []
    for r in w:
        if r.get("own_x", "") == "":
            continue
        try:
            out.append({"frame": int(r["frame"]), "seq": int(r["seq"]),
                        "x": float(r["own_x"]), "z": float(r["own_z"]),
                        "fx": float(r["rec_9d4"]), "fz": float(r["rec_9dc"]),
                        "a144": float(r["rec_144"]), "a148": float(r["rec_148"]),
                        "a14c": float(r["rec_14c"]), "arm": int(r["rec_10"])})
        except (ValueError, KeyError):
            continue
    return out


def head(d):
    """Body heading in degrees, measured DIRECTLY from the record's forward row.
    No err inversion -- that inversion is the algebraic identity U-9192 records."""
    return math.degrees(math.atan2(d["fz"], d["fx"]))


def med(x):
    return statistics.median(x) if x else None


def u32_views(u):
    i, = struct.unpack("<i", struct.pack("<I", u))
    f, = struct.unpack("<f", struct.pack("<I", u))
    return {"u32": "0x%08x" % u, "i32": i, "f32": f}


# ===========================================================================
# STEP 1 -- G-ARM. The original's arm, offline. Gates the accumulator leg.
# ===========================================================================
def g_arm(msd):
    n = len(msd)
    hist = {}
    for d in msd:
        hist[d["arm_u32"]] = hist.get(d["arm_u32"], 0) + 1
    z = hist.get(0, 0)
    nz = n - z
    return {"N_frames": n, "n_zero": z, "n_nonzero": nz,
            "frac_zero": (z / n) if n else None,
            "distinct": [dict(count=k, frac=k / n, **u32_views(u))
                         for u, k in sorted(hist.items(), key=lambda kv: -kv[1])],
            "constant": len(hist) == 1,
            "pass_needs_zero_at_least": math.ceil(ARM_PASS_FRAC * n),
            "fail_if_nonzero_above": math.floor(ARM_FAIL_FRAC * n),
            "pass": z >= ARM_PASS_FRAC * n,
            "fail": nz > ARM_FAIL_FRAC * n}


# ===========================================================================
# STEP 2 -- G-INERT. A VOID condition on the whole leg.
# ===========================================================================
def g_inert(ref_path, new_path):
    ref, new = load_rows(ref_path), load_rows(new_path)
    if not ref or not new:
        return {"status": "one side empty", "pass": False}
    cols = [c for c in ref[0].keys() if c is not None]
    nd = {tuple(r[k] for k in INERT_KEY): r for r in new}
    m = 0
    bad = []
    for r in ref:
        k = tuple(r[k2] for k2 in INERT_KEY)
        o = nd.get(k)
        if o is None:
            continue
        m += 1
        for c in cols:
            if r[c] != o.get(c):
                if len(bad) < 10:
                    bad.append({"key": k, "col": c, "ref": r[c], "new": o.get(c)})
    return {"ref": ref_path, "ref_cols": len(cols), "M_common_rows": m,
            "cells_compared": len(cols) * m, "mismatching_cells": len(bad),
            "examples": bad, "pass": m > 0 and not bad}


# ===========================================================================
# STEP 2b -- G-PORTARM. Does the port's record even carry the gate value?
# ===========================================================================
def g_portarm(port):
    nw = len(port)
    hist = {}
    for p in port:
        u = struct.unpack("<I", struct.pack("<i", p["arm"]))[0]
        hist[u] = hist.get(u, 0) + 1
    z = hist.get(0, 0)
    nz = nw - z
    v = "INCONCLUSIVE"
    if nw and nz >= PORTARM_DATA * nw:
        v = "MISMATCH-DATA"
    elif nw and z >= PORTARM_UPSTREAM * nw:
        v = "MISMATCH-UPSTREAM"
    return {"n_windowed_rows": nw, "n_zero": z, "n_nonzero": nz,
            "distinct": [dict(count=k, frac=k / nw, **u32_views(u))
                         for u, k in sorted(hist.items(), key=lambda kv: -kv[1])],
            "data_needs_nonzero_at_least": math.ceil(PORTARM_DATA * nw),
            "upstream_needs_zero_at_least": math.ceil(PORTARM_UPSTREAM * nw),
            "verdict": v}


# ===========================================================================
# STEP 3 -- pairing, then G-ONSET and G-SPAN. Heading units throughout.
# ===========================================================================
def pair(msd, port, radius):
    """Port rows -> nearest original .msd frame by record position, radius-gated.

    Key is port own_x/own_z against the original's +0x958/+0x960, the substitute
    PREREG_LEG3.md section 5c registered and measured: the port writes +0x958/+0x960
    as identically 0.0 because it keeps position in a.pos[], so the record-position
    key does not exist on that side. Not revisited here.
    """
    out = []
    for p in port:
        best, bi, bd = None, None, 1e18
        for i, o in enumerate(msd):
            if o["fx"] == 0.0 and o["fz"] == 0.0:
                continue
            dd = math.hypot(o["x"] - p["x"], o["z"] - p["z"])
            if dd < bd:
                bd, best, bi = dd, o, i
        if best is not None and bd <= radius:
            out.append((p, best, bi, bd))
    return out


def join_induced(msd, pairs):
    """What one frame of join slip costs, IN THE UNITS OF THE CLAIM (degrees of
    heading), on the ORIGINAL's own consecutive frames at the matched indices.

    This is PREREG_LEG3 section 5c's construction, re-measured here. Reported twice on
    purpose: the registered gate uses the plain median over matched pairs, and the
    second figure restricts to frames where the heading actually moves, because a
    median of exactly 0 that comes from a frozen heading is a construction zero and
    must not be presented as a tight floor (memory all-zero-reads-prove-nothing-alone).
    """
    step, moving = [], []
    for _, _, i, _ in pairs:
        if i + 1 >= len(msd):
            continue
        a, b = msd[i], msd[i + 1]
        if b["fx"] == 0.0 and b["fz"] == 0.0:
            continue
        d = abs(wrap180(head(b) - head(a)))
        step.append(d)
        if d > 0.0:
            moving.append(d)
    return {"n": len(step), "median_deg": med(step),
            "n_moving": len(moving), "median_moving_deg": med(moving),
            "void_above": ONSET_VOID_MEDIAN,
            "admissible": bool(step) and med(step) <= ONSET_VOID_MEDIAN}


def g_onset(pairs, induced, min_n):
    """THE DISCRIMINATOR. Is the residual already there at the window start?"""
    ordered = sorted(pairs, key=lambda t: t[0]["seq"])
    resid = [abs(wrap180(head(p) - head(o))) for p, o, _, _ in ordered]
    n = len(resid)
    t = n // 3
    out = {"n_pairs": n, "tercile_size": t, "min_n": min_n,
           "all_pairs_median_abs_deg": med(resid),
           "induced": induced,
           "thresholds": {"T1_accumulated_at_most": ONSET_LO,
                          "T3_accumulated_at_least": ONSET_HI,
                          "T1_generated_at_least": ONSET_HI,
                          "generated_ratio_at_most": ONSET_RATIO}}
    if not induced["admissible"]:
        out["verdict"] = ("VOID -- the join's median induced heading error %.4f deg "
                          "exceeds the registered %.3f deg"
                          % (induced["median_deg"] or 0.0, ONSET_VOID_MEDIAN))
        return out
    if t < min_n:
        out["verdict"] = "NO-VERDICT (tercile n=%d < %d)" % (t, min_n)
        return out
    t1, t3 = med(resid[:t]), med(resid[-t:])
    out["T1_median_abs_deg"], out["T3_median_abs_deg"] = t1, t3
    out["T3_over_T1"] = (t3 / t1) if t1 else None
    if t1 <= ONSET_LO and t3 >= ONSET_HI:
        out["verdict"] = "ACCUMULATED"
    elif t1 >= ONSET_HI and t1 and (t3 / t1) <= ONSET_RATIO:
        out["verdict"] = "GENERATED"
    else:
        out["verdict"] = "INCONCLUSIVE"
    return out


def sweep(seq):
    """Total heading swept, as the sum of per-step wrap180 increments, plus the largest
    single increment so the caller can check the alias VOID condition."""
    tot, mx, n = 0.0, 0.0, 0
    for a, b in zip(seq, seq[1:]):
        if (a["fx"] == 0.0 and a["fz"] == 0.0) or (b["fx"] == 0.0 and b["fz"] == 0.0):
            continue
        d = wrap180(head(b) - head(a))
        tot += d
        mx = max(mx, abs(d))
        n += 1
    return tot, mx, n


def g_span(msd, orig_aistep, port, car):
    """PAIRING-FREE. Each side's own total swept heading over its own window; only the
    WINDOW DEFINITION is shared. A total sweep does not depend on sample count provided
    no single step aliases, which is the registered VOID check."""
    ow, _ = window(orig_aistep, car)
    frames = [int(r["frame"]) for r in ow if r.get("frame", "") != ""]
    if not frames or not port:
        return {"status": "empty window on one side"}
    f0, f1 = min(frames), max(frames)
    omsd = [d for d in msd if f0 <= d["frame"] <= f1]
    dho, mxo, no = sweep(omsd)
    dhp, mxp, np_ = sweep(sorted(port, key=lambda p: p["seq"]))
    out = {"orig_frame_range": [f0, f1], "orig_aistep_rows": len(ow),
           "orig_msd_frames": len(omsd), "orig_steps": no,
           "port_rows": len(port), "port_steps": np_,
           "dH_orig_deg": dho, "dH_port_deg": dhp,
           "abs_diff_deg": abs(dhp - dho),
           "max_step_orig_deg": mxo, "max_step_port_deg": mxp,
           "alias_void_above": SPAN_ALIAS,
           "accumulated_at_least": SPAN_ACC, "inherited_at_most": SPAN_INH}
    if mxo >= SPAN_ALIAS or mxp >= SPAN_ALIAS:
        out["verdict"] = ("VOID -- a per-step increment reached %.3f deg (orig) / %.3f "
                          "deg (port), at or above the %.1f deg alias bound"
                          % (mxo, mxp, SPAN_ALIAS))
    elif abs(dhp - dho) >= SPAN_ACC:
        out["verdict"] = "ACCUMULATED-consistent"
    elif abs(dhp - dho) <= SPAN_INH:
        out["verdict"] = "INHERITED-BEFORE-WINDOW / GENERATED-consistent"
    else:
        out["verdict"] = "INCONCLUSIVE"
    return out


def collateral(msd, orig_aistep, port, car):
    """POST-HOC, UNREGISTERED, DESCRIPTIVE. No pass/fail, and it cannot change a gate.

    Run after the verdicts and declared as such, because G-SPAN's registered clause
    compares two totals taken over the same FRAME COUNT and says nothing about the two
    sides covering the same DISTANCE. The port is independently documented as running
    15-31% over the original's speed (U-9185 / verify/d3_noboost_20261003), so the
    distance confound had to be measured before G-SPAN's number could be read as a
    statement about the orientation law.
    """
    ow, _ = window(orig_aistep, car)
    fr = [int(r["frame"]) for r in ow if r.get("frame", "") != ""]
    om = [d for d in msd if min(fr) <= d["frame"] <= max(fr)]
    pr = sorted(port, key=lambda p: p["seq"])

    def arc(seq):
        return sum(math.hypot(b["x"] - a["x"], b["z"] - a["z"])
                   for a, b in zip(seq, seq[1:]))

    def cum(seq):
        out, a_, d_ = [(0.0, 0.0)], 0.0, 0.0
        for u, v in zip(seq, seq[1:]):
            a_ += math.hypot(v["x"] - u["x"], v["z"] - u["z"])
            # the atan2(0,0) degenerate row is skipped for heading but still carries
            # its distance: the port's first windowed row has fx == fz == 0.0 exactly,
            # which reads as heading 0 and is NOT a 90 deg convention difference.
            if not ((u["fx"] == 0.0 and u["fz"] == 0.0) or
                    (v["fx"] == 0.0 and v["fz"] == 0.0)):
                d_ += wrap180(head(v) - head(u))
            out.append((a_, d_))
        return out

    ao, ap = arc(om), arc(pr)
    dho, _, _ = sweep(om)
    dhp, _, _ = sweep(pr)
    co, cp = cum(om), cum(pr)
    L = min(co[-1][0], cp[-1][0])

    def at(c, s):
        prev = c[0]
        for a_, d_ in c:
            if a_ >= s:
                if a_ == prev[0]:
                    return d_
                t = (s - prev[0]) / (a_ - prev[0])
                return prev[1] + t * (d_ - prev[1])
            prev = (a_, d_)
        return c[-1][1]

    prof = [{"frac": f, "arclength": L * f,
             "dH_orig": at(co, L * f), "dH_port": at(cp, L * f),
             "diff": at(cp, L * f) - at(co, L * f)}
            for f in (0.2, 0.4, 0.6, 0.8, 1.0)]

    def terc(seq):
        n = len(seq); t = n // 3
        out = []
        for a_, b_ in ((0, t), (t, 2 * t), (2 * t, n)):
            s = seq[a_:b_]
            aa = arc(s)
            dd, _, _ = sweep(s)
            out.append({"arclength": aa, "dH": dd, "dH_per_arclength": (dd / aa) if aa else None})
        return out

    return {"declared": "POST-HOC, UNREGISTERED, DESCRIPTIVE -- cannot change any gate",
            "arclength_orig": ao, "arclength_port": ap, "arclength_ratio": ap / ao,
            "dH_orig": dho, "dH_port": dhp, "dH_ratio": dhp / dho if dho else None,
            "dH_per_arclength_orig": dho / ao, "dH_per_arclength_port": dhp / ap,
            "dH_per_arclength_ratio": (dhp / ap) / (dho / ao),
            "common_span_L": L, "profile_over_common_span": prof,
            "terciles_orig": terc(om), "terciles_port": terc(pr),
            "start_heading_orig": head(om[0]),
            "start_fwd_port": [pr[0]["fx"], pr[0]["fz"]],
            "note": ("tercile and dH-per-arclength comparisons are CONFOUNDED: at the "
                     "same tercile or the same total arclength the two sides are on "
                     "different stretches of road, because the port covers "
                     "%.4fx the distance in the same 220 frames" % (ap / ao))}


def combine(onset, span):
    """The combination rule, fixed in PREREG_ARM.md section 4 before anything ran."""
    ov, sv = onset.get("verdict", ""), span.get("verdict", "")
    if ov == "ACCUMULATED" and sv == "ACCUMULATED-consistent":
        return "ACCUMULATED"
    if ov == "GENERATED" and sv.startswith("INHERITED-BEFORE-WINDOW"):
        return ("GENERATED-OR-INHERITED-BEFORE-WINDOW -- these two are NOT separable "
                "by this instrument")
    return "INCONCLUSIVE (G-ONSET %s / G-SPAN %s)" % (ov or "n/a", sv or "n/a")


def run(a):
    msd, rec, base = read_msd(a.msd)
    port_rows = load_rows(a.port)
    port = port_window(port_rows, a.car)
    out = {"prereg": "verify/d3_arm_20261005/PREREG_ARM.md",
           "msd": a.msd, "msd_rec_size": "0x%x" % rec, "msd_base_va": "0x%08x" % base,
           "orig_aistep": a.orig_aistep, "port": a.port, "car": a.car,
           "R": a.radius, "min_n": a.min_n,
           "g_arm": g_arm(msd)}

    if a.inert_ref:
        out["g_inert"] = g_inert(a.inert_ref, a.port)
        if not out["g_inert"]["pass"]:
            out["verdict"] = ("VOID -- G-INERT failed; appending read-only record reads "
                              "changed a pre-existing column")
            return out

    out["g_portarm"] = g_portarm(port)
    # Registered FAIL clause, section 2: G-ACC is not run when the arms differ, and
    # section 3b states G-PORTARM does not revive it in either branch.
    out["g_acc"] = ("NOT RUN -- G-ARM %s. Comparing the +0x144/+0x148/+0x14c "
                    "accumulator across sides that provably take DIFFERENT omega arms "
                    "is the torque-vs-rate error in another costume."
                    % ("FAILED" if out["g_arm"]["fail"] else "did not pass"))
    if out["g_arm"]["pass"]:
        out["g_acc"] = "ELIGIBLE -- arms agree; not implemented in this leg"

    pairs = pair(msd, port, a.radius)
    out["n_matched"] = len(pairs)
    if pairs:
        out["pair_dist"] = {"median": med([d for _, _, _, d in pairs]),
                            "max": max(d for _, _, _, d in pairs)}
    if len(pairs) < a.min_n:
        out["g_onset"] = {"verdict": "NO-VERDICT (matched n=%d < %d)"
                                     % (len(pairs), a.min_n)}
    else:
        out["g_onset"] = g_onset(pairs, join_induced(msd, pairs), a.min_n)
    out["g_span"] = g_span(msd, load_rows(a.orig_aistep), port, a.car)
    out["combined"] = combine(out["g_onset"], out["g_span"])
    if a.collateral:
        out["collateral"] = collateral(msd, load_rows(a.orig_aistep), port, a.car)
    return out


def report(o):
    print("U-9191 arm + generated-vs-accumulated -- PREREG %s" % o["prereg"])
    print("msd %s  rec_size %s  base_va %s" % (o["msd"], o["msd_rec_size"], o["msd_base_va"]))
    print("port %s   car %s   R %s" % (o["port"], o["car"], o["R"]))

    a = o["g_arm"]
    print("\nG-ARM  which omega arm does the ORIGINAL take?  (record +0x10, ESI[4])")
    print("      +0x10 == 0 (steer arm) on %d of %d frames = %.4f%%"
          % (a["n_zero"], a["N_frames"], 100.0 * a["frac_zero"]))
    print("      +0x10 != 0 (torque arm) on %d of %d frames = %.4f%%"
          % (a["n_nonzero"], a["N_frames"], 100.0 * (1.0 - a["frac_zero"])))
    for d in a["distinct"]:
        print("        %s  i32 %d  f32 %r   count %d (%.4f%%)"
              % (d["u32"], d["i32"], d["f32"], d["count"], 100.0 * d["frac"]))
    print("      PASS needs zero on >= %d of %d; FAIL if non-zero on > %d of %d"
          % (a["pass_needs_zero_at_least"], a["N_frames"],
             a["fail_if_nonzero_above"], a["N_frames"]))
    print("      G-ARM %s" % ("PASS" if a["pass"] else "FAIL"))

    if "g_inert" in o:
        g = o["g_inert"]
        print("\nG-INERT  no-behaviour-change control (VOID condition)")
        print("      %d mismatching cells of %d (%d pre-existing cols x %d common rows) "
              "vs %s" % (g["mismatching_cells"], g["cells_compared"], g["ref_cols"],
                         g["M_common_rows"], g["ref"]))
        print("      G-INERT %s" % ("PASS" if g["pass"] else "FAIL"))
        for e in g["examples"]:
            print("        %s %s: ref %r new %r" % (e["key"], e["col"], e["ref"], e["new"]))
    if "verdict" in o:
        print("\n%s" % o["verdict"])
        return

    p = o["g_portarm"]
    print("\nG-PORTARM  does the PORT's record carry the gate value?  (rec_10)")
    print("      zero on %d of %d windowed rows; non-zero on %d of %d"
          % (p["n_zero"], p["n_windowed_rows"], p["n_nonzero"], p["n_windowed_rows"]))
    for d in p["distinct"]:
        print("        %s  i32 %d  f32 %r   count %d (%.4f%%)"
              % (d["u32"], d["i32"], d["f32"], d["count"], 100.0 * d["frac"]))
    print("      MISMATCH-DATA needs non-zero >= %d of %d; MISMATCH-UPSTREAM needs "
          "zero >= %d of %d" % (p["data_needs_nonzero_at_least"], p["n_windowed_rows"],
                                p["upstream_needs_zero_at_least"], p["n_windowed_rows"]))
    print("      VERDICT: %s" % p["verdict"])
    print("\nG-ACC  %s" % o["g_acc"])

    print("\nmatched pairs %s   pair distance median %s max %s"
          % (o.get("n_matched"),
             None if not o.get("pair_dist") else round(o["pair_dist"]["median"], 5),
             None if not o.get("pair_dist") else round(o["pair_dist"]["max"], 5)))
    g = o["g_onset"]
    print("\nG-ONSET  is the residual already there at the window start?")
    if "induced" in g:
        i = g["induced"]
        print("      join induced heading error: median %.6f deg over %d matched pairs "
              "(VOID above %.3f)" % (i["median_deg"] or 0.0, i["n"], i["void_above"]))
        print("      same, restricted to the %d of %d pairs where the heading MOVES: "
              "median %.6f deg -- reported because a median of 0 from a frozen heading "
              "is a construction zero, not a tight floor"
              % (i["n_moving"], i["n"], i["median_moving_deg"] or 0.0))
    if "T1_median_abs_deg" in g:
        print("      all %d pairs: median_abs %.4f deg" % (g["n_pairs"], g["all_pairs_median_abs_deg"]))
        print("      T1 (first %d of %d): %.4f deg   T3 (last %d of %d): %.4f deg   "
              "T3/T1 %.4f" % (g["tercile_size"], g["n_pairs"], g["T1_median_abs_deg"],
                              g["tercile_size"], g["n_pairs"], g["T3_median_abs_deg"],
                              g["T3_over_T1"] or 0.0))
        t = g["thresholds"]
        print("      ACCUMULATED needs T1 <= %.2f AND T3 >= %.2f; GENERATED needs "
              "T1 >= %.2f AND T3/T1 <= %.1f"
              % (t["T1_accumulated_at_most"], t["T3_accumulated_at_least"],
                 t["T1_generated_at_least"], t["generated_ratio_at_most"]))
    print("      VERDICT: %s" % g["verdict"])

    s = o["g_span"]
    print("\nG-SPAN  total heading swept per side, PAIRING-FREE")
    if "dH_orig_deg" in s:
        print("      orig: frames %s, %d msd frames, %d steps -> dH %.4f deg "
              "(max step %.4f)" % (s["orig_frame_range"], s["orig_msd_frames"],
                                   s["orig_steps"], s["dH_orig_deg"], s["max_step_orig_deg"]))
        print("      port: %d rows, %d steps -> dH %.4f deg (max step %.4f)"
              % (s["port_rows"], s["port_steps"], s["dH_port_deg"], s["max_step_port_deg"]))
        print("      |dH_port - dH_orig| = %.4f deg; ACCUMULATED-consistent >= %.2f, "
              "INHERITED <= %.2f, alias VOID at %.1f"
              % (s["abs_diff_deg"], s["accumulated_at_least"], s["inherited_at_most"],
                 s["alias_void_above"]))
    print("      VERDICT: %s" % s["verdict"])
    print("\nCOMBINED (rule fixed before running): %s" % o["combined"])

    c = o.get("collateral")
    if not c:
        return
    print("\n--- COLLATERAL: %s" % c["declared"])
    print("      arclength over the window: orig %.4f  port %.4f  ratio %.4f"
          % (c["arclength_orig"], c["arclength_port"], c["arclength_ratio"]))
    print("      dH:                        orig %.4f  port %.4f  ratio %.4f"
          % (c["dH_orig"], c["dH_port"], c["dH_ratio"]))
    print("      dH per unit arclength:     orig %.6f  port %.6f  ratio %.4f"
          % (c["dH_per_arclength_orig"], c["dH_per_arclength_port"],
             c["dH_per_arclength_ratio"]))
    print("      common spatial span L = %.4f; cumulative sweep along it:" % c["common_span_L"])
    for p in c["profile_over_common_span"]:
        print("        %3.0f%% of L (arclen %7.4f): orig %9.4f  port %9.4f  diff %9.4f deg"
              % (p["frac"] * 100, p["arclength"], p["dH_orig"], p["dH_port"], p["diff"]))
    for lbl, ts in (("orig", c["terciles_orig"]), ("port", c["terciles_port"])):
        for j, t in enumerate(ts):
            print("        %s T%d  arclen %8.3f  dH %9.4f  dH/arclen %9.6f"
                  % (lbl, j + 1, t["arclength"], t["dH"], t["dH_per_arclength"]))
    print("      start heading orig %.4f deg; port's first windowed forward row is "
          "(%.6f, %.6f) -- atan2(0,0), NOT a 90 deg convention difference"
          % (c["start_heading_orig"], c["start_fwd_port"][0], c["start_fwd_port"][1]))
    print("      %s" % c["note"])


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--msd", required=True)
    ap.add_argument("--orig-aistep", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--inert-ref")
    ap.add_argument("--car", type=int, default=1)
    ap.add_argument("--radius", type=float, default=R_DEFAULT)
    ap.add_argument("--min-n", type=int, default=MIN_N)
    ap.add_argument("--collateral", action="store_true",
                    help="post-hoc descriptive distance controls; no pass/fail")
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args(argv)
    out = run(a)
    print(json.dumps(out, indent=1)) if a.json else report(out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
