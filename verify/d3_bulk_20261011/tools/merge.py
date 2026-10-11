# Merge first-hit batches -> executed.tsv; join hooks.csv, exe/asi source mentions, call edges, groups.
import csv, json, re, os, collections, sys
R = r"C:\Users\maria\Desktop\Proyectos\Mashed"
V = os.path.join(R, "verify", "d3_bulk_20261011")
SP = os.path.dirname(os.path.abspath(__file__))
CTRL0 = {0x004847d0, 0x0047cd50, 0x00458d00}
hits = {}
for b in "ab":
    with open(os.path.join(V, "fh_%s.msd.firsthit.tsv" % b)) as f:
        for r in csv.DictReader(f, delimiter="\t"):
            v = int(r["rva"], 16)
            hits[v] = r
cand = {}
with open(os.path.join(V, "candidates.tsv")) as f:
    for r in csv.DictReader(f, delimiter="\t"):
        cand[int(r["rva"], 16)] = r
hooks = {}
with open(os.path.join(R, "hooks.csv"), encoding="utf-8", errors="replace", newline="") as f:
    for r in csv.DictReader(f):
        try: hooks[int(r["rva"], 16)] = r
        except Exception: pass
src = os.path.join(R, "mashedmod", "src", "mashed_re")
def rsp(name):
    out = set()
    for ln in open(os.path.join(R, "mashedmod", name)):
        ln = ln.strip().strip('"')
        if ln: out.add(os.path.normpath(ln).lower())
    return out
EXE, ASI = rsp("exe_sources.rsp"), rsp("asi_sources.rsp")
mention = collections.defaultdict(set)
pat = re.compile(r"(?:0x|FUN_|_)(00[4-5][0-9a-fA-F]{5})\b")
for dp, dn, fn in os.walk(src):
    for n in fn:
        if not n.endswith((".cpp", ".h", ".inc", ".hpp")): continue
        p = os.path.join(dp, n)
        rel = os.path.relpath(p, src)
        try: txt = open(p, encoding="utf-8", errors="replace").read()
        except OSError: continue
        for m in set(pat.findall(txt)):
            mention[int(m, 16)].add(rel)
cg = {int(k, 16): [int(t, 16) for t in v] for k, v in json.load(open(os.path.join(SP, "cg.json"))).items()}
callers = collections.defaultdict(set)
for k, v in cg.items():
    for t in v: callers[t].add(k)
ex = sorted(v for v, r in hits.items() if int(r["p3"]) > 0)
exs = set(ex)
def tu_class(files):
    e = [f for f in files if os.path.normpath(f).lower() in EXE or (f.endswith((".inc", ".h")) )]
    ecpp = [f for f in files if os.path.normpath(f).lower() in EXE]
    acpp = [f for f in files if os.path.normpath(f).lower() in ASI and os.path.normpath(f).lower() not in EXE]
    return ecpp, acpp
rows = []
for v in ex:
    h = hooks.get(v, {})
    files = sorted(mention.get(v, ()))
    ecpp, acpp = tu_class(files)
    exe_status = "exe-tu" if ecpp else ("asi-only" if acpp else ("mention-other" if files else "none"))
    ex_callers = sorted(c for c in callers.get(v, ()) if c in exs)
    ex_callees = sorted(c for c in cg.get(v, ()) if c in exs)
    r = hits[v]
    rate = ""
    try:
        if int(r["tcap_tick"]) >= 0: rate = "%d/%d" % (int(r["p3"]), int(r["tcap_tick"]) - int(r["t3_tick"]))
    except Exception: pass
    rows.append(dict(rva="0x%08x" % v, conf=h.get("confidence", ""), status=h.get("status", ""), subsystem=h.get("subsystem", ""),
        name=h.get("name", ""), p2=r["p2"], p3=r["p3"], f3=r["f3_sdframe"], hits_per_ticks=rate,
        exe_status=exe_status, exe_tus=";".join(ecpp), asi_tus=";".join(acpp),
        exec_callers=";".join("0x%08x" % c for c in ex_callers), exec_callees=";".join("0x%08x" % c for c in ex_callees),
        exe_file_hooks=h.get("exe_file", ""), notes=h.get("notes", "")[:160].replace("\t", " ").replace("\n", " ")))
with open(os.path.join(V, "executed.tsv"), "w", newline="", encoding="utf-8") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0].keys()), delimiter="\t", lineterminator="\n")
    w.writeheader(); w.writerows(rows)
zero = sorted(v for v, r in hits.items() if int(r["p3"]) == 0)
with open(os.path.join(V, "not_executed.tsv"), "w", newline="", encoding="utf-8") as f:
    f.write("rva\tp1\tp2\tconf\tsubsystem\tname\n")
    for v in zero:
        h = hooks.get(v, {}); r = hits[v]
        f.write("0x%08x\t%s\t%s\t%s\t%s\t%s\n" % (v, r["p1"], r["p2"], h.get("confidence", ""), h.get("subsystem", ""), h.get("name", "")))
c = collections.Counter((r["conf"], r["exe_status"]) for r in rows)
print("executed", len(rows), "not", len(zero)); print(sorted(c.items()))
print(collections.Counter(r["subsystem"] for r in rows).most_common())
