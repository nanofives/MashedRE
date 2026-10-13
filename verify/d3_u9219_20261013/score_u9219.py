"""U-9219 scorer (PREREG_U9219.md section 2). Read-only on its inputs.
usage: py -3.12 score_u9219.py            (run from this directory)
Inputs: o_rd{1,2}.msd.rd.tsv (original FUN_00442a60 entry+leave), o_rd{1,2}.msd.c30.tsv (original consumer),
replay_port.tsv (port ComputeDistancesCore on the original's inputs), and the exe consumer traces named on argv
(default ../d3_gatefire_20261008/M4_ON.c30.tsv)."""
import collections, math, struct, sys
from pathlib import Path

D = Path(__file__).resolve().parent
RUNS = ("rd1", "rd2", "rd3")   # rd3 = amendment A1 confirmatory run
hexf = lambda h: struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]


def load(path):
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip()]
    ix = {k: i for i, k in enumerate(rows[0])}
    return ix, rows[1:]


def rd_rows(run):
    ix, rows = load(D / f"o_{run}.msd.rd.tsv")
    out = []
    for r in rows:
        cars = [c.split(":") for c in r[ix["cars"]].split(";")]
        out.append(dict(n=int(r[ix["n"]]), clk=int(r[ix["clk"]]), ba8c=int(r[ix["ba8c"]]),
                        a8_in=int(r[ix["a8_in"]]), c8_in=int(r[ix["c8_in"]]), a8=int(r[ix["a8"]]), c8=int(r[ix["c8"]]),
                        st=[int(c[2], 16) for c in cars], pct=[hexf(c[4]) for c in cars],
                        pos=[(hexf(c[5]), hexf(c[6]), hexf(c[7])) for c in cars],
                        act=[c[0] for c in cars], alive=[int(c[1]) for c in cars],
                        rd=r[ix["rd"]].split(";")))
    return out


def window(rows):
    """W: first call with ba8c == 6 until the first call where any st != 0 or ba8c != 6 (exclusive)."""
    w, on = [], False
    for r in rows:
        if not on:
            if r["ba8c"] == 6 and not any(r["st"]): on = True
            else: continue
        if r["ba8c"] != 6 or any(r["st"]): break
        w.append(r)
    return w


print("== G-ORIG-DET / window")
R, W = {}, {}
for run in RUNS:
    R[run] = rd_rows(run); W[run] = window(R[run])
    w = W[run]
    z = [sum(1 for r in w if hexf(r["rd"][v]) == 0.0) for v in range(4)]
    refs = collections.Counter(r["a8"] for r in w)
    print(f"  {run}: producer calls {len(R[run])}, W = {len(w)} calls, clk {w[0]['clk']}..{w[-1]['clk']}; "
          f"refdist==0 per car {z}; reference (0x008989a8) {dict(sorted(refs.items()))}")
zs = {run: [sum(1 for r in W[run] if hexf(r["rd"][v]) == 0.0) for v in range(4)] for run in W}
spread = [max(zs[r][v] for r in RUNS) - min(zs[r][v] for r in RUNS) for v in range(4)]
print(f"  spread per car {spread} -> G-ORIG-DET {'PASS' if max(spread[1:]) <= 2 else 'FAIL (spread is the floor)'}")

print("== G-PROBE (producer refdist[v] at leave vs the consumer's refdist at its next call, same run)")
c30 = {}
for run in RUNS:
    ix, rows = load(D / f"o_{run}.msd.c30.tsv"); c30[run] = (ix, rows)
    lo, hi = W[run][0]["clk"], W[run][-1]["clk"]
    rds = R[run]; j = 0; tot = eq = 0
    for r in rows:
        clk = int(r[ix["frame"]])
        if not (lo <= clk <= hi): continue
        while j + 1 < len(rds) and rds[j + 1]["clk"] <= clk: j += 1
        v = int(r[ix["v"]]); tot += 1
        eq += int(int(rds[j]["rd"][v], 16) == int(r[ix["refdist"]], 16))
    print(f"  {run}: {eq} / {tot} consumer calls equal bits ({100.0 * eq / max(tot, 1):.2f} %) -> "
          f"{'PASS' if eq >= 0.95 * tot else 'FAIL'}")

print("== G-PROBE' (amendment A1: consumer refdist at clock c vs the producer's leave output at c - 50)")
for run in RUNS:
    ix, rows = c30[run]; lo, hi = W[run][0]["clk"], W[run][-1]["clk"]
    byclk = {}
    for r in R[run]: byclk.setdefault(r["clk"], r)
    tot = eq = 0
    for r in rows:
        clk = int(r[ix["frame"]])
        if not (lo <= clk <= hi) or (clk - 50) not in byclk: continue
        v = int(r[ix["v"]]); tot += 1
        eq += int(int(byclk[clk - 50]["rd"][v], 16) == int(r[ix["refdist"]], 16))
    print(f"  {run}: {eq} / {tot} ({100.0 * eq / max(tot, 1):.2f} %) -> {'PASS' if eq >= 0.95 * tot else 'FAIL'}"
          f"{'   <- confirmatory (A1)' if run == 'rd3' else '   (exploratory)'}")

print("== G-REPLAY-SEL / G-REPLAY-DIST (port ComputeDistancesCore on the original's inputs)")
rp = []
for _p in ("replay_port.tsv", "replay_port_rd3.tsv"):
    ix, _r = load(D / _p); rp += _r
byk = {(Path(r[ix["file"]]).name.split(".")[0].replace("o_", ""), int(r[ix["n"]])): r for r in rp}
for run in RUNS:
    sel_ok = sel_n = d_ok = d_n = 0; bad = []
    for r in W[run]:
        p = byk[(run, r["n"])]; sel_n += 1
        if int(p[ix["port_stored"]]):
            ok = (int(p[ix["port_ref"]]), int(p[ix["port_other"]])) == (r["a8"], r["c8"])
        else:
            ok = (r["a8"], r["c8"]) == (r["a8_in"], r["c8_in"])
        sel_ok += ok
        if not ok and len(bad) < 5: bad.append((r["n"], r["clk"], p[ix["port_ref"]], p[ix["port_other"]], r["a8"], r["c8"]))
        po = p[ix["port_rd"]].split(";")
        for v in range(4):
            o, q = hexf(r["rd"][v]), hexf(po[v]); d_n += 1
            d_ok += abs(o - q) <= 0.01 + 0.08 * abs(o)
    print(f"  {run}: SEL {sel_ok} / {sel_n} ({100.0 * sel_ok / sel_n:.2f} %) -> {'PASS' if sel_ok >= 0.99 * sel_n else 'FAIL'};"
          f" DIST {d_ok} / {d_n} ({100.0 * d_ok / d_n:.2f} %) -> {'PASS' if d_ok >= 0.99 * d_n else 'FAIL'}")
    if bad: print("    first mismatches (n, clk, port ref, port other, orig a8, orig c8):", bad)
    # bit-exactness and magnitude, report only
    rel = []
    for r in W[run]:
        po = byk[(run, r["n"])][ix["port_rd"]].split(";")
        for v in range(4):
            o = hexf(r["rd"][v])
            if o: rel.append(abs(hexf(po[v]) - o) / o)
    rel.sort()
    print(f"    rel |port-orig|/orig on nonzero: median {rel[len(rel)//2]:.2e}, max {rel[-1]:.2e}, "
          f"bit-equal {sum(1 for r in W[run] for v in range(4) if r['rd'][v] == byk[(run, r['n'])][ix['port_rd']].split(';')[v])} / {4 * len(W[run])}")

print("== G-REFDIST / G-MODE7 (consumer calls in W; exe = same number of calls per car from its first call)")
orig_cnt = {}
for run in RUNS:
    ix, rows = c30[run]; lo, hi = W[run][0]["clk"], W[run][-1]["clk"]
    n = collections.Counter(); z = collections.Counter(); m7 = collections.Counter()
    for r in rows:
        if not (lo <= int(r[ix["frame"]]) <= hi): continue
        v = int(r[ix["v"]]); n[v] += 1; z[v] += int(r[ix["refdist"]], 16) == 0; m7[v] += int(r[ix["ret"]]) == 2
    orig_cnt[run] = (n, z, m7)
    print(f"  orig {run}: calls {dict(sorted(n.items()))} refdist0 {dict(sorted(z.items()))} ret2 {dict(sorted(m7.items()))}")
# the original's own returns of 2 over its whole driving phase (W ends at the first st != 0)
for run in RUNS:
    ix, rows = c30[run]
    m7 = collections.Counter(int(r[ix["v"]]) for r in rows if int(r[ix["ret"]]) == 2 and r[ix["ba8c"]] == "6")
    print(f"  orig {run}: ret2 over all ba8c==6 calls {dict(sorted(m7.items()))}")
nW = {v: max(orig_cnt[run][0][v] for run in orig_cnt) for v in (1, 2, 3)}
sp = {v: max(orig_cnt[r][1][v] for r in RUNS) - min(orig_cnt[r][1][v] for r in RUNS) for v in (1, 2, 3)}
for path in (sys.argv[1:] or [str(D.parent / "d3_gatefire_20261008" / "M4_ON.c30.tsv")]):
    ix, rows = load(path)
    n = collections.Counter(); z = collections.Counter(); m7 = collections.Counter(); m7all = collections.Counter()
    for r in rows:
        v = int(r[ix["v"]]); m7all[v] += int(r[ix["ret"]]) == 2
        if n[v] >= nW[v]: continue
        n[v] += 1; z[v] += int(r[ix["refdist"]], 16) == 0; m7[v] += int(r[ix["ret"]]) == 2
    res = []
    for v in (1, 2, 3):
        om = sum(orig_cnt[run][1][v] for run in orig_cnt) / float(len(orig_cnt))
        res.append(abs(z[v] - om) <= max(3, 2 * sp[v]))
    print(f"  exe {Path(path).name}: calls {dict(sorted(n.items()))} refdist0 {dict(sorted(z.items()))} ret2(W) {dict(sorted(m7.items()))}"
          f" ret2(all) {dict(sorted(m7all.items()))} -> G-REFDIST {'PASS' if all(res) else 'FAIL'} {res};"
          f" G-MODE7 {'PASS' if m7all[2] <= 5 and m7all[3] <= 5 else 'FAIL'}")
    # report-only: the same counts over the rd3 round length (rd2 = rd3, the reproducible original round)
    n3 = orig_cnt["rd3"][0]; n = collections.Counter(); z = collections.Counter(); m7 = collections.Counter()
    for r in rows:
        v = int(r[ix["v"]])
        if n[v] >= n3[v]: continue
        n[v] += 1; z[v] += int(r[ix["refdist"]], 16) == 0; m7[v] += int(r[ix["ret"]]) == 2
    print(f"    (report) over the rd3 round length: calls {dict(sorted(n.items()))} refdist0 {dict(sorted(z.items()))}"
          f" ret2 {dict(sorted(m7.items()))}  vs orig rd3 refdist0 {dict(sorted(orig_cnt['rd3'][1].items()))}")
