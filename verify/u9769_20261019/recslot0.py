"""u9769: the player's record (slot 0), original MSD vs port MSD1, aligned on AI car <va>'s window start (original frame of
its first aistep row with c4 != 0 = port list index 0). Per call: differing dwords outside the fields that also differ on
the exact car 1 (d3bc's noise set) and the read-time / torque-ring fields. usage: recslot0.py <orig.msd> <orig.aistep.csv>
<port.rec.msd> <va> [k0] [k1] [nshow]"""
import csv, os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
om, oa, pm, va = sys.argv[1:5]; k0 = int(sys.argv[5]) if len(sys.argv) > 5 else -5; k1 = int(sys.argv[6]) if len(sys.argv) > 6 else 45
ns = int(sys.argv[7]) if len(sys.argv) > 7 else 8
rows = [r for r in csv.DictReader(open(oa, newline='')) if r['v'] == va]
f0 = int(rows[next(i for i, r in enumerate(rows) if int(r['c4']) != 0)]['frame'])
O = {fi - f0: r for fi, r in m.parse(om)}; P = [r for fi, r in m.parse(pm)]
skip = {0x944, 0x954, 0x964, 0x984, 0x9a4, 0xd00, 0x9d4, 0x9dc} | set(range(0x1ac, 0x1ec, 4)) | set(range(0x270, 0x2b0, 4))
noise = {0x44, 0x48, 0x13c, 0x140, 0x460, 0x994} | set(range(0xaf4, 0xb00, 4)) | {0xb14, 0xb1c} | set(range(0xb2c, 0xbec, 4))
for k in range(k0, k1):
    if k not in O or not (0 <= k < len(P)): continue
    o, p = O[k], P[k]
    d = [x for x in range(0, len(o), 4) if o[x:x + 4] != p[x:x + 4] and x not in skip and x not in noise]
    print(k, len(d), ' '.join('+0x%x(o %.9g p %.9g)' % (x, f(o, x), f(p, x)) for x in d[:ns]))
