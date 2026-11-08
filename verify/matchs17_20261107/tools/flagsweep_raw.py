"""Flag sweep over raw g2s outputs (D:/MashedWT/MATCH-S17_data/g2s/<va8>.c) that compile and are below 100%."""
import sys, pathlib, csv
sys.path.insert(0, "re/tools/match")
import match
G = pathlib.Path("D:/MashedWT/MATCH-S17_data/g2s")
VAR = ["/O2 /GS /Ob1", "/O1", "/O1 /GS", "/Os", "/Ox", "/O2", "/O2 /Ob1", "/O1 /Gy", "/O2 /G5", "/O1 /G5", "/Ox /G5"]
rows = [r for r in csv.DictReader(open(G / "results.tsv"), delimiter="\t") if r["stage"] == "ok" and r["acc"] != "100.0"]
for r in rows:
    va = int(r["rva"], 16); res = []
    for v in VAR:
        try:
            x = [y for y in match.compare(G / f"{va:08x}.c", v, quiet=True) if y["addr"] == va]
            res.append((x[0]["acc"], x[0]["regs"], v) if x else (0, 0, v))
        except SystemExit:
            res.append((0, 0, v + " ERR"))
    b = max(res)
    print(f"0x{va:08x}\t" + "\t".join(f"{a*100:.0f}:{v}" for a, _, v in res) + f"\tBEST {b[0]*100:.1f} {b[2]}", flush=True)
