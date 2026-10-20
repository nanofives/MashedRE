"""d2off: the player's record, original MSD vs port MSD1 dump, aligned on the first moving frame (+0x9e4 > 0.01) of each side.
Prints selected fields per call k (k = 0 at the first moving frame). usage: d2rec.py <orig.msd> <port.rec.msd> [k_list|a:b:s] [fields]"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
I = lambda r, o: struct.unpack_from('<i', r, o)[0]
def mv(L): return next(i for i, (fi, r) in enumerate(L) if f(r, 0x9e4) > 0.01)
O = list(m.parse(sys.argv[1])); P = list(m.parse(sys.argv[2])); o0, p0 = mv(O), mv(P)
ks = sys.argv[3] if len(sys.argv) > 3 else '0:40:1'
ks = range(*map(int, ks.split(':'))) if ':' in ks else [int(x) for x in ks.split(',')]
F = [int(x, 16) for x in (sys.argv[4].split(',') if len(sys.argv) > 4 else ['958', '95c', '960', '9b0', '9b4', '9b8', '9e4'])]
print('k  ' + '  '.join('%22s' % ('+0x%x o/p' % o) for o in F))
for k in ks:
    if o0 + k >= len(O) or p0 + k >= len(P): break
    a, b = O[o0 + k][1], P[p0 + k][1]
    print('%4d ' % k + '  '.join('%11.6f/%-11.6f' % (f(a, o), f(b, o)) for o in F))
