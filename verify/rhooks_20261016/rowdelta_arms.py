"""Row-level comparison of two scoring runs of the same capfix frames (rhooks).
Usage: rowdelta_arms.py <base_dir> <base_prefix> <arm_dir> <arm_prefix> [--cite-filter C22,C24]
Prints per track/frame: rows only in base (gone), rows only in arm (new), and a per-cite count table."""
import json, sys, collections
from pathlib import Path
bd, bp, ad, ap = Path(sys.argv[1]), sys.argv[2], Path(sys.argv[3]), sys.argv[4]
flt = None
if "--cite-filter" in sys.argv:
    flt = tuple(sys.argv[sys.argv.index("--cite-filter") + 1].split(","))
def key(r):
    return (r.get("cite") or "UNCITED", r["kind"], r["tex"])
tot = collections.Counter(); tota = collections.Counter()
new_total = 0; gone_total = 0
for td in sorted(p for p in bd.iterdir() if p.is_dir()):
    for f in sorted(td.glob(f"{bp}*.json")):
        if f.name.startswith("cites_used"): continue
        row = f.stem[len(bp):]
        g = ad / td.name / f"{ap}{row}.json"
        if not g.exists():
            print(f"{td.name} {row}: arm missing"); continue
        B = json.loads(f.read_text()); A = json.loads(g.read_text())
        kb = collections.Counter(key(r) for r in B["rows"]); ka = collections.Counter(key(r) for r in A["rows"])
        for k, v in kb.items(): tot[k[0]] += v
        for k, v in ka.items(): tota[k[0]] += v
        gone = kb - ka; new = ka - kb
        gone_total += sum(gone.values()); new_total += sum(new.values())
        sel = lambda c: [k for k in c if (flt is None or k[0].startswith(flt))]
        if gone or new:
            print(f"{td.name} {row}: matched {B['matched']}->{A['matched']} rows {len(B['rows'])}->{len(A['rows'])}")
            for k in sorted(gone): print(f"    - {k[0]:24s} {k[1]:8s} {k[2]}")
            for k in sorted(new):  print(f"    + {k[0]:24s} {k[1]:8s} {k[2]}")
print(f"rows gone {gone_total}, rows new {new_total}")
print("per-cite rows base -> arm:")
for c in sorted(set(tot) | set(tota)):
    if tot[c] != tota[c]: print(f"  {c:26s} {tot[c]:4d} -> {tota[c]:4d}")
