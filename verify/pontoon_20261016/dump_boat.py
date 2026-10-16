"""Read-only structure dump of BOAT.DFF (frames, HAnim PLG, atomics) and BOATANIM.ANM (header, keyframes)."""
import struct, sys
D = r'D:\MashedWT\pontoon_data\warzone_piz'

def chunks(b, off, end):
    while off + 12 <= end:
        t, s, v = struct.unpack_from('<III', b, off)
        yield t, off + 12, s, v
        off += 12 + s

def dff(path):
    b = open(path, 'rb').read()
    t, p, s, v = next(chunks(b, 0, len(b)))
    print(f'clump 0x{t:x} size {s} ver 0x{v:08x}')
    for t, q, s, v in chunks(b, p, p + s):
        if t == 0x01:
            print('  clump struct', struct.unpack_from('<%di' % (s // 4), b, q))
        elif t == 0x0E:  # frame list
            it = chunks(b, q, q + s)
            st = next(it)
            n = struct.unpack_from('<i', b, st[1])[0]
            print(f'  frames {n}')
            o = st[1] + 4
            for i in range(n):
                f = struct.unpack_from('<12fiI', b, o)
                o += 56
                print(f'   F{i} parent {f[12]} flags 0x{f[13]:x} right {f[0:3]} up {f[3:6]} at {f[6:9]} pos {tuple(round(x,4) for x in f[9:12])}')
            for i, ext in enumerate(it):
                for et, eq, es, ev in chunks(b, ext[1], ext[1] + ext[2]):
                    if et == 0x11E:
                        w = struct.unpack_from('<%di' % (es // 4), b, eq)
                        ver, nid, nn = w[0], w[1], w[2]
                        line = f'   ext{i} HAnimPLG ver 0x{ver:x} nodeId {nid} numNodes {nn}'
                        if nn:
                            flags, kfs = w[3], w[4]
                            nodes = [w[5 + 3 * k:8 + 3 * k] for k in range(nn)]
                            line += f' flags 0x{flags:x} keyFrameSize {kfs} nodes(id,idx,flags) {nodes}'
                        print(line)
                    else:
                        print(f'   ext{i} chunk 0x{et:x} size {es}')
        elif t == 0x1A:
            print(f'  geometry list size {s}')
            for gt, gq, gs, gv in chunks(b, q, q + s):
                if gt == 0x0F:
                    gst = next(chunks(b, gq, gq + gs))
                    fmt, ntri, nv, nmt = struct.unpack_from('<4i', b, gst[1])
                    print(f'   geometry format 0x{fmt:x} tris {ntri} verts {nv} morph {nmt}')
        elif t == 0x14:
            ast = next(chunks(b, q, q + s))
            print('  atomic (frame, geom, flags, unused)', struct.unpack_from('<4i', b, ast[1]))

def anm(path):
    b = open(path, 'rb').read()
    t, s, v = struct.unpack_from('<III', b, 0)
    ver, scheme, nf, flags, dur = struct.unpack_from('<4if', b, 12)
    print(f'anm 0x{t:x} ver 0x{ver:x} scheme {scheme} numFrames {nf} flags 0x{flags:x} duration {dur}')
    o = 32
    for i in range(nf):
        k = struct.unpack_from('<8fi', b, o)
        o += 36
        if i < 12 or i >= nf - 4:
            print(f'   K{i} t {k[0]:.4f} q {tuple(round(x,4) for x in k[1:5])} tr {tuple(round(x,3) for x in k[5:8])} prev {k[8]}')

dff(D + r'\BOAT.DFF')
anm(D + r'\BOATANIM.ANM')
