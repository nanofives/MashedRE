#!/usr/bin/env python3
"""Print a draw record as runs: world-sector draws collapse to 'S<id>', other draws keep
texture/tri and states. Usage: annotate_frame.py <track> <draws.json> [--stand]"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE)); sys.path.insert(0, str(HERE.parents[1] / "re" / "tools"))
import bsp_order  # noqa: E402
import drawlist3d  # noqa: E402
from run_bsp9 import TRACKS, MAIN  # noqa: E402


def main():
    track, path = sys.argv[1], sys.argv[2]
    piz, bsp, _far, _rows = TRACKS[track]
    root, sectors, mats = bsp_order.load_world(str(MAIN / "original/TOASTART/TRACKS" / piz), bsp)
    index = {}
    for s in sectors:
        for m, (nm, n, bb) in bsp_order.sector_meshes(s, mats).items():
            index.setdefault(((nm or "").lower(), n), []).append((s["id"], bb))
    doc = json.load(open(path))
    run = None
    for e in drawlist3d.cut_frame(doc):
        if not drawlist3d.nprims(e):
            continue
        key = ((e.get("tex") or "").lower(), e.get("tri"))
        sid = None
        for s, bb in index.get(key, []):
            if e.get("bb") and max(abs(x - y) for x, y in zip(bb, e["bb"])) < 0.05:
                sid = s
        rs = e["rs"]
        st = f"AB{rs[6]} C{rs[9]} Z{rs[0]}"
        if sid is not None:
            tag = f"S{sid}"
            if run and run[0] == tag:
                run[1] += 1
                run[2].add(st)
                continue
            if run:
                print(f"  {run[0]} x{run[1]} {sorted(run[2])}")
            run = [tag, 1, {st}]
            continue
        if run:
            print(f"  {run[0]} x{run[1]} {sorted(run[2])}")
            run = None
        print(f"  {e.get('tex')}/{e.get('tex1')} {e.get('tri')} {st} vs{e.get('vs')}")
    if run:
        print(f"  {run[0]} x{run[1]} {sorted(run[2])}")


if __name__ == "__main__":
    main()
