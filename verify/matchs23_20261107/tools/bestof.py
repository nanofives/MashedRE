"""Merge g2s_game and g2s_g5 into g2s/: per function the higher (acc, regs) wins; copies the source."""
import csv, pathlib, shutil
D = pathlib.Path("D:/MashedWT/MATCH-S23_data")
def load(n): return {r["rva"]: r for r in csv.DictReader(open(D / n / "results.tsv"), delimiter="\t")}
a, b = load("g2s_game"), load("g2s_g5")
out = D / "g2s"; out.mkdir(exist_ok=True)
def key(r):
    try: return (float(r["acc"]), float(r["regs"]))
    except Exception: return (-1, -1)
rows = []; cnt = {"game": 0, "g5": 0, "eq": 0}
for k in a:
    ra, rb = a[k], b.get(k, a[k])
    pick, src = (rb, "g2s_g5") if key(rb) > key(ra) else (ra, "g2s_game")
    cnt["eq" if key(ra) == key(rb) else ("g5" if src == "g2s_g5" else "game")] += 1
    f = D / src / f"{k[2:]}.c"
    if f.exists(): shutil.copy(f, out / f.name)
    pick = dict(pick, flags="/O2 /Ob1 /G5" if src == "g2s_g5" else "/O2 /GS /Ob1"); rows.append(pick)
with open(out / "results.tsv", "w", newline="") as fo:
    w = csv.DictWriter(fo, fieldnames=list(rows[0].keys()), delimiter="\t"); w.writeheader(); w.writerows(rows)
print(cnt, "100%:", sum(1 for r in rows if key(r)[0] >= 100), "game100:", sum(1 for r in a.values() if key(r)[0] >= 100), "g5_100:", sum(1 for r in b.values() if key(r)[0] >= 100))
