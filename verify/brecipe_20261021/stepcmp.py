"""u9779: port step dump vs original aistep, aligned on k (original: rows from the first substate-6 row; port: frame).
Prints per car the first k where a compared column differs, and rows k0..k1 for one car. usage: stepcmp.py <orig.aistep.csv> <port.step.csv> [car k0 k1]"""
import csv, sys, struct
F32 = lambda x: struct.pack('<f', float(x))   # compare as binary32: the two sides print with different digit counts
FLT = {'own_x', 'own_z', 'rec_9e4'}
COLS = ['c0', 'c1', 'c3', 'c4', 'c5', 'substate', 'own_x', 'own_z', 'rec_9e4']
def load(p, orig):
    rows = list(csv.DictReader(open(p)))
    out = {}
    if orig:
        f0 = int(next(r for r in rows if r['substate'] == '6')['frame'])
        c0 = int(next(r for r in rows if r['substate'] == '6')['clk_0ff4'])
        for r in rows:
            k = int(r['frame']) - f0
            if k >= 0: r['_clk'] = int(r['clk_0ff4']) - c0; out[(int(r['v']), k)] = r
    else:
        c0 = int(rows[0]['clk_0ff4'])
        for r in rows: r['_clk'] = int(r['clk_0ff4']) - c0; out[(int(r['v']), int(r['frame']))] = r
    return out
O, P = load(sys.argv[1], True), load(sys.argv[2], False)
for v in (1, 2, 3):
    ko = max(k for (vv, k) in O if vv == v); kp = max(k for (vv, k) in P if vv == v)
    first = None
    for k in range(0, max(ko, kp) + 1):
        a, b = O.get((v, k)), P.get((v, k))
        if (a is None) != (b is None): first = (k, 'row exists orig=%s port=%s' % (a is not None, b is not None)); break
        if a is None: continue
        d = [c for c in COLS if ((F32(a[c]) != F32(b[c])) if c in FLT else (a[c] != b[c]))] + (['clk'] if a['_clk'] != b['_clk'] else [])
        if d: first = (k, d); break
    print(f'car {v}: orig last k {ko}, port last k {kp}; first difference {first}')
if len(sys.argv) > 3:
    v, k0, k1 = int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
    for k in range(k0, k1 + 1):
        a, b = O.get((v, k)), P.get((v, k))
        f = lambda r: '-' if r is None else ' '.join(f"{c}={r[c]}" for c in ('substate', 'c4', 'c5', 'c0')) + f" clk={r['_clk']} x={r['own_x'][:10]} z={r['own_z'][:10]}"
        print(k, 'O', f(a), '| P', f(b))
