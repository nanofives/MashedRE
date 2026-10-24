"""List a DFF's frames, atomics, geometries and the textures of each geometry's materials."""
import sys, struct
b = open(sys.argv[1], 'rb').read()

def chunks(off, end):
    while off + 12 <= end:
        t, sz, ver = struct.unpack_from('<III', b, off)
        yield t, off + 12, sz
        off += 12 + sz

def find(off, end, t):
    return [(o, s) for tt, o, s in chunks(off, end) if tt == t]

def cstr(o, s):
    return b[o:o + s].split(b'\0')[0].decode('latin1')

t, o, s = next(chunks(0, len(b)))
assert t == 0x10
kids = list(chunks(o, o + s))
# struct: numAtomics, numLights, numCameras
st = kids[0]
natom = struct.unpack_from('<I', b, st[1])[0]
# frame list
fl = [k for k in kids if k[0] == 0x0e][0]
fk = list(chunks(fl[1], fl[1] + fl[2]))
nfr = struct.unpack_from('<I', b, fk[0][1])[0]
names = []
for ext in fk[1:]:
    nm = ''
    for tt, oo, ss in chunks(ext[1], ext[1] + ext[2]):
        for t2, o2, s2 in chunks(oo, oo + ss):
            if t2 == 0x0253F2FE:
                nm = b[o2:o2 + s2].decode('latin1')
    names.append(nm)
frames = []
for i in range(nfr):
    p = fk[0][1] + 4 + i * 56
    parent = struct.unpack_from('<i', b, p + 48)[0]
    frames.append(parent)
# geometry list
gl = [k for k in kids if k[0] == 0x1a][0]
gk = list(chunks(gl[1], gl[1] + gl[2]))
geoms = []
for g in gk[1:]:
    gch = list(chunks(g[1], g[1] + g[2]))
    gs = gch[0]
    flags, ntri, nvert, nmt = struct.unpack_from('<IiiI', b, gs[1])
    ml = [c for c in gch if c[0] == 0x08][0]
    mk = list(chunks(ml[1], ml[1] + ml[2]))
    texs = []
    for m in mk[1:]:
        mch = list(chunks(m[1], m[1] + m[2]))
        mst = mch[0]
        col = struct.unpack_from('<4B', b, mst[1] + 4)
        istex = struct.unpack_from('<I', b, mst[1] + 12)[0]
        tex = ''
        for tc in mch:
            if tc[0] == 0x06:
                tch = list(chunks(tc[1], tc[1] + tc[2]))
                tex = cstr(tch[1][1], tch[1][2])
        hasfx = any(c[0] == 0x03 and find(c[1], c[1] + c[2], 0x120) for c in mch)
        texs.append((tex, col, 'fx' if hasfx else ''))
    geoms.append((flags, ntri, nvert, texs))
for a in [k for k in kids if k[0] == 0x14]:
    ach = list(chunks(a[1], a[1] + a[2]))
    fi, gi, aflags = struct.unpack_from('<III', b, ach[0][1])
    g = geoms[gi]
    txt = [x[0] for x in g[3]]
    mark = '*' if any('hadow' in x for x in txt) else ' '
    print(f"{mark} atomic frame={fi:3d} '{names[fi] if fi < len(names) else ''}' parent={frames[fi]} geom={gi} aflags=0x{aflags:x} gflags=0x{g[0]:x} tri={g[1]} vert={g[2]} mats={g[3]}")
