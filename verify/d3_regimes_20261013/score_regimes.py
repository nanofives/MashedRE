"""Criterion (b) per regime for the default build (d3_regimes_20261013). Read-only on the inputs.

usage: py -3.12 score_regimes.py <envelopes.json> <orig_label_list.txt> <port csv> [<port csv> ...]
  orig_label_list.txt: one "<aistep.csv>=<A|B|C>" per line (the runs that built the envelopes).
Prints, per regime:
  1. original noise: per car, the window hash and every metric across that regime's runs
     (identical hashes = zero run-to-run noise);
  2. each port csv's per-car per-item verdict against that regime's envelope
     (same rule as ai_ctrl_window.py --check-regime; the port csv's regime is its own flag_a368 label);
  3. a DIAGNOSTIC at n=196 for regime C (the original's cars 1/2 are stepped only 196 calls in C,
     so the committed 220-call window cannot score them). Not the committed window."""
import csv, hashlib, json, os, sys, importlib.util
from collections import defaultdict
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.abspath(os.path.join(here, "..", ".."))
spec = importlib.util.spec_from_file_location("w", os.path.join(root, "re", "tools", "ai_ctrl_window.py"))
w = importlib.util.module_from_spec(spec); spec.loader.exec_module(w)
M = list(w.TOLERANCE.keys())
env = json.load(open(sys.argv[1]))["envelopes"]
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
    for v in (1, 2, 3):
        hs, vals = set(), defaultdict(set)
        for p in runs:
            s = w.window_stats(os.path.join(root, p), 220)[v]
            hs.add(whash(os.path.join(root, p), v, 220) if not s.get("short") else "SHORT%d" % s["calls"])
            if not s.get("short"):
                for m in M: vals[m].add(s[m])
        print("  orig car %d: window hashes %s | %s" % (v, sorted(hs),
              " ".join("%s=%s" % (short(m), sorted(vals[m])) for m in M) if vals else "no full window"))
    if reg not in env:
        print("  no envelope"); continue
    print("  envelope: " + " ".join("%s=%s" % (short(m), env[reg][m]) for m in M))
    for p in ports:
        rg = w.window_regime(p, 220); st = w.window_stats(p, 220)
        if set(rg.values()) != {reg}: continue
        npass = 0
        for v in (1, 2, 3):
            s = st[v]
            f = ["%s=%s not in %s" % (short(m), s[m], env[reg][m]) for m in M if not (env[reg][m][0] <= s[m] <= env[reg][m][1])]
            npass += not f
            print("  port %s car %d: %s  [%s]" % (os.path.basename(p), v, "PASS" if not f else "FAIL " + "; ".join(f),
                  " ".join("%s=%s" % (short(m), s[m]) for m in M)))
        print("  port %s regime %s: %d/3" % (os.path.basename(p), reg, npass))

print("=" * 100)
print("DIAGNOSTIC (not the committed window): regime C at n=196, cars 1..3 of the original C runs")
cenv = {}
for p, l in orig:
    if l != "C": continue
    for v, s in w.window_stats(os.path.join(root, p), 196).items():
        if v < 1 or s.get("short") or not s.get("calls"): continue
        e = cenv.setdefault(v, {m: [s[m], s[m]] for m in M})
        for m in M: e[m] = [min(e[m][0], s[m]), max(e[m][1], s[m])]
for v, e in sorted(cenv.items()):
    print("  orig C car %d @196: %s" % (v, " ".join("%s=%s" % (short(m), e[m]) for m in M)))
for p in ports:
    if set(w.window_regime(p, 196).values()) != {"C"}: continue
    st = w.window_stats(p, 196)
    for v in (1, 2, 3):
        if v not in cenv: continue
        f = [short(m) for m in M if not (cenv[v][m][0] <= st[v][m] <= cenv[v][m][1])]
        print("  port %s car %d @196 vs original car %d: %s  [%s]" % (os.path.basename(p), v, v,
              "MATCH" if not f else "DIFF " + ",".join(f), " ".join("%s=%s" % (short(m), st[v][m]) for m in M)))
    # cars pooled, as the committed rule pools A and B
    pool = {m: [min(cenv[v][m][0] for v in cenv), max(cenv[v][m][1] for v in cenv)] for m in M}
    print("  pooled C envelope @196: " + " ".join("%s=%s" % (short(m), pool[m]) for m in M))
    npass = 0
    for v in (1, 2, 3):
        f = [short(m) for m in M if not (pool[m][0] <= st[v][m] <= pool[m][1])]
        npass += not f
        print("  port %s car %d @196 vs pooled C: %s" % (os.path.basename(p), v, "PASS" if not f else "FAIL " + ",".join(f)))
    print("  port %s regime C @196 pooled: %d/3" % (os.path.basename(p), npass))
    break
