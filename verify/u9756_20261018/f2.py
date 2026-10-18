import sys, struct
sys.path.insert(0, r'C:/Users/maria/Desktop/Proyectos/Mashed/re/tools/statediff'); import msd_fields as m
for fi, r in m.parse(sys.argv[1]):
    if fi > 3: break
    p = struct.unpack_from('<3f', r, 0x958)
    st = [struct.unpack_from('<i', r, 0x198 + w * 0xc4)[0] for w in range(4)]
    fv = [struct.unpack_from('<f', r, 0x194 + w * 0xc4)[0] for w in range(4)]
    print('frame %d pos=(%.9g, %.9g, %.9g) st=%s fv=%s gc=%.9g' % (fi, p[0], p[1], p[2], st, ['%.9g' % x for x in fv], struct.unpack_from('<f', r, 0x9e0)[0]))
