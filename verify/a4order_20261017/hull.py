"""Car-v contact inputs per (b)-window call, original (.msd record joined on aistep 'frame') vs port (step dump columns):
ring slot 0 / 1 translation (+0x958.. / +0x998..), selectors +0x9a8 / +0x9ac, velocity +0x9b0..+0x9b8, speed +0x9e4, and the
four hull points +0xa28 / +0xa34 / +0xa40 / +0xa4c that VehicleCarCarContact (0x00469df0) runs its SAT over.
usage: py -3.12 hull.py <orig.msd> <orig aistep csv> <port step.csv> <car> <k0> <k1>"""
import csv, struct, sys

F = {'s0x': 0x958, 's0y': 0x95c, 's0z': 0x960, 's1x': 0x998, 's1y': 0x99c, 's1z': 0x9a0,
     'vx': 0x9b0, 'vy': 0x9b4, 'vz': 0x9b8, 'sp': 0x9e4, 'rad': 0x4a4}
H = [0xa28, 0xa34, 0xa40, 0xa4c]
I = {'a8': 0x9a8, 'ac': 0x9ac}


def read_msd(path):
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1'
    rec, _base, _ = struct.unpack_from('<III', b, 4)
    out, off = {}, 16
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        p = b[off + 4: off + 4 + rec]
        d = {k: struct.unpack_from('<f', p, o)[0] for k, o in F.items()}
        d.update({k: struct.unpack_from('<i', p, o)[0] for k, o in I.items()})
        for n, o in enumerate(H):
            d['h%d' % (n + 4)] = struct.unpack_from('<3f', p, o)
        out[fi] = d
        off += 4 + rec
    return out


def win(path, v):
    rows = [r for r in csv.DictReader(open(path, newline='')) if int(r['v']) == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:i0 + 220]


msd, oa, ps, v, k0, k1 = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6])
M = read_msd(msd)
O = win(oa, v)
P = win(ps, v)
f3 = lambda t: '(%.4f,%.4f,%.4f)' % tuple(t)
for k in range(k0, k1):
    o = M.get(int(O[k]['frame']))
    p = P[k]
    print('k=%d frame %s' % (k, O[k]['frame']))
    if o:
        print('  orig s0=(%.4f,%.4f,%.4f) s1=(%.4f,%.4f,%.4f) a8/ac=%d/%d v=(%.2f,%.2f,%.2f) sp=%.3f rad=%.4f' % (
            o['s0x'], o['s0y'], o['s0z'], o['s1x'], o['s1y'], o['s1z'], o['a8'], o['ac'], o['vx'], o['vy'], o['vz'], o['sp'], o['rad']))
        print('       hull ' + ' '.join(f3(o['h%d' % n]) for n in (4, 5, 6, 7)))
    g = lambda c: float(p[c]) if p.get(c, '') not in ('', None) else float('nan')
    print('  port s0=(%.4f,%.4f,%.4f) s1=(%.4f,%.4f,%.4f) a8/ac=%s/%s sp=%.3f rad=%.4f' % (
        g('rec_958'), g('rec_95c'), g('rec_960'), g('rec_998'), g('rec_99c'), g('rec_9a0'), p.get('sel_9a8'), p.get('sel_9ac'),
        g('rec_9e4'), g('rec_4a4')))
    print('       hull ' + ' '.join(f3((g('h%dx' % n), g('h%dy' % n), g('h%dz' % n))) for n in (4, 5, 6, 7)))
