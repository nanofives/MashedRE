import csv, sys, collections
hooks, reach, out = sys.argv[1], sys.argv[2], sys.argv[3]
SUB = "third-party-library[RenderWare-Physics-3.7]"
rows = [r for r in csv.DictReader(open(hooks, newline="", encoding="utf-8-sig")) if r["subsystem"] == SUB]
rch = {r["rva"]: r for r in csv.DictReader(open(reach, newline=""), delimiter="\t")}
BANDS = [("BROAD", 0x558000, 0x55f000), ("BVH", 0x55f000, 0x566000), ("SOLV", 0x566000, 0x570000),
         ("NARROW", 0x570000, 0x57c000), ("TAIL", 0x57c000, 0x57d000)]
def band(v):
    for n, lo, hi in BANDS:
        if lo <= v < hi: return n
    return "OTHER"
sub = [r for r in rows if r["confidence"] != "C4"]
print("rows", len(rows), "sub-C4", len(sub))
miss = []
tab = collections.Counter()
recs = []
for r in sub:
    v = int(r["rva"], 16) + (0x400000 if int(r["rva"], 16) < 0x400000 else 0)
    b = band(v)
    c = r["confidence"] + "/" + r["status"]
    x = rch.get(r["rva"])
    if x is None:
        miss.append(r["rva"]); ex = "MISSING"; m = rr = "-"; ph = ""
    else:
        menu, race = x["reached_menu"] == "1", x["reached_race"] == "1"
        ex = "EXEC" if (menu or race) else "NOT-EXEC"
        ph = ",".join(p for p, k in (("menu", "reached_menu"), ("race_load", "reached_race_load"),
                                     ("race_run", "reached_race_run")) if x[k] == "1")
        m, rr = x["runs_menu"], x["runs_race"]
    tab[(b, c, ex)] += 1
    recs.append((r["rva"], b, r["confidence"], r["status"], ex, ph, m, rr, r["name"]))
with open(out, "w", newline="") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "band", "confidence", "status", "census", "phases", "runs_menu", "runs_race", "name"])
    for t in sorted(recs, key=lambda t: int(t[0], 16)): w.writerow(t)
print("missing", miss)
tot = collections.Counter(t[4] for t in recs); print(dict(tot))
for b, _, _ in BANDS + [("OTHER", 0, 0)]:
    line = {k: v for k, v in tab.items() if k[0] == b}
    if line: print(b, sorted((k[1], k[2], v) for k, v in line.items()))
byc = collections.Counter((t[2] + "/" + t[3], t[4]) for t in recs); print(sorted(byc.items()))
borderline = [t for t in recs if t[6] not in ("0/3", "3/3", "-") or t[7] not in ("0/3", "3/3", "-")]
print("non-unanimous", [(t[0], t[6], t[7]) for t in borderline])
