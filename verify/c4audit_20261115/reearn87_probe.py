"""C4-NOTAG-87-REEARN step 1 probe: one batch TSV per candidate-source round (a batch refuses a duplicate rva).

Round k holds, for every row of reearn87_rows.tsv with at least k+1 candidate sources, its k-th source, route match,
reearn=1. Run each with `c4_reclass.py batch --tsv <file> --report <out>` (dry run, the gate recompiles).
"""
import csv, pathlib

D = pathlib.Path(__file__).resolve().parent
rows = list(csv.DictReader(open(D / "reearn87_rows.tsv", encoding="utf-8", newline=""), delimiter="\t"))
k = 0
while True:
    out = [(r["rva"], s) for r in rows for i, s in enumerate(r["sources"].split(",")) if s and i == k]
    if not out:
        break
    with open(D / f"reearn87_probe{k}.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["rva", "route", "evidence", "reearn"])
        w.writerows((r, "match", s, "1") for r, s in out)
    print(f"round {k}: {len(out)} rows")
    k += 1
