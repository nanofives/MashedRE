#!/usr/bin/env python3
"""Pivot a run_h4 arm's per-(frame, car) CSVs into one row per frame for collateral.py.

<arm>.step.csv (AI step dump, cars 1..3) -> columns s.v<car>.<field>; <arm>.gates.csv (U9186 gate dump,
cars 0..3) -> g.v<car>.<field>. Same layout as the piv_*.csv the P3 collateral read
(verify/d3_modes37_20261009/COLLATERAL_P3_stdout.txt).
Usage: py -3.12 verify/d3_mode7_m3_20261010/pivot_m3.py <dir> <arm> <out.csv>
"""
import csv, sys, collections
from pathlib import Path

d, arm, out = Path(sys.argv[1]), sys.argv[2], sys.argv[3]
rows = collections.defaultdict(dict)
cols = []
for pre, name, skip in (("s", f"{arm}.step.csv", {"frame", "v", "seq"}), ("g", f"{arm}.gates.csv", {"frame", "v"})):
    with open(d / name, newline="") as f:
        for r in csv.DictReader(f):
            try:
                fr, v = int(r["frame"]), int(r["v"])
            except (KeyError, ValueError):
                continue
            for k, x in r.items():
                if k in skip or k is None:
                    continue
                c = f"{pre}.v{v}.{k}"
                if c not in rows[fr]:
                    rows[fr][c] = x
                if fr == 0 or len(cols) < 4000:
                    pass
for fr in rows:
    for c in rows[fr]:
        cols.append(c)
cols = sorted(set(cols))
with open(out, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["frame"] + cols)
    for fr in sorted(rows):
        w.writerow([fr] + [rows[fr].get(c, "") for c in cols])
print(f"{out}: {len(rows)} frames, {len(cols)} columns")
