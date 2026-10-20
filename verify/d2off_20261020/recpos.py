"""u9762: car-1 record position (+0x958 / +0x960) and velocity (+0x9b0 / +0x9b8) gap per window call, original MSD vs port
MSD1 dump, full float precision (posdrift.py uses the AI's own_x/own_z, the previous frame's position, from a 9-digit CSV).
usage: recpos.py <orig.msd> <orig.aistep.csv> <port.rec.msd> <car> [k1]"""
import csv, struct, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
o_msd, o_ais, p_msd, v = sys.argv[1:5]; k1 = int(sys.argv[5]) if len(sys.argv) > 5 else 30
rows = [r for r in csv.DictReader(open(o_ais, newline='')) if r['v'] == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); f0 = int(rows[i0]['frame'])
orec = {fi - f0: rec for fi, rec in m.parse(o_msd)}
prec = [rec for fi, rec in m.parse(p_msd)]
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
print('  k        dx          dz         dvx         dvz')
for k in range(0, k1):
    if k not in orec or k >= len(prec): break
    a, b = orec[k], prec[k]
    print('%3d %+.3e %+.3e %+.3e %+.3e' % (k, f(b, 0x958) - f(a, 0x958), f(b, 0x960) - f(a, 0x960),
                                          f(b, 0x9b0) - f(a, 0x9b0), f(b, 0x9b8) - f(a, 0x9b8)))
