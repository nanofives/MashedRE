#!/usr/bin/env python3
"""bsp_order.py over the 9 d1r1 frames.
Usage: py -3.12 run_bsp9.py [cell|stream] [skip-plane-ids...]   (default: cell)"""
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE)); sys.path.insert(0, str(HERE.parents[1] / "re" / "tools"))
import bsp_order  # noqa: E402

MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EV = HERE.parent / "d1r1_20261016" / "evidence"
# track: (piz, World_Bsp_Filename, Setup_Fog far = RwCamera +0x84, rows)
TRACKS = {
    "training": ("training.piz", "graph.bsp", 360.0, (884, 924, 959)),
    "arctic": ("Arctic.piz", "graph.bsp", 70.0, (1051, 1091, 1126)),
    "city": ("City.piz", "Graphics.bsp", 250.0, (761, 801, 836)),
}


def main(argv):
    bsp_order.BOX_MODE = "cell"
    if argv and argv[0] in ("cell", "stream"):
        bsp_order.BOX_MODE = argv.pop(0)
    skip = tuple(int(x) for x in argv)
    n_ok = 0
    for t, (piz, bsp, far, rows) in TRACKS.items():
        for r in rows:
            o, m = bsp_order.compare(str(MAIN / "original/TOASTART/TRACKS" / piz), bsp,
                                     str(MAIN / f"log/d1r1/orig_{t}/trace.csv"), r, far,
                                     str(EV / f"{t}_orig_row_{r:05d}.draws.json"), skip=skip)
            n_ok += bsp_order.report(o, m, f"{t}:{r}")
    print(f"EXACT {n_ok}/9 (box {bsp_order.BOX_MODE}, skip planes {skip})")


if __name__ == "__main__":
    main(sys.argv[1:])
