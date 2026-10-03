"""D3 offline: position-matched curvature (U-9183) and the c0 branch split (U-9182).

Implements EXACTLY the rules pre-registered, unrun, in
verify/d3_offline_20261002/PREREG.md. No rule in this file may be changed after a run;
if one is replaced the replacement is stated in the RESULT.

Both analyses run on committed .aistep.csv captures only. No game run, no Ghidra, no
build. Every constant carries the RVA or _DAT_ address it was read from.

Usage:
  py -3.12 re/tools/ai_posmatch.py --orig <orig.csv> --port <port.csv> [--json]
  py -3.12 re/tools/ai_posmatch.py --orig ... --port ... --part A
  py -3.12 re/tools/ai_posmatch.py --orig ... --port ... --part B
"""
import argparse
import csv
import json
import math
import statistics
import sys
from collections import Counter, defaultdict

# --- pre-registered constants (PREREG.md A.3 / A.4 / B.3) --------------------
R_MATCH = 0.12          # world units; the original's own median in-window step
KA_A2_MED = 2.0         # degrees
KA_A2_P90 = 10.0        # degrees
VERDICT_MED = 2.0       # degrees
VERDICT_P90 = 10.0      # degrees
VERDICT_MIN_N = 50
WINDOW_N = 220

# --- FUN_00416250 constants, by _DAT_ address (PREREG.md B.1/B.3) -----------
STEER_SPLIT = 180.0     # _DAT_005cd09c   0x004162be / 0x0041683e
STEER_DEADBAND = 1.0    # _DAT_005cc320   0x004165f1
ACCEL_ERR_LO = 30.0     # _DAT_005cc72c   0x0041684b
MAG_SCALE = 0.0030034   # _DAT_005cd0e8   0x00416656
MAG_EXTRA = 0.05        # _DAT_005cc9a0   0x00416673
CURV_GATE = 20.0        # _DAT_005ccd6c   0x00416662
MAG_CLAMP = 255.0       # _DAT_005cd04c   0x0041667b / 0x0041668a
WRAP = 360.0            # _DAT_005ccac4   0x004165cc / 0x004162cb


def load(path):
    return list(csv.DictReader(open(path, newline="")))


def window(rows, v, n=WINDOW_N):
    """ai_ctrl_window.py's window, verbatim: first n calls from the first c4 != 0."""
    r = [x for x in rows if int(x["v"]) == v]
    i0 = next((i for i, x in enumerate(r) if int(x["c4"]) != 0), None)
    return ([], None) if i0 is None else (r[i0:i0 + n], i0)


def _f(r, k):
    s = r[k]
    return None if s == "" else float(s)


# ===========================================================================
# PART A -- U-9183, position-matched curvature
# ===========================================================================
def ka_a1(rows, label):
    """Bit-exact determinism at identical position (PREREG A.1).

    Same car, string-identical own_x AND own_z, equal ai_spline_idx -> curv must be
    string-equal. Groups rather than pairs: a group of k rows is k*(k-1)/2 pairs.
    """
    groups = defaultdict(list)
    for r in rows:
        if r["own_x"] == "" or r["curv"] == "":
            continue
        groups[(r["v"], r["ai_spline_idx"], r["own_x"], r["own_z"])].append(r["curv"])
    pairs = viol = 0
    examples = []
    for key, cs in groups.items():
        k = len(cs)
        if k < 2:
            continue
        pairs += k * (k - 1) // 2
        u = set(cs)
        if len(u) > 1:
            n_bad = k * (k - 1) // 2 - sum(c * (c - 1) // 2 for c in Counter(cs).values())
            viol += n_bad
            if len(examples) < 5:
                examples.append({"key": key, "curv_values": sorted(u)[:4]})
    return {"side": label, "pairs": pairs, "violations": viol, "examples": examples,
            "underpowered": pairs < 100,
            "pass": viol == 0 and pairs >= 100}


def ka_a2(rows, label, radius=R_MATCH):
    """Single-valuedness within the matching radius (PREREG A.2).

    Same car, same ai_spline_idx, |dpos| <= radius -> |dcurv| median <= 2, p90 <= 10.
    O(n^2) inside each (car, idx) bucket; the buckets are <= ~3.5k rows so this is fine.
    """
    d = []
    for key, rs in _bucket(rows).items():
        pts = [(_f(r, "own_x"), _f(r, "own_z"), _f(r, "curv")) for r in rs
               if r["own_x"] != "" and r["curv"] != ""]
        pts.sort(key=lambda p: p[0])
        for i, a in enumerate(pts):
            for b in pts[i + 1:]:
                if b[0] - a[0] > radius:
                    break
                if math.hypot(b[0] - a[0], b[1] - a[1]) <= radius:
                    d.append(abs(b[2] - a[2]))
    if not d:
        return {"side": label, "pairs": 0, "pass": False, "note": "no pair within R"}
    d.sort()
    med = statistics.median(d)
    p90 = d[int(0.9 * (len(d) - 1))]
    return {"side": label, "pairs": len(d), "median": round(med, 4), "p90": round(p90, 4),
            "max": round(d[-1], 4), "pass": med <= KA_A2_MED and p90 <= KA_A2_P90}


def _bucket(rows):
    b = defaultdict(list)
    for r in rows:
        b[(r["v"], r["ai_spline_idx"])].append(r)
    return b


def part_a(orig, port, radius=R_MATCH):
    out = {"R": radius, "ka_a1": {}, "ka_a2": {}, "cars": {}}
    for label, rows in (("orig", orig), ("port", port)):
        out["ka_a1"][label] = ka_a1(rows, label)
        out["ka_a2"][label] = ka_a2(rows, label, radius)
    out["ka_pass"] = all(out["ka_a1"][s]["pass"] for s in ("orig", "port")) and \
                     all(out["ka_a2"][s]["pass"] for s in ("orig", "port"))

    for v in (1, 2, 3):
        pw, _ = window(port, v)
        ow, oi0 = window(orig, v)
        cand = defaultdict(list)          # ai_spline_idx -> original rows of car v
        for i, r in enumerate(orig):
            if int(r["v"]) == v and r["own_x"] != "" and r["curv"] != "":
                cand[r["ai_spline_idx"]].append((i, _f(r, "own_x"), _f(r, "own_z"),
                                                 _f(r, "curv"), _f(r, "rec_9e4")))
        diffs, recs, outside, unmatched = [], [], 0, 0
        owin = set(range(oi0, oi0 + WINDOW_N)) if oi0 is not None else set()
        # the original's own index space is per-car; rebuild it as absolute row ids
        ow_ids = set()
        if oi0 is not None:
            cv = [i for i, r in enumerate(orig) if int(r["v"]) == v]
            ow_ids = set(cv[oi0:oi0 + WINDOW_N])
        for pr in pw:
            if pr["own_x"] == "" or pr["curv"] == "":
                unmatched += 1
                continue
            px, pz = _f(pr, "own_x"), _f(pr, "own_z")
            best, bd = None, 1e9
            for c in cand.get(pr["ai_spline_idx"], ()):
                dd = math.hypot(c[1] - px, c[2] - pz)
                if dd < bd:
                    bd, best = dd, c
            if best is None or bd > radius:
                unmatched += 1
                continue
            if best[0] not in ow_ids:
                outside += 1
            diffs.append(abs(_f(pr, "curv") - best[3]))
            recs.append({"d_pos": round(bd, 4),
                         "curv_port": _f(pr, "curv"), "curv_orig": best[3],
                         "spd_port": _f(pr, "rec_9e4"), "spd_orig": best[4]})
        s = {"n_window": len(pw), "n_matched": len(diffs), "n_unmatched": unmatched,
             "matched_from_outside_orig_window": outside}
        if diffs:
            d = sorted(diffs)
            s.update(median=round(statistics.median(d), 4),
                     p90=round(d[int(0.9 * (len(d) - 1))], 4), max=round(d[-1], 4),
                     mean_dpos=round(statistics.mean(r["d_pos"] for r in recs), 4),
                     curv_med_port=round(statistics.median(r["curv_port"] for r in recs), 3),
                     curv_med_orig=round(statistics.median(r["curv_orig"] for r in recs), 3),
                     spd_med_port=round(statistics.median(r["spd_port"] for r in recs), 1),
                     spd_med_orig=round(statistics.median(r["spd_orig"] for r in recs), 1))
            s["underpowered"] = len(diffs) < VERDICT_MIN_N
            s["pass"] = (not s["underpowered"] and s["median"] <= VERDICT_MED
                         and s["p90"] <= VERDICT_P90)
        else:
            s["underpowered"] = True
            s["pass"] = False
        out["cars"][v] = s
    out["verdict"] = ("CLOSE" if out["ka_pass"] and all(out["cars"][v]["pass"] for v in (1, 2, 3))
                      else "DOES NOT CLOSE")
    return out


# ===========================================================================
# PART B -- U-9182, the c0 branch split
# ===========================================================================
def classify(r):
    """PREREG B.2 I-RULE, identical on both sides."""
    d8, dc = _f(r, "hist_d8"), _f(r, "hist_dc")
    if d8 is None or dc is None:
        return "AMBIG", None
    lo = (d8 == WRAP) and (0.0 <= dc < STEER_SPLIT)
    hi = (dc == 0.0) and (STEER_SPLIT < d8 <= WRAP)
    if lo == hi:
        return "AMBIG", None
    return ("LO", dc) if lo else ("HI", d8)


def label(r):
    band, err = classify(r)
    c0 = int(r["c0"])
    if band == "AMBIG":
        return "AMBIG", None
    if band == "LO":
        if err <= STEER_DEADBAND:
            return "DEAD", err
        if err > ACCEL_ERR_LO:
            return "FF30", err
        return ("MAG" if c0 != 0 else "CTR1"), err
    return ("CTRHI" if c0 != 0 else "MAGHI"), err


def recompute_c0(err, speed, curv, mode):
    """MAG, 0x00416648..0x00416697: m = err*speed*0.0030034, *= curv*0.05 when
    mode==0 && curv>20 (0x0041665c/0x00416662), clamp to 255 (0x0041667b), ftol
    truncation (call 0x4a2c48), low byte stored (0x00416697)."""
    m = err * speed * MAG_SCALE
    if mode == 0 and curv > CURV_GATE:
        m = m * (curv * MAG_EXTRA)
    if not (m <= MAG_CLAMP):
        m = MAG_CLAMP
    t = int(m)                     # truncation toward zero
    return 0 if t < 0 else (255 if t > 255 else t)


def ka_b(rows, side):
    v1 = v2 = n1 = n2 = 0
    ex1, ex2 = [], []
    for r in rows:
        if int(r["ai_override"]) != 0:
            continue
        lab, err = label(r)
        c0 = int(r["c0"])
        if lab == "FF30":
            n1 += 1
            if c0 != 255:
                v1 += 1
                if len(ex1) < 5:
                    ex1.append({"frame": r["frame"], "v": r["v"], "err": err, "c0": c0})
        elif lab == "MAG":
            n2 += 1
            hat = recompute_c0(err, _f(r, "rec_9e4"), _f(r, "curv"), int(r["ai_mode"]))
            if hat != c0:
                v2 += 1
                if len(ex2) < 5:
                    ex2.append({"frame": r["frame"], "v": r["v"], "err": round(err, 4),
                                "speed": _f(r, "rec_9e4"), "curv": _f(r, "curv"),
                                "c0": c0, "c0_hat": hat})
    r1 = {"n": n1, "violations": v1, "examples": ex1, "pass": n1 > 0 and v1 == 0}
    ok = n2 - v2
    r2 = {"n": n2, "exact": ok, "frac": round(ok / n2, 4) if n2 else None,
          "examples": ex2, "pass": n2 >= 20 and n2 and ok / n2 >= 0.95}
    return {"side": side, "KA_B1_ff30": r1, "KA_B2_mag": r2,
            "pass": r1["pass"] and r2["pass"]}


def part_b(orig, port, car=1):
    out = {"car": car, "ka": {}, "split": {}}
    for side, rows in (("orig", orig), ("port", port)):
        out["ka"][side] = ka_b(rows, side)
    out["ka_pass"] = all(out["ka"][s]["pass"] for s in ("orig", "port"))

    for side, rows in (("orig", orig), ("port", port)):
        w, i0 = window(rows, car)
        by = defaultdict(list)
        for r in w:
            by[label(r)[0]].append(r)
        det = {}
        for lab, rs in sorted(by.items()):
            errs = [label(r)[1] for r in rs if label(r)[1] is not None]
            det[lab] = {
                "n": len(rs),
                "c0_set": sorted({int(r["c0"]) for r in rs}),
                "c1_distinct": len({int(r["c1"]) for r in rs}),
                "err_med": round(statistics.median(errs), 3) if errs else None,
                "err_min": round(min(errs), 3) if errs else None,
                "err_max": round(max(errs), 3) if errs else None,
                "spd_med": round(statistics.median([_f(r, "rec_9e4") for r in rs]), 1),
                "curv_med": round(statistics.median([_f(r, "curv") for r in rs]), 2),
                "mode_set": sorted({int(r["ai_mode"]) for r in rs}),
                "ovr_nonzero": sum(int(r["ai_override"]) != 0 for r in rs),
            }
        out["split"][side] = {
            "start_index": i0, "n": len(w),
            "c0_distinct": len({int(r["c0"]) for r in w}),
            "c0_set": sorted({int(r["c0"]) for r in w}),
            "labels": det,
        }
    return out


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--part", choices=["A", "B", "AB"], default="AB")
    ap.add_argument("--car", type=int, default=1)
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args(argv)
    orig, port = load(a.orig), load(a.port)
    res = {"orig": a.orig, "port": a.port}
    if a.part in ("A", "AB"):
        res["A"] = part_a(orig, port)
    if a.part in ("B", "AB"):
        res["B"] = part_b(orig, port, a.car)
    print(json.dumps(res, indent=1))


if __name__ == "__main__":
    main()
