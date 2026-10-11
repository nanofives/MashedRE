import sys, csv, json, os, collections
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\scripts")
import rva_body_scan as S
R = r"C:\Users\maria\Desktop\Proyectos\Mashed"; V = os.path.join(R, "verify", "d3_bulk_20261011"); SP = os.path.dirname(os.path.abspath(__file__))
sc = S.scan_all(); exe = S.rva_to_tus(sc, "exe")
cg = {int(k, 16): [int(t, 16) for t in v] for k, v in json.load(open(os.path.join(SP, "cg.json"))).items()}
rows = {int(r["rva"], 16): r for r in csv.DictReader(open(os.path.join(V, "executed.tsv"), encoding="utf-8"), delimiter="\t")}
nodes = {v for v, r in rows.items() if r["port"] in ("none", "asi-only")}
def sub(v, seen):
    st=[v]; out=[]
    while st:
        x=st.pop()
        if x in seen: continue
        seen.add(x); out.append(x); st.extend(t for t in cg.get(x,()) if t in nodes)
    return out
maxd = int(sys.argv[2])
def show(v, d, seen):
    kids = [t for t in cg.get(v, ()) if t in nodes and t not in seen]
    for k in kids:
        if k in seen: continue
        s = sub(k, set(seen))
        subs = collections.Counter(rows[x]["subsystem"] for x in s)
        print("  " * d + "0x%08x %s n=%d r=%s %s" % (k, rows[k]["subsystem"], len(s), rows[k]["hits_per_ticks"], ",".join("%s:%d" % kv for kv in subs.most_common(4))))
        if d < maxd:
            seen.add(k); show(k, d + 1, seen)
        else:
            seen.update(s)
seen = {int(sys.argv[1], 16)}
show(int(sys.argv[1], 16), 0, seen)
