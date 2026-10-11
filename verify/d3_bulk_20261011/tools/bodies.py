import sys, csv, json
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\scripts")
import rva_body_scan as S
sc = S.scan_all()
exe = S.rva_to_tus(sc, "exe"); asi = S.rva_to_tus(sc, "asi")
p = r"C:\Users\maria\Desktop\Proyectos\Mashed\verify\d3_bulk_20261011\executed.tsv"
rows = list(csv.DictReader(open(p, encoding="utf-8"), delimiter="\t"))
from collections import Counter
c = Counter()
for r in rows:
    v = int(r["rva"], 16)
    e, a = exe.get(v, []), asi.get(v, [])
    r["body"] = "exe" if e else ("asi-only" if a else "none")
    r["exe_body_tu"] = ";".join(e); r["asi_body_tu"] = ";".join(a)
    c[r["body"]] += 1
w = csv.DictWriter(open(p, "w", newline="", encoding="utf-8"), fieldnames=list(rows[0].keys()), delimiter="\t", lineterminator="\n")
w.writeheader(); w.writerows(rows)
print(c)
