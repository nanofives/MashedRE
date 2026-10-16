#!/usr/bin/env python3
"""PREDICTION, not measurement: rewrite d1r1's recorded standalone draw lists the way the
planned port would draw them, and diff the result against the original.

Edits applied to each standalone record (F1 build, d1r1 stand_rb_f1):
  W  world sectors in the 0x004e38a0 order with the cell-box frustum test (bsp_order.py),
     each sector's meshes kept in their recorded order;
  V  Arctic only: sector meshes blend iff texture alpha OR the sticky RW vertex-alpha flag;
     the flag is 1 for the first listed sector (left by the slot-3 sky) and 0 after it
     (the original draws an opaque tied atomic after its first sector on 3/3 frames);
  C  Arctic only: car draws (Advantage*, Glass, CarGloss) take D3D cull NONE (weather pass
     FUN_00499d90 0x0049a084 runs on RainEnable tracks).
Props are NOT re-interleaved here (the tie model needs atomic spheres the record lacks).

Usage: py -3.12 sim_fix.py [W] [V] [C]     (no args = all three)
"""
import copy
import json
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE)); sys.path.insert(0, str(HERE.parents[1] / "re" / "tools"))
import bsp_order  # noqa: E402
import drawlist3d  # noqa: E402
from run_bsp9 import TRACKS, MAIN, EV  # noqa: E402

CAR = ("advantage", "advantagesmall", "glass", "cargloss")


def sector_index(sectors, mats):
    index = {}
    for s in sectors:
        for m, (nm, n, bb) in bsp_order.sector_meshes(s, mats).items():
            index.setdefault(((nm or "").lower(), n), []).append((s["id"], bb))
    return index


def sid_of(index, e):
    for s, bb in index.get(((e.get("tex") or "").lower(), e.get("tri")), []):
        if e.get("bb") and max(abs(x - y) for x, y in zip(bb, e["bb"])) < 0.05:
            return s
    return None


def rewrite(doc, track, row, edits):
    piz, bsp, far, _rows = TRACKS[track]
    root, sectors, mats = bsp_order.load_world(str(MAIN / "original/TOASTART/TRACKS" / piz), bsp)
    index = sector_index(sectors, mats)
    ev = [e for e in doc["events"] if e.get("e") == "d"]
    tag = [sid_of(index, e) if drawlist3d.nprims(e) else None for e in ev]
    first = next(i for i, t in enumerate(tag) if t is not None)
    last = max(i for i, t in enumerate(tag) if t is not None)
    pre, block, post = ev[:first], ev[first:last + 1], ev[last + 1:]
    stray = [e for e, t in zip(block, tag[first:last + 1]) if t is None]
    if "W" in edits:
        import csv
        cam = bsp_order.Cam(list(csv.DictReader(open(MAIN / f"log/d1r1/orig_{track}/trace.csv")))[row], far)
        bsp_order.BOX_MODE = "cell"
        order = bsp_order.traverse(root, cam)
        by = {}
        for e, t in zip(block, tag[first:last + 1]):
            if t is not None:
                by.setdefault(t, []).append(e)
        block = [e for s in order for e in by.get(s, [])] + stray
    if "V" in edits and track == "arctic":
        sticky = 1
        cur = None
        for e in block:
            s = sid_of(index, e)
            if s is None:
                continue
            if cur is not None and s != cur:
                sticky = 0
            cur = s
            texa = e["rs"][6] == 1          # F1 build: blend == texture alpha for sectors
            ab = 1 if (texa or sticky) else 0
            e["rs"] = list(e["rs"])
            e["rs"][6] = ab
            e["rs"][3] = ab                 # RW switches alpha test with blend (FUN_004d6ce0)
            if ab:
                e["rs"][5], e["rs"][7], e["rs"][8] = 5, 5, 6
    if "C" in edits and track == "arctic":
        for e in post:
            if (e.get("tex") or "").lower() in CAR:
                e["rs"] = list(e["rs"]); e["rs"][9] = 1
    out = copy.deepcopy(doc)
    out["events"] = [{"e": "arm"}] + pre + block + post
    return out


def main(argv):
    edits = set(argv) or {"W", "V", "C"}
    tot = Counter(); tot_base = Counter()
    for t, (_piz, _bsp, _far, rows) in TRACKS.items():
        for r in rows:
            a = json.load(open(EV / f"{t}_orig_row_{r:05d}.draws.json"))
            sb = MAIN / f"log/d1r1/orig_{t}/stand_rb_f1/d1r2/stand_row_{r:05d}.draws.json"
            b0 = json.load(open(sb))
            b1 = rewrite(copy.deepcopy(b0), t, r, edits)
            fr = drawlist3d.Frustum(str(MAIN / f"log/d1r1/orig_{t}/trace.csv"), r)
            _, _, _, rows0, _ = drawlist3d.diff3d(a, b0, frustum=fr)
            _, _, _, rows1, _ = drawlist3d.diff3d(a, b1, frustum=fr)
            c0 = Counter(x["kind"] for x in rows0); c1 = Counter(x["kind"] for x in rows1)
            tot += c1; tot_base += c0
            k0 = {(x["kind"], x["tex"]) for x in rows0}
            new = sorted({(x["kind"], x["tex"]) for x in rows1} - k0)
            print(f"{t}:{r} F1 {sum(c0.values())} -> sim {sum(c1.values())} "
                  f"ORDER {c0['ORDER']}->{c1['ORDER']} STATE {c0['STATE']}->{c1['STATE']} "
                  f"EXTRA {c0['EXTRA']}->{c1['EXTRA']} new(kind,tex) {new[:8]}")
    print("TOTAL F1", dict(tot_base), "\nTOTAL sim", dict(tot))


if __name__ == "__main__":
    main(sys.argv[1:])
