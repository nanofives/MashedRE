"""brecipe: stepcmp.py without own_x/own_z on the same k (the port dump's own pos is the previous frame's, memory
posdrift-reads-ai-own-pos-not-physics). Prints per car the first k where c0,c1,c3,c4,c5,substate,rec_9e4,clk differ, and the
first k where the port's own_x/own_z at k+1 differ from the original's at k. usage: stepcmp2.py <orig.aistep.csv> <port.step.csv>"""
import csv, sys, struct
F32 = lambda x: struct.pack('<f', float(x))
COLS = ['c0', 'c1', 'c3', 'c4', 'c5', 'substate', 'rec_9e4']
def load(p, orig):
    rows = list(csv.DictReader(open(p))); out = {}
    if orig:
        f0 = int(next(r for r in rows if r['substate'] == '6')['frame']); c0 = int(next(r for r in rows if r['substate'] == '6')['clk_0ff4'])
        for r in rows:
            k = int(r['frame']) - f0
            if k >= 0: r['_clk'] = int(r['clk_0ff4']) - c0; out[(int(r['v']), k)] = r
    else:
        c0 = int(rows[0]['clk_0ff4'])
        for r in rows: r['_clk'] = int(r['clk_0ff4']) - c0; out[(int(r['v']), int(r['frame']))] = r
    return out
O, P = load(sys.argv[1], True), load(sys.argv[2], False)
for v in (1, 2, 3):
    ko = max(k for (vv, k) in O if vv == v)
    first = fpos = None
    for k in range(0, ko + 1):
        a, b = O.get((v, k)), P.get((v, k))
        if b is None: first = first or (k, 'no port row'); break
        if first is None:
            d = [c for c in COLS if ((F32(a[c]) != F32(b[c])) if c == 'rec_9e4' else (a[c] != b[c]))] + (['clk'] if a['_clk'] != b['_clk'] else [])
            if d: first = (k, d)
        b1 = P.get((v, k + 1))
        if fpos is None and b1 is not None and (F32(a['own_x']) != F32(b1['own_x']) or F32(a['own_z']) != F32(b1['own_z'])): fpos = k
    print(f'car {v}: orig last k {ko}; first ctrl/state difference {first}; first own pos difference (port k+1) {fpos}')
