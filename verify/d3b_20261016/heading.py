"""Car-v body heading and angular-velocity accumulator per AI window step, original vs port (d3b lane).
Original: <orig>.msd (MSD1, --statediff-car v, the record per frame) joined on 'frame' to <orig>.msd.aistep.csv.
Port: the step dump's rec_9d4 / rec_9dc / rec_144 / rec_148 / rec_14c / rec_9c0 columns (same call).
heading = atan2(+0x9d4, +0x9dc) in degrees, measured on the record on both sides (re/tools/ai_yawrate.py OFF).
usage: py -3.12 heading.py <orig.msd> <orig aistep csv> <port step.csv> [car] [k0] [k1]"""
import csv, math, struct, sys

def read_msd(path, offs):
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1'
    rec, _base, _ = struct.unpack_from('<III', b, 4)
    out, off = {}, 16
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        p = b[off + 4: off + 4 + rec]
        out[fi] = {k: struct.unpack_from('<f', p, o)[0] for k, o in offs.items()}
        off += 4 + rec
    return out

def window(path, v):
    rows = [r for r in csv.DictReader(open(path, newline='')) if int(r['v']) == v]
    i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    return rows[i0:i0 + 220]

OFFS = {'fx': 0x9d4, 'fz': 0x9dc, 'a144': 0x144, 'a148': 0x148, 'a14c': 0x14c, 'w9c0': 0x9c0}
msd, oc, pc = sys.argv[1], sys.argv[2], sys.argv[3]
v = int(sys.argv[4]) if len(sys.argv) > 4 else 1
k0 = int(sys.argv[5]) if len(sys.argv) > 5 else 0
k1 = int(sys.argv[6]) if len(sys.argv) > 6 else 60
M = read_msd(msd, OFFS)
O = window(oc, v)
P = window(pc, v)
hd = lambda fx, fz: math.degrees(math.atan2(fx, fz))
print(' k | orig st  head     a148       w9c0     | port st  head     a148       w9c0     | dhead')
for k in range(k0, min(k1, len(O), len(P))):
    o, p = O[k], P[k]
    m = M.get(int(o['frame']))
    if m is None:
        print('%3d | no msd frame %s' % (k, o['frame'])); continue
    ho = hd(m['fx'], m['fz'])
    hp = hd(float(p['rec_9d4']), float(p['rec_9dc']))
    print('%3d | %4d %8.4f %10.6f %10.6f | %4d %8.4f %10.6f %10.6f | %7.4f' % (
        k, int(o['c1']) - int(o['c0']), ho, m['a148'], m['w9c0'],
        int(p['c1']) - int(p['c0']), hp, float(p['rec_148']), float(p['rec_9c0']), hp - ho))
