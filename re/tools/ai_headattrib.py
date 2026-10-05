"""U-9185 item (b): re-attribute the matched-position BODY-HEADING share.

Pre-registration: verify/d3_headattrib_20261005/PREREG_HEADATTRIB.md. OFFLINE and READ-ONLY
on committed captures -- no game run, no injection, no build.

WHY THIS TOOL EXISTS, and why it is not a patch to ai_posmatch.py.

`ai_posmatch.py --part COLL` reports a three-way split of the matched-position steering-error
difference into `d_target_dir`, `d_body_heading` and `d_err`, and U-9185 cites the heading
term (0.9796 / 0.9085 / 0.6506 deg on cars 1/2/3) as the carrier of criterion (b). Two
problems, both found by reading that code rather than by measuring:

  1. `d_body_heading` is NOT an independent measurement. ai_posmatch.py:411 computes
         hp, ho = tp - (ep % WRAP), to - (eo % WRAP)
     so `d_body_heading == wrap180(d_target_dir - d_err)` identically. There is one measured
     angular series (`signed_err`) plus a positional one. The split is an ALGEBRAIC IDENTITY.

  2. `signed_err` comes from `hist_d8` / `hist_dc`, which U-9186 established are the
     steer-history stores at 0x004165cc / 0x0041670c and are FROZEN on the 100 of 660 window
     calls where the ORIGINAL takes an early return. The PORT pins its mode to 0 and has
     neither early return, so its value is fresh on every call. The contamination is
     ONE-SIDED, which is the shape that manufactures a defect on the side that has it.

So this tool re-scores the same matched pairs with the contamination removed, and separately
conditions on a matched steering command. `ai_posmatch.py` is deliberately left UNMODIFIED so
its committed results stay reproducible; every shared primitive is imported from it rather
than re-derived, so the pairing cannot drift between the two tools.

The FROZEN detector is deliberately SYMMETRIC and empirical -- a call is frozen when its
(hist_d8, hist_dc) pair is bit-identical to the previous logged call for the same car -- so it
cannot be accused of being tuned to the side that has the early returns. It is cross-validated
against the original's own `ret14a70` column where that column exists (Format D captures).

Usage:
  py -3.12 re/tools/ai_headattrib.py --orig <orig.aistep.csv> --port <port.aistep.csv>
           [--radius 0.12] [--json] [--min-n 30]

Writes nothing. Prints the gate table, or JSON with --json.
"""
import argparse
import csv
import json
import math
import statistics
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

# Every shared primitive comes from ai_posmatch.py so the two tools cannot diverge.
from ai_posmatch import (  # noqa: E402
    R_MATCH, WRAP, _ang_of, _f, _wrap180, signed_err, window,
)

MIN_N = 30          # PREREG section 4: below this a car is NO-VERDICT, not a pass or a fail
# PREREG section 4 thresholds. Between PASS and FAIL the verdict is INCONCLUSIVE.
T_PASS = 0.25       # deg -- artifact
T_FAIL = 0.50       # deg -- real
# U-9185's published medians, fixed before this ran; used only for reporting the reduction.
PUBLISHED = {1: 0.9796, 2: 0.9085, 3: 0.6506}
# PREREG section 4b, tightest first. Relative tolerance on the ORIGINAL's rec_9e4.
SPEED_TOLS = (0.01, 0.05, 0.10)


def load(path):
    with open(path, newline="", encoding="utf-8", errors="replace") as fh:
        return [r for r in csv.DictReader(fh) if r.get("v") not in (None, "")]


def pair_up(orig, port, v, radius):
    """Byte-for-byte the pairing in ai_posmatch.collateral(), same window, same R."""
    pw, _ = window(port, v)
    cand = defaultdict(list)
    for r in orig:
        if int(r["v"]) == v and r["own_x"] != "":
            cand[r["ai_spline_idx"]].append(r)
    pairs = []
    for pr in pw:
        if pr["own_x"] == "":
            continue
        px, pz = _f(pr, "own_x"), _f(pr, "own_z")
        best, bd = None, 1e9
        for orow in cand.get(pr["ai_spline_idx"], ()):
            dd = math.hypot(_f(orow, "own_x") - px, _f(orow, "own_z") - pz)
            if dd < bd:
                bd, best = dd, orow
        if best is not None and bd <= radius:
            pairs.append((pr, best))
    return pairs


def mark_frozen(rows):
    """FROZEN = (hist_d8, hist_dc) bit-identical to the previous logged call for the same car.

    Keyed by id() of the row dict, so a pair can look up either side's verdict without the
    detector depending on which side it came from. Applied identically to both sides.
    """
    frozen, prev = {}, {}
    for r in rows:
        v = r.get("v")
        key = (r.get("hist_d8", ""), r.get("hist_dc", ""))
        frozen[id(r)] = (v in prev and prev[v] == key and key != ("", ""))
        prev[v] = key
    return frozen


def decompose(pairs):
    """ai_posmatch._decompose's arithmetic, plus the identity residual it never checks."""
    dt, dh, de, ident = [], [], [], []
    for p, o in pairs:
        if "" in (p["look_x"], o["look_x"]):
            continue
        ep, eo = signed_err(p), signed_err(o)
        if ep is None or eo is None:
            continue
        tp = _ang_of(_f(p, "look_x") - _f(p, "own_x"), -(_f(p, "look_z") - _f(p, "own_z")))
        to = _ang_of(_f(o, "look_x") - _f(o, "own_x"), -(_f(o, "look_z") - _f(o, "own_z")))
        hp, ho = tp - (ep % WRAP), to - (eo % WRAP)
        a, b, c = _wrap180(tp - to), _wrap180(hp - ho), _wrap180(ep - eo)
        dt.append(a)
        dh.append(b)
        de.append(c)
        # G-IDENT: d_body_heading must equal wrap180(d_target_dir - d_err) exactly.
        # Both sides are wrapped the same way, so compare on the circle, not on the line.
        ident.append(abs(_wrap180(b - _wrap180(a - c))))
    if not dt:
        return None
    return {"n": len(dt), "d_target_dir": stat(dt), "d_body_heading": stat(dh),
            "d_err": stat(de), "identity_max_resid": max(ident)}


def stat(x):
    s = sorted(x)
    return {"median": round(statistics.median(s), 4),
            "median_abs": round(statistics.median([abs(y) for y in s]), 4),
            "p90_abs": round(sorted(abs(y) for y in s)[int(0.9 * (len(s) - 1))], 4)}


def verdict(med_abs, n, min_n):
    if n < min_n:
        return "NO-VERDICT (n<%d)" % min_n
    if med_abs < T_PASS:
        return "ARTIFACT"
    if med_abs > T_FAIL:
        return "REAL"
    return "INCONCLUSIVE"


def run(orig_rows, port_rows, radius, min_n):
    fo, fp = mark_frozen(orig_rows), mark_frozen(port_rows)
    out = {"R": radius, "min_n": min_n,
           "thresholds": {"pass_below": T_PASS, "fail_above": T_FAIL},
           "prereg": "verify/d3_headattrib_20261005/PREREG_HEADATTRIB.md", "cars": {}}
    for v in (1, 2, 3):
        pairs = pair_up(orig_rows, port_rows, v, radius)
        clean = [(p, o) for p, o in pairs if not fp[id(p)] and not fo[id(o)]]
        # G-CMD: identical commanded steer on both sides.
        cmd = [(p, o) for p, o in clean
               if p["c0"] != "" and o["c0"] != ""
               and int(p["c0"]) == int(o["c0"]) and int(p["c1"]) == int(o["c1"])]
        # Leg 2b (PREREG section 4b): matched position AND identical (c0,c1) AND matched
        # speed. Tolerance ladder fixed in the pre-registration so it cannot be fitted to
        # the answer; the verdict is taken at the TIGHTEST tolerance reaching n >= min_n.
        speed = {}
        for tol in SPEED_TOLS:
            sel = []
            for p, o in cmd:
                if p["rec_9e4"] == "" or o["rec_9e4"] == "":
                    continue
                so = _f(o, "rec_9e4")
                if so == 0.0:
                    continue
                if abs(_f(p, "rec_9e4") - so) / abs(so) <= tol:
                    sel.append((p, o))
            speed["tol_%g" % tol] = decompose(sel)
        chosen, chosen_tol = None, None
        for tol in SPEED_TOLS:                     # tightest first
            d = speed["tol_%g" % tol]
            if d and d["n"] >= min_n:
                chosen, chosen_tol = d, tol
                break
        no, npo = sum(fo[id(o)] for _, o in pairs), sum(fp[id(p)] for p, _ in pairs)
        # G-DETECT: does FROZEN cover the original's own early-return column?
        # Distinguish "capture has no such column" from "column present, no qualifying row".
        # Collapsing the two printed a false "not cross-validated" on cars 1 and 2, whose
        # captures DO carry ret14a70 and simply have no ret14a70==2 among the matched pairs.
        has_ret = bool(pairs) and "ret14a70" in pairs[0][1]
        cov = None
        if not has_ret:
            cov = {"status": "no ret14a70 column in this capture"}
        else:
            er = [o for _, o in pairs if o.get("ret14a70", "") not in ("", None)
                  and int(float(o["ret14a70"])) == 2]
            if not er:
                cov = {"status": "column present, 0 rows with ret14a70==2 in the matched "
                                 "population -- consistent with frozen=0, nothing to validate"}
            else:
                cov = {"status": "validated", "n_ret14a70_eq2": len(er),
                       "of_which_frozen": sum(fo[id(o)] for o in er),
                       "coverage": round(sum(fo[id(o)] for o in er) / len(er), 4)}
        rec = {
            "n_matched": len(pairs),
            "frozen_orig": no, "frozen_port": npo,
            "frozen_frac_orig": round(no / len(pairs), 4) if pairs else None,
            "frozen_frac_port": round(npo / len(pairs), 4) if pairs else None,
            "asymmetry_x": (round(no / npo, 2) if npo else ("inf" if no else None)),
            "detector_coverage": cov,
            "published_d_body_heading": PUBLISHED[v],
            "all_pairs": decompose(pairs),
            "frozen_excluded": decompose(clean),
            "cmd_matched": decompose(cmd),
            "speed_ladder": speed,
            "speed_chosen_tol": chosen_tol,
            "speed_chosen": chosen,
        }
        rec["cmd_speed_matched_verdict"] = (
            verdict(chosen["d_body_heading"]["median_abs"], chosen["n"], min_n) if chosen
            else "NO-VERDICT (no tolerance reached n>=%d)" % min_n)
        for k in ("frozen_excluded", "cmd_matched"):
            d = rec[k]
            rec[k + "_verdict"] = (verdict(d["d_body_heading"]["median_abs"], d["n"], min_n)
                                   if d else "NO-DATA")
        out["cars"][v] = rec
    return out


def report(o):
    print("U-9185 item (b) heading re-attribution -- PREREG %s" % o["prereg"])
    print("R=%s  min_n=%d  ARTIFACT<%s  REAL>%s deg\n"
          % (o["R"], o["min_n"], o["thresholds"]["pass_below"], o["thresholds"]["fail_above"]))
    for v in (1, 2, 3):
        c = o["cars"][v]
        print("=== car %d ===  matched pairs %d" % (v, c["n_matched"]))
        print("  G-ASYM   frozen: orig %d (%s)  port %d (%s)  asymmetry %sx"
              % (c["frozen_orig"], c["frozen_frac_orig"], c["frozen_port"],
                 c["frozen_frac_port"], c["asymmetry_x"]))
        d = c["detector_coverage"] or {"status": "no data"}
        if d.get("status") == "validated":
            print("  G-DETECT ret14a70==2 in matched pop: %d, of which FROZEN %d -> coverage %s"
                  % (d["n_ret14a70_eq2"], d["of_which_frozen"], d["coverage"]))
        else:
            print("  G-DETECT %s" % d["status"])
        for k, lbl in (("all_pairs", "all pairs      "),
                       ("frozen_excluded", "FROZEN excluded"),
                       ("cmd_matched", "(c0,c1) matched")):
            d = c[k]
            if not d:
                print("  %s  NO DATA" % lbl)
                continue
            print("  %s  n=%-4d d_body_heading med_abs=%-8s d_target_dir=%-8s d_err=%-8s"
                  % (lbl, d["n"], d["d_body_heading"]["median_abs"],
                     d["d_target_dir"]["median_abs"], d["d_err"]["median_abs"]))
        print("  G-IDENT  max identity residual = %.3e deg (PASS if < 1e-6)"
              % c["all_pairs"]["identity_max_resid"] if c["all_pairs"] else "  G-IDENT no data")
        print("  published %s -> frozen-excluded %s | verdict %s"
              % (c["published_d_body_heading"],
                 c["frozen_excluded"]["d_body_heading"]["median_abs"] if c["frozen_excluded"] else "-",
                 c["frozen_excluded_verdict"]))
        print("  G-CMD verdict %s" % c["cmd_matched_verdict"])
        for tol in SPEED_TOLS:
            d = c["speed_ladder"]["tol_%g" % tol]
            mark = " <- CHOSEN" if c["speed_chosen_tol"] == tol else ""
            print("  G-SPEED  +(c0,c1)+speed<=%4.0f%%  n=%-4s d_body_heading med_abs=%s%s"
                  % (tol * 100, d["n"] if d else 0,
                     d["d_body_heading"]["median_abs"] if d else "-", mark))
        print("  G-SPEED verdict %s\n" % c["cmd_speed_matched_verdict"])


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--radius", type=float, default=R_MATCH)
    ap.add_argument("--min-n", type=int, default=MIN_N)
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args(argv)
    out = run(load(a.orig), load(a.port), a.radius, a.min_n)
    out["orig"], out["port"] = a.orig, a.port
    if a.json:
        print(json.dumps(out, indent=1))
    else:
        report(out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
