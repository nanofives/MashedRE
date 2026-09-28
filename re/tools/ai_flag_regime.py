"""D3 AI criterion (b), split by the DAT_0089a368 regime.

Companion to re/tools/ai_ctrl_window.py. Same racing window (per car v, the first N
calls from the first call with c4 != 0; N default 220) and the SAME ten band columns,
but each window is cut into its `flag_a368 == 0` part and its `flag_a368 == 1` part and
scored separately.

Why: the flag switches the spline bank, the curvature, the steer multiplier
(0x0041665c) and the accel byte (0x004169e0) (D3_AI_RESIDUE_2026-09-27.md section 3),
so the pooled 12-observation envelope in ai_ctrl_window.TOLERANCE may be the union of
two populations rather than one.

The *_distinct columns grow with the row count, so every observation inside one regime
envelope is measured over the SAME number of calls K_r (--k0 / --k1, or the minimum
qualifying sub-window length when not given). An observation qualifies for a regime only
if it has at least --minrows calls of it (default 30).

Usage:
  py -3.12 re/tools/ai_flag_regime.py --envelope <orig.aistep.csv> [...]     # build
  py -3.12 re/tools/ai_flag_regime.py --envelope <orig...> --score <sa.csv> [...]
  add --json for machine output, --n / --k0 / --k1 / --minrows to override.
"""
import csv, json, statistics, sys

sys.path.insert(0, __file__.rsplit("\\", 1)[0].rsplit("/", 1)[0])
from ai_ctrl_window import TOLERANCE  # the pooled envelope, for reference only

BANDS = list(TOLERANCE)  # the ten (b) columns, in ai_ctrl_window's order


def stats(rows):
    """The ten (b) columns of ai_ctrl_window.window_stats, on an arbitrary row list."""
    c0 = [int(x["c0"]) for x in rows]
    c1 = [int(x["c1"]) for x in rows]
    st = [b - a for a, b in zip(c0, c1)]
    a = [int(x["c4"]) for x in rows]
    b = [int(x["c5"]) for x in rows]
    return {
        "c0_distinct": len(set(c0)), "c0_median": statistics.median(c0),
        "c1_distinct": len(set(c1)), "c1_median": statistics.median(c1),
        "steer_distinct": len(set(st)),
        "abs_steer_median": statistics.median([abs(s) for s in st]),
        "accel_distinct": len(set(a)), "accel_median": statistics.median(a),
        "brake_distinct": len(set(b)), "brake_median": statistics.median(b),
    }


def speed(rows):
    s = sorted(int(float(x["rec_9e4"])) for x in rows)
    q = lambda p: s[min(len(s) - 1, int(p * (len(s) - 1)))]
    return {"min": s[0], "p05": q(0.05), "median": statistics.median(s),
            "p95": q(0.95), "max": s[-1]}


def windows(path, n):
    """path -> {car: {'rows': window rows, 'flag': [per-row flag]}}"""
    rows = list(csv.DictReader(open(path, newline="")))
    out = {}
    for v in sorted({int(r["v"]) for r in rows}):
        r = [x for x in rows if int(x["v"]) == v]
        i0 = next((i for i, x in enumerate(r) if int(x["c4"]) != 0), None)
        if i0 is None:
            continue
        w = r[i0:i0 + n]
        out[v] = {"rows": w, "flag": [int(x["flag_a368"]) for x in w],
                  "start_frame": int(r[i0]["frame"]), "calls": len(w)}
    return out


def split(w, regime):
    return [x for x, f in zip(w["rows"], w["flag"]) if f == regime]


def build(paths, n, minrows, kfix, pure=False):
    """-> {regime: {'k': K, 'obs': {(path,car): stats}, 'env': {col: (lo,hi)},
                    'speed': {(path,car): speed}}}

    pure=True keeps only windows that are 100% one regime, so K = the full window and
    the envelope is directly comparable to ai_ctrl_window.TOLERANCE (same N=220).
    """
    per = {0: {}, 1: {}}
    for p in paths:
        for v, w in windows(p, n).items():
            for r in (0, 1):
                sub = split(w, r)
                if pure:
                    if len(sub) == w["calls"] and w["calls"] >= minrows:
                        per[r][(p, v)] = sub
                elif len(sub) >= minrows:
                    per[r][(p, v)] = sub
    out = {}
    for r in (0, 1):
        if not per[r]:
            out[r] = {"k": 0, "obs": {}, "env": {}, "speed": {}, "n_obs": 0}
            continue
        k = kfix[r] or min(len(s) for s in per[r].values())
        obs = {key: stats(s[:k]) for key, s in per[r].items()}
        env = {c: (min(o[c] for o in obs.values()), max(o[c] for o in obs.values()))
               for c in BANDS}
        spd = {key: speed(s[:k]) for key, s in per[r].items()}
        out[r] = {"k": k, "obs": obs, "env": env, "speed": spd, "n_obs": len(obs)}
    return out


def score(sub, env, k):
    """sub = the standalone's rows of one regime. -> (fails, stats) or None."""
    if len(sub) < k:
        return None
    s = stats(sub[:k])
    fails = ["%s=%s not in [%s,%s]" % (c, s[c], env[c][0], env[c][1])
             for c in BANDS if not (env[c][0] <= s[c] <= env[c][1])]
    return fails, s


def main(argv):
    n = int(argv[argv.index("--n") + 1]) if "--n" in argv else 220
    minrows = int(argv[argv.index("--minrows") + 1]) if "--minrows" in argv else 30
    kfix = {0: int(argv[argv.index("--k0") + 1]) if "--k0" in argv else 0,
            1: int(argv[argv.index("--k1") + 1]) if "--k1" in argv else 0}

    def group(flag):
        if flag not in argv:
            return []
        out = []
        for a in argv[argv.index(flag) + 1:]:
            if a.startswith("--"):
                break
            out.append(a)
        return out

    env_paths, sc_paths = group("--envelope"), group("--score")
    pure = "--pure" in argv
    E = build(env_paths, n, minrows, kfix, pure)

    result = {"n": n, "minrows": minrows, "pure": pure,
              "envelope_paths": env_paths, "regimes": {},
              "scored": {}, "flag1_runs": {}}
    for r in (0, 1):
        result["regimes"][r] = {
            "k": E[r]["k"], "n_obs": E[r]["n_obs"], "env": E[r]["env"],
            "obs": {"%s|v%d" % (p.replace("\\", "/").split("/")[-1], v): o
                    for (p, v), o in E[r]["obs"].items()},
            "speed": {"%s|v%d" % (p.replace("\\", "/").split("/")[-1], v): s
                      for (p, v), s in E[r]["speed"].items()},
        }
    # flag-1 reach, per source window (the roll is once per RUN; per-car is reported
    # so the "all cars flip together" claim can be re-checked, not assumed).
    for p in env_paths:
        for v, w in windows(p, n).items():
            result["flag1_runs"].setdefault(p.replace("\\", "/").split("/")[-1], {})[
                "v%d" % v] = {"calls": w["calls"], "flag0": w["flag"].count(0),
                              "flag1": w["flag"].count(1),
                              "other": sorted(set(w["flag"]) - {0, 1})}

    for p in sc_paths:
        key = p.replace("\\", "/").split("/")[-1]
        result["scored"][key] = {}
        for v, w in windows(p, n).items():
            e = {}
            for r in (0, 1):
                sub = split(w, r)
                if not E[r]["env"]:
                    e["regime%d" % r] = {"verdict": "NO_ENVELOPE"}
                    continue
                sc = score(sub, E[r]["env"], E[r]["k"])
                if sc is None:
                    e["regime%d" % r] = {"verdict": "NOT_IN_REGIME",
                                         "rows_available": len(sub),
                                         "rows_needed": E[r]["k"]}
                else:
                    fails, s = sc
                    e["regime%d" % r] = {"verdict": "PASS" if not fails else "FAIL",
                                         "fails": fails, "stats": s,
                                         "speed": speed(sub[:E[r]["k"]])}
            e["flag_counts"] = {"flag0": w["flag"].count(0), "flag1": w["flag"].count(1)}
            result["scored"][key]["v%d" % v] = e

    if "--json" in argv:
        print(json.dumps(result, indent=1, default=str)); return

    for r in (0, 1):
        R = result["regimes"][r]
        print("== regime flag_a368 == %d :  K=%d  observations=%d" % (r, R["k"], R["n_obs"]))
        for c in BANDS:
            if R["env"]:
                print("   %-18s %s..%s   (pooled %s..%s)" % (
                    c, R["env"][c][0], R["env"][c][1], TOLERANCE[c][0], TOLERANCE[c][1]))
        print()
    for key, cars in result["scored"].items():
        print(key)
        for v, e in cars.items():
            print("  %s  flags=%s" % (v, e["flag_counts"]))
            for r in (0, 1):
                d = e["regime%d" % r]
                print("     vs regime%d: %s %s" % (r, d["verdict"],
                                                   "; ".join(d.get("fails", []))))


if __name__ == "__main__":
    main(sys.argv[1:])
