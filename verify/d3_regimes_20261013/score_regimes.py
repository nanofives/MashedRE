"""Criterion (b) per regime for the default build (d3_regimes_20261013). Read-only on the inputs.

usage: py -3.12 score_regimes.py <envelopes.json> <orig_label_list.txt> <port csv> [<port csv> ...]
  orig_label_list.txt: one "<aistep.csv>=<A|B|C>" per line (the runs that built the envelopes).
Prints, per regime:
  1. original noise: per car, the window hash and every metric across that regime's runs
     (identical hashes = zero run-to-run noise);
  2. each port csv's per-car per-item verdict against that regime's envelope
     (same rule as ai_ctrl_window.py --check-regime; the port csv's regime is its own flag_a368 label);
  3. (removed 2026-10-13) the n=196 regime-C diagnostic. User decision 2026-10-13 made C at 196 calls
     per car the committed rule (ai_ctrl_window.check_regime), so sections 1-2 score it directly.
     Pre-decision output: SCORE.txt. Post-decision output: SCORE_REGIMES_C196.txt."""
import csv, hashlib, json, os, sys, importlib.util
from collections import defaultdict
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.abspath(os.path.join(here, "..", ".."))
spec = importlib.util.spec_from_file_location("w", os.path.join(root, "re", "tools", "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
M = list(w.TOLERANCE.keys())
ENV = json.load(open(sys.argv[1])); env = ENV["envelopes"]
orig = [l.strip().rsplit("=", 1) for l in open(sys.argv[2]) if l.strip() and not l.startswith("#")]
ports = sys.argv[3:]

def whash(p, v, n):
    r = [x for x in csv.DictReader(open(p, newline="")) if int(x["v"]) == v]
    i0 = next((i for i, x in enumerate(r) if int(x["c4"]) != 0), None)
    if i0 is None: return "-"
    return hashlib.sha256("|".join("%s,%s,%s,%s" % (x["c0"], x["c1"], x["c4"], x["c5"])
                                   for x in r[i0:i0 + n]).encode()).hexdigest()[:8]

short = lambda m: m.replace("_distinct", "#").replace("_median", "~")
for reg in ("A", "B", "C"):
    runs = [p for p, l in orig if l == reg]
    print("=" * 100)
    print("REGIME %s: %d original runs" % (reg, len(runs)))
    n = w.REGIME_N[reg]
    for v in (1, 2, 3):
        hs, vals = set(), defaultdict(set)
        for p in runs:
            s = w.window_stats(os.path.join(root, p), n)[v]
            hs.add(whash(os.path.join(root, p), v, n) if not s.get("short") else "SHORT%d" % s["calls"])
            if not s.get("short"):
                for m in M: vals[m].add(s[m])
        print("  orig car %d @%d: window hashes %s | %s" % (v, n, sorted(hs),
              " ".join("%s=%s" % (short(m), sorted(vals[m])) for m in M) if vals else "no full window"))
    if reg not in env:
        print("  no envelope"); continue
    for ek, e in (sorted(env[reg].items()) if reg in w.REGIME_PER_CAR else [(None, env[reg])]):
        print("  envelope%s: " % (" car " + ek if ek else "") + " ".join("%s=%s" % (short(m), e[m]) for m in M))
    for p in ports:
        cr = w.check_regime(p, ENV)
        if set(r["regime"] for r in cr.values()) != {reg}: continue
        npass = 0
        for v in (1, 2, 3):
            r = cr[v]; s = r["stats"]; f = r["fails"]
            npass += f == []
            print("  port %s car %d @%d: %s  [%s]" % (os.path.basename(p), v, r["n"],
                  ("NOT SCORED " + r["why"]) if f is None else ("PASS" if not f else "FAIL " + "; ".join(f)),
                  " ".join("%s=%s" % (short(m), s[m]) for m in M) if s and not s.get("short") else ""))
        print("  port %s regime %s: %d/3" % (os.path.basename(p), reg, npass))

print("=" * 100)
