#!/usr/bin/env python3
"""Pr-1 check: the world-sector id sequence of a standalone record against the original's.

    py -3.12 seqcheck.py <arm> [tracks]      (records under log/rstates/<arm>/<track>/d1r2/)
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE)); sys.path.insert(0, str(HERE.parents[1] / "re" / "tools"))
import bsp_order  # noqa: E402
from run_bsp9 import TRACKS, MAIN, EV  # noqa: E402

LOG = HERE.parents[1] / "log" / "rstates"


def main(argv):
    arm = argv[0]
    tracks = argv[1].split(",") if len(argv) > 1 else list(TRACKS)
    ok = n = 0
    for t in tracks:
        piz, bsp, _far, rows = TRACKS[t]
        _root, sectors, mats = bsp_order.load_world(str(MAIN / "original/TOASTART/TRACKS" / piz), bsp)
        for r in rows:
            so = bsp_order.orig_sequence(sectors, mats, EV / f"{t}_orig_row_{r:05d}.draws.json")
            sb = LOG / arm / t / "d1r2" / f"stand_row_{r:05d}.draws.json"
            ss = bsp_order.orig_sequence(sectors, mats, sb) if sb.exists() else None
            n += 1
            same = so == ss
            ok += same
            print(f"{t}:{r} {'SAME' if same else 'DIFF'} orig={so} stand={ss}")
    print(f"world-sector sequence identical on {ok}/{n} frames")


if __name__ == "__main__":
    main(sys.argv[1:])
