"""Read-only: list COLLIDE.BSP triangles near the Warzone pontoon deck (RW 3.x world, chunk 0x0B).
Usage: py -3.12 bsp_probe.py <COLLIDE.BSP> <cx> <cz> <half>"""
import struct, sys

def chunks(b, off, end):
    while off + 12 <= end:
        t, s, v = struct.unpack_from('<III', b, off)
        yield t, off + 12, s
        off += 12 + s

def main():
    path, cx, cz, half = sys.argv[1], float(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4])
    b = open(path, 'rb').read()
    t, p, s = next(chunks(b, 0, len(b)))
    assert t == 0x0B, hex(t)
    st = next(chunks(b, p, p + s))
    assert st[0] == 0x01
    hdr = b[st[1]:st[1] + st[2]]
    root, ox, oy, oz, ntri, nvert, nplane, nsec, colsz, fmt = struct.unpack_from('<i3f6i', hdr, 0)
    print(f'world: tris {ntri} verts {nvert} planes {nplane} sectors {nsec} format 0x{fmt:08x} origin ({-ox:.2f},{-oy:.2f},{-oz:.2f})')
    nuv = (fmt >> 16) & 0xff
    if nuv == 0:
        nuv = 2 if fmt & 0x80 else (1 if fmt & 0x04 else 0)
    hits = []
    def walk(off, end):
        for t, p, s in chunks(b, off, end):
            if t == 0x09:
                cs = next(chunks(b, p, p + s))
                q = cs[1]
                base, nt, nv = struct.unpack_from('<3i', b, q)
                q += 44
                verts = [struct.unpack_from('<3f', b, q + 12 * i) for i in range(nv)]
                q += 12 * nv
                if fmt & 0x10: q += 4 * nv
                if fmt & 0x08: q += 4 * nv
                q += 8 * nv * nuv
                for i in range(nt):
                    a, c, d, m = struct.unpack_from('<4H', b, q + 8 * i)
                    tri = [verts[k] for k in (a, c, d) if k < nv]
                    if len(tri) < 3: continue
                    xs = [v[0] for v in tri]; zs = [v[2] for v in tri]
                    if max(xs) >= cx - half and min(xs) <= cx + half and max(zs) >= cz - half and min(zs) <= cz + half:
                        hits.append((tri, m))
            elif t == 0x0A:
                walk(p, p + s)
    walk(p, p + s)
    ys = sorted(v[1] for tri, m in hits for v in tri)
    print(f'triangles overlapping x {cx}+-{half}, z {cz}+-{half}: {len(hits)}')
    if ys:
        import collections
        hist = collections.Counter(round(y, 1) for y in ys)
        print('vertex y histogram (top 20):', hist.most_common(20))

main()
