"""Per-car entries into committed mode 7 (ai_mode column of MASHED_AI_STEPDUMP: a row with 7 whose previous row for that car was not 7)
and rows with mode 10, for each step dump given.     py -3.12 mode_entries.py arms/new_K00.step.csv ..."""
import csv
import sys
from pathlib import Path

for p in sys.argv[1:]:
    prev, e7, r10 = {}, {}, {}
    with open(p, newline="") as f:
        for r in csv.DictReader(f):
            v, m = r["v"], r["ai_mode"]
            if m == "7" and prev.get(v) != "7":
                e7[v] = e7.get(v, 0) + 1
            if m == "10":
                r10[v] = r10.get(v, 0) + 1
            prev[v] = m
    print(f"{Path(p).name}: mode-7 entries {dict(sorted(e7.items())) or 0}  mode-10 rows {dict(sorted(r10.items())) or 0}")
