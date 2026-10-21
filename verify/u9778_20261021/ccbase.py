"""u9778: car-car rows (ccdiff.py alignment, matched on (k, self, other, pass, ordinal)). For each car in a row, the record
dwords that differ orig vs port at entry, MINUS the dwords that already differed for that car at the entry of the latest
earlier matched row holding it (baseline). usage: ccbase.py <orig.msd> <port.carcar.tsv> <port.step.csv> kmin kmax"""
import sys, struct
src = open(__file__.replace('ccbase.py', 'ccdiff.py')).read().split("O = load(")[0]
ns = {}; exec(src, ns)
O = ns['load'](sys.argv[1] + '.carcar.tsv', ns['onset'](sys.argv[1] + '.aistep.csv'))
P = ns['load'](sys.argv[2], ns['onset'](sys.argv[3]))
kmin, kmax = int(sys.argv[4]), int(sys.argv[5])
def keyed(R):
    c, out = {}, {}
    for r in R:
        n = c.get(r[:4], 0); c[r[:4]] = n + 1; out[r[:4] + (n,)] = r
    return out
Ok, Pk = keyed(O), keyed(P)
M = sorted((k, Ok[k], Pk[k]) for k in Ok if k in Pk)
print('orig only:', sorted(k for k in Ok if k not in Pk and kmin <= k[0] <= kmax))
print('port only:', sorted(k for k in Pk if k not in Ok and kmin <= k[0] <= kmax))
f = lambda b, o: struct.unpack_from('<f', b, o)[0]
def offs(a, b): return {o for o in range(0, 0xd04, 4) if a[o:o+4] != b[o:o+4]}
base = {}
for key, x, y in M:
    for car, j in ((x[1], 0), (x[2], 1)):
        d = offs(x[5][j], y[5][j])
        if kmin <= key[0] <= kmax:
            new = sorted(d - base.get(car, set()))
            gone = sorted(base.get(car, set()) - d)
            print(f'k={key[0]} pair={x[1]}-{x[2]} pass={x[3]} #{key[4]} car {car} entry: {len(d)} differ, new {len(new)}: ' +
                  ', '.join(f'+0x{o:03x} {f(x[5][j],o):.7g}/{f(y[5][j],o):.7g}' for o in new[:14]) + (f' | gone {len(gone)}' if gone else ''))
        base[car] = d
