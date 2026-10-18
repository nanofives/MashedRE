import sys, struct, math, csv
sys.path.insert(0, r'../../re/tools/statediff'); import msd_fields as m
o = 'C:/Users/maria/Desktop/Proyectos/Mashed/verify/d3_modes37_20261009/o_wobj2.msd'
rows = [r for r in csv.DictReader(open(o + '.aistep.csv', newline='')) if r['v'] == '1']
f0 = int(next(r for r in rows if int(r['c4']) != 0)['frame'])
orig = {fi - f0: rec for fi, rec in m.parse(o)}
port = [rec for fi, rec in m.parse('arms2/B_SPLSBR1_K1.rec.msd')]
for k in list(range(0, 61, 6)) + [100, 150]:
    for nm, r in (('o', orig.get(k)), ('p', port[k] if k >= 0 else None)):
        if r is None: continue
        v = struct.unpack_from('<4f', r, 0x1fc)
        n = math.sqrt(sum(x * x for x in v[1:4]))
        print(k, nm, ' +1fc..+208 = ' + ' '.join('%.9g' % x for x in v), ' |200..208|=%.9f' % n)

