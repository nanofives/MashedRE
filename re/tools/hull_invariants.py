#!/usr/bin/env python3
"""Car<->car hull invariants: what 0x00469df0 reads at record +0xa28..+0xa54, scored
against the AABB top face 0x0046b1c0 builds from its 6-float box.

WHY THIS EXISTS. `VehicleCarCarContact` (0x00469df0) runs a 4-halfplane SAT over four
world-space points at record +0xa28/+0xa34/+0xa40/+0xa4c (dword 0x28a/0x28d/0x290/0x293,
read at 0x00469e58, 0x00469e73, 0x00469e86, 0x00469e91, 0x00469ea5, 0x00469eb9,
0x00469f0a, 0x00469f1e, 0x00469eca, 0x00469ee5, 0x00469ef2 and again from 0x0046a0ab).
Nothing in the image writes that displacement -- `findconst.py 0xa28` returns exactly
TWO raw matches in .text and both are reads inside 0x00469df0 itself, and Ghidra reports
0 references to car 0's absolute 0x00881fc8. The producer reaches it through a computed
base, which is the blind spot memory `findoffset-blind-to-computed-bases` names:

    FUN_00469aa0  (the contact scan)
      Rw_VtableDispatch(self + 0x27e, self + 0x18, 0x12, self + self[0x26b]*0x10 + 0x24a)
      i.e. transform 18 body points at +0x60 (stride 0xc) into +0x9f8 (stride 0xc).
    +0x9f8 + 4*0xc == +0xa28, so the SAT hull is WORLD POINTS 4..7 of that array.

    FUN_0046b1c0  (VehicleSlotAabbExpand / VehicleBuildContactHull, C3, ported)
      body points 4..7 at +0x90/+0x9c/+0xa8/+0xb4 are
        (b0,b4,b2) (b3,b4,b2) (b0,b4,b5) (b3,b4,b5)
      from the 6-float box -- the TOP FACE of the AABB, which is why all four share a y.

So the four world points are a RIGID transform of that face, and their three edge
lengths are invariants of the box alone. That is what this tool scores: it is a check on
the CHAIN, not on any single function, and it needs no instrumentation on either side --
the full 0xd04 record is already in every committed MSD1 capture.

Usage:
  py -3.12 re/tools/hull_invariants.py <msd> [<msd> ...] [--box b0,b1,b2,b3,b4,b5]
                                       [--tol 1e-3] [--min-frac 0.95] [--json]

Default --box is `kContactHullBox` from mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp,
the six values measured at 0x0063dc10..0x0063dc24 on the running anchored original.

DENOMINATOR, stated and printed: frames whose four hull points are not all-zero. An
all-zero hull is a pre-race frame, not a failure, and scoring it would mix two
populations -- memory `read-the-rva-plate-before-building-a-probe` and the ARM
retraction. Both counts are printed, always.
"""
import argparse
import json
import math
import statistics
import struct
import sys

# Record byte offsets. Grepped against re/analysis/vehicle_dynamics_d2/00469df0.md and
# re/analysis/vehicle_promote_c2_b/00469df0.md before use: both plates name
# +0xA28..+0xA54 "vehicle bounding-hull vertices (4 x vec3)" and neither names a
# producer. +0x9f8 is the base of the 18-point world array (dword 0x27e).
OFF_HULL = (0xa28, 0xa34, 0xa40, 0xa4c)
OFF_WORLD_BASE = 0x9f8

# mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp:148-155
DEFAULT_BOX = (
    +0.21879999339580536,
    +0.30860000848770140,
    +0.45379999279975890,
    -0.21879999339580536,
    +0.03739999979734421,
    -0.52329999208450320,
)


def load(path):
    raw = open(path, "rb").read()
    if raw[:4] != b"MSD1":
        raise SystemExit("%s: not MSD1" % path)
    rec, base, _ = struct.unpack_from("<III", raw, 4)
    frames, payloads, off = [], [], 16
    while off + 4 + rec <= len(raw):
        frames.append(struct.unpack_from("<I", raw, off)[0])
        payloads.append(raw[off + 4: off + 4 + rec])
        off += 4 + rec
    if rec < OFF_HULL[-1] + 12:
        raise SystemExit("%s: record 0x%x too short for +0x%x" % (path, rec, OFF_HULL[-1]))
    return frames, payloads, rec, base


def dist(a, b):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(a, b)))


def expected(box):
    """The four body-space points 0x0046b1c0 writes at +0x90/+0x9c/+0xa8/+0xb4."""
    b = box
    return [(b[0], b[4], b[2]), (b[3], b[4], b[2]),
            (b[0], b[4], b[5]), (b[3], b[4], b[5])]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("msd", nargs="+")
    ap.add_argument("--box", default=None,
                    help="six comma-separated floats; default = kContactHullBox")
    ap.add_argument("--tol", type=float, default=1e-3,
                    help="absolute tolerance on each edge length (default 1e-3)")
    ap.add_argument("--min-frac", type=float, default=0.95,
                    help="fraction of non-degenerate frames that must be within --tol")
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()

    box = DEFAULT_BOX
    if a.box:
        parts = [float(x) for x in a.box.split(",")]
        if len(parts) != 6:
            raise SystemExit("--box needs exactly 6 floats")
        box = tuple(parts)

    exp = expected(box)
    ref = (dist(exp[0], exp[1]), dist(exp[0], exp[2]), dist(exp[0], exp[3]))

    out = {"box": list(box),
           "ref_edges": {"e01": ref[0], "e02": ref[1], "e03": ref[2]},
           "tol": a.tol, "min_frac": a.min_frac, "files": []}
    ok_all = True
    for path in a.msd:
        frames, payloads, rec, base = load(path)
        f32 = lambda pl, o: struct.unpack_from("<f", pl, o)[0]
        meas = {"e01": [], "e02": [], "e03": []}
        nz = 0
        within = 0
        for pl in payloads:
            pts = [(f32(pl, o), f32(pl, o + 4), f32(pl, o + 8)) for o in OFF_HULL]
            if all(abs(c) == 0.0 for p in pts for c in p):
                continue                      # pre-race / unspawned frame
            nz += 1
            e = (dist(pts[0], pts[1]), dist(pts[0], pts[2]), dist(pts[0], pts[3]))
            meas["e01"].append(e[0]); meas["e02"].append(e[1]); meas["e03"].append(e[2])
            if all(abs(e[i] - ref[i]) <= a.tol for i in range(3)):
                within += 1
        frac = (within / nz) if nz else 0.0
        verdict = "PASS" if (nz > 0 and frac >= a.min_frac) else "FAIL"
        if verdict != "PASS":
            ok_all = False
        rowj = {"path": path, "record_size": rec, "base": "0x%x" % base,
                "frames_total": len(payloads), "frames_nondegenerate": nz,
                "within_tol": within, "frac": frac, "verdict": verdict}
        for k in ("e01", "e02", "e03"):
            if meas[k]:
                rowj[k] = {"median": statistics.median(meas[k]),
                           "min": min(meas[k]), "max": max(meas[k])}
        out["files"].append(rowj)
        if not a.json:
            print("%s  base=%s record=0x%x" % (path, rowj["base"], rec))
            print("  non-degenerate frames %d of %d (an all-zero hull is a pre-race "
                  "frame, not a failure)" % (nz, len(payloads)))
            for k, r in (("e01", ref[0]), ("e02", ref[1]), ("e03", ref[2])):
                if k in rowj:
                    m = rowj[k]
                    print("  %s median %.6f  [%.6f, %.6f]   ref %.6f   dmed %.2e"
                          % (k, m["median"], m["min"], m["max"], r,
                             abs(m["median"] - r)))
            print("  within tol %.0e on %d of %d = %.6f  vs min-frac %.6f  -> %s"
                  % (a.tol, within, nz, frac, a.min_frac, verdict))
    if a.json:
        out["verdict"] = "PASS" if ok_all else "FAIL"
        print(json.dumps(out, indent=2))
    return 0 if ok_all else 1


if __name__ == "__main__":
    sys.exit(main())
