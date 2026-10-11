import sys, csv, json, os
R = r"C:/Users/maria/Desktop/Proyectos/Mashed/"; SP = os.path.dirname(os.path.abspath(__file__))
cg = {int(k, 16): [int(t, 16) for t in v] for k, v in json.load(open(SP + "/cg.json")).items()}
rows = {int(r["rva"], 16): r for r in csv.DictReader(open(R + "verify/d3_bulk_20261011/executed.tsv", encoding="utf-8"), delimiter="\t")}
def walk(v, d, seen):
    for t in cg.get(v, ()):
        if t in seen: continue
        seen.add(t)
        r = rows.get(t)
        if r is None: continue
        print("  " * d + "%s %-9s %-10s %-24s r=%s up=%s" % (r["rva"], r["port"], r["subsystem"], r["name"][:24], r["hits_per_ticks"], r["exec_callers"][:40]))
        walk(t, d + 1, seen)
for a in sys.argv[1:]:
    v = int(a, 16); r = rows.get(v, {})
    print("ROOT", a, r.get("port"), r.get("subsystem"), r.get("name"))
    walk(v, 1, {v})
