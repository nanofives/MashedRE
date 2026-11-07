"""Prints the flag counts, the below-100% table and the raw-hash lines for RESULT_MATCH-S28.md.
Run from the worktree: py -3.12 verify/matchs28_20261107/tools/mkresult_tables.py"""
import csv, re, pathlib, collections, hashlib
M = pathlib.Path("verify/matchs28_20261107"); D = pathlib.Path("D:/MashedWT/MATCH-S28_data")
rows = list(csv.DictReader(open(M / "results.tsv"), delimiter="\t"))
notes = {}
for h in ("h1", "h2", "h3"):
    for ln in open(D / "notes" / f"{h}.md", encoding="utf-8"):
        m = re.match(r"(0x[0-9a-f]{8})\s+(\d+)\s+([\d.]+)\s+([\d.]+)\s+cxx:(\w+)\s+(\S+)\s+(.*)", ln.strip())
        if m: notes[m.group(1)] = m.groups()
fl = collections.Counter(); ident = [r for r in rows if r["best%"] == "100.0"]
for r in ident:
    fl[open(r["source"], encoding="utf-8").readline().strip()] += 1
print("identical", len(ident), dict(fl))
cls = collections.Counter()
print("| rva | size | best % | regs % | class | residual |\n|---|---|---|---|---|---|")
low = [r for r in rows if r["best%"] != "100.0"]
for r in sorted(low, key=lambda r: -float(r["best%"])):
    n = notes.get(r["rva"]); c = n[5] if n else "?"; cls[c] += 1
    print(f"| {r['rva']} | {r['size']} | {r['best%']} | {r['regs%']} | {c} | {n[6] if n else ''} |")
print(dict(cls))
