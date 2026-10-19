"""d3bc: emucrt.py's CRT form on the PORT's inputs (step CSV own / look, port record call k-1 +0x948/+0x950), scored against the
port's stored error. usage: emuportcrt.py <port.rec.msd> <port.step.csv> <v> [rows]"""
import csv, os, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import emucrt as E
pm, pa, v = sys.argv[1:4]; n = int(sys.argv[4]) if len(sys.argv) > 4 else 200
rows = [r for r in csv.DictReader(open(pa)) if r['v'] == v]; i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); P = rows[i0:]
rec = [r for fi, r in E.m.parse(pm)]
ok = 0; bad = []
for k in range(2, min(n, len(P), len(rec) + 1)):
    o = P[k]
    if '' in (o['hist_dc'], o['hist_d8']): continue
    h = E.F(float(o['hist_dc'])) if float(o['hist_d8']) == 360 else E.F(float(o['hist_d8']))
    e = E.err(*[float(o[c]) for c in ('own_x', 'own_z', 'look_x', 'look_z')], E.g(rec[k - 1], 0x948), E.g(rec[k - 1], 0x950), 'crt24')
    if e == h: ok += 1
    else: bad.append(k)
print('crt24 on port inputs: exact %d / %d, misses %s' % (ok, ok + len(bad), bad[:10]))
