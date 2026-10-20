"""u9771: A6a wheel-force terms, original --a6a-probe-wheel rows (stack dwords at 0x004c3ac0 from 0x00468343) vs the port's
MASHED_FORCE_SPLIT rows, for one car / wheel. Rows are aligned on the first row with le4 != 0 on each side. Prints the first rows where any shared term differs.
usage: fscmp.py <orig.a6aprobe.csv> <port.fs.csv> [rows]"""
import csv, struct, sys
def hx(s): return struct.unpack('<f', struct.pack('<I', int(s, 16)))[0]
def f32(x): return struct.unpack('<f', struct.pack('<f', float(x)))[0]
O = [{k: (hx(v) if k != 'n' else int(v)) for k, v in r.items()} for r in csv.DictReader(open(sys.argv[1]))]
P = [{k: (f32(v) if k not in ('n', 'branch') else int(v)) for k, v in r.items()} for r in csv.DictReader(open(sys.argv[2]))]
n = int(sys.argv[3]) if len(sys.argv) > 3 else 40
PAIRS = [('le4', 'le4'), ('ld4', 'ld4'), ('lc8', 'lc8'), ('lc4', 'lc4'), ('lc0p', 'lc0p'), ('lbc', 'lbc'), ('lac', 'lac'),
         ('la8', 'la8'), ('la4', 'la4'), ('dFx', 'dF_x'), ('dFy', 'dF_y'), ('le0', 'le0'), ('ldc', 'ldc'), ('ld8', 'ld8'),
         ('s0', 's0'), ('s1', 's1'), ('Fx', 'post_x'), ('Fz', 'post_z')]
oz = [i for i, r in enumerate(O) if r['le4'] != 0]; pz = [i for i, r in enumerate(P) if r['le4'] != 0]
d = pz[0] - oz[0]
m = sum(1 for i in oz[:40] if 0 <= i + d < len(P) and all(O[i][a] == P[i + d][b] for a, b in PAIRS[:6]))
print('alignment: port row = orig row %+d (%d of 40 rows bit-equal on le4..lbc); first nonzero orig row %d' % (d, m, oz[0]))
shown = 0
for i in oz:
    j = i + d
    if not (0 <= j < len(P)): break
    diff = [(a, O[i][a], P[j][b]) for a, b in PAIRS if O[i][a] != P[j][b]]
    if diff:
        print('orig row %d (k=%d from first nonzero) port row %d:' % (i, i - oz[0], j), ' '.join('%s %.9g|%.9g' % x for x in diff[:8]))
        shown += 1
        if shown >= 4: break
if not shown: print('no difference over', len(oz), 'rows')
