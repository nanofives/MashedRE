"""Build the regime-conditioned criterion-(b) envelopes (ROADMAP.md §D3 amendment 2026-10-09,
verify/d3_bscore_20261009/PREREG_BREGIME.md). Read-only on the inputs.

usage: py -3.12 re/tools/b_envelopes.py <out.json> <aistep.csv>=<A|B> [<aistep.csv>=<A|B> ...]
Each labelled ORIGINAL run contributes its cars 1..3 (220-call window); envelope = min..max per metric,
cars pooled, per regime. Labels come from the run's DAT_0089a368 poll (label_regime.py), or are
inherited per PREREG_BREGIME §2 when a run's metrics equal a labelled run's exactly."""
import json, sys, importlib.util, os
here = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("w", os.path.join(here, "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
M = list(w.TOLERANCE.keys())
out, runs = sys.argv[1], [a.rsplit("=", 1) for a in sys.argv[2:]]
env, prov = {}, []
for path, lab in runs:
    st = w.window_stats(path, 220)
    for v, s in st.items():
        if v < 1 or s.get("short") or not s.get("calls"):
            continue
        e = env.setdefault(lab, {m: [s[m], s[m]] for m in M})
        for m in M:
            e[m] = [min(e[m][0], s[m]), max(e[m][1], s[m])]
    prov.append({"run": path.replace("\\", "/"), "regime": lab})
json.dump({"envelopes": env, "runs": prov, "metrics": M}, open(out, "w"), indent=1)
for lab, e in sorted(env.items()):
    print(lab, {m: e[m] for m in M})
