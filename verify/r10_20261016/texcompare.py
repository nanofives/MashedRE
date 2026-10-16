"""r10: per RED (track, texture) compare the original's and the standalone's draws of that texture.

    py -3.12 texcompare.py [--prefix diff0_] [--tracks 7,11] [--tex REGEX] [--frames N]

For every RED row (no "cite") of every scored frame, prints the original-side and standalone-side draws carrying the
row's texture in the recorded frame: triangle count, world AABB and whether the AABB sits at the world origin
(max |coord| <= 3, the "drawn at the origin / unplaced clump" signature of C13). Used to read the cause of a RED
row off the data before any cite is written. Output goes to stdout (redirect into red_detail_<prefix>.txt).
"""
import argparse
import glob
import json
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent
CAP = HERE.parent / "capfix_20261016"


import importlib.util
_spec = importlib.util.spec_from_file_location("d3", HERE.parent.parent / "re" / "tools" / "drawlist3d.py")
d3 = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(d3)


def draws(path, tex):
    """Draws of one texture in the frame of the record, cut exactly as the differ cuts it (cut_frame)."""
    doc = json.loads(Path(path).read_text())
    out = []
    for e in d3.cut_frame(doc):
        if d3.clean(e.get("tex")) != tex:
            continue
        if e.get("tri") in (None, 0):
            continue
        out.append((e["tri"], e.get("bb"), e.get("pt"), e.get("rs"), e.get("w"), e.get("fvf"), d3.clean(e.get("tex1", "-")),
                    e.get("bt")))
    return out


def fmt(bb):
    return "-" if not bb else "[" + ",".join(f"{v:.1f}" for v in bb) + "]"


def at_origin(bb):
    return bb is not None and max(abs(v) for v in bb) <= 3.0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--prefix", default="diff0_")
    ap.add_argument("--tracks", default="")
    ap.add_argument("--tex", default=".*")
    ap.add_argument("--max", type=int, default=12, help="draws printed per side")
    a = ap.parse_args()
    for f in sorted(glob.glob(str(HERE / "[0-9][0-9]_*" / f"{a.prefix}[0-9]*.json"))):
        p = Path(f)
        trk = p.parent.name
        if a.tracks and trk.split("_")[0].lstrip("0") not in a.tracks.split(","):
            continue
        k = int(p.stem.split("_")[-1])
        rows = json.loads(p.read_text())["rows"]
        texs = sorted({r["tex"] for r in rows if "cite" not in r and r["tex"] not in ("-", "*")
                       and re.search(a.tex, r["tex"])})
        if not texs:
            continue
        print(f"##### {trk} row {k}")
        for t in texs:
            A = draws(CAP / trk / f"orig_row_{k:05d}.draws.json", t)
            B = draws(p.parent / "stand" / "d1r2" / f"stand_row_{k:05d}.draws.json", t)
            kinds = sorted({r["kind"] for r in rows if r["tex"] == t and "cite" not in r})
            print(f"  [{t}] RED kinds {kinds}: orig {len(A)} draws / {sum(x[0] for x in A)} tri, "
                  f"stand {len(B)} draws / {sum(x[0] for x in B)} tri; "
                  f"stand at-origin {sum(1 for x in B if at_origin(x[1]))}")
            for side, L in (("A", A), ("B", B)):
                for x in L[:a.max]:
                    print(f"      {side} tri={x[0]:<5} pt={x[2]} bb={fmt(x[1])}{' ORIGIN' if at_origin(x[1]) else ''}")
                if len(L) > a.max:
                    print(f"      {side} ... {len(L) - a.max} more")


if __name__ == "__main__":
    main()
