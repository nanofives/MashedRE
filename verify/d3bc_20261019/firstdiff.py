"""u9765: first call where each car-1 physics field differs from the original (record dumps, read-time fields excluded).
usage: firstdiff.py <orig.msd> <port.rec.msd> [calls]"""
import os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
recs = list(m.parse(sys.argv[1])); O = dict(recs); k0 = next(fi for fi, r in recs if f(r, 0x9b8) != 0)
P = dict(m.parse(sys.argv[2])); n = int(sys.argv[3]) if len(sys.argv) > 3 else 60
F = [(0x958, 'pos.x'), (0x95c, 'pos.y'), (0x960, 'pos.z'), (0x9b0, 'vel.x'), (0x9b4, 'vel.y'), (0x9b8, 'vel.z'),
     (0x9bc, 'trq.x'), (0x9c0, 'trq.y'), (0x9c4, 'trq.z'), (0x144, 'av.x'), (0x148, 'av.y'), (0x14c, 'av.z'),
     (0x928, 'right.x'), (0x93c, 'up.y'), (0x948, 'at.x'), (0x9e4, 'spd'), (0x9e8, 'angsp')]
first = {}
for k in range(min(n, len(P))):
    if k0 + k not in O: break
    o, p = O[k0 + k], P[k]
    for x, nm in F:
        if nm not in first and o[x:x + 4] != p[x:x + 4]: first[nm] = (k, f(o, x), f(p, x))
for nm, (k, a, b) in sorted(first.items(), key=lambda t: t[1][0]): print('%-8s first diff call %3d  o %.9g  p %.9g' % (nm, k, a, b))
