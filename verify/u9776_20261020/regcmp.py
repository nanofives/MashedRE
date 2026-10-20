"""u9776: first record call (k >= 0, aligned on car 3's aistep c4 onset) where two ORIGINAL captures' aistep own_x / own_z
differ, per car 1..3. usage: regcmp.py <a.aistep.csv> <b.aistep.csv>"""
import csv, sys
def own(f):
    R = {}
    for r in csv.DictReader(open(f)): R.setdefault(r['v'], []).append(r)
    i = next(i for i, r in enumerate(R['3']) if int(r['c4']) != 0); f0 = int(R['3'][i]['frame'])
    return {(r['v'], int(r['frame']) - f0): (r['own_x'], r['own_z']) for v in R for r in R[v]}
a, b = own(sys.argv[1]), own(sys.argv[2])
for v in ('1', '2', '3'):
    bad = sorted(k[1] for k in a if k[0] == v and k in b and 0 <= k[1] < 400 and a[k] != b[k])
    print(f'car {v}: ' + ('equal k 0..399' if not bad else f'first difference k={bad[0]}'))
