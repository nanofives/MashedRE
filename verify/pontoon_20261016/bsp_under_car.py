"""P5 follow-up: COLLIDE.BSP surface heights under each aboard car position of an OP capture.
    py -3.12 bsp_under_car.py <tag> <COLLIDE.BSP>"""
import struct
import sys

import pn_rows as P


def tris(path):
    b = open(path, 'rb').read()

    def chunks(off, end):
        while off + 12 <= end:
            t, s, v = struct.unpack_from('<III', b, off)
            yield t, off + 12, s
            off += 12 + s
    t, p, s = next(chunks(0, len(b)))
    st = next(chunks(p, p + s))
    fmt = struct.unpack_from('<i3f6i', b, st[1])[9]
    nuv = (fmt >> 16) & 0xff
    out = []

    def walk(off, end):
        for t, q, s in chunks(off, end):
            if t == 0x09:
                cs = next(chunks(q, q + s)); o = cs[1]
                base, nt, nv = struct.unpack_from('<3i', b, o); o += 44
                vs = [struct.unpack_from('<3f', b, o + 12 * i) for i in range(nv)]; o += 12 * nv
                if fmt & 0x10: o += 4 * nv
                if fmt & 0x08: o += 4 * nv
                o += 8 * nv * nuv
                for i in range(nt):
                    a, c, d, m = struct.unpack_from('<4H', b, o + 8 * i)
                    if max(a, c, d) < nv:
                        out.append((vs[a], vs[c], vs[d]))
            elif t == 0x0A:
                walk(q, q + s)
    walk(p, p + s)
    return out


def heights(T, x, z):
    hs = []
    for a, b, c in T:
        d = (b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2])
        if abs(d) < 1e-9:
            continue
        l1 = ((b[2] - c[2]) * (x - c[0]) + (c[0] - b[0]) * (z - c[2])) / d
        l2 = ((c[2] - a[2]) * (x - c[0]) + (a[0] - c[0]) * (z - c[2])) / d
        l3 = 1 - l1 - l2
        if l1 >= 0 and l2 >= 0 and l3 >= 0:
            hs.append(l1 * a[1] + l2 * b[1] + l3 * c[1])
    return sorted(hs)


def main():
    tag, bsp = sys.argv[1], sys.argv[2]
    T = tris(bsp)
    rs = P.load(f'{tag}.msd.pontoon.csv')
    rows = [r for r in rs if r['L']['state'] in (2, 3)]
    stat = {'top_matches_car': 0, 'no_surface_near_car': 0, 'n': 0}
    lines = []
    for r in rows[::10]:
        for v in range(4):
            if r['L']['ff50'][v] != 1:
                continue
            m = r['Lcars'][v]['mat']
            x, y, z = P.f(m[12]), P.f(m[13]), P.f(m[14])
            hs = heights(T, x, z)
            near = [h for h in hs if abs(h - y) < 0.1]
            stat['n'] += 1
            if near: stat['top_matches_car'] += 1
            else: stat['no_surface_near_car'] += 1
            lines.append(f'n={r["n"]} state={r["L"]["state"]} car{v} ({x:.2f},{y:.3f},{z:.2f}) bsp_y={[round(h, 3) for h in hs]}')
    txt = '\n'.join(lines) + f'\n# {stat}\n'
    print(txt)
    open(f'{tag}_bsp_under_car.txt', 'w').write(txt)


if __name__ == '__main__':
    main()
