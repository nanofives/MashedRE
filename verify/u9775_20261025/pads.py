"""u9775: print the record's matrix pad / flag words per call around a window (aligned on first moving frame).
usage: pads.py <msd> lo hi"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
A = list(m.parse(sys.argv[1])); a0 = next(i for i, (fi, r) in enumerate(A) if f(r, 0x9e4) > 0.01)
W = [0x934, 0x944, 0x954, 0x964, 0x984, 0x994, 0x9a4, 0x9b4 - 0x10]
for k in range(int(sys.argv[2]), int(sys.argv[3])):
    r = A[a0 + k][1]; print('k=%d ' % k + ' '.join('%03x:%08x' % (o, u(r, o)) for o in W))
