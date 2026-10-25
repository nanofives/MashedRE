"""u9780: car 2's record, original MSD (o_c2_2 / o_c2_3) vs port MSD1 dump (MASHED_RECDUMP_SLOT=2), aligned on k as in
verify/u9778_20261021/velcmp.py (k = frame - the car-3 aistep c4 onset of that side's step dump).
Per k: position +0x958/95c/960 and velocity +0x9b0/9b4/9b8 as raw hex (orig/port), ulp distance where they differ, and the
first k where each group differs. usage: reccmp.py <orig.msd> <port.rec.msd> <port.step.csv> k0 k1"""
import sys, csv, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
f = lambda r, o: struct.unpack_from('<f', r, o)[0]


def onset(c):
    return int(next(r for r in csv.DictReader(open(c)) if r['v'] == '3' and int(r['c4']) != 0)['frame'])


def ulp(a, b):
    s = lambda x: x if x < 0x80000000 else 0x80000000 - x
    return abs(s(a) - s(b))


O = {fi - onset(sys.argv[1] + '.aistep.csv'): r for fi, r in m.parse(sys.argv[1])}
P = {fi - onset(sys.argv[3]): r for fi, r in m.parse(sys.argv[2])}
GROUPS = {'pos': (0x958, 0x95c, 0x960), 'vel': (0x9b0, 0x9b4, 0x9b8), 'angvel': (0x9bc, 0x9c0, 0x9c4)}
first = {}
k0, k1 = int(sys.argv[4]), int(sys.argv[5])
for k in range(k0, k1 + 1):
    if k not in O or k not in P:
        print(f'{k} missing orig={k in O} port={k in P}'); continue
    a, b = O[k], P[k]
    parts = []
    for g, offs in GROUPS.items():
        d = [(o, u(a, o), u(b, o)) for o in offs if u(a, o) != u(b, o)]
        if d and g not in first: first[g] = k
        parts.append(g + ' ' + ('=' if not d else ' '.join(f'+{o:x} {x:08x}/{y:08x} ({f(a,o):.6g}/{f(b,o):.6g}, {ulp(x,y)} ulp)' for o, x, y in d)))
    print(f'{k} ' + ' | '.join(parts))
print('first difference: ' + ', '.join(f'{g} k={first.get(g, "none")}' for g in GROUPS))
