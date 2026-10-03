"""D3 STEP 2 gates G2-MODE / G2-JOINT / G2-STEER — read-only, nothing is launched.

Splits an *.aistep.csv window by the committed behaviour mode `ai_mode`
(`DAT_0089a52c + v*0x74`, written at 0x00416590) and reports the statistics the
H-LINK hypothesis predicts. Window = the same 220 calls from the first `c4 != 0`
that re/tools/ai_ctrl_window.py uses, so every number is on criterion (b)'s own
population.

Median CALL INDEX is printed for every group: two groups inside one window are two
different moments, and a magnitude read across them needs that stated.

Usage:
  py -3.12 re/tools/ai_mode_split.py <csv> [<csv> ...] [--n 220] [--json]

Pre-registration: verify/d3_modes37_20261002/PREREG_STEP2.md.
"""
import csv, collections, json, statistics, sys

N_DEFAULT = 220
K20 = 20.0                       # _DAT_005ccd6c, the curv gate at 0x0041665c


def window(path, v, n):
    rows = [r for r in csv.DictReader(open(path, newline="")) if int(r["v"]) == v]
    i0 = next((i for i, x in enumerate(rows) if int(x["c4"]) != 0), None)
    if i0 is None:
        return None, None
    return rows[i0:i0 + n], i0


def steer(r):
    return int(r["c1"]) - int(r["c0"])


def group_stats(g, idx):
    """idx = the call indices (0-based within the window) of the rows in g."""
    if not g:
        return {"n": 0}
    curv = [float(r["curv"]) for r in g]
    return {
        "n": len(g),
        "median_call_index": statistics.median(idx),
        "abs_steer_median": statistics.median([abs(steer(r)) for r in g]),
        "c1_distinct": len(set(int(r["c1"]) for r in g)),
        "c0_distinct": len(set(int(r["c0"]) for r in g)),
        "curv_median": round(statistics.median(curv), 4),
        "frac_curv_gt20": round(sum(1 for c in curv if c > K20) / len(curv), 4),
        "speed_median": round(statistics.median([float(r["rec_9e4"]) for r in g]), 1),
    }


def main(argv):
    n = int(argv[argv.index("--n") + 1]) if "--n" in argv else N_DEFAULT
    as_json = "--json" in argv
    paths = [a for a in argv if not a.startswith("--") and not a.isdigit()]
    out = {}
    for p in paths:
        print(p)
        out[p] = {}
        for v in (1, 2, 3):
            w, i0 = window(p, v, n)
            if w is None:
                print("  v%d  NO WINDOW (no c4 != 0 row)" % v)
                continue
            modes = [int(r["ai_mode"]) for r in w]
            hist = dict(sorted(collections.Counter(modes).items()))

            # G2-JOINT: non-full-throttle == (c4 != 255) or (c5 != 0)
            nft = [(i, r) for i, r in enumerate(w)
                   if int(r["c4"]) != 255 or int(r["c5"]) != 0]
            nft_m0 = sum(1 for _, r in nft if int(r["ai_mode"]) == 0)
            nft_mx = len(nft) - nft_m0

            # G2-STEER: split by mode == 0
            g0 = [(i, r) for i, r in enumerate(w) if int(r["ai_mode"]) == 0]
            gx = [(i, r) for i, r in enumerate(w) if int(r["ai_mode"]) != 0]
            s0 = group_stats([r for _, r in g0], [i for i, _ in g0])
            sx = group_stats([r for _, r in gx], [i for i, _ in gx])

            print("  v%d start=%d n=%d  ai_mode hist=%s" % (v, i0, len(w), hist))
            print("     G2-JOINT non-full-throttle=%d  of which mode0=%d  mode!=0=%d"
                  % (len(nft), nft_m0, nft_mx))
            for tag, s in (("mode==0", s0), ("mode!=0", sx)):
                if s["n"] == 0:
                    print("     G2-STEER %-8s n=0" % tag); continue
                print("     G2-STEER %-8s n=%-3d medIdx=%-5.1f |steer|Med=%-6.1f "
                      "c1D=%-4d c0D=%-4d curvMed=%-9.4f curv>20=%.3f spdMed=%.1f"
                      % (tag, s["n"], s["median_call_index"], s["abs_steer_median"],
                         s["c1_distinct"], s["c0_distinct"], s["curv_median"],
                         s["frac_curv_gt20"], s["speed_median"]))
            out[p]["v%d" % v] = {"start": i0, "n": len(w), "mode_hist": hist,
                                 "nft": len(nft), "nft_mode0": nft_m0,
                                 "nft_modex": nft_mx, "mode0": s0, "modex": sx}
    if as_json:
        print(json.dumps(out, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
