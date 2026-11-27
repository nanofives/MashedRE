# RACE-PIZ-DEVICE CUT wall count: the race_load functions the ORIGINAL first executes AFTER the COURSE.LUA open
# (counter 0 at the first enter of 0x0047a020 CourseDescInit, above 0 at the race anchor), from detorig --prov-count /
# --prov-snap (prov_anchor.json), crossed with hooks.csv and the binder state into the RO-AUDIOINIT-DEPTH classes
# (verify/audioinit_depth_20261127/depth_join.py), plus a prefix list: functions first executed between the 0x0047a020
# enter and the open (they ran on the exe arm before its stop, so they are not walls of the cut).
#   py -3.12 verify/racepiz_20261127/cut_join.py <prov_anchor.json> <out.tsv>
# Class precedence (first that holds): bound (bind / reuse line in any state/<profile>/plan.json), c4_unbound,
# no_row, source_not_c4, no_source.
import csv, json, re, sys
from collections import Counter, defaultdict
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
prov, outp = Path(sys.argv[1]), Path(sys.argv[2])
P = json.loads(prov.read_text())
rows, S = P["rows"], P["snaps"]
names = ["0x%08x" % int(r["rva"], 16) for r in rows]
A = "0x0047a020:enter"
for k in S:
    print(k, {x: S[k].get(x) for x in ("seq", "n", "tick", "tid", "err")})
a = S[A]["counts"]
anchor = [r["calls_to_anchor"] for r in rows]
warp = [r["calls_before_warp"] for r in rows]

sizes = {}
for r in csv.reader(open(WT / "re/console/pc_funcs.csv")):
    if r and r[0].startswith("0x"):
        sizes["0x%08x" % int(r[0], 16)] = int(r[2])
hooks = {}
with open(WT / "hooks.csv", newline="", encoding="utf-8") as f:
    for r in csv.DictReader(f):
        try: k = "0x%08x" % int(r["rva"], 16)
        except Exception: continue
        m = re.search(r"c4=([a-z-]+)", r.get("notes") or "")
        hooks[k] = (r["confidence"], m.group(1) if m else "", r["name"], r["subsystem"])
pend = set()
for p in (WT / "re/promotions").glob("*.tsv"):
    for r in csv.DictReader((l for l in open(p, encoding="utf-8") if not l.startswith("#")), delimiter="\t"):
        if r.get("route") == "match" and r.get("rva", "").startswith("0x"):
            pend.add("0x%08x" % int(r["rva"], 16))
bound, stops = defaultdict(list), defaultdict(list)
for pj in sorted((WT / "re/tools/bind/state").glob("*/plan.json")):
    for k, v in json.loads(pj.read_text()).items():
        kk = "0x" + k.lower()
        if v[0] in ("bind", "reuse"): bound[kk].append(f"{pj.parent.name}:{v[0]}:{v[2] if v[0] == 'bind' else v[1]}")
        elif v[0] == "stop": stops[kk].append(pj.parent.name)
srcs = defaultdict(list)
for p in (WT / "re/match").rglob("*.c"):
    m = re.match(r"([0-9a-fA-F]{8})", p.name)
    if m: srcs["0x" + m.group(1).lower()].append(p.relative_to(WT).as_posix())

cut = [(k, anchor[i], a[i]) for i, k in enumerate(names) if a[i] == 0 and anchor[i] > 0]
race_load = sum(1 for i in range(len(names)) if anchor[i] - warp[i] > 0)
cls_n, cls_b = Counter(), Counter()
with open(outp, "w", newline="", encoding="utf-8") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "class", "calls_to_anchor", "size", "confidence", "c4_route", "name", "subsystem", "bound_in",
                "stop_in", "src"])
    for k, n, _ in sorted(cut):
        conf, route, name, sub = hooks.get(k, ("", "", "", ""))
        if k in pend and conf != "C4": conf, route = "C4", "match (pending re/promotions)"
        if bound.get(k): c = "bound"
        elif conf == "C4": c = "c4_unbound"
        elif k not in hooks: c = "no_row"
        elif srcs.get(k): c = "source_not_c4"
        else: c = "no_source"
        cls_n[c] += 1; cls_b[c] += sizes.get(k, 0)
        w.writerow([k, c, n, sizes.get(k, ""), conf, route, name, sub, ";".join(bound.get(k, [])),
                    ";".join(stops.get(k, [])), ";".join(srcs.get(k, []))])
print("instrumented", len(names), "race_load (anchor - warp > 0)", race_load, "cut (0 at", A, ", > 0 at anchor)", len(cut))
for c in ("bound", "c4_unbound", "source_not_c4", "no_source", "no_row"):
    print(f"{c}\t{cls_n[c]}\t{cls_b[c]} B")
rest = [c for c in cls_n if c != "bound"]
print("CUT walls (not bound):", sum(cls_n[c] for c in rest), "functions,", sum(cls_b[c] for c in rest), "B")
