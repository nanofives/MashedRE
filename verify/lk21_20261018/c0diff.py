"""Per car: where the window's steer bytes c0 / c1 differ, original aistep vs port step dump, in call bands, with
the distinct counts each side reaches by the end of each band (the (b) items c0# / c1#).
usage: c0diff.py <orig.aistep.csv> <port.step.csv> [--band 20]"""
import csv, sys, collections


def win(p):
    d = collections.defaultdict(list)
    for r in csv.DictReader(open(p, newline='')):
        d[int(r['v'])].append(r)
    out = {}
    for v, rs in d.items():
        i0 = next((i for i, x in enumerate(rs) if int(x['c4']) != 0), None)
        out[v] = rs[i0:i0 + 220] if i0 is not None else []
    return out


o, p = win(sys.argv[1]), win(sys.argv[2])
band = int(sys.argv[sys.argv.index('--band') + 1]) if '--band' in sys.argv else 20
for v in (1, 2, 3):
    n = min(len(o[v]), len(p[v]))
    parts = []
    so, sp, so1, sp1 = set(), set(), set(), set()
    for b0 in range(0, n, band):
        nd = mx = 0
        for k in range(b0, min(n, b0 + band)):
            a, b = int(o[v][k]['c0']), int(p[v][k]['c0'])
            a1, b1 = int(o[v][k]['c1']), int(p[v][k]['c1'])
            so.add(a); sp.add(b); so1.add(a1); sp1.add(b1)
            if a != b or a1 != b1:
                nd += 1
                mx = max(mx, abs((a1 - a) - (b1 - b)))
        parts.append('%3d:%2d/%3d c0#%2d/%2d c1#%2d/%2d' % (b0, nd, mx, len(so), len(sp), len(so1), len(sp1)))
    print('car %d (n=%d) [band: ndiff/max|dsteer| c0# o/p c1# o/p cumulative]' % (v, n))
    for x in parts:
        print('   ' + x)
