"""u9776: recpos for any slot (the player has no aistep rows): original MSD aligned on CAR 3's aistep c4 onset, port MSD1 dump
aligned on its own first record + the port step dump's car-3 onset. Prints k, dx, dz (+0x958 / +0x960) and dvx, dvz (+0x9b0 / +0x9b8).
usage: recpos0.py <orig.msd> <port.rec.msd> <port.step.csv> [kmax]"""
import sys, csv, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
def onset(csvf): return int(next(r for r in csv.DictReader(open(csvf)) if r['v'] == '3' and int(r['c4']) != 0)['frame'])
O = {fi - onset(sys.argv[1] + '.aistep.csv'): rec for fi, rec in m.parse(sys.argv[1])}
P = list(m.parse(sys.argv[2]))
p0 = onset(sys.argv[3])
Pk = {fi - p0: rec for fi, rec in P}
kmax = int(sys.argv[4]) if len(sys.argv) > 4 else 300
print('orig k range', min(O), max(O), '| port k range', min(Pk), max(Pk))
for k in range(-5, kmax):
    if k not in O or k not in Pk: continue
    a, b = O[k], Pk[k]
    d = [f(b, o) - f(a, o) for o in (0x958, 0x960, 0x9b0, 0x9b8)]
    if any(d): print(f'{k:4d} ' + ' '.join(f'{x:+.3e}' for x in d)); break
else: print('equal through', kmax - 1)
