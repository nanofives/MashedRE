#!/usr/bin/env python3
"""U-9196: is the car<->car (b) movement a genuine per-car steering match, or a shallow
median coincidence?

Leg 3 (verify/d3_carcar_20261005/RESULT_LEG3.md) wired 0x00469df0 and criterion (b) went
13 -> 5 failing bands, with cars 1 and 3's abs_steer_median entering the original's [0,23]
envelope. The (b) gate checks the MEDIAN and a distinct-count; it does not check that the
port's per-frame steering DISTRIBUTION matches the original's. This tool does.

The original (o_t1/o_t2/o_t3, deterministic -- identical to the integer across all three)
steers BIMODALLY: a low median with a fat tail of full-rail corrections. abs(c1-c0) over the
220-call window:
    car 1  med=7   p75=10  p90=255  >128: 28/220   distinct=33
    car 3  med=23  p75=80  p90=182  >128: 33/220   distinct=96
A genuine match reproduces that low-median-plus-correction-tail shape. A SHALLOW pass has a
low median but NO correction tail (p90 low, rail-fraction near 0) -- smooth-but-not-cornering,
a different behaviour that happens to share the median.

Window = ai_ctrl_window / ai_speed_env's: the first N=220 calls from the first c4 != 0, per
car v. Identical definition, so the population is the one (b) is scored on.

Usage:
  py -3.12 re/tools/ai_steerdist.py --orig <orig.csv> --port <port.csv> [--cars 1,3]
                                    [--n 220] [--json]

No game run, no Ghidra, no build. Reads committed .aistep CSVs only.
"""
import argparse
import csv
import json
import statistics
import sys

N_DEFAULT = 220
RAIL = 128   # abs(c1-c0) > RAIL == a near-full-rail steering correction


def window_steer(path, v, n):
    rows = list(csv.DictReader(open(path, newline="")))
    r = [x for x in rows if int(x["v"]) == v]
    i0 = next((i for i, x in enumerate(r) if int(x["c4"]) != 0), None)
    if i0 is None:
        return None
    w = r[i0:i0 + n]
    st = [int(x["c1"]) - int(x["c0"]) for x in w]
    a = sorted(abs(s) for s in st)
    if not a:
        return None

    def q(p):
        return a[int(p * (len(a) - 1))]

    rail = sum(1 for s in a if s > RAIL)
    return {
        "n": len(a),
        "median": statistics.median(a),
        "p25": q(0.25), "p75": q(0.75), "p90": q(0.90),
        "max": max(a),
        "distinct": len(set(st)),
        "rail_frac": rail / len(a),
        "rail_n": rail,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--cars", default="1,3")
    ap.add_argument("--n", type=int, default=N_DEFAULT)
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()
    cars = [int(x) for x in a.cars.split(",")]

    out = {"orig": a.orig, "port": a.port, "cars": {}}
    for v in cars:
        o = window_steer(a.orig, v, a.n)
        p = window_steer(a.port, v, a.n)
        if o is None or p is None:
            out["cars"][v] = {"error": "missing car %d" % v}
            continue
        # ---- pre-registered gates (PREREG_U9196.md), NOT editable after a run ----
        # G-SHAPE-MED: |port.median - orig.median| <= 8  (the (b) envelope [0,23] is 23 wide;
        #              8 is roughly a third of it -- a loose "same neighbourhood" test).
        g_med = abs(p["median"] - o["median"]) <= 8
        # G-SHAPE-P90: port.p90 >= 90.  The original's p90 is 182/255 -- a real driver makes
        #              occasional rail corrections. A port with p90 < 90 has NO tail.
        g_p90 = p["p90"] >= 90
        # G-SHAPE-TAIL: port.rail_frac in [orig.rail_frac/3, orig.rail_frac*3].  Does the port
        #              make rail corrections at a comparable rate (the fat tail exists, and is
        #              neither absent nor wildly inflated)?
        lo, hi = o["rail_frac"] / 3.0, o["rail_frac"] * 3.0
        g_tail = lo <= p["rail_frac"] <= hi
        verdict = "GENUINE" if (g_med and g_p90 and g_tail) else "SHALLOW"
        out["cars"][v] = {
            "orig": o, "port": p,
            "G-SHAPE-MED": {"pass": g_med, "d": abs(p["median"] - o["median"]), "thresh": 8},
            "G-SHAPE-P90": {"pass": g_p90, "port_p90": p["p90"], "thresh": 90},
            "G-SHAPE-TAIL": {"pass": g_tail, "port_frac": round(p["rail_frac"], 4),
                             "orig_frac": round(o["rail_frac"], 4),
                             "band": [round(lo, 4), round(hi, 4)]},
            "verdict": verdict,
        }

    # overall: CONFIRMED iff every requested car is GENUINE; KILLED iff none; else PARTIAL
    verdicts = [c.get("verdict") for c in out["cars"].values() if "verdict" in c]
    if verdicts and all(x == "GENUINE" for x in verdicts):
        out["overall"] = "CONFIRMED"
    elif verdicts and all(x == "SHALLOW" for x in verdicts):
        out["overall"] = "KILLED"
    else:
        out["overall"] = "PARTIAL"

    if a.json:
        print(json.dumps(out, indent=2))
    else:
        for v, c in out["cars"].items():
            if "error" in c:
                print("car%d: %s" % (v, c["error"]))
                continue
            o, p = c["orig"], c["port"]
            print("car%d  (window n: orig %d / port %d)" % (v, o["n"], p["n"]))
            print("        median   p75   p90   max   distinct   rail>%d" % RAIL)
            print("  orig  %6d %5d %5d %5d %9d   %d/%d=%.3f"
                  % (o["median"], o["p75"], o["p90"], o["max"], o["distinct"],
                     o["rail_n"], o["n"], o["rail_frac"]))
            print("  port  %6d %5d %5d %5d %9d   %d/%d=%.3f"
                  % (p["median"], p["p75"], p["p90"], p["max"], p["distinct"],
                     p["rail_n"], p["n"], p["rail_frac"]))
            print("  G-SHAPE-MED  |%d-%d|=%d <= 8        -> %s"
                  % (p["median"], o["median"], abs(p["median"] - o["median"]),
                     "PASS" if c["G-SHAPE-MED"]["pass"] else "FAIL"))
            print("  G-SHAPE-P90  port p90 %d >= 90       -> %s"
                  % (p["p90"], "PASS" if c["G-SHAPE-P90"]["pass"] else "FAIL"))
            print("  G-SHAPE-TAIL port frac %.3f in [%.3f,%.3f] -> %s"
                  % (p["rail_frac"], c["G-SHAPE-TAIL"]["band"][0],
                     c["G-SHAPE-TAIL"]["band"][1],
                     "PASS" if c["G-SHAPE-TAIL"]["pass"] else "FAIL"))
            print("  -> car%d %s" % (v, c["verdict"]))
            print()
        print("OVERALL: %s" % out["overall"])
    return 0


if __name__ == "__main__":
    sys.exit(main())
