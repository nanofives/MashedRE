# RO-WARP-NORMALIZE scoring: checkpoint class counts per pair, before (batch 2, no normalisation file) and after
# (batch 3, boot-counter pins + --norm masks / rules), plus batch 2 re-scored with the masks only.
#   py -3.12 verify/replayoracle_20261111/score_norm.py [--json OUT]
import argparse, itertools, json, sys
from collections import Counter
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "frida"))
from detorig_report import report
NORM = str(Path(__file__).resolve().parents[2] / "re" / "frida" / "warp_normalize.tsv")
B2, B3 = "D:/MashedWT/REPLAY-ORACLE-DESIGN_data/", "D:/MashedWT/RO-WARP-NORMALIZE_data/"
ap = argparse.ArgumentParser(); ap.add_argument("--json", default=""); a = ap.parse_args()
sets = [("before: batch 2, no file", B2, ["t2_a", "t2_b", "t2_c", "t2_h"], None, "t2_neg"),
        ("batch 2, masks/rules only", B2, ["t2_a", "t2_b", "t2_c", "t2_h"], NORM, "t2_neg"),
        ("after: batch 3, pins + masks/rules", B3, ["n3_a", "n3_b", "n3_c", "n3_h", "n3_w"], NORM, "n3_neg")]
out = {}
for name, D, runs, norm, neg in sets:
    print(f"== {name}")
    tot = Counter(); rows = []
    for x, y in list(itertools.combinations(runs, 2)) + [(runs[1], neg)]:
        r = report(D + x, D + y, 0, norm)
        cc = {k: v for k, v in r["class_counts"].items()}
        nonptr = {k: v for k, v in cc.items() if k != "PTR"}
        print(f"  {x} vs {y}: normalized {r['normalized_identical']}/{r['checkpoints']} race-state {r['race_state_identical']}/{r['checkpoints']}  "
              f"non-PTR classes {sum(nonptr.values())} {dict(sorted(nonptr.items()))}  PTR {cc.get('PTR', 0)}")
        rows.append({"a": x, "b": y, "normalized": r["normalized_identical"], "race_state": r["race_state_identical"], "classes": cc,
                     "residue": r["residue"], "unexplained_sample": r["unexplained_sample"][:10],
                     "remaining": sorted({f"0x{va:08x} {c}" for va, c in r["_cls"].items() if c != "PTR"})})
    out[name] = rows
if a.json:
    Path(a.json).write_text(json.dumps(out, indent=1))
