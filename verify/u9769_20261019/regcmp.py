"""d3bc: first window row where two ORIGINAL aistep captures differ, per car (all columns except frame / seq / clocks).
usage: regcmp.py <a.aistep.csv> <b.aistep.csv> [skipcol ...]"""
import csv, sys
def load(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:]
skip = {'frame', 'seq', 'clk_0ff4', 'step_1008', 'c4_in', 'c5_in'} | set(sys.argv[3:])
for v in '123':
    A, B = load(sys.argv[1], v), load(sys.argv[2], v)
    k = next((k for k in range(min(len(A), len(B))) if any(A[k][c] != B[k][c] for c in A[k] if c not in skip)), None)
    cols = [c for c in A[k] if c not in skip and A[k][c] != B[k][c]] if k is not None else []
    print('car %s rows %d/%d first diff row %s cols %s' % (v, len(A), len(B), k, cols[:8]))
