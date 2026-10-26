"""u9775: per call k of one capture: speed +0x9e4, position +0x958/95c/960, wheel state bytes +0x198+w*0xc4, and the
record words that change from the previous call among the low (non-dynamic) block 0x000..0x158. usage: trunkview.py <msd> lo hi"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
A = [r for _, r in m.parse(sys.argv[1])]; a0 = next(i for i, r in enumerate(A) if f(r, 0x9e4) > 0.01); A = A[a0:]
for k in range(int(sys.argv[2]), int(sys.argv[3])):
    r, p = A[k], A[k - 1]
    ch = [o for o in list(range(0, 0x158, 4)) + list(range(0xa00, 0xd04, 4)) if u(r, o) != u(p, o)]
    print('k=%d v=%.2f pos=(%.3f %.3f %.3f) ws=%s chg=%s' % (k, f(r, 0x9e4), f(r, 0x958), f(r, 0x95c), f(r, 0x960),
          ''.join(str(r[0x198 + w * 0xc4]) for w in range(4)), ' '.join('%03x' % o for o in ch[:14])))
