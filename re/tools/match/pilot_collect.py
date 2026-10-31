"""Collect the matching pilot's results: best score per sampled function over every source in re/match/pilot/
(several MATCH lines may name one address: the best counts) and the raw converter score from
verify/matchpilot_20261030/g2s_raw/results.tsv. Residual classes come from verify/matchpilot_20261030/classes.tsv
(rva, class, reason), written from the helpers' notes.
    py -3.12 re/tools/match/pilot_collect.py [--toolchain ddk|vc19] > verify/matchpilot_20261030/results.tsv"""
import argparse, csv, pathlib, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match

V = match.ROOT / "verify" / "matchpilot_20261030"
ap = argparse.ArgumentParser(); ap.add_argument("--toolchain", default=None); ap.add_argument("--flags", default="")
a = ap.parse_args()
sample = list(csv.DictReader(open(V / "sample.tsv"), delimiter="\t"))
raw = {r["rva"]: r for r in csv.DictReader(open(V / "g2s_raw" / "results.tsv"), delimiter="\t")}
cls = {}
if (V / "classes.tsv").is_file():
    cls = {r["rva"]: r for r in csv.DictReader(open(V / "classes.tsv"), delimiter="\t")}
best = {}
for src in sorted((match.ROOT / "re" / "match" / "pilot").glob("*.c")):
    try:
        res = match.compare(src, a.flags, quiet=True, toolchain=a.toolchain)
    except SystemExit as e:
        print(f"# {src.name}: {str(e).splitlines()[0][:100]}", file=sys.stderr)
        continue
    for r in res:
        k = f"0x{r['addr']:08x}"
        if r["acc"] < 1 and not r["bytes_bad"] and r["bytes_n"] >= match.ghidra_size(r["addr"]):
            # whole obj symbol byte-identical (relocations masked, same-section targets compared): Ghidra's body
            # size cut the function short (epilogue after a jump table), so the instruction score undercounts
            r = dict(r, acc=1.0, regs=1.0, ops=1.0)
        if k not in best or (r["acc"], r["regs"]) > (best[k][0], best[k][1]):
            best[k] = (r["acc"], r["regs"], r["ops"], src.name, r["n_mine"], r["n_orig"])
print("rva\tsize\tsubsystem\tgroup\tstratum\traw_g2s%\tbest%\tregs%\tops%\tsource\tclass\treason")
for s in sample:
    k = s["rva"]
    rw = raw.get(k, {})
    rawacc = rw.get("acc") or rw.get("stage", "")
    if k in best:
        acc, regs, ops, name = best[k][:4]
    else:
        try:
            acc, regs, ops, name = float(rw["acc"]) / 100, float(rw["regs"]) / 100, float(rw["ops"]) / 100, "g2s_raw"
        except (KeyError, ValueError):
            acc = regs = ops = None; name = "-"
    if acc is not None and rw.get("acc") and float(rw["acc"]) / 100 > acc:
        acc, regs, ops, name = float(rw["acc"]) / 100, float(rw["regs"]) / 100, float(rw["ops"]) / 100, "g2s_raw"
    c = cls.get(k, {})
    f = (lambda x: f"{100 * x:.1f}" if x is not None else "-")
    print("\t".join([k, s["size"], s["subsystem"], s["group"], s["stratum"], rawacc, f(acc), f(regs), f(ops), name,
                     "-" if acc == 1 else c.get("class", "?"), "" if acc == 1 else c.get("reason", "")]))
