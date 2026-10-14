#!/usr/bin/env python3
"""Pivot a per-(frame, car) AI step CSV (original <out>.aistep.csv or exe <arm>.step.csv) to one row per frame
(columns s.v<car>.<field>, the verify/d3_u9219_20261013/pivot_aistep.py layout) and keep the 600 frames starting at
the first frame with s.v2.substate >= 6 (the collateral anchor of RESULT_CAR2PICKUP.md section 5).
Usage: py -3.12 cut600.py <in.csv> <out.csv>"""
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
frames = sorted(rows)
def ss(fr):
    try: return float(rows[fr].get("s.v2.substate", ""))
    except ValueError: return -1.0
a = next(i for i, fr in enumerate(frames) if ss(fr) >= 6)
keep = frames[a:a + 600]
cols = sorted(cols)
with open(out, "w", newline="") as f:
    w = csv.writer(f); w.writerow(["frame"] + cols)
    for fr in keep: w.writerow([fr] + [rows[fr].get(c, "") for c in cols])
print(f"{out}: anchor frame {frames[a]}, kept {len(keep)} frames, {len(cols)} columns")
