"""u9759: recompute FUN_0046f6c0's four wheel points (scratch = mount * 0.2778f through the [+0x9ac] half, 0x004c3670
order ((x*m0 + y*m4) + z*m8) + m12) from a record dump, to compare with a batch-probe / batch-dump row.
usage: xform.py <msd> <record index or 'first'>"""
import sys, struct
import numpy as np
sys.path.insert(0, r'C:\Users\maria\Desktop\Proyectos\Mashed\re\tools\statediff')
import msd_fields as m
f32 = np.float32
recs = list(m.parse(sys.argv[1]))
if sys.argv[2] == 'first':
    k = next(i for i, (fi, r) in enumerate(recs) if any(r[0x968:0x9a8]))
else:
    k = int(sys.argv[2])
fi, r = recs[k]
F = lambda o: f32(struct.unpack_from('<f', r, o)[0])
sel = struct.unpack_from('<i', r, 0x9ac)[0]
M = [F(0x928 + sel * 0x40 + 4 * i) for i in range(16)]
print('record', k, 'frame', fi, 'sel9ac', sel, 'rows', ['%08x' % struct.unpack_from('<I', r, 0x928 + sel*0x40 + 4*i)[0] for i in range(16)])
K = f32(struct.unpack('<f', bytes.fromhex('3e8e3bcd')[::-1])[0])   # _DAT_005cea60 0.2778f
for w in range(4):
    x, y, z = f32(F(0x16c + w * 0xc4) * K), f32(0), f32(F(0x174 + w * 0xc4) * K)
    d = [f32(f32(f32(f32(x * M[c]) + f32(y * M[4 + c])) + f32(z * M[8 + c])) + M[12 + c]) for c in range(3)]
    print(' w%d' % w, ['%08x' % struct.unpack('<I', struct.pack('<f', float(v)))[0] for v in d])
