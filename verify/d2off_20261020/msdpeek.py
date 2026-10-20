"""d2off: print an MSD's frame range and the rows around the first moving frame (+0x9e4 > 0.01).
usage: msdpeek.py <msd> [...]"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
I = lambda r, o: struct.unpack_from('<i', r, o)[0]
for p in sys.argv[1:]:
    L = list(m.parse(p)); print(p, len(L), 'frames', L[0][0], L[-1][0])
    on = next(i for i, (fi, r) in enumerate(L) if f(r, 0x9e4) > 0.01)
    print(' first moving idx', on, 'frame', L[on][0])
    for i in range(max(0, on - 2), on + 4):
        fi, r = L[i]
        print('  %d v=(%.6f %.6f %.6f) 9e4=%.6f bf4=%d pos=(%.6f %.6f %.6f)' % (fi, f(r, 0x9b0), f(r, 0x9b4), f(r, 0x9b8), f(r, 0x9e4),
              I(r, 0xbf4), f(r, 0x958), f(r, 0x95c), f(r, 0x960)))
