import sys, struct, csv
sys.path.insert(0, r'C:/Users/maria/Desktop/Proyectos/Mashed/re/tools/statediff'); import msd_fields as m
o = sys.argv[1]
rows = [r for r in csv.DictReader(open(o + '.aistep.csv', newline='')) if r['v'] == '1']
f0 = int(next(r for r in rows if int(r['c4']) != 0)['frame'])
prev = None; out = []
for fi, r in m.parse(o):
    y = struct.unpack_from('<f', r, 0x95c)[0]
    vy = struct.unpack_from('<f', r, 0x9b4)[0]
    if y != prev:
        out.append((fi, fi - f0, y, vy)); prev = y
print('window f0', f0, 'y changes', len(out))
for t in out[:14] + [('..',)] + out[-6:]:
    print(t if len(t) == 1 else 'frame %d k=%d y=%.9g vy=%.9g' % t)
