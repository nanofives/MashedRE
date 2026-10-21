"""u9777: A5 (FUN_0046ddb0) entry / leave velocity +0x9b0..+0x9b8 on one car, original --a5-probe rows (hex) vs the port's
MASHED_D2SINK a5_in / a5_out marks (%.9g, float-exact). Aligns on the longest run of equal entry velocities, then prints the
first rows where entry or leave differ, plus the port's other stage marks of that frame.
usage: a5vcmp.py <orig.a5probe.csv> <port.sink.txt> [n_show]"""
import csv, re, struct, sys
h2f = lambda s: struct.unpack('<f', struct.pack('<I', int(s, 16)))[0]
f2h = lambda x: '%08x' % struct.unpack('<I', struct.pack('<f', x))[0]
O = [r for r in csv.DictReader(open(sys.argv[1]))]
Oe = [tuple(r['v%d' % j].lower().zfill(8) for j in range(3)) for r in O]
Ol = [tuple(r['o%d' % j].lower().zfill(8) for j in range(3)) for r in O]
frames = {}
for l in open(sys.argv[2]):
    m = re.match(r'f=(-?\d+) tag=(\S+) .*?v=\(([^,]+),([^,]+),([^)]+)\)', l)
    if m: frames.setdefault(int(m[1]), []).append((m[2], tuple(f2h(float(m[j])) for j in (3, 4, 5))))
pf = sorted(f for f in frames if any(t == 'a5_in' for t, _ in frames[f]))
Pe = [next(v for t, v in frames[f] if t == 'a5_in') for f in pf]
Pl = [next(v for t, v in frames[f] if t == 'a5_out') for f in pf]
best = (0, 0, 0)
for j in range(len(Pe)):
    if Pe[j] == ('00000000',) * 3: continue
    for i in [i for i, e in enumerate(Oe) if e == Pe[j]][:20]:
        n = 0
        while i + n < len(Oe) and j + n < len(Pe) and Oe[i + n] == Pe[j + n] and Ol[i + n] == Pl[j + n]: n += 1
        if n > best[0]: best = (n, i, j)
    if best[0] > 50: break
n, i0, j0 = best
print('best run %d A5 calls equal (entry+leave): orig row %d = port frame %d' % (n, i0, pf[j0]))
show = int(sys.argv[3]) if len(sys.argv) > 3 else 2
for d in range(n, n + show):
    i, j = i0 + d, j0 + d
    if i >= len(O) or j >= len(Pe): break
    print('-- call +%d: orig row %d (sdframe %s) / port frame %d' % (d, i, O[i]['sdframe'], pf[j]))
    print('   entry orig %s  port %s %s' % (' '.join(Oe[i]), ' '.join(Pe[j]), '' if Oe[i] == Pe[j] else '<<'))
    print('   leave orig %s  port %s %s' % (' '.join(Ol[i]), ' '.join(Pl[j]), '' if Ol[i] == Pl[j] else '<<'))
    print('   orig w %s s9e4 %s s9e8 %s g9e0 %s' % (' '.join(O[i]['w%d' % k] for k in range(3)), O[i]['s9e4'], O[i]['s9e8'], O[i]['g9e0']))
    for t, v in frames[pf[j]]: print('     port %-10s %s' % (t, ' '.join(v)))
