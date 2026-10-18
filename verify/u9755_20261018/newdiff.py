import sys, struct, csv
sys.path.insert(0, r'../../re/tools/statediff'); import msd_fields as m
o = 'C:/Users/maria/Desktop/Proyectos/Mashed/verify/d3_modes37_20261009/o_wobj2.msd'
rows = [r for r in csv.DictReader(open(o + '.aistep.csv', newline='')) if r['v'] == '1']
f0 = int(next(r for r in rows if int(r['c4']) != 0)['frame'])
orig = {fi - f0: rec for fi, rec in m.parse(o)}
port = [rec for fi, rec in m.parse(sys.argv[1])]
RING = range(0x1a8, 0x1ec)
seen = set()
for k in range(0, 5):
    ro, rp = orig[k], port[k]
    new = []
    for off in range(0, min(len(ro), len(rp)) - 3, 4):
        if off in RING or ro[off:off+4] == rp[off:off+4]:
            continue
        if off not in seen:
            seen.add(off)
            fo, fp = struct.unpack('<f', ro[off:off+4])[0], struct.unpack('<f', rp[off:off+4])[0]
            new.append('+%03x %.9g/%.9g' % (off, fo, fp))
    print('k=%d new diffs %d: %s' % (k, len(new), '  '.join(new[:40])))
