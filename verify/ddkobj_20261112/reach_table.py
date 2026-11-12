"""DDKOBJ-42AD90: join the static caller scan (regconv_reach.py static), the two reach runs and the original-side
callers (orig_callers_43.py) into one row per register-convention static.

    py -3.12 verify/ddkobj_20261112/reach_table.py <reach_static.tsv> <reach_run_off.tsv> <reach_run_on.tsv> \
        <REGCONV_orig_callers.tsv> <ddkobj_build.json> <out_tsv> [<reach_nav_off.tsv> <reach_nav_on.tsv>]
"""
import collections, csv, json, sys
st, off, on, orig, bj, out = sys.argv[1:7]
def rd(p): return list(csv.DictReader(open(p), delimiter="\t"))
hit = {}
nav = sys.argv[7:9]
for tag, p in (("off", off), ("on", on)) + ((("navoff", nav[0]), ("navon", nav[1])) if nav else ()):
    for r in rd(p):
        hit[(tag, r["symbol"].split(" ")[0])] = r["reached"] == "yes"
rep = json.load(open(bj))
rows = collections.OrderedDict()
for u in rep["units"]:
    for d in u.get("defs", []):
        if d.get("regconv"):
            rows[d["rva"]] = {"rva": d["rva"], "unit": u["src"], "body": d["exe"], "adapter": "", "in_obj": set(),
                              "vs": set(), "addr": set()}
    for a in u.get("adapters", []):
        if a["rva"] in rows:
            rows[a["rva"]]["adapter"] = a["exe"]
linked = set()
for r in rd(st):
    if r["kind"] == "not-linked" or not r["rva"]:
        continue
    linked.add(r["exe_symbol"])
    row = rows.get(r["rva"])
    if not row or r["ref_kind"] == "none":
        continue
    c = r["caller"]
    if r["ref_kind"] == "imm32":
        row["addr"].add(c)
    elif r["kind"] == "body":
        if c not in (row["adapter"],):
            row["in_obj"].add(c)
    else:
        row["vs"].add(c)
oc = collections.defaultdict(set)
for r in rd(orig):
    if r["caller_rva"]:
        oc[r["rva"]].add(f"{r['caller_rva']} {r['caller_name']} [{r['subsystem']}]")
with open(out, "w") as f:
    f.write("rva\tunit\tbody_linked\tadapter\tadapter_linked\treached_off\treached_on\tin_object_callers\t"
            "vs2022_callers_via_adapter\timm32_refs_unverified\torig_callers\n")
    for k, r in rows.items():
        b, a = r["body"], r["adapter"]
        yn = lambda t, s: "yes" if hit.get((t, s)) else ("no" if s in linked else "-")
        ro = "yes" if hit.get(("off", b)) or hit.get(("off", a)) else ("no" if b in linked else "not-linked")
        rch = lambda t: "yes" if hit.get((t, b)) or hit.get((t, a)) else ("no" if b in linked else "not-linked")
        rn = rch("on")
        f.write("\t".join([k, r["unit"], "yes" if b in linked else "no", a or "-", "yes" if a in linked else "no",
                           ro, rn, rch("navoff") if nav else "-", rch("navon") if nav else "-", ",".join(sorted(r["in_obj"])) or "-", ",".join(sorted(r["vs"])) or "-",
                           ",".join(sorted(r["addr"])) or "-", "; ".join(sorted(oc.get(k, []))) or "-"]) + "\n")
print(len(rows), "statics")
