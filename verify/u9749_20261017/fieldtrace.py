"""u9749: per-k trace of chosen record fields, original vs port, aligned as recdiff.py does (car 2 window + coast match).
usage: py -3.12 fieldtrace.py <orig.msd> <orig.aistep.csv> <port.rec.msd> <k0> <k1> <off>[:i] [...]"""
import csv, struct, sys
sys.path.insert(0, r'C:\Users\maria\Desktop\Proyectos\Mashed\re\tools\statediff')
import msd_fields as m

o_msd, o_ais, p_msd, k0, k1 = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), int(sys.argv[5])
specs = []
for s in sys.argv[6:]:
    o, _, t = s.partition(':')
    specs.append((int(o, 16), t or 'f'))
rows = [r for r in csv.DictReader(open(o_ais, newline='')) if r['v'] == '2']
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
f0 = int(rows[i0]['frame'])
orig = {fi - f0: rec for fi, rec in m.parse(o_msd)}
port = [rec for fi, rec in m.parse(p_msd)]
spd = lambda r: struct.unpack_from('<f', r, 0x9e4)[0]
ko = next(k for k in sorted(orig) if k >= 0 and spd(orig[k]) > 1000 and all(k + j in orig and spd(orig[k + j]) == spd(orig[k]) for j in range(9)))
kp = next(i for i in range(len(port) - 9) if spd(port[i]) > 1000 and all(spd(port[i + j]) == spd(port[i]) for j in range(9)))
sh = kp - ko
val = lambda r, o, t: struct.unpack_from('<i' if t == 'i' else '<f', r, o)[0]
print('  k | ' + ' | '.join(f'+0x{o:03x} orig/port' for o, t in specs))
for k in range(k0, k1 + 1):
    o = orig.get(k); p = port[k + sh] if 0 <= k + sh < len(port) else None
    if o is None or p is None: continue
    cells = []
    for off, t in specs:
        a, b = val(o, off, t), val(p, off, t)
        cells.append(f'{a}/{b}' if t == 'i' else f'{a:.5f}/{b:.5f}')
    print(f'{k:3d} | ' + ' | '.join(cells))
