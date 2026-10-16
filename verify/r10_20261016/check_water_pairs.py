"""r10: evidence for C11b. For each Sands / Forest frame: the original's animated waterNN draws (cited by C11) against the
standalone's static wet_watercol00 draws: draw count, triangle count and the union AABB."""
import glob, json, importlib.util, re
from pathlib import Path
HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("d3", HERE.parent.parent / "re" / "tools" / "drawlist3d.py")
d3 = importlib.util.module_from_spec(spec); spec.loader.exec_module(d3)
CAP = HERE.parent / "capfix_20261016"
lines, ok = [], True
for trk in ("05_Sands", "09_Forest"):
    for f in sorted(glob.glob(str(CAP / trk / "orig_row_*.draws.json"))):
        k = int(Path(f).name.split("_")[2].split(".")[0])
        A = [e for e in d3.cut_frame(json.loads(Path(f).read_text())) if re.match(r"^water\d+$", d3.clean(e.get("tex")), re.I) and e.get("tri")]
        B = [e for e in d3.cut_frame(json.loads((HERE / trk / "stand" / "d1r2" / f"stand_row_{k:05d}.draws.json").read_text()))
             if d3.clean(e.get("tex")).lower() == "wet_watercol00" and e.get("tri")]
        def agg(L):
            bb = [min(e["bb"][i] for e in L) if i < 3 else max(e["bb"][i] for e in L) for i in range(6)] if L else None
            return len(L), sum(e["tri"] for e in L), bb
        (na, ta, ba), (nb, tb, bb_) = agg(A), agg(B)
        same = (na, ta) == (nb, tb) and ba and bb_ and all(abs(x - y) <= 0.06 for x, y in zip(ba, bb_))
        ok &= bool(same)
        fmt = lambda b: "-" if not b else "[" + ",".join(f"{v:.2f}" for v in b) + "]"
        lines.append(f"{trk} row {k}: A waterNN {na} draws {ta} tri {fmt(ba)} | B wet_watercol00 {nb} draws {tb} tri {fmt(bb_)} | {'EQUAL' if same else 'DIFFER'}")
(HERE / "evidence_water_pairs.txt").write_text("\n".join(lines) + f"\nALL EQUAL: {ok}\n")
print("\n".join(lines)); print("ALL EQUAL:", ok)
