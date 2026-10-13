"""Build the regime-conditioned criterion-(b) envelopes (ROADMAP.md §D3 amendment 2026-10-09,
verify/d3_bscore_20261009/PREREG_BREGIME.md; regime-C window amendment 2026-10-13,
verify/d3_regimes_20261013/RESULT_REGIMES.md). Read-only on the inputs.

usage: py -3.12 re/tools/b_envelopes.py <out.json> <aistep.csv>=<A|B|C> [<aistep.csv>=<A|B|C> ...]
Each labelled ORIGINAL run contributes its cars 1..3 on its regime's window (ai_ctrl_window.REGIME_N:
A/B 220 calls, C 196). Envelope = min..max per metric over runs, per regime; cars pooled for A/B, per
car for C (ai_ctrl_window.REGIME_PER_CAR -> envelopes["C"]["<v>"]). Labels come from the run's
DAT_0089a368 column (label_run.py), or are inherited per PREREG_BREGIME §2 when a run's metrics equal
a labelled run's exactly."""
import json, sys, importlib.util, os
here = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("w", os.path.join(here, "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
M = list(w.TOLERANCE.keys())
out, runs = sys.argv[1], [a.rsplit("=", 1) for a in sys.argv[2:]]
env, prov = {}, []
for path, lab in runs:
    st = w.window_stats(path, w.REGIME_N[lab])
    for v, s in st.items():
        if v < 1 or s.get("short") or not s.get("calls"):
            continue
        e = env.setdefault(lab, {})
        if lab in w.REGIME_PER_CAR:
            e = e.setdefault(str(v), {})
        if not e:
            e.update({m: [s[m], s[m]] for m in M})
        for m in M:
            e[m] = [min(e[m][0], s[m]), max(e[m][1], s[m])]
    prov.append({"run": path.replace("\\", "/"), "regime": lab})
json.dump({"envelopes": env, "runs": prov, "metrics": M,
           "windows": w.REGIME_N, "per_car": sorted(w.REGIME_PER_CAR)}, open(out, "w"), indent=1)
for lab, e in sorted(env.items()):
    if lab in w.REGIME_PER_CAR:
        for v, ev in sorted(e.items()):
            print("%s car %s @%d" % (lab, v, w.REGIME_N[lab]), {m: ev[m] for m in M})
    else:
        print("%s @%d" % (lab, w.REGIME_N[lab]), {m: e[m] for m in M})
