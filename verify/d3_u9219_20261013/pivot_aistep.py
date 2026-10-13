#!/usr/bin/env python3
"""Pivot a per-(frame, car) AI step CSV (original <out>.aistep.csv or exe <arm>.step.csv) into one row per frame,
columns s.v<car>.<field>, the layout pivot_m3.py writes, so collateral.py can read both sides.
Usage: py -3.12 pivot_aistep.py <in.csv> <out.csv>"""
import csv, sys, collections
src, out = sys.argv[1], sys.argv[2]
rows = collections.defaultdict(dict); cols = set()
with open(src, newline="") as f:
    for r in csv.DictReader(f):
        try: fr, v = int(r["frame"]), int(r["v"])
        except (KeyError, ValueError): continue
        for k, x in r.items():
            if k in ("frame", "v", "seq") or k is None: continue
            c = f"s.v{v}.{k}"
            if c not in rows[fr]: rows[fr][c] = x; cols.add(c)
cols = sorted(cols)
with open(out, "w", newline="") as f:
    w = csv.writer(f); w.writerow(["frame"] + cols)
    for fr in sorted(rows): w.writerow([fr] + [rows[fr].get(c, "") for c in cols])
print(f"{out}: {len(rows)} frames, {len(cols)} columns")
