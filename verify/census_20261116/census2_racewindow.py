# T2-RACEPATH-CENSUS fallback for the race tick (NOT this row's measurement): the only measured race-window call counts
# on record, PROV-INSITU's census2 (verify/provinsitu_20261111/state/census2_prov.json; 326 counted RVAs, scenario
# track 0 / mode 10 / 4 cars, dn_warp 121, 3000-tick cap: a different recipe from ref1). Joined with this row's status
# and binding rules (census_join.py precedence: plan.json, BINDINGS.tsv, ROUTED.tsv, static reach, else unbound).
#   py -3.12 verify/census_20261116/census2_racewindow.py <out.tsv>
import csv, json, re, sys
from collections import Counter
from pathlib import Path
WT = Path(__file__).resolve().parent.parent.parent
P = json.loads((WT / "verify/provinsitu_20261111/state/census2_prov.json").read_text())
c8 = {}
for r in csv.DictReader(open(WT / "verify/census_20261116/census_T2-RACEPATH-CENSUS.tsv"), delimiter="\t"):
    c8.setdefault(r["rva"], set()).add(r["phase"])
def rv(x): return "0x%08x" % int(x, 16)
hooks = {}
for r in csv.DictReader(open(WT / "hooks.csv", encoding="utf-8")):
    try: k = rv(r["rva"])
    except ValueError: continue
    m = re.search(r"c4=([a-z-]+)", r.get("notes") or "")
    hooks[k] = (r["confidence"] + (":" + m.group(1) if m and r["confidence"] == "C4" else ""), r["name"], r["subsystem"])
def tsv(path):
    lines = [l for l in open(path, encoding="utf-8") if not l.startswith("#")]
    return {rv(r["rva"]): r for r in csv.DictReader(lines, delimiter="\t") if r.get("rva", "").startswith("0x")}
provis = tsv(WT / "re/match/PROVISIONAL.tsv"); admitted = tsv(WT / "re/match/ADMITTED.tsv")
bindings = tsv(WT / "verify/porttop_20261108/BINDINGS.tsv"); routed = tsv(WT / "verify/rwccorerun_20261111/ROUTED.tsv")
plan = {("0x" + k.lower()): v for k, v in json.loads((WT / "verify/portorigtop_20261113/tools/plan.json").read_text()).items()}
reach = {l.split("\t")[0] for l in (WT / "verify/census_20261116/reach_top_static.tsv").read_text().splitlines()[1:]}
BK = {"matched": "matched", "bound": "rw-lib", "crt": "crt-lib", "shim": "shim", "port": "hand-port", "near-match": "provisional",
      "stand-in": "stand-in", "under-stand-in": "under-stand-in", "missing": "missing", "rw": "unbound"}
def binding(k):
    st = hooks.get(k, ("none", "", ""))[0]
    if k in plan:
        v = plan[k]
        if v[0] == "bind": return {"match": "matched", "replay": "replay", "admitted": "admitted", "provisional": "provisional"}[v[2]]
        if v[0] == "stop": return "stop"
        if k in bindings: return BK.get(bindings[k]["kind"], bindings[k]["kind"])
        return "matched" if st in ("C4:match", "C4:libid") else "hand-port"
    if k in bindings: return BK.get(bindings[k]["kind"], bindings[k]["kind"])
    if k in routed and routed[k]["verdict"] == "routed": return {"match100": "matched"}.get(routed[k]["kind"], routed[k]["kind"])
    if k in reach: return "matched" if st in ("C4:match", "C4:libid") else "exe-body"
    return "unbound"
rows = []
for r in P["rows"]:
    n = r["calls_total"] - (r["calls_to_anchor"] or 0)
    if n <= 0: continue
    k = r["rva"].lower(); st, nm, ss = hooks.get(k, ("none", "", ""))
    rows.append([k, nm, n, st + (f" prov({provis[k]['status']} {provis[k]['best_pct']})" if k in provis else "") + (" admitted" if k in admitted else ""),
                 binding(k), ss, "race-tick-only in this scenario" if k not in c8 else "also ran up to the anchor in c8 (" + ",".join(sorted(c8[k])) + ")"])
rows.sort(key=lambda x: -x[2])
with open(sys.argv[1], "w", newline="") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "name", "race_window_calls_census2", "status", "binding", "subsystem", "c8"]); w.writerows(rows)
print("census2 race-window executed", len(rows), "of", len(P["rows"]))
print("binding", dict(Counter(r[4] for r in rows)))
print("status", dict(Counter(r[3].split(" ")[0] for r in rows)))
only = [r for r in rows if r[6].startswith("race-tick-only")]
print("race-tick-only", len(only), "binding", dict(Counter(r[4] for r in only)), "status", dict(Counter(r[3].split(" ")[0] for r in only)))
for cat in ("provisional", "stand-in", "missing", "stop", "unbound", "exe-body", "hand-port"):
    print(cat, [(r[0], r[1][:24], r[2]) for r in rows if r[4] == cat][:12])
