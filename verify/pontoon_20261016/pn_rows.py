"""Parser for <tag>.msd.pontoon.csv rows written by scenario_launch.py --pontoon-probe (layout: pnProbeArm there).

    import pn_rows; rows = pn_rows.load(path)   -> list of dict rows (raw u32 dwords, floats via pn_rows.f())
"""
import struct


def f(u):
    return struct.unpack('<f', struct.pack('<I', u & 0xffffffff))[0]


def _obj(v, i):
    o = {}
    o['state'] = v[i]; i += 1
    o['frame10'] = v[i]; i += 1
    o['box'] = v[i:i + 6]; i += 6            # +0x14 sup x,y,z ; +0x20 inf x,y,z
    o['pos38'] = v[i:i + 3]; i += 3
    o['d44'] = v[i]; i += 1
    o['anim4c'] = v[i]; i += 1
    o['m54'] = v[i:i + 16]; i += 16
    o['p94'] = v[i]; i += 1
    o['m94'] = v[i:i + 16]; i += 16
    o['a4'] = v[i]; i += 1
    o['mov'] = v[i:i + 3]; i += 3
    o['b4'] = v[i:i + 4]; i += 4
    o['cb'] = v[i]; i += 1
    o['c8'] = v[i]; i += 1
    o['ff50'] = v[i:i + 4]; i += 4
    return o, i


def _cars(v, i):
    cars = []
    for _ in range(4):
        c = {'sel': v[i]}; i += 1
        c['mat'] = v[i:i + 16]; i += 16
        c['rwp'] = v[i]; c['rwp_t'] = v[i + 1]; i += 2
        c['vel'] = v[i:i + 6]; i += 6
        c['mk'] = v[i]; c['mk_on'] = v[i + 1]; i += 2
        cars.append(c)
    return cars, i


def parse(line):
    t = line.strip().split(',')
    n, frame, units, count = int(t[0]), int(t[1]), int(t[2]), int(t[3])
    assert t[4] == 'E'
    li = t.index('L')
    ev = [int(x) & 0xffffffff for x in t[5:li]]
    lv = [int(x) & 0xffffffff for x in t[li + 1:]]
    r = {'n': n, 'frame': frame, 'units': units, 'count': count}
    r['E'], i = _obj(ev, 0)
    r['Ecars'], i = _cars(ev, i)
    r['tower'] = ev[i:i + 16]; i += 16
    r['deck'] = ev[i:i + 16]; i += 16
    if i < len(ev):
        r['tower_rest'] = ev[i:i + 16]; i += 16
        r['deck_rest'] = ev[i:i + 16]; i += 16
    assert i == len(ev), (i, len(ev))
    r['L'], j = _obj(lv, 0)
    r['Lcars'], j = _cars(lv, j)
    r['hflags'], r['hnodes'] = lv[j], lv[j + 1]; j += 2
    r['nodes'] = [lv[j + 16 * k:j + 16 * k + 16] for k in range(4)]; j += 64
    r['itime'], r['inext'], r['iframes'], r['icbt'] = lv[j:j + 4]; j += 4
    r['atoms'] = []
    for _ in range(3):
        r['atoms'].append({'flags': lv[j], 'ltm': lv[j + 1:j + 17]}); j += 17
    assert j == len(lv), (j, len(lv))
    return r


def load(path):
    with open(path) as fh:
        return [parse(ln) for ln in fh if ln.strip()]


def fm(m):
    return '[' + ' '.join('%.6g' % f(x) for x in m[:3]) + ' | ' + ' '.join('%.6g' % f(x) for x in m[4:7]) + ' | ' + \
        ' '.join('%.6g' % f(x) for x in m[8:11]) + ' | ' + ' '.join('%.6g' % f(x) for x in m[12:15]) + ' fl %08x]' % m[3]
