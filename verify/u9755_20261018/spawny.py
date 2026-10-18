import sys, struct, glob, os, json
sys.path.insert(0, r'C:/Users/maria/Desktop/Proyectos/Mashed/re/tools/statediff'); import msd_fields as m
for p in sys.argv[1:]:
    if not os.path.exists(p): print(p, 'missing'); continue
    prov = p + '.provenance.json'
    car = '?'
    if os.path.exists(prov):
        car = json.load(open(prov)).get('statediff', {}).get('car', '?')
    recs = list(m.parse(p))
    for fi, r in recs[:4]:
        x, y, z = struct.unpack_from('<3f', r, 0x958)
        if y != 0.0:
            print('%-40s car %s frame %d spawn pos=(%.9g, %.9g, %.9g)' % (os.path.basename(p), car, fi, x, y, z)); break
