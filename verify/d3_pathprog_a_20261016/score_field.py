"""G-FIELD / G-PROG-RG (PREREG_pathprog_a.md amendments A1, A2). Read-only on its inputs.
usage: py -3.12 score_field.py <h4_arm> [<h4_arm> ...]       (run from this directory; needs <arm>.gates.csv and <arm>.step.csv)
Original reference: ../d3_modes37_20261009/o_pp1.msd.pathprog.csv (FUN_00408610's row per call: lap +0x28, node progress +0xc8, race_pct +0xcc)."""
import csv
import struct
import sys
from pathlib import Path

D = Path(__file__).resolve().parent
O = D.parent / "d3_modes37_20261009" / "o_pp1.msd.pathprog.csv"


def orig():
    per = {}
    for r in csv.DictReader(open(O)):
        b = bytes.fromhex(r["block_hex"])
        per.setdefault(int(r["v"]), []).append((struct.unpack("<i", b[0x28:0x2c])[0], struct.unpack("<f", b[0xc8:0xcc])[0],
                                                struct.unpack("<f", b[0xcc:0xd0])[0]))
    return per


def main():
    o = orig()
    print("original first published row (call 1): " + ", ".join("car %d prog %.4f pct %.3f lap %d" % (v, o[v][1][1], o[v][1][2], o[v][1][0]) for v in range(4)))
    for v in range(4):
        c = next((k for k in range(1, len(o[v])) if o[v][k][2] < 50 and o[v][k - 1][2] > 90), None)
        print("   car %d: lap counter -1 until call %s then %s (crossing: pct %.3f -> %.3f)" % (
            v, c, o[v][c][0] if c else "-", o[v][c - 1][2] if c else 0, o[v][c][2] if c else 0))
    for arm in sys.argv[1:]:
        g = {}
        for r in csv.DictReader(open(D / (arm + ".gates.csv"))):
            g.setdefault(int(r["frame"]), {})[int(r["v"])] = (float(r["prog_96e8"]), float(r["racepct_ec"]))
        print("== %s: live node progress (gates prog_96e8) at frame 1: %s" % (arm, ", ".join(
            "car %d %.4f" % (v, g[1][v][0]) for v in range(4))))
        print("   descending order at frame 1: %s -> two least = %s (original: [0, 1, 2, 3] -> [2, 3])" % (
            *(lambda L: ([v for _, v in L], [v for _, v in L[-2:]]))(sorted(((g[1][v][0], v) for v in range(4)), reverse=True)),))
        ok = True
        for v in (1, 2, 3):
            d = abs(g[1][v][0] - o[v][1][1]); ok &= d <= 0.05
            print("   car %d frame-1 progress %.4f vs original %.4f (|d| %.4f)" % (v, g[1][v][0], o[v][1][1], d))
        # lap counter timeline from the step dump (AI cars)
        lap = {}
        for r in csv.DictReader(open(D / (arm + ".step.csv"))):
            lap.setdefault(int(r["v"]), []).append((int(r["frame"]), int(r["lap_9648"])))
        for v in (1, 2, 3):
            L = lap.get(v, [])
            ch = next((f for f, l in L if l != -1), None)
            prev = next((f for f in range(2, 60) if f in g and f - 1 in g and g[f][v][0] < 1.0 and g[f - 1][v][0] > 29), None)
            print("   car %d: lap counter first != -1 at step frame %s; progress wraps at gates frame %s; first lap value %s" % (
                v, ch, prev, next((l for f, l in L if l != -1), None)))
            ok &= (ch is not None and prev is not None and ch in (prev, prev + 1))
        print("   G-FIELD cars 1-3: %s" % ("PASS" if ok else "FAIL"))


main()
