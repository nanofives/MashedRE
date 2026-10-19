"""d3bc: offline emulation of FUN_00415e20 (0x00415e20) on the ORIGINAL's inputs, scored against the original's own
stored steer error (aistep hist_dc / hist_d8 = 0x008032dc / 0x008032d8). Inputs per aistep row k: own = row own_x/own_z
(the +0x928 matrix position row), look = row look_x/look_z, forward = the original record of call k-1, +0x948/+0x950
(the `at` row; FUN_0046d510 = (0,0,1) through the vectors slot). Variants of the arithmetic are scored by exact matches.
LUT = Math/RwLutTables.cpp's generator (fitted 4096/4096 on the live tables).
usage: emu15e20.py <orig.msd> <orig.aistep.csv> <v> [rows]"""
import csv, math, os, struct, sys
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f32 = np.float32
def fb(x): return struct.unpack('<I', struct.pack('<f', float(x)))[0]
def bf(b): return struct.unpack('<f', struct.pack('<I', b & 0xffffffff))[0]
SQ, IV = [], []
for i in range(0x1000):
    xb = ((0x7f if i & 0x800 else 0x80) << 23) | ((i & 0x7ff) << 12)
    x = f32(bf(xb)); s = f32(np.sqrt(x)); r = f32(f32(1.0) / s)
    SQ.append((fb(s) - (((xb + 0x800) >> 1) & 0x3fc00000)) & 0xffffffff)
    IV.append((fb(r) - ((~(xb + 0x800) >> 1) & 0x3fc00000)) & 0xffffffff)
def lut_norm_x(x, z, sum_mode):
    x, z = f32(x), f32(z)
    if sum_mode == 'f32': s = f32(f32(f32(x * x) + f32(0)) + f32(z * z))
    else: s = f32(float(x) * float(x) + float(z) * float(z))
    b = fb(s)
    if b == 0: return x
    bi = (b + 0x800) & 0xffffffff
    sc = f32(bf((IV[(bi >> 12) & 0xfff] + ((~bi >> 1) & 0x3fc00000)) & 0xffffffff))
    return f32(sc * x)
SC_EXACT = struct.unpack('<d', bytes.fromhex('000000205ca54c40'[::-1][::-1]))[0]
SC_EXACT = struct.unpack('<d', (0x404ca5dc20000000).to_bytes(8, 'little'))[0]
def ang(x, z, V):
    d = lut_norm_x(x, z, V['sum']) if V['lut'] else f32(float(x) / math.hypot(float(x), float(z)))
    d = float(d)
    if d < -1.0: d = -1.0
    if d > 1.0: d = 1.0
    a = math.acos(d) * V['sc']
    a = float(f32(a)) if V['r24'] else a
    if float(f32(z)) < 0: a = -a
    while a <= 0: a = float(f32(a + 360.0)) if V['r24'] else a + 360.0
    return a
def err(ox, oz, lx, lz, fx, fz, V):
    a = float(f32(ang(f32(f32(lx) - f32(ox)), f32(-f32(f32(lz) - f32(oz))), V)))     # param_2 = (float) at 0x00415f13 / fst
    h = ang(f32(fx), f32(-f32(fz)), V)
    e = a - h; e = float(f32(e)) if V['r24'] else e
    while e <= 0: e = float(f32(e + 360.0)) if V['r24'] else e + 360.0
    while e > 360: e = float(f32(e - 360.0)) if V['r24'] else e - 360.0
    return float(f32(e))                                                                # FST dword 0x004165f7 / 0x0041670c
om, oa, v = sys.argv[1:4]; n = int(sys.argv[4]) if len(sys.argv) > 4 else 100
rows = [r for r in csv.DictReader(open(oa, newline='')) if r['v'] == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); f0 = int(rows[i0]['frame']); O = rows[i0:]
rec = {fi - f0: r for fi, r in m.parse(om)}
g = lambda r, o: struct.unpack_from('<f', r, o)[0]
VARS = {'port(exactsqrt,dec-scale)': dict(lut=False, sum='f32', sc=57.2957802, r24=True),
        'LUT f32sum dec-scale r24': dict(lut=True, sum='f32', sc=57.2957802, r24=True),
        'LUT f32sum exact-scale r24': dict(lut=True, sum='f32', sc=SC_EXACT, r24=True),
        'LUT f64sum exact-scale r24': dict(lut=True, sum='f64', sc=SC_EXACT, r24=True),
        'LUT f32sum exact-scale r53': dict(lut=True, sum='f32', sc=SC_EXACT, r24=False)}
hits = {k: [] for k in VARS}
for k in range(1, min(n, len(O))):
    r = rec.get(k - 1)
    if r is None: continue
    o = O[k]; h = float(f32(float(o['hist_dc']))) if float(o['hist_d8']) == 360 else float(f32(float(o['hist_d8'])))
    ins = [float(o[c]) for c in ('own_x', 'own_z', 'look_x', 'look_z')] + [g(r, 0x948), g(r, 0x950)]
    for name, V in VARS.items(): hits[name].append((k, err(*ins, V) - h))
for name, L in hits.items():
    ex = sum(1 for k, d in L if d == 0)
    bad = [(k, d) for k, d in L if d != 0]
    print('%-30s exact %d/%d  first misses %s' % (name, ex, len(L), ' '.join('%d:%+.1e' % x for x in bad[:6])))
