import struct, glob, os, sys
import numpy as np
BASE = 0x005ea000
A = 0x0068bc0c
K = sys.argv[1] if len(sys.argv) > 1 else '00'
runs = []
for pat in sys.argv[2:]:
    runs += [os.path.dirname(m) for m in glob.glob(pat + '/meta.json')]
arrs, vals = [], []
for d in sorted(runs):
    f = os.path.join(d, 'ckpt%s.data.bin' % K)
    if not os.path.exists(f):
        continue
    b = np.frombuffer(open(f, 'rb').read(), dtype='<u4')
    arrs.append(b); vals.append(int(b[(A - BASE) // 4]))
n = min(len(a) for a in arrs)
M = np.stack([a[:n] for a in arrs])
v = np.array(vals)
g4, g0 = M[v == 4], M[v == 0]
print('runs: 4=%d 0=%d' % (len(g4), len(g0)))
const4 = (g4 == g4[0]).all(0)
const0 = (g0 == g0[0]).all(0)
sep = np.where(const4 & const0 & (g4[0] != g0[0]))[0]
for i in sep:
    print('0x%08x  4-group=%08x  0-group=%08x' % (BASE + 4 * i, g4[0][i], g0[0][i]))
# looser: values sets disjoint
dis = []
for i in np.where(~(const4 & const0))[0]:
    s4, s0 = set(g4[:, i].tolist()), set(g0[:, i].tolist())
    if not (s4 & s0) and len(s4) <= 3 and len(s0) <= 3:
        dis.append(i)
print('disjoint (<=3 values each):', len(dis))
for i in dis[:60]:
    print('0x%08x  4:%s  0:%s' % (BASE + 4 * i, sorted('%x' % x for x in set(g4[:, i].tolist())), sorted('%x' % x for x in set(g0[:, i].tolist()))))
z4 = (g4 == 0).all(0); nz4 = (g4 != 0).all(0); z0 = (g0 == 0).all(0); nz0 = (g0 != 0).all(0)
for i in np.where((z4 & nz0) | (z0 & nz4))[0]:
    print('zero-split 0x%08x  4-group:%s  0-group:%s' % (BASE + 4 * i, 'zero' if z4[i] else 'nonzero', 'zero' if z0[i] else 'nonzero'))
