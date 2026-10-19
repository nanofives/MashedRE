"""d3bc: recfirst.py with recpos.py's alignment (original record keyed by frame - f0, f0 = the car's first aistep row with
c4 != 0; port MSD1 list index = window call), for any car. Per call: number of differing dwords outside read-time fields
(+0x9d4 / +0x9dc), the torque-ring history and pad dwords, and the first few.
usage: recfirst2.py <orig.msd> <orig.aistep.csv> <port.rec.msd> <v> [k0] [k1]"""
import csv, os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
om, oa, pm, v = sys.argv[1:5]; k0 = int(sys.argv[5]) if len(sys.argv) > 5 else 0; k1 = int(sys.argv[6]) if len(sys.argv) > 6 else 10
rows = [r for r in csv.DictReader(open(oa, newline='')) if r['v'] == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); f0 = int(rows[i0]['frame'])
O = {fi - f0: r for fi, r in m.parse(om)}; P = [r for fi, r in m.parse(pm)]
skip = {0x944, 0x954, 0x964, 0x984, 0x9a4, 0xd00, 0x9d4, 0x9dc} | set(range(0x1ac, 0x1ec, 4)) | set(range(0x270, 0x2b0, 4))
for k in range(k0, k1):
    if k not in O or k >= len(P): break
    o, p = O[k], P[k]
    d = [x for x in range(0, len(o), 4) if o[x:x + 4] != p[x:x + 4] and x not in skip]
    print(k, len(d), ' '.join('+0x%x(o %.9g p %.9g)' % (x, f(o, x), f(p, x)) for x in d[:7]))
