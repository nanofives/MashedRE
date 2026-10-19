"""d3bc: like headchk.py but scores the PORT's hist (port step CSV) against forwards from the port's own record dump
(MSD1, list index = window call). Prints err(fwd at call k+dk) - port hist. usage: headchk2.py <port.rec.msd>
<port.step.csv> <v> [rows]"""
import csv, math, os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
F = lambda x: struct.unpack('<f', struct.pack('<f', float(x)))[0]
pm, pa, v = sys.argv[1:4]; n = int(sys.argv[4]) if len(sys.argv) > 4 else 40
rows = [r for r in csv.DictReader(open(pa, newline='')) if r['v'] == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); P = rows[i0:]
rec = [r for fi, r in m.parse(pm)]
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
print(' row  hist        d(k-2)     d(k-1)     d(k)')
for k in range(1, n):
    o = P[k]; h = F(o['hist_dc']) if F(o['hist_d8']) == 360 else F(o['hist_d8'])
    ox, oz, lx, lz = (float(o[c]) for c in ('own_x', 'own_z', 'look_x', 'look_z'))
    out = []
    for dk in (-2, -1, 0):
        j = k + dk
        out.append('%+.2e' % (err(ox, oz, lx, lz, f(rec[j], 0x948), f(rec[j], 0x950)) - h) if 0 <= j < len(rec) else '   -    ')
    print('%4d %.6f  %s' % (k, h, '  '.join(out)))
