"""d3bc: every column shared by the original aistep CSV and the port step CSV, for car v at window rows k0..k1 (o / p).
usage: rowcmp.py <orig.aistep.csv> <port.step.csv> <v> <k0> <k1>"""
import csv, sys
def load(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:]
v, k0, k1 = sys.argv[3], int(sys.argv[4]), int(sys.argv[5])
O, P = load(sys.argv[1], v), load(sys.argv[2], v)
cols = [c for c in O[0] if c in P[0] and c not in ('frame', 'seq')]
for k in range(k0, k1 + 1):
    print('row', k, ' '.join('%s=%s/%s%s' % (c, O[k][c], P[k][c], '' if O[k][c] == P[k][c] else '*') for c in cols))
print('port-only cols:', [c for c in P[0] if c not in O[0]])
