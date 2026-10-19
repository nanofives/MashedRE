"""d3bc: per-car side-by-side of the AI step's integer columns, original aistep CSV vs port step CSV, rows from the first
c4 != 0 (the (b) window start). usage: cmpint.py <orig.aistep.csv> <port.step.csv> <v> [every] [rows]"""
import csv, sys
def load(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:]
v = sys.argv[3]; ev = int(sys.argv[4]) if len(sys.argv) > 4 else 5; n = int(sys.argv[5]) if len(sys.argv) > 5 else 240
O, P = load(sys.argv[1], v), load(sys.argv[2], v)
print('rows o=%d p=%d' % (len(O), len(P)))
C = ['c0', 'c1', 'c4', 'c5', 'ai_mode', 'substate', 'ai_override', 'flag_a368', 'tgt_7ffc', 'ai_spline_idx', 'march_idx0', 'march_n', 'ret14a70']
print('k   ' + ' '.join('%-13s' % c for c in C))
for k in range(0, min(n, max(len(O), len(P))), ev):
    o = O[k] if k < len(O) else {}; p = P[k] if k < len(P) else {}
    print('%-4d' % k + ' '.join('%-13s' % ('%s/%s' % (o.get(c, '-'), p.get(c, '-'))) for c in C))
