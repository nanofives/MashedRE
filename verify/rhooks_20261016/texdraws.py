"""List the draws of given textures on both sides of one matched-pose frame (3D draw records).
Usage: texdraws.py <orig.draws.json> <stand.draws.json> tex[,tex...]   (case-insensitive; '*' = all)"""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "tools"))
import drawlist3d as d3

def side(path):
    doc = json.loads(Path(path).read_text())
    fr = d3.cut_frame(doc)
    return [d3.norm(e, i) for i, e in enumerate(e for e in fr if d3.nprims(e) > 0)]

A = side(sys.argv[1]); B = side(sys.argv[2])
want = None if sys.argv[3] == '*' else {t.lower() for t in sys.argv[3].split(',')}
for lab, S in (("A", A), ("B", B)):
    print(f"== {lab} ({len(S)} draws)")
    for d in S:
        if want is None or d["texl"] in want:
            print(f"  {lab}{d['i']:4d} {d['tex']:18s} tri={d['tri']} fog={d['fog']} ks={d['ks']} bb={d3.fmtbb(d['bb'])}")
