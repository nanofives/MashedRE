"""d3bc: per car, the first window row where the AI step's command-shape columns (c4, c5, ai_mode, ai_override, substate)
differ, original aistep CSV vs port step CSV, with the rows around it. usage: firstmode.py <orig.aistep.csv> <port.step.csv> [ctx]"""
import csv, sys
def load(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:]
ctx = int(sys.argv[3]) if len(sys.argv) > 3 else 3
C = ['c4', 'c5', 'ai_mode', 'ai_override', 'substate']
S = ['frame', 'c0', 'c1', 'c4', 'c5', 'ai_mode', 'ai_override', 'substate', 'ret14a70', 'ret14c30', 'ret150e0', 'ret16060', 'ret148b0', 'curv', 'look_x', 'look_z', 'own_x', 'own_z']
for v in ('1', '2', '3'):
    O, P = load(sys.argv[1], v), load(sys.argv[2], v)
    k = next((k for k in range(min(len(O), len(P))) if any(O[k][c] != P[k][c] for c in C)), None)
    print('== car %s first command-shape diff at row %s' % (v, k))
    if k is None: continue
    for j in range(max(0, k - ctx), min(k + ctx + 1, len(O), len(P))):
        print('  o', j, ' '.join('%s=%s' % (c, O[j].get(c, '')) for c in S))
        print('  p', j, ' '.join('%s=%s' % (c, P[j].get(c, '')) for c in S))
