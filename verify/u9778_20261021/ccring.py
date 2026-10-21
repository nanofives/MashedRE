"""u9778: the +0x928 ring (two 0x40 slots), selectors +0x9a8 / +0x9ac, velocity +0x9b0 and +0x9bc.. for each car of the matched
car-car rows at k (ccdiff alignment), orig / port, entry and leave. usage: ccring.py <orig.msd> <port.carcar.tsv> <port.step.csv> k"""
import sys, struct
src = open(__file__.replace('ccring.py', 'ccdiff.py')).read().split("O = load(")[0]
ns = {}; exec(src, ns)
O = ns['load'](sys.argv[1] + '.carcar.tsv', ns['onset'](sys.argv[1] + '.aistep.csv'))
P = ns['load'](sys.argv[2], ns['onset'](sys.argv[3]))
K = int(sys.argv[4])
f = lambda b, o: struct.unpack_from('<f', b, o)[0]
i = lambda b, o: struct.unpack_from('<i', b, o)[0]
for x, y in zip([r for r in O if r[0] == K], [r for r in P if r[0] == K]):
    print(f'== k={K} pair {x[1]}-{x[2]} pass {x[3]}/{y[3]} ret {x[4]}/{y[4]}')
    for lab, j in (('self in ', 0), ('other in', 1), ('self out', 2), ('oth out ', 3)):
        a, b = x[5][j], y[5][j]
        print(f'  {lab} sel {i(a,0x9a8)},{i(a,0x9ac)} / {i(b,0x9a8)},{i(b,0x9ac)}')
        for s in (0, 1):
            for row in range(4):
                o = 0x928 + s * 0x40 + row * 0x10
                va = [f(a, o + 4 * c) for c in range(4)]; vb = [f(b, o + 4 * c) for c in range(4)]
                mark = '' if a[o:o+16] == b[o:o+16] else '  <> ' + ' '.join(f'{p - q:+.2e}' for p, q in zip(vb, va))
                print(f'    +0x{o:03x} ' + ' '.join(f'{v:11.6f}' for v in va) + mark)
        o = 0x9b0
        print(f'    vel  ' + ' '.join(f'{f(a,o+4*c):.4f}/{f(b,o+4*c):.4f}' for c in range(3)) + f'   +0x9bc.. ' +
              ' '.join(f'{f(a,0x9bc+4*c):.5g}/{f(b,0x9bc+4*c):.5g}' for c in range(3)))
