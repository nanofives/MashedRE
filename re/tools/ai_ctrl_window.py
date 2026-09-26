"""D3 AI criterion (b) comparison on the RACING WINDOW.

Window, per car v: the first N calls (default 220) starting at the first call where
accel c4 != 0 (the race start; before it the original steps the AI through the countdown
with an all-zero block, ~790 calls on Training). 220 = the shortest racing span the
original gives any car on the recipe (cars 1/2 stop being stepped ~220 calls after the
start, D3_AI_TICK_WIRING section 4.3).

Per car prints: steer = c1 - c0 distinct count and median, |steer| median, accel c4
distinct set + fraction 255, brake c5 distinct set + fraction 255.

Usage: py -3.12 re/tools/ai_ctrl_window.py <csv> [<csv> ...] [--n 220] [--json] [--check]

--check applies the D3 AI criterion (b) tolerance written into ROADMAP.md section D3
(2026-09-26, before any post-port capture): each car must fall inside the envelope of the
ORIGINAL's 12 observations (4 runs x cars 1..3) in verify/d3_ai_20260914 + d3_ai_20260926.
"""
import csv, json, statistics, sys

def window_stats(path, n):
    rows = list(csv.DictReader(open(path, newline="")))
    out = {}
    for v in sorted({int(r["v"]) for r in rows}):
        r = [x for x in rows if int(x["v"]) == v]
        i0 = next((i for i, x in enumerate(r) if int(x["c4"]) != 0), None)
        if i0 is None:
            out[v] = {"calls": 0}
            continue
        w = r[i0:i0 + n]
        st = [int(x["c1"]) - int(x["c0"]) for x in w]
        a = [int(x["c4"]) for x in w]
        b = [int(x["c5"]) for x in w]
        c0 = [int(x["c0"]) for x in w]
        c1 = [int(x["c1"]) for x in w]
        out[v] = {
            "c0_distinct": len(set(c0)), "c0_median": statistics.median(c0),
            "c1_distinct": len(set(c1)), "c1_median": statistics.median(c1),
            "accel_distinct": len(set(a)), "accel_median": statistics.median(a),
            "brake_distinct": len(set(b)), "brake_median": statistics.median(b),
            "start_frame": int(r[i0]["frame"]), "calls": len(w),
            "steer_distinct": len(set(st)), "steer_median": statistics.median(st),
            "abs_steer_median": statistics.median([abs(s) for s in st]),
            "accel_set": sorted(set(a)), "accel255": round(sum(x == 255 for x in a) / len(a), 3),
            "brake_set": sorted(set(b)), "brake255": round(sum(x == 255 for x in b) / len(b), 3),
        }
    return out

# ROADMAP.md section D3 AI (b) tolerance. Envelope = min/max over the original's
# 4 runs x 3 cars (verify/d3_ai_20260914/orig_step_slots + verify/d3_ai_20260926/o_spread1..3).
TOLERANCE = {
    "c0_distinct": (13, 37), "c1_distinct": (17, 70), "steer_distinct": (29, 96),
    "c0_median": (0, 0), "c1_median": (0, 0), "abs_steer_median": (0, 23),
    "accel_distinct": (2, 4), "accel_median": (25, 255),
    "brake_distinct": (2, 2), "brake_median": (0, 0),
}

def check(s):
    fails = []
    for k, (lo, hi) in TOLERANCE.items():
        if not (lo <= s[k] <= hi):
            fails.append("%s=%s not in [%s,%s]" % (k, s[k], lo, hi))
    return fails

def main(argv):
    n = 220
    if "--n" in argv:
        n = int(argv[argv.index("--n") + 1])
    paths = [a for i, a in enumerate(argv) if not a.startswith("--") and (i == 0 or argv[i-1] != "--n")]
    res = {p: window_stats(p, n) for p in paths}
    if "--json" in argv:
        print(json.dumps(res, indent=1)); return
    for p, cars in res.items():
        print(p)
        for v, s in cars.items():
            if not s["calls"]:
                print("  v%d  no call with c4 != 0" % v); continue
            print("  v%d start=%d n=%d steerD=%d steerMed=%s |steer|Med=%s accel=%s a255=%.3f brake=%s b255=%.3f" % (
                v, s["start_frame"], s["calls"], s["steer_distinct"], s["steer_median"],
                s["abs_steer_median"], s["accel_set"], s["accel255"], s["brake_set"], s["brake255"]))
            if "--check" in argv:
                f = check(s)
                print("      (b) %s%s" % ("PASS" if not f else "FAIL: ", "; ".join(f)))

if __name__ == "__main__":
    main(sys.argv[1:])
