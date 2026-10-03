"""D3 STEP 2B — where the port's AI speed leaves the original's, and why.

Implements EXACTLY verify/d3_elim_20261003/PREREG_STEP2B.md sections 2B.1-2B.4.
No rule here may change after a run; a replacement must be stated in the RESULT.

2B.1  onset k = first window call with |dspeed| > max(P*speed_orig, U), speed_orig > 100,
      evaluated at the three registered (P, U) pairs.
2B.2  COMMAND     iff c4 or c5 differ at k or in the two calls before it
      NON-COMMAND iff the commands agree over k-2..k and speed_orig is falling
      AMBIGUOUS   otherwise
2B.3  pre-onset median/max |dspeed| over [0, k), absolute and as a fraction.
2B.4  the mode-7 c4==0x40 signature count on the original.

Usage:
  py -3.12 re/tools/ai_speed_onset.py --orig <o.aistep.csv> [--orig2 <o2.csv>] \
        --port <p.csv> [--port2 <p2.csv>] [--n 220] [--json OUT.json]
"""
import argparse
import json
import statistics
import sys

sys.path.insert(0, __file__.rsplit("\\", 1)[0].rsplit("/", 1)[0])
from ai_band_sim import car_rows, N_DEFAULT         # noqa: E402

THRESHOLDS = [(0.005, 25.0), (0.01, 50.0), (0.02, 100.0)]   # registered, 2B.1
MIN_SPEED = 100.0                                            # registered, 2B.1
PRE_ONSET_FRACTION = 0.01                                    # registered, 2B.3


def series(path, v, n):
    rows, i0 = car_rows(path, v)
    if i0 is None:
        return None
    w = rows[i0:i0 + n]
    return {"speed": [float(x["rec_9e4"]) for x in w],
            "c4": [int(x["c4"]) for x in w],
            "c5": [int(x["c5"]) for x in w],
            "mode": [int(x["ai_mode"]) for x in w],
            "b0c": [float(x["rec_b0c"]) for x in w]}


def onset(o, p, n, P, U):
    for k in range(n):
        if o["speed"][k] <= MIN_SPEED:
            continue
        if abs(p["speed"][k] - o["speed"][k]) > max(P * o["speed"][k], U):
            return k
    return None


def classify(o, p, k):
    """2B.2, verbatim."""
    lo = max(0, k - 2)
    cmd_differs = any(o["c4"][j] != p["c4"][j] or o["c5"][j] != p["c5"][j]
                      for j in range(lo, k + 1))
    if cmd_differs:
        return "COMMAND"
    if o["speed"][k] < o["speed"][lo]:
        return "NON-COMMAND"
    return "AMBIGUOUS"


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--orig2", default="")
    ap.add_argument("--port", required=True)
    ap.add_argument("--port2", default="")
    ap.add_argument("--n", type=int, default=N_DEFAULT)
    ap.add_argument("--json", default="")
    a = ap.parse_args(argv)
    n = a.n
    rep = {"orig": a.orig, "port": a.port, "n": n,
           "thresholds": THRESHOLDS, "cars": {}}

    print("### 2B.1 / 2B.2 — ONSET AND CLASSIFICATION, three registered thresholds")
    for v in (1, 2, 3):
        o, p = series(a.orig, v, n), series(a.port, v, n)
        if not o or not p:
            continue
        rows = []
        for P, U in THRESHOLDS:
            k = onset(o, p, n, P, U)
            cls = classify(o, p, k) if k is not None else "NO ONSET"
            rows.append({"P": P, "U": U, "k": k, "class": cls})
            print("  car %d  P=%.3f U=%5.0f  onset k=%s  -> %s"
                  % (v, P, U, k, cls))
        clss = {r["class"] for r in rows}
        agree = len(clss) == 1
        print("  car %d  G2B-SENS: %s  (%s)"
              % (v, "PASS" if agree else "*** FAIL -- no carrier named ***",
                 ", ".join(sorted(clss))))
        k = rows[1]["k"]          # the middle threshold is the headline
        rep["cars"][v] = {"onset": rows, "sens_agree": agree, "k_headline": k}
        if k is None:
            continue
        print("  car %d  window around the onset (headline k=%d):" % (v, k))
        print("       %-5s %10s %10s %10s | %-9s %-9s %-9s"
              % ("k", "sp orig", "sp port", "delta", "c4 o|p", "c5 o|p", "mode o|p"))
        ctx = []
        for j in range(max(0, k - 2), min(n, k + 3)):
            print("       %-5d %10.1f %10.1f %+10.1f | %-9s %-9s %-9s"
                  % (j, o["speed"][j], p["speed"][j], p["speed"][j] - o["speed"][j],
                     "%d|%d" % (o["c4"][j], p["c4"][j]),
                     "%d|%d" % (o["c5"][j], p["c5"][j]),
                     "%d|%d" % (o["mode"][j], p["mode"][j])))
            ctx.append({"k": j, "sp_orig": o["speed"][j], "sp_port": p["speed"][j],
                        "c4_orig": o["c4"][j], "c4_port": p["c4"][j],
                        "c5_orig": o["c5"][j], "c5_port": p["c5"][j],
                        "mode_orig": o["mode"][j], "mode_port": p["mode"][j]})
        rep["cars"][v]["context"] = ctx
        # ---- 2B.3 pre-onset budget ----
        d = [abs(p["speed"][j] - o["speed"][j]) for j in range(k)]
        fr = [abs(p["speed"][j] - o["speed"][j]) / o["speed"][j]
              for j in range(k) if o["speed"][j] > MIN_SPEED]
        med = statistics.median(d) if d else 0.0
        mx = max(d) if d else 0.0
        mxf = max(fr) if fr else 0.0
        print("  car %d  2B.3 PRE-ONSET [0,%d): median|dspeed| %.3f  max %.3f  "
              "max fraction %.5f  -> %s"
              % (v, k, med, mx, mxf,
                 "UNDER 1%% -- section 2.5 SUPERSEDED" if mxf < PRE_ONSET_FRACTION
                 else "OVER 1% -- section 2.5 RUNS"))
        rep["cars"][v]["pre_onset"] = {"n": k, "median": med, "max": mx,
                                       "max_fraction": mxf,
                                       "supersedes_2_5": mxf < PRE_ONSET_FRACTION}
        print()

    # ---- 2B.4 the mode-7 signature on the ORIGINAL ----
    print("### 2B.4 — the unported mode tails' signature on the ORIGINAL window")
    sig = {}
    for v in (1, 2, 3):
        o = series(a.orig, v, n)
        if not o:
            continue
        m7 = [j for j in range(n) if o["mode"][j] == 7]
        m7_64 = [j for j in m7 if o["c4"][j] == 64]
        m3 = [j for j in range(n) if o["mode"][j] == 3]
        m3_lift = [j for j in m3 if o["c4"][j] != 255 or o["c5"][j] != 0]
        m0 = [j for j in range(n) if o["mode"][j] == 0]
        m0_lift = [j for j in m0 if o["c4"][j] != 255 or o["c5"][j] != 0]
        print("  car %d  mode7 %3d calls (%.3f of window), of which c4==0x40: %3d (%.3f)"
              % (v, len(m7), len(m7) / n, len(m7_64),
                 len(m7_64) / len(m7) if m7 else 0.0))
        print("          mode3 %3d calls (%.3f), of which not-full-throttle: %3d (%.3f)"
              % (len(m3), len(m3) / n, len(m3_lift),
                 len(m3_lift) / len(m3) if m3 else 0.0))
        print("          mode0 %3d calls (%.3f), of which not-full-throttle: %3d (%.3f)"
              % (len(m0), len(m0) / n, len(m0_lift),
                 len(m0_lift) / len(m0) if m0 else 0.0))
        sig[v] = {"m7": len(m7), "m7_c4_64": len(m7_64), "m3": len(m3),
                  "m3_lift": len(m3_lift), "m0": len(m0), "m0_lift": len(m0_lift)}
    rep["signature"] = sig

    # ---- G2B-DET ----
    print("\n### G2B-DET — repeats give the same onset classification")
    for lbl, base, rep2 in (("orig", a.orig, a.orig2), ("port", a.port, a.port2)):
        if not rep2:
            print("  %s: no repeat supplied -- NOT CHECKED" % lbl)
            continue
        for v in (1, 2, 3):
            if lbl == "orig":
                o1, o2 = series(base, v, n), series(rep2, v, n)
                p = series(a.port, v, n)
                k1 = onset(o1, p, n, *THRESHOLDS[1])
                k2 = onset(o2, p, n, *THRESHOLDS[1])
                c1 = classify(o1, p, k1) if k1 is not None else "NONE"
                c2 = classify(o2, p, k2) if k2 is not None else "NONE"
            else:
                o = series(a.orig, v, n)
                p1, p2 = series(base, v, n), series(rep2, v, n)
                k1 = onset(o, p1, n, *THRESHOLDS[1])
                k2 = onset(o, p2, n, *THRESHOLDS[1])
                c1 = classify(o, p1, k1) if k1 is not None else "NONE"
                c2 = classify(o, p2, k2) if k2 is not None else "NONE"
            print("  %s car %d: k %s vs %s, class %s vs %s  %s"
                  % (lbl, v, k1, k2, c1, c2,
                     "AGREE" if (k1 == k2 and c1 == c2) else "*** DISAGREE ***"))

    if a.json:
        json.dump(rep, open(a.json, "w"), indent=1, default=str)
        print("\n  -> %s" % a.json)
    return 0


if __name__ == "__main__":
    sys.exit(main())
