"""u9759: the port's tri-normal arithmetic (Collision/WheelContactSolver.cpp, MASHED_UP_TRINORMAL; FUN_0046c5f0) in float32 on
a batch-probe row's wheel points (a = wheel 0, b = wheel 3, c = wheel 2), with the generated RW sqrt LUT.
usage: trinorm.py <batchprobe.csv> <Q-row index> [...]"""
import csv, struct, sys
import numpy as np
f32 = np.float32
def fb(u): return struct.unpack('<f', struct.pack('<I', u & 0xffffffff))[0]
def bf(x): return struct.unpack('<I', struct.pack('<f', float(x)))[0]
T = []
for i in range(0x1000):
    xb = ((0x7f if i & 0x800 else 0x80) << 23) | ((i & 0x7ff) << 12)
    s = np.sqrt(f32(fb(xb)))
    T.append((bf(s) - (((xb + 0x800) >> 1) & 0x3fc00000)) & 0xffffffff)
def lutsqrt(x):
    b = bf(x)
    if b == 0: return f32(0)
    bi = (b + 0x800) & 0xffffffff
    return f32(fb((T[(bi >> 12) & 0xfff] + ((bi >> 1) & 0x3fc00000)) & 0xffffffff))
h = lambda s: f32(struct.unpack('<f', bytes.fromhex(s)[::-1])[0])
rows = [r for r in csv.DictReader(open(sys.argv[1])) if r['kind'] == 'Q']
for k in map(int, sys.argv[2:]):
    r = rows[k]; p = [h(x) for x in r['wpos'].split(';')[:12]]; up = [h(x) for x in r['vel'].split(';')[:3]]
    a, b, c = p[0:3], p[9:12], p[6:9]
    f2 = (a[2]-b[2])*(a[1]-c[1]) - (a[1]-b[1])*(a[2]-c[2])
    f3 = (a[0]-b[0])*(a[2]-c[2]) - (a[2]-b[2])*(a[0]-c[0])
    f1 = (a[1]-b[1])*(a[0]-c[0]) - (a[0]-b[0])*(a[1]-c[1])
    l2 = f32(f32(f32(f2*f2) + f32(f3*f3)) + f32(f1*f1)); l = lutsqrt(l2); inv = f32(f32(1)/l)
    port = [f32(f2*inv), f32(f3*inv), f32(inv*f1)]
    print(k, 'orig', ['%08x' % bf(x) for x in up], 'port-formula', ['%08x' % bf(x) for x in port], 'f3=%r l2=%08x l=%08x' % (float(f3), bf(l2), bf(l)))
