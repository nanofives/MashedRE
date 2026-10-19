"""d2rebase: historic D2 logs, old verdict (score_d2.py reduction + d81a8df6 band) against new (--max-lines 1080 + band.json).
usage: py -3.12 hist_table.py hist_legacy.txt hist_v2.txt"""
import re, sys, json, os, collections
OLD = {'slip1500': (0.18855, 0.19635), 'slip2000': (0.24488, 0.25487), 'drvmed': (1904.70, 1982.44)}
nb = json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'band.json')))
NEW = {k: tuple(nb[k]['band']) for k in OLD}
def parse(p):
    d = {}
    for line in open(p):
        m = re.match(r'(.+?\.motion_diag\.log): slip1500=([\d.]+)\S* \S* ?slip2000=([\d.]+)\S* \S* ?drvmed=([\d.]+)', line.replace(' in ', ' ').replace(' OUT ', ' '))
        m = re.match(r'(.+?\.motion_diag\.log): slip1500=([\d.NA]+)(?: \w+)? slip2000=([\d.NA]+)(?: \w+)? drvmed=([\d.NA]+)', line)
        if m:
            d[m.group(1)] = tuple(float(x) if x != 'NA' else None for x in m.group(2, 3, 4))
    return d
def ok(v, b):
    return all(x is not None and b[k][0] <= x <= b[k][1] for x, k in zip(v, ('slip1500', 'slip2000', 'drvmed')))
L, V = parse(sys.argv[1]), parse(sys.argv[2])
groups = collections.OrderedDict()
for f in sorted(L):
    g = re.sub(r'_\d+\.motion_diag\.log$', '', f.replace(chr(92), '/'))
    groups.setdefault(g, []).append(f)
print('%-70s %5s %5s  old(legacy red., old band)  new(1080, new band)' % ('arm', 'runs', ''))
for g, fs in groups.items():
    o = sum(ok(L[f], OLD) for f in fs); n = sum(ok(V[f], NEW) for f in fs if f in V)
    lv = L[fs[0]]; vv = V.get(fs[0])
    w = sum(ok(L[f], NEW) for f in fs)   # whole-log (legacy) reduction against the new band
    flip = '' if (o == len(fs)) == (n == len(fs)) else '  <-- 1080 verdict changes'
    flip += '' if (o == len(fs)) == (w == len(fs)) else '  <-- whole-log/new-band verdict changes'
    flip = ('  whole-log+new %d/%d' % (w, len(fs))) + flip
    print('%-70s %5d  old %d/%d (%s)  new %d/%d (%s)%s' % (g[-70:], len(fs), o, len(fs), ' / '.join('%.4f' % x if x else 'NA' for x in lv),
          n, len(fs), ' / '.join('%.4f' % x if x else 'NA' for x in vv) if vv else '-', flip))
