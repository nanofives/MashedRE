"""Merge the three raw converter runs (g2s_g5 = /O2 /Ob1 /G5, g2s_game = /O2 /GS /Ob1, g2s_o1 = /O1) into g2s/:
per row the best score, ties to /G5 (the RW build). Writes g2s/<va8>.c and g2s/results.tsv."""
import csv, pathlib, shutil
D = pathlib.Path("D:/MashedWT/MATCH-S28_data")
runs = ["g2s_g5", "g2s_game", "g2s_o1"]
res = {r: {x["rva"]: x for x in csv.DictReader(open(D / r / "results.tsv"), delimiter="\t")} for r in runs}
out = D / "g2s"; out.mkdir(exist_ok=True)
hdr = list(next(iter(res["g2s_g5"].values())).keys()) + ["run"]
with open(out / "results.tsv", "w", newline="") as f:
    w = csv.writer(f, delimiter="\t"); w.writerow(hdr)
    for rva in res["g2s_g5"]:
        sc = lambda r: float(res[r].get(rva, {}).get("acc") or -1)
        b = max(runs, key=lambda r: (sc(r), r == "g2s_g5"))
        x = res[b][rva]; w.writerow(list(x.values()) + [b])
        src = D / b / f"{rva[2:]}.c"
        if src.exists(): shutil.copy(src, out / src.name)
