"""G-PROBE diagnosis (report-only, after the pre-registered G-PROBE failed): which producer row does the consumer's
refdist equal? Per in-W consumer call at clock c, compare its refdist bits with the producer's output at c (same
tick), c-50 (previous tick) and c-100. Read-only.  usage: py -3.12 probe_align.py"""
import collections
from pathlib import Path
D = Path(__file__).resolve().parent


def load(p):
    rows = [l.rstrip("\n").split("\t") for l in open(p) if l.strip()]
    return {k: i for i, k in enumerate(rows[0])}, rows[1:]


for run in ("rd1", "rd2"):
    ix, rd = load(D / f"o_{run}.msd.rd.tsv")
    byclk = collections.defaultdict(list)
    for r in rd: byclk[int(r[ix["clk"]])].append(r[ix["rd"]].split(";"))
    w = [r for r in rd if r[ix["ba8c"]] == "6"]
    lo, hi = int(w[0][ix["clk"]]), int(w[-1][ix["clk"]])
    cx, c30 = load(D / f"o_{run}.msd.c30.tsv")
    res = collections.Counter(); multi = sum(1 for k in byclk if len(byclk[k]) > 1)
    for r in c30:
        c = int(r[cx["frame"]])
        if not (lo <= c <= hi) or r[cx["ba8c"]] != "6": continue
        v = int(r[cx["v"]]); val = int(r[cx["refdist"]], 16)
        hit = [d for d in (0, 50, 100) if any(int(x[v], 16) == val for x in byclk.get(c - d, []))]
        res["same" if hit[:1] == [0] else ("prev" if hit[:1] == [50] else ("prev2" if hit else "none"))] += 1
        res["prev_matches"] += 50 in hit
    print(run, "producer clocks with >1 call:", multi, "| consumer refdist equals producer output at:", dict(res))
