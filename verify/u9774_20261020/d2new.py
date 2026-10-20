"""d2off: the first calls where a record dword differs that did not differ at k = 0 (the static layout set), skipping the RW
matrix pad words, the dump's +0x9d4..dc read lag and A5's write-only face-normal ring +0xb2c..+0xbeb.
usage: d2new.py <orig.msd> <port.rec.msd> [n_calls]"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
def mv(L): return next(i for i, (fi, r) in enumerate(L) if f(r, 0x9e4) > 0.01)
O = list(m.parse(sys.argv[1])); P = list(m.parse(sys.argv[2])); o0, p0 = mv(O), mv(P)
n = int(sys.argv[3]) if len(sys.argv) > 3 else 3
L = min(len(O[0][1]), len(P[0][1]))
SKIP = {0x954, 0x984, 0x994, 0x9a4, 0x9d4, 0x9d8, 0x9dc} | set(range(0xb2c, 0xbec, 4))
a, b = O[o0][1], P[p0][1]
static = {o for o in range(0, L - 3, 4) if u(a, o) != u(b, o)}
print('static set at k=0: %d offsets: %s' % (len(static), ' '.join('%x' % o for o in sorted(static))))
shown = 0
for k in range(1, min(len(O) - o0, len(P) - p0)):
    a, b = O[o0 + k][1], P[p0 + k][1]
    d = [o for o in range(0, L - 3, 4) if o not in SKIP and o not in static and u(a, o) != u(b, o)]
    if d:
        print('k=%d (orig frame %d, port frame %d): %d offsets' % (k, O[o0 + k][0], P[p0 + k][0], len(d)))
        for o in d[:30]: print('   +0x%03x  %.9g / %.9g  (%08x / %08x)' % (o, f(a, o), f(b, o), u(a, o), u(b, o)))
        shown += 1
        if shown >= n: break
