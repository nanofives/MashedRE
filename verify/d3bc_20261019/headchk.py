"""d3bc: offline FUN_00415e20 (0x00415e20) with exact sqrt, original inputs: own / look from the aistep row, forward =
the record's +0x948 / +0x950 (`at` row of the +0x928 matrix) at a record call offset from the row, plus the port step
CSV's rec_9d4 / rec_9dc. Prints |err - hist| for each forward source. usage: headchk.py <orig.msd> <orig.aistep.csv>
<port.step.csv> <v> [rows]"""
import csv, math, os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
F = lambda x: struct.unpack('<f', struct.pack('<f', float(x)))[0]
def load(p, v):
    rows = [r for r in csv.DictReader(open(p, newline='')) if r['v'] == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:]
om, oa, pa, v = sys.argv[1:5]; n = int(sys.argv[5]) if len(sys.argv) > 5 else 40
rows = [r for r in csv.DictReader(open(oa, newline='')) if r['v'] == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); f0 = int(rows[i0]['frame']); O = rows[i0:]
rec = {fi - f0: r for fi, r in m.parse(om)}
P = load(pa, v)
def ang(x, z):
    n_ = math.sqrt(x * x + z * z); d = x / n_ if n_ else x
    d = max(-1.0, min(1.0, d)); a = math.acos(d) * 57.295780181884765625
    if z < 0: a = -a
    while a <= 0: a += 360
    return a
def err(ox, oz, lx, lz, fx, fz):
    e = ang(lx - ox, -(lz - oz)) - ang(fx, -fz)
    while e <= 0: e += 360
    while e > 360: e -= 360
    return e
print(' row  hist        d(k-2)     d(k-1)     d(k)       d(port 9d4)')
for k in range(1, n):
    o = O[k]; h = F(o['hist_dc']) if F(o['hist_d8']) == 360 else F(o['hist_d8'])
    ox, oz, lx, lz = (float(o[c]) for c in ('own_x', 'own_z', 'look_x', 'look_z'))
    out = []
    for dk in (-2, -1, 0):
        r = rec.get(k + dk)
        out.append('%+.2e' % (err(ox, oz, lx, lz, f(r, 0x948), f(r, 0x950)) - h) if r else '   -    ')
    p = P[k]; out.append('%+.2e' % (err(ox, oz, lx, lz, float(p['rec_9d4']), float(p['rec_9dc'])) - h))
    print('%4d %.6f  %s' % (k, h, '  '.join(out)))
