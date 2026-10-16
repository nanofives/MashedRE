"""D3 (b) per car, every item, port vs the original's per-car value in the same regime (d3b lane, 2026-10-16).
The verdict is ai_ctrl_window.check_regime against re/tools/b_envelopes.json (the decided rule, A/B 220 pooled,
C 196 per car). The 'orig' column is the same car's value in one labelled original run of that regime (the original
is noise-free inside a regime, verify/d3_regimes_20261013/RESULT_REGIMES.md), printed only as a reading aid.
usage: py -3.12 percar.py <port step.csv> [...]"""
import json, os, sys, importlib.util
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
spec = importlib.util.spec_from_file_location("w", os.path.join(W, "re", "tools", "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
ENV = json.load(open(os.path.join(W, r"re\tools\b_envelopes.json")))
REF = {"A": r"verify\d3_regimes_20261013\o_reg_r1.msd.aistep.csv",
       "B": r"verify\d3_regimes_20261013\o_reg_r13.msd.aistep.csv",
       "C": r"verify\d3_regimes_20261013\o_reg_r19.msd.aistep.csv"}
KEYS = ["c0_distinct", "c1_distinct", "steer_distinct", "c0_median", "c1_median", "abs_steer_median",
        "accel_distinct", "accel_median", "brake_distinct", "brake_median"]
_ref = {}


def ref_stats(reg):
    if reg not in _ref:
        _ref[reg] = w.window_stats(os.path.join(W, REF[reg]), w.REGIME_N[reg])
    return _ref[reg]


for p in sys.argv[1:]:
    cr = w.check_regime(p, ENV)
    npass = 0
    for v in (1, 2, 3):
        r = cr[v]
        reg = r["regime"]
        f = r["fails"]
        npass += f == []
        verdict = ("NOT SCORED " + r.get("why", "")) if f is None else ("PASS" if not f else "FAIL %d" % len(f))
        print("%s car %d regime %s @%s: %s" % (os.path.basename(p), v, reg, r["n"], verdict))
        s = r["stats"] or {}
        e = w.regime_envelope(ENV, reg, v) or {}
        o = ref_stats(reg).get(v, {}) if reg in REF else {}
        for k in KEYS:
            if k not in s:
                continue
            lo, hi = e.get(k, (None, None))
            bad = lo is not None and not (lo <= s[k] <= hi)
            print("    %-17s port %7s  env [%s,%s]  orig car%d %7s  %s" % (k, s[k], lo, hi, v, o.get(k, "-"),
                                                                       "<-- FAIL" if bad else ""))
    print("%s: %d/3" % (os.path.basename(p), npass))
