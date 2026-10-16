"""Per-texture draw and triangle counts on both sides of one frame. Usage: texcount.py <orig> <stand> [--only-diff]"""
import json, sys, collections
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "tools"))
import drawlist3d as d3
def side(p):
    doc = json.loads(Path(p).read_text()); fr = d3.cut_frame(doc)
    return [d3.norm(e, i) for i, e in enumerate(e for e in fr if d3.nprims(e) > 0)]
A, B = side(sys.argv[1]), side(sys.argv[2])
ca, cb = collections.Counter(), collections.Counter(); ta, tb = collections.Counter(), collections.Counter()
for d in A: ca[d["tex"]] += 1; ta[d["tex"]] += d["tri"] or 0
for d in B: cb[d["tex"]] += 1; tb[d["tex"]] += d["tri"] or 0
for t in sorted(set(ca) | set(cb), key=str.lower):
    if "--only-diff" in sys.argv and ca[t] == cb[t] and ta[t] == tb[t]: continue
    print(f"{t:22s} A {ca[t]:3d} draws {ta[t]:6d} tri | B {cb[t]:3d} draws {tb[t]:6d} tri")
