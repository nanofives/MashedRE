"""u9774: A6a tail on car 0, original --a6t-probe (hex) vs the port's MASHED_A6A_TAILLOG (floats), aligned on each side's first
row with a nonzero velocity. Prints, for a call range, the shared columns that differ bit for bit.
usage: a6tcmp.py <orig.a6tprobe.csv> <port.tail.csv> k0 k1"""
import csv, struct, sys
hx = lambda s: struct.unpack('<I', struct.pack('<I', int(s, 16)))[0]
fb = lambda s: struct.unpack('<I', struct.pack('<f', float(s)))[0]
bf = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
O = list(csv.DictReader(open(sys.argv[1]))); P = list(csv.DictReader(open(sys.argv[2])))
cols = [c for c in P[0].keys() if c in O[0] and c != 'n']
INTS = {'g490', 't494'}   # integer fields: hex on the original side, the int value on the port side
mv = lambda r, f: (f(r['s9e4']) & 0x7fffffff) != 0
o0 = next(i for i, r in enumerate(O) if mv(r, hx)); p0 = next(i for i, r in enumerate(P) if mv(r, fb))
print('shared', cols, '| orig first moving row n', O[o0]['n'], 'port', P[p0]['n'])
for k in range(int(sys.argv[3]), int(sys.argv[4])):
    if o0 + k >= len(O) or p0 + k >= len(P): break
    a, b = O[o0 + k], P[p0 + k]
    d = [c for c in cols if (hx(a[c]) != (int(float(b[c])) & 0xffffffff) if c in INTS else hx(a[c]) != fb(b[c]))]
    if d: print('%4d ' % k + '  '.join('%s %.9g/%.9g' % (c, bf(hx(a[c])), float(b[c])) for c in d))
