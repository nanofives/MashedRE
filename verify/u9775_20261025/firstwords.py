"""u9775: full-record words that differ between two original captures at call k (aligned on first moving frame),
with float / hex values and the ulp distance. usage: firstwords.py <a.msd> <b.msd> k [k2]"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
i_ = lambda r, o: struct.unpack_from('<i', r, o)[0]
def mv(L): return next(i for i, (fi, r) in enumerate(L) if f(r, 0x9e4) > 0.01)
A = list(m.parse(sys.argv[1])); B = list(m.parse(sys.argv[2])); a0, b0 = mv(A), mv(B)
k1 = int(sys.argv[3]); k2 = int(sys.argv[4]) if len(sys.argv) > 4 else k1
for k in range(k1, k2 + 1):
    ra, rb = A[a0 + k][1], B[b0 + k][1]
    d = [o for o in range(0, min(len(ra), len(rb)) - 3, 4) if u(ra, o) != u(rb, o)]
    print('k=%d: %d words' % (k, len(d)))
    for o in d:
        print('  +0x%03x  %.9g / %.9g  (%08x / %08x)  ulp %d  int %d / %d' % (o, f(ra, o), f(rb, o), u(ra, o), u(rb, o),
              abs(i_(ra, o) - i_(rb, o)), i_(ra, o), i_(rb, o)))
