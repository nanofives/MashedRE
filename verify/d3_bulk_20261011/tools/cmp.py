import json, collections
V = r"C:/Users/maria/Desktop/Proyectos/Mashed/verify/d3_bulk_20261011/"
SP = r"C:/Users/maria/AppData/Local/Temp/claude/C--Users-maria--claude-account3-spares-spare-d5Fgde/789b5c2d-9291-49f9-b481-ca9cfeff820e/scratchpad/"
o = json.load(open(V + "peek_orig.json"))["samples"]; s = json.load(open(V + "peek_sa.json"))["samples"]
o = [x for x in o if x["phase"] == 3]
s = [x for x in s if (x["tick"] or 0) > 0]
print("orig samples", len(o), "sa samples", len(s), "sa phase values", collections.Counter(x["phase"] for x in s).most_common(3))
inp = json.load(open(SP + "inputs.json"))
users = collections.defaultdict(list)
for ent, f in inp.items():
    for g in f["globals"]:
        if g["writable"]: users[g["addr"]].append(ent)
res = {}
for ah in o[0]["v"]:
    def stat(S):
        nz = [any((x["v"][ah][i] or 0) != 0 for x in S) for i in range(4)]
        dv = [len({x["v"][ah][i] for x in S}) for i in range(4)]
        return nz, dv
    on, od = stat(o); sn, sd = stat(s)
    cls = []
    for i in range(4):
        if on[i] and sn[i]: c = "both"
        elif on[i]: c = "NOWRITER"
        elif sn[i]: c = "exe-only"
        else: c = "zero"
        cls.append(c)
    res[ah] = dict(cls=cls, od=od, sd=sd, users=users[ah])
json.dump(res, open(V + "inputs_live.json", "w"), indent=1)
cnt = collections.Counter()
for ah, r in res.items():
    k = "NOWRITER" if "NOWRITER" in r["cls"] else ("both" if "both" in r["cls"] else ("exe-only" if "exe-only" in r["cls"] else "zero"))
    r["k"] = k; cnt[k] += 1
print(cnt)
fn = collections.defaultdict(collections.Counter)
nw = collections.defaultdict(list)
for ah, r in res.items():
    for u in r["users"]:
        fn[u][r["k"]] += 1
        if r["k"] == "NOWRITER": nw[u].append(ah)
for u in inp:
    print(u, inp[u]["name"][:14], dict(fn[u]), "NOWRITER:", " ".join(nw[u])[:150])
