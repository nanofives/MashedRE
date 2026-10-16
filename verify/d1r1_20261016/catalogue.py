"""d1r1: aggregate the 3D draw-list rows of every adjudicated frame into one catalogue.

    py -3.12 catalogue.py <log/d1r1 dir> <diff json glob, e.g. diff0_*.json> [--cite cites.json]

Prints one line per (kind, texture) with the frames it occurs on, its draw count range and how
many of its draws are off-screen, plus the per-frame totals. With --cite, the rows are run through
the same citation matcher as drawlist_diff.py and the uncited ones are listed.
"""
import json
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "tools"))
import drawlist3d  # noqa: E402


def main() -> int:
    base, pat = Path(sys.argv[1]), sys.argv[2]
    cites = []
    if "--cite" in sys.argv:
        cites = json.loads(Path(sys.argv[sys.argv.index("--cite") + 1]).read_text())
    agg = defaultdict(list)
    frames = []
    for f in sorted(base.glob(f"orig_*/{pat}")):
        j = json.loads(f.read_text())
        rows = j["rows"]
        drawlist3d.apply_cites(rows, cites)
        tag = f"{f.parent.name[5:]}:{f.stem.split('_')[-1].lstrip('0')}"
        red = [r for r in rows if "cite" not in r]
        frames.append((tag, j["na"], j["matched"], j["tri_a"], len(rows), len(red)))
        for r in rows:
            agg[(r["kind"], r["tex"])].append((tag, r["n"], r.get("offscreen"), r.get("cite"), r["detail"]))
    print("frame           orig_draws matched orig_tri rows red")
    for t in frames:
        print(f"  {t[0]:<14} {t[1]:>9} {t[2]:>7} {t[3]:>8} {t[4]:>4} {t[5]:>3}")
    print()
    for (kind, tex), lst in sorted(agg.items(), key=lambda kv: (kv[0][0], kv[0][1].lower())):
        tracks = sorted({x[0].split(':')[0] for x in lst})
        ns = [x[1] for x in lst]
        offs = [x[2] for x in lst if x[2] is not None]
        allo = offs and all(x[2] == x[1] for x in lst)
        cited = {x[3] for x in lst}
        print(f"{kind:<8} {tex:<22} frames={len(lst)} tracks={','.join(tracks)} n={min(ns)}-{max(ns)}"
              + (f" offscreen={'ALL' if allo else sum(offs)}" if offs else "")
              + (f" cite={','.join(sorted(c or '-' for c in cited))}" if cites else "")
              + f" | {lst[0][4][:150]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
