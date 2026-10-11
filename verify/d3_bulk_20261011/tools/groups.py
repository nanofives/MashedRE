import sys, csv, json, os, collections
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\scripts")
import rva_body_scan as S
R = r"C:\Users\maria\Desktop\Proyectos\Mashed"
V = os.path.join(R, "verify", "d3_bulk_20261011")
SP = os.path.dirname(os.path.abspath(__file__))
sc = S.scan_all(); exe = S.rva_to_tus(sc, "exe")
cg = {int(k, 16): [int(t, 16) for t in v] for k, v in json.load(open(os.path.join(SP, "cg.json"))).items()}
callers = collections.defaultdict(set)
for k, v in cg.items():
    for t in v: callers[t].add(k)
hooks = {}
for r in csv.DictReader(open(os.path.join(R, "hooks.csv"), encoding="utf-8", errors="replace")):
    try: hooks[int(r["rva"], 16)] = r
    except Exception: pass
rows = {int(r["rva"], 16): r for r in csv.DictReader(open(os.path.join(V, "executed.tsv"), encoding="utf-8"), delimiter="\t")}
nodes = {v for v, r in rows.items() if r["port"] in ("none", "asi-only")}
kids = {v: sorted(t for t in cg.get(v, ()) if t in nodes) for v in nodes}
roots = sorted(v for v in nodes if not any(c in nodes for c in callers.get(v, ())))
def desc(r):
    out, st = [], [r]; seen = set()
    while st:
        v = st.pop()
        if v in seen: continue
        seen.add(v); out.append(v); st.extend(kids[v])
    return out
grp = {r: desc(r) for r in roots}
owner = collections.defaultdict(list)
for r, m in grp.items():
    for v in m: owner[v].append(r)
# cycle-only components (no root) -> pick min rva
left = nodes - set(owner)
while left:
    r = min(left); m = desc(r); grp[r] = m
    for v in m: owner[v].append(r)
    left -= set(m)
def name(v): return (hooks.get(v) or {}).get("name", "")
def conf(v): return (hooks.get(v) or {}).get("confidence", "")
out = []
for r, m in grp.items():
    hosts = sorted(callers.get(r, ()))
    hs = ["0x%08x%s[%s]" % (h, "*" if h in exe else "", conf(h)) for h in hosts]
    subs = collections.Counter(rows[v]["subsystem"] for v in m)
    shared = [v for v in m if len(owner[v]) > 1]
    ext = sorted({t for v in m for t in cg.get(v, ()) if t not in nodes})
    ext_noexe = [t for t in ext if t not in exe]
    out.append(dict(root="0x%08x" % r, root_name=name(r), size=len(m), subsystems=";".join("%s:%d" % kv for kv in subs.most_common()),
        p3_root=rows[r]["p3"], rate_root=rows[r]["hits_per_ticks"], hosts=";".join(hs),
        host_has_exe_body=int(any(h in exe for h in hosts)),
        members=";".join("0x%08x" % v for v in sorted(m)), shared=";".join("0x%08x" % v for v in shared),
        ext_callees=len(ext), ext_callees_no_exe_body=";".join("0x%08x" % t for t in ext_noexe)))
out.sort(key=lambda d: -d["size"])
w = csv.DictWriter(open(os.path.join(V, "groups.tsv"), "w", newline="", encoding="utf-8"), fieldnames=list(out[0].keys()), delimiter="\t", lineterminator="\n")
w.writeheader(); w.writerows(out)
print("nodes", len(nodes), "groups", len(out), "singletons", sum(1 for d in out if d["size"] == 1))
for d in out[:60]:
    print(d["root"], d["root_name"][:28], "n=%d" % d["size"], d["subsystems"][:60], "host:", d["hosts"][:90], "| extNoExe:", len(d["ext_callees_no_exe_body"].split(";")) if d["ext_callees_no_exe_body"] else 0)
