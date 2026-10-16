"""U-9685 scorer for G-TOOK-R and G-PRELINE (PREREG_u9685.md section 3). Read-only on its inputs.
usage: py -3.12 score_racepct.py [R_ARM [PP_ARM]]        (defaults R_H PP_H; run from this directory)
Inputs: <arm>.gates.csv (racepct_ec = the live array 0x008a96ec + v * 0x30c at the AI tick) and the original's
../d3_u9219_20261013/o_rd3.msd.rd.tsv (producer-input race_pct per driving call)."""
import csv
import struct
import sys
from pathlib import Path

D = Path(__file__).resolve().parent
O = D.parent / "d3_u9219_20261013"
hexf = lambda h: struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]


def gates(arm):
    by = {}
    for r in csv.DictReader(open(D / (arm + ".gates.csv"))):
        by.setdefault(int(r["frame"]), {})[int(r["v"])] = float(r["racepct_ec"])
    return by


def orig_pct():
    rows = [l.rstrip("\n").split("\t") for l in open(O / "o_rd3.msd.rd.tsv") if l.strip()]
    ix = {k: i for i, k in enumerate(rows[0])}
    out, on = [], False
    for r in rows[1:]:
        ba8c = int(r[ix["ba8c"]]); cars = [c.split(":") for c in r[ix["cars"]].split(";")]
        st = [int(c[2], 16) for c in cars]
        if not on:
            if ba8c == 6 and not any(st): on = True
            else: continue
        if ba8c != 6 or any(st): break
        out.append([hexf(c[4]) for c in cars])
    return out


def main():
    ra = sys.argv[1] if len(sys.argv) > 1 else "R_H"
    pp = sys.argv[2] if len(sys.argv) > 2 else "PP_H"
    R, P = gates(ra), gates(pp)
    print("== G-TOOK-R (%s frame f == %s frame f-1, cars 1-3, frames 2..14)" % (ra, pp))
    n = eq = 0
    worst = 0.0
    for f in range(2, 15):
        for v in (1, 2, 3):
            if f in R and f - 1 in P:
                n += 1
                d = abs(R[f][v] - P[f - 1][v]); worst = max(worst, d)
                eq += d == 0.0
    print("  bit-equal %d / %d (%.1f %%), max abs diff %.3g -> %s   <- as registered (relation derived wrongly, see PREREG A1)" % (eq, n, 100.0 * eq / max(n, 1), worst, "PASS" if eq >= 0.9 * n else "FAIL"))
    print("== G-TOOK-R corrected (PREREG A1): %s frame f vs %s frame f (both read the positions after physics f-1)" % (ra, pp))
    n2 = eq2 = 0; worst2 = 0.0
    for f in range(1, 15):
        for v in (1, 2, 3):
            if f in R and f in P:
                n2 += 1; d = abs(R[f][v] - P[f][v]); worst2 = max(worst2, d); eq2 += d == 0.0
    f1 = all(R[1][v] == P[1][v] for v in (1, 2, 3))
    print("  frame 1 first published values bit-equal on cars 1-3: %s; frames 1..14 bit-equal %d / %d, max abs diff %.3g (the AI differs from PP once PP's live lap counter / progress reach it)" % (f1, eq2, n2, worst2))
    print("  -> %s" % ("PASS" if f1 and worst2 <= 0.25 else "FAIL"))

    print("== G-PRELINE (%s array cars 1-3, frames 1..19 in [97,100]; vs the original at the crossing-aligned call, calls 5..18)" % ra)
    inr = all(97.0 <= R[f][v] <= 100.0 for f in range(1, 20) if f in R for v in (1, 2, 3))
    print("  frames 1..19 all in [97, 100]: %s" % inr)
    o = orig_pct()
    # Both arms' step reads the positions after physics f-1 at the AI tick of frame f (PP in UpdateRace f, R at the end of f-1), so
    # the array the gates dump shows at frame f is the value for the original's producer call f-1 (the PP arm crosses the line at
    # gates frame 20, the original at call 19): orig call c <-> gates frame c+1.
    mx = 0.0; rows = []
    for c in range(5, 19):
        f = c + 1
        if f not in R or c >= len(o): continue
        ds = [abs(R[f][v] - o[c][v]) for v in (1, 2, 3)]
        mx = max(mx, max(ds)); rows.append((c, f, [round(R[f][v], 3) for v in (1, 2, 3)], [round(o[c][v], 3) for v in (1, 2, 3)]))
    for c, f, e, oo in rows:
        print("  orig call %2d / gates frame %2d: exe %s orig %s" % (c, f, e, oo))
    print("  max |exe - orig| = %.3f -> %s" % (mx, "PASS" if (inr and mx <= 0.25) else "FAIL"))


main()
