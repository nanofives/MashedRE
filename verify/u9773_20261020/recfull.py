"""u9773: every record dword that differs, original MSD (aligned on its aistep c4 onset) vs port MSD1 dump, at given calls.
usage: recfull.py <orig.msd> <port.rec.msd> <car> k[,k...]"""
import sys, csv, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]; u = lambda r, o: struct.unpack_from('<I', r, o)[0]
rows = [r for r in csv.DictReader(open(sys.argv[1] + '.aistep.csv', newline='')) if r['v'] == sys.argv[3]]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); f0 = int(rows[i0]['frame'])
O = {fi - f0: rec for fi, rec in m.parse(sys.argv[1])}; P = [rec for fi, rec in m.parse(sys.argv[2])]
for k in map(int, sys.argv[4].split(',')):
    a, b = O[k], P[k]
    d = [o for o in range(0, min(len(a), len(b)) - 3, 4) if u(a, o) != u(b, o)]
    print('k=%d: %d words' % (k, len(d)))
    for o in d[:40]: print('   +0x%03x %.9g / %.9g (%08x / %08x)' % (o, f(a, o), f(b, o), u(a, o), u(b, o)))
