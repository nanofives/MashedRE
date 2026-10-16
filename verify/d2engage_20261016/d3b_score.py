"""D3 (b) per regime, the verdict half of verify/d3_regimes_20261013/score_regimes.py (its envelope-print line
crashes on the per-car regime-C envelope; the verdict path, ai_ctrl_window.check_regime, is unchanged).
Envelopes: re/tools/b_envelopes.json (per-car C, user decision 2026-10-13). The d3_regimes copy pools C, which
check_regime (REGIME_PER_CAR = {'C'}) reads as "no envelope" for every C car.
usage: py -3.12 d3b_score.py <port step.csv> [...]"""
import json, os, sys, importlib.util
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
spec = importlib.util.spec_from_file_location("w", os.path.join(W, "re", "tools", "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
ENV = json.load(open(os.path.join(W, r"re\tools\b_envelopes.json")))
for p in sys.argv[1:]:
    cr = w.check_regime(p, ENV)
    reg = sorted(set(r["regime"] for r in cr.values()))
    npass = 0
    for v in (1, 2, 3):
        r = cr[v]; f = r["fails"]
        npass += f == []
        print("  %s car %d @%d: %s" % (os.path.basename(p), v, r["n"],
              ("NOT SCORED " + r["why"]) if f is None else ("PASS" if not f else "FAIL " + "; ".join(f))))
    print("%s regime %s: %d/3" % (os.path.basename(p), "/".join(reg), npass))
