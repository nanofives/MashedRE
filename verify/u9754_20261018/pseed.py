import sys, struct
sys.path.insert(0, r'C:/Users/maria/Desktop/Proyectos/Mashed/re/tools/statediff'); import msd_fields as m
prev = None
recs = list(m.parse(sys.argv[1]))
print('frames', len(recs))
for i, (fi, r) in enumerate(recs):
    w = [struct.unpack_from('<3f', r, 0x200 + j * 0xc4) for j in range(4)]
    if tuple(w) != prev:
        print('idx %d frame %d y=%.9g ' % (i, fi, struct.unpack_from('<f', r, 0x95c)[0]) + ' | '.join('(%.9g,%.9g,%.9g)' % t for t in w))
        prev = tuple(w)
    if i > 80: break
