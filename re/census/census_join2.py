"""Classify the T2 census functions: what is bound, what only needs binding,
and what the functions without a C4 body have as source / verdicts."""
import csv
import collections
import os
import sys

ROOT = r"C:/Users/maria/Desktop/Proyectos/Mashed"


def rows(path, skip_comment=True):
    with open(os.path.join(ROOT, path), encoding="utf-8", errors="replace", newline="") as f:
        lines = [ln for ln in f if not (skip_comment and ln.startswith("#"))]
    return list(csv.DictReader(lines, delimiter="\t"))


census = rows("verify/census_20261116/census_T2-RACETICK-CENSUS.tsv")
ledger = {r["rva"].lower(): r for r in rows("re/analysis/match_ledger.tsv")}
prov = {r["rva"].lower(): r for r in rows("re/match/PROVISIONAL.tsv")}
try:
    adm = {r["rva"].lower(): r for r in rows("re/match/ADMITTED.tsv")}
except Exception as e:  # noqa
    adm = {}
insitu = {r["rva"].lower(): r for r in rows("verify/provinsitu_20261111/PROV_VERDICTS.tsv")}

phases = collections.Counter(r["phase"] for r in census)
print("census rows by phase:", dict(phases))

# per-rva aggregate over the T2 phases
T2 = {"race-load", "race-tick"}
agg = {}
for r in census:
    rva = r["rva"].lower()
    a = agg.setdefault(rva, {"calls": 0, "t2calls": 0, "status": r["status"], "binding": set(), "phases": set(), "name": r["name"], "sub": r["subsystem"]})
    c = int(r["calls"] or 0)
    a["calls"] += c
    a["phases"].add(r["phase"])
    a["binding"].add(r["binding"])
    if r["phase"] in T2:
        a["t2calls"] += c

allr = agg
t2 = {k: v for k, v in agg.items() if v["phases"] & T2}
print(f"distinct functions executed (all phases): {len(allr)}; on race-load or race-tick: {len(t2)}")


def lvl(s):
    return s.split()[0].split(":")[0]


by = collections.Counter()
for k, v in t2.items():
    L = lvl(v["status"])
    b = "bound" if (v["binding"] & {"matched", "admitted", "replay"}) else \
        "provisional" if "provisional" in v["binding"] else \
        "standin/exe/hand" if (v["binding"] & {"stand-in", "under-stand-in", "exe-body", "hand-port", "rw-lib"}) else \
        "stop" if "stop" in v["binding"] else "unbound"
    v["b"] = b
    by[(("C4" if L == "C4" else "nonC4"), b)] += 1
print("\nT2-path functions, C4 vs not, by binding state:")
for k in sorted(by):
    print(f"  {k[0]:6s} {k[1]:18s} {by[k]}")

non = {k: v for k, v in t2.items() if lvl(v["status"]) != "C4"}
print(f"\nT2-path functions with no C4 body: {len(non)}")
print("  by level:", dict(collections.Counter(lvl(v['status']) for v in non.values())))


def bucket(p):
    if p is None:
        return "no source"
    if p >= 100:
        return "100 (unpromoted)"
    if p >= 95:
        return "95-99.9"
    if p >= 80:
        return "80-95"
    if p >= 50:
        return "50-80"
    return "<50"


bk = collections.Counter()
calls_bk = collections.Counter()
ins = collections.Counter()
inprov = inadm = 0
for k, v in non.items():
    p = None
    if k in ledger:
        try:
            p = float(ledger[k]["best_pct"])
        except ValueError:
            p = None
    if p is None and k in prov:
        try:
            p = float(prov[k]["best_pct"])
        except ValueError:
            p = None
    v["pct"] = p
    b = bucket(p)
    bk[b] += 1
    calls_bk[b] += v["t2calls"]
    if k in prov:
        inprov += 1
    if k in adm:
        inadm += 1
    if k in insitu:
        ins[insitu[k]["verdict"].split("@")[0]] += 1
    else:
        ins["not tested in situ"] += 1
tot_calls = sum(v["t2calls"] for v in non.values()) or 1
print("  by best match % of an existing re/match source (functions, share of their T2 calls):")
for b in ["100 (unpromoted)", "95-99.9", "80-95", "50-80", "<50", "no source"]:
    print(f"    {b:18s} {bk[b]:4d}   {100*calls_bk[b]/tot_calls:5.1f}% of calls")
print(f"  already in PROVISIONAL.tsv: {inprov}; in ADMITTED.tsv: {inadm}")
print("  in-situ replay verdict of 2026-11-11 (PROV_VERDICTS.tsv):", dict(ins))

ranked = sorted(non.items(), key=lambda kv: -kv[1]["t2calls"])
for n in (50, 150):
    print(f"  top {n} by calls cover {100*sum(v['t2calls'] for _, v in ranked[:n])/tot_calls:.1f}% of non-C4 calls")
print("  by subsystem:", dict(collections.Counter(v["sub"] for v in non.values()).most_common(12)))
print("\n  top 15 non-C4 by T2 calls:")
for k, v in ranked[:15]:
    print(f"    {k} {v['name'][:28]:28s} calls={v['t2calls']:>9d} {v['status'][:26]:26s} pct={v.get('pct')} bind={','.join(sorted(v['binding']))}")

c4un = {k: v for k, v in t2.items() if lvl(v["status"]) == "C4" and v["b"] == "unbound"}
print(f"\nC4 but unbound on the T2 path: {len(c4un)}  routes:", dict(collections.Counter(v['status'] for v in c4un.values())))

# size of no-source set and whether they are big
nosrc = [(k, v) for k, v in non.items() if v.get("pct") is None]
print(f"\nno re/match source at all: {len(nosrc)}; levels:", dict(collections.Counter(lvl(v['status']) for _, v in nosrc)))
out = os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), "t2_nonc4.tsv")
with open(out, "w", encoding="utf-8") as f:
    f.write("rva\tname\tlevel\tsubsystem\tt2_calls\tbest_pct\tbinding\tin_prov\tinsitu\n")
    for k, v in ranked:
        f.write(f"{k}\t{v['name']}\t{lvl(v['status'])}\t{v['sub']}\t{v['t2calls']}\t{v.get('pct')}\t{','.join(sorted(v['binding']))}\t{int(k in prov)}\t{insitu.get(k, {}).get('verdict', '')}\n")
print("wrote", out)
