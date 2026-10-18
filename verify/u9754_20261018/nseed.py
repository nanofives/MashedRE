import sys, struct, csv
sys.path.insert(0, r'C:/Users/maria/Desktop/Proyectos/Mashed/re/tools/statediff'); import msd_fields as m
o = sys.argv[1]; v = sys.argv[2]
rows = [r for r in csv.DictReader(open(o + '.aistep.csv', newline='')) if r['v'] == v]
f0 = int(next(r for r in rows if int(r['c4']) != 0)['frame'])
prev = None
recs = list(m.parse(o))
print('frames', len(recs), 'first', recs[0][0], 'last', recs[-1][0], 'window f0', f0)
for fi, r in recs:
    w = [struct.unpack_from('<3f', r, 0x200 + i * 0xc4) for i in range(4)]
    y = struct.unpack_from('<f', r, 0x95c)[0]
    key = tuple(w)
    if key != prev:
        print('frame %d (k=%d) y=%.9g ' % (fi, fi - f0, y) + ' | '.join('(%.9g,%.9g,%.9g)' % t for t in w))
        prev = key
