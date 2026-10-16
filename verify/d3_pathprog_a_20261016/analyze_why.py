"""G-WHY (PREREG_pathprog_a.md): for a test arm vs a base arm (step CSVs, same recipe), per car, the first row where any control byte
(c0 c1 c3 c4 c5) differs, and every column ranked by the first row where it differs (the earliest-diverging inputs come first).
usage: py -3.12 analyze_why.py <base_arm> <test_arm> [top]        (run from this directory)"""
import csv
import sys
from pathlib import Path

D = Path(__file__).resolve().parent
CTRL = ("c0", "c1", "c3", "c4", "c5")


def rows(arm):
    by = {}
    for r in csv.DictReader(open(D / (arm + ".step.csv"))):
        by.setdefault(int(r["v"]), []).append(r)
    return by


def main():
    a, b = sys.argv[1], sys.argv[2]
    top = int(sys.argv[3]) if len(sys.argv) > 3 else 8
    A, B = rows(a), rows(b)
    for v in sorted(set(A) & set(B)):
        ra, rb = A[v], B[v]
        n = min(len(ra), len(rb))
        first = {}
        ctrl = None
        for i in range(n):
            for c in ra[i]:
                if c in ("frame", "seq", "v"): continue
                if ra[i][c] != rb[i][c] and c not in first:
                    first[c] = i
            if ctrl is None and any(ra[i][c] != rb[i][c] for c in CTRL):
                ctrl = i
        print("car %d: rows %d / %d, first control-byte difference at row %s (frame %s)" % (
            v, len(ra), len(rb), ctrl, ra[ctrl]["frame"] if ctrl is not None else "-"))
        order = sorted(first.items(), key=lambda t: t[1])
        print("   earliest-diverging columns: %s" % ", ".join("%s@%d" % (c, i) for c, i in order[:top]))
        if ctrl is not None:
            pre = [c for c, i in order if i < ctrl]
            print("   columns that already differed BEFORE the first control-byte difference: %s" % (", ".join(pre) if pre else "none"))


main()
