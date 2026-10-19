"""u9765: per call, car-1 record fields that differ from the original outside read-time fields (+0x9d4 / +0x9dc), the
torque-ring history (+0x1ac..+0x1e8, +0x270..+0x2ac) and pad dwords. usage: recfirst.py <orig.msd> <port.rec.msd> [calls]"""
import os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
recs = list(m.parse(sys.argv[1])); O = dict(recs); k0 = next(fi for fi, r in recs if f(r, 0x9b8) != 0)
P = dict(m.parse(sys.argv[2])); n = int(sys.argv[3]) if len(sys.argv) > 3 else 8
skip = {0x944, 0x954, 0x964, 0x984, 0x9a4, 0xd00, 0x9d4, 0x9dc} | set(range(0x1ac, 0x1ec, 4)) | set(range(0x270, 0x2b0, 4))
for k in range(n):
    o, p = O[k0 + k], P[k]
    d = [x for x in range(0, len(o), 4) if o[x:x + 4] != p[x:x + 4] and x not in skip]
    print(k, len(d), ' '.join('+0x%x(o %.9g p %.9g)' % (x, f(o, x), f(p, x)) for x in d[:6]))
