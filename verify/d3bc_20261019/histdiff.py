"""d3bc: per car, the first window rows where the steer-error history (hist_dc / hist_d8 = 0x008032dc / 0x008032d8)
differs at float precision, original aistep CSV vs port step CSV, and the first command (c0/c1/c4/c5/ai_mode) row.
usage: histdiff.py <orig.aistep.csv> <port.step.csv> [...]"""
import csv, struct, sys
f = lambda x: struct.unpack('<f', struct.pack('<f', float(x)))[0]
def load(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:]
for pp in sys.argv[2:]:
    out = []
    for v in '123':
        O, P = load(sys.argv[1], v), load(pp, v); n = min(len(O), len(P), 196)
        h = [k for k in range(n) if f(O[k]['hist_dc']) != f(P[k]['hist_dc']) or f(O[k]['hist_d8']) != f(P[k]['hist_d8'])]
        c = next((k for k in range(n) if any(O[k][x] != P[k][x] for x in ('c0', 'c1', 'c4', 'c5', 'ai_mode'))), None)
        pz = next((k for k in range(n) if f(O[k]['own_x']) != f(P[k]['own_x']) or f(O[k]['own_z']) != f(P[k]['own_z'])), None)
        out.append('car%s hist %s (n=%d) pos %s cmd %s' % (v, h[:1], len(h), pz, c))
    print('%-24s' % pp.split('/')[-1].replace('.step.csv', ''), ' | '.join(out))
