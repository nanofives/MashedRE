"""D3 AI criterion (e) -- AI-car speed response on the RACING WINDOW.

Metric definitions (fixed here, applied identically to original and standalone):

  window       per car v, the first N=220 calls starting at the first call with
               c4 != 0. Identical definition to re/tools/ai_ctrl_window.py, which
               is where criterion (b) is scored.
  ft_median    median of rec_9e4 over the window calls that are FULL THROTTLE and
               NOT braking, i.e. c4 == 255 and c5 == 0. This is the conditional
               median of D3_SPEED_GAP_2026-09-28.md section 2.1 control B. It is a
               matched-ctrl statistic: both sides are restricted to the calls where
               they issue the same two bytes.
  launch       rec_9e4 at window index 11 minus rec_9e4 at window index 0, i.e. the
               speed gained over the first 11 calls of the racing window. This is
               the launch statistic of section 2.3, stated here as an explicit
               endpoint difference so it is reproducible without rounding.
  ft_median_m0 the same conditional median, restricted to the MODE-CLEAN span: window
               calls [0, k) where k is the index of the ORIGINAL's first call with
               ai_mode != 0. k is a per-car constant of the recipe, measured at
               100 / 23 / 39 for cars 1 / 2 / 3, identical on all 8 committed
               non-flip originals and on the 4 same-day ones. Outside that span the
               original's speed history has passed through behaviour modes 3 and 7,
               which the standalone cannot have while residue D3-R1 is open, so the
               full-window ft_median is not a physics-only comparison and this one is.
  ft_n         number of calls the ft_median is taken over (reported, not gated).
  wmax         max rec_9e4 over the window (reported, not gated).

Usage:
  py -3.12 re/tools/ai_speed_env.py <csv> [<csv> ...]              # per-run table
  py -3.12 re/tools/ai_speed_env.py --envelope <csv> ...           # min/max per car
  py -3.12 re/tools/ai_speed_env.py --check <csv> ...              # vs ENVELOPE below
  ... [--n 220] [--json]
"""
import csv, json, statistics, sys

N_DEFAULT = 220
LAUNCH_IDX = 11

# Index of the ORIGINAL's first ai_mode != 0 call inside the window, per car.
# Measured, identical on all 12 original captures (8 committed non-flip + e1..e4).
MODE_CLEAN_K = {1: 100, 2: 23, 3: 39}


def run_stats(path, n=N_DEFAULT):
    rows = list(csv.DictReader(open(path, newline="")))
    out = {}
    for v in sorted({int(r["v"]) for r in rows}):
        r = [x for x in rows if int(x["v"]) == v]
        i0 = next((i for i, x in enumerate(r) if int(x["c4"]) != 0), None)
        if i0 is None:
            out[v] = {"calls": 0}
            continue
        w = r[i0:i0 + n]
        sp = [float(x["rec_9e4"]) for x in w]
        ft = [float(x["rec_9e4"]) for x in w
              if int(x["c4"]) == 255 and int(x["c5"]) == 0]
        k = MODE_CLEAN_K.get(v, len(w))
        ft0 = [float(x["rec_9e4"]) for x in w[:k]
               if int(x["c4"]) == 255 and int(x["c5"]) == 0]
        # k as this run's own original would report it (0 on a mode-0-only run)
        kown = next((i for i, x in enumerate(w) if int(x["ai_mode"]) != 0), len(w))
        out[v] = {
            "calls": len(w),
            "start_frame": int(r[i0]["frame"]),
            "ft_n": len(ft),
            "ft_median": round(statistics.median(ft), 1) if ft else None,
            "ft_m0_n": len(ft0),
            "ft_median_m0": round(statistics.median(ft0), 1) if ft0 else None,
            "launch": round(sp[min(LAUNCH_IDX, len(sp) - 1)] - sp[0], 1),
            "wmax": round(max(sp), 1),
            "k_own": kown,
            # DAT_0089a368 takes 0, 1 and 2 on this recipe. The reference regime for
            # criterion (e) is flag == 0 on EVERY window call, which is the regime the
            # 8 committed non-flip originals and every standalone capture sit in.
            "flagset": sorted({int(x["flag_a368"]) for x in w}),
            "regime0": all(int(x["flag_a368"]) == 0 for x in w),
        }
    return out


def envelope(paths, n=N_DEFAULT):
    per = {p: run_stats(p, n) for p in paths}
    env = {}
    for p, cars in per.items():
        for v, s in cars.items():
            if not s["calls"]:
                continue
            e = env.setdefault(v, {})
            for k in ("ft_median", "ft_median_m0", "launch", "ft_n", "wmax"):
                if s[k] is None:
                    continue
                lo, hi = e.get(k, (s[k], s[k]))
                e[k] = (min(lo, s[k]), max(hi, s[k]))
    return per, env


# ---------------------------------------------------------------------------
# Criterion (e) gate. Written into ROADMAP.md section D3 on 2026-09-28 from the
# ORIGINAL's 8 committed non-flip captures of the speed-gap recipe
# (verify/d3_ai_20260927b/p{1,2,3,4,8,10,11,12}.msd.aistep.csv) plus the 4 same-day
# originals verify/d3_drive_20260928/e{1,2,3,4}.msd.aistep.csv -- 12 runs x cars 1..3 --
# BEFORE any post-fix standalone capture. See re/analysis/D3_DRIVE_2026-09-28.md section 2.
#
# REFERENCE = the original's measured value. Its run-to-run spread over those 12 runs
# is EXACTLY ZERO on every gated statistic, so the band cannot be taken from the spread.
# BAND_PCT is therefore inherited, not measured: 2% is the speed bound already on record
# for this project (U9138_FIX_2026-09-28.md section 5 "driving-median speed: within 2%",
# "max speed: within 2%"), and the D2 closure itself accepted 1887 vs 1901 = 0.74%.
# GATED: launch, ft_median_m0.  REPORTED, NOT GATED: ft_median (see module docstring).
REFERENCE = {
    1: {"launch": 1425.7, "ft_median_m0": 2551.6, "ft_median": 2254.8},
    2: {"launch": 2052.5, "ft_median_m0": 2052.5, "ft_median": 2648.1},
    3: {"launch": 2055.0, "ft_median_m0": 2278.1, "ft_median": 2691.4},
}
BAND_PCT = 2.0
GATED = ("launch", "ft_median_m0")


def check(cars):
    res = {}
    for v, s in cars.items():
        if v not in REFERENCE:
            continue
        if not s["calls"]:
            res[v] = ["no call with c4 != 0"]
            continue
        fails = []
        for k in GATED:
            ref = REFERENCE[v][k]
            lo, hi = ref * (1 - BAND_PCT / 100), ref * (1 + BAND_PCT / 100)
            got = s[k]
            if got is None or not (lo <= got <= hi):
                fails.append("%s=%s not in [%.1f,%.1f] (ref %.1f, %+.1f%%)"
                             % (k, got, lo, hi, ref,
                                100.0 * (got - ref) / ref if got is not None else float("nan")))
        res[v] = fails
    return res


def main(argv):
    n = N_DEFAULT
    if "--n" in argv:
        n = int(argv[argv.index("--n") + 1])
    paths = [a for i, a in enumerate(argv)
             if not a.startswith("--") and (i == 0 or argv[i - 1] != "--n")]
    per, env = envelope(paths, n)
    if "--json" in argv:
        print(json.dumps({"per_run": {p: {str(k): v for k, v in c.items()}
                                      for p, c in per.items()},
                          "envelope": {str(k): v for k, v in env.items()}}, indent=1))
        return
    for p, cars in per.items():
        print(p)
        for v, s in cars.items():
            if not s["calls"]:
                print("  v%d  no call with c4 != 0" % v)
                continue
            print("  v%d start=%d n=%d flag=%s regime0=%d k_own=%d launch=%s "
                  "ft_median_m0=%s (n=%d) ft_median=%s (n=%d) wmax=%s"
                  % (v, s["start_frame"], s["calls"], s["flagset"], s["regime0"],
                     s["k_own"], s["launch"], s["ft_median_m0"], s["ft_m0_n"],
                     s["ft_median"], s["ft_n"], s["wmax"]))
        if "--check" in argv:
            for v, f in check(cars).items():
                print("      v%d (e) %s%s" % (v, "PASS" if not f else "FAIL: ", "; ".join(f)))
    if "--envelope" in argv:
        print("\nENVELOPE over %d run(s)" % len(paths))
        for v in sorted(env):
            e = env[v]
            print("  v%d launch %s..%s  ft_median_m0 %s..%s  ft_median %s..%s  "
                  "(ft_n %s..%s, wmax %s..%s)"
                  % (v, e["launch"][0], e["launch"][1],
                     e["ft_median_m0"][0], e["ft_median_m0"][1],
                     e["ft_median"][0], e["ft_median"][1], e["ft_n"][0], e["ft_n"][1],
                     e["wmax"][0], e["wmax"][1]))


if __name__ == "__main__":
    main(sys.argv[1:])
