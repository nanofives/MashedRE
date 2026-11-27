# RO-AUDIOINIT-DEPTH: the functions the ORIGINAL executes between the first enter of 0x005b8910 (the stop) and the
# first enter of 0x0045d748 (the last point before 0x0045d6a5 stores 0x00604604, no call between), from the counter
# snapshots of detorig --prov-count / --prov-snap (prov_*.json), crossed with hooks.csv and the binder state
# (re/tools/bind/state/*/plan.json) into five classes.
#   py -3.12 verify/audioinit_depth_20261127/depth_join.py <prov.json> <out.tsv>
# Class precedence (first that holds):
#   bound        a bind or reuse line in any state/<profile>/plan.json
#   c4_unbound   hooks.csv confidence C4, not bound
#   no_row       no hooks.csv row (a source file, if any, is listed in the src column)
#   source_not_c4  hooks.csv row below C4 with a re/match source (<rva>.c anywhere under re/match)
#   no_source    hooks.csv row below C4 with no re/match source
import csv, json, re, sys
from collections import Counter, defaultdict
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
prov, outp = Path(sys.argv[1]), Path(sys.argv[2])
P = json.loads(prov.read_text())
rows, S = P["rows"], P["snaps"]
names = ["0x%08x" % int(r["rva"], 16) for r in rows]
A, B = "0x005b8910:enter", "0x0045d748:enter"
for k in S:
    print(k, {x: S[k].get(x) for x in ("seq", "n", "tick", "tid", "err")})
a, b = S[A]["counts"], S[B]["counts"]

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

win = []
for i, k in enumerate(names):
    d = b[i] - a[i]
    if d > 0 or k == "0x005b8910":
        win.append((k, d, a[i]))
cls_n, cls_b = Counter(), Counter()
with open(outp, "w", newline="", encoding="utf-8") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "class", "calls_in_window", "calls_before_window", "size", "confidence", "c4_route", "name", "subsystem", "bound_in", "stop_in", "src"])
    for k, d, pre in sorted(win):
        conf, route, name, sub = hooks.get(k, ("", "", "", ""))
        if bound.get(k): c = "bound"
        elif conf == "C4": c = "c4_unbound"
        elif k not in hooks: c = "no_row"
        elif srcs.get(k): c = "source_not_c4"
        else: c = "no_source"
        cls_n[c] += 1; cls_b[c] += sizes.get(k, 0)
        w.writerow([k, c, d, pre, sizes.get(k, ""), conf, route, name, sub, ";".join(bound.get(k, [])), ";".join(stops.get(k, [])), ";".join(srcs.get(k, []))])
print("distinct", len(win), "instrumented", len(names))
for c in ("bound", "c4_unbound", "source_not_c4", "no_source", "no_row"):
    print(f"{c}\t{cls_n[c]}\t{cls_b[c]} B")
rest = [c for c in cls_n if c != "bound"]
print("rest of lane (not bound):", sum(cls_n[c] for c in rest), "functions,", sum(cls_b[c] for c in rest), "B")
