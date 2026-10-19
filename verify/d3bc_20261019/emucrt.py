"""d3bc: FUN_00415e20 on the ORIGINAL's own inputs, comparing two acos forms. acos53 = double acos. crt24 = the CRT body
FUN_004a3384 under the caller's PC_24: FUN_004a70a5 loads CW (cw & 0x300) | 0x7f (0x004a70a9..0x004a70b7, the precision bits
kept), so 1+x, 1-x, the product and FSQRT round to float (0x004a33a8..0x004a33b2) and FPATAN (0x004a33b6, not under PC)
is extended; the FMUL qword by 0x005cc970 then rounds the exact product to float once.
usage: emucrt.py <orig.msd> <orig.aistep.csv> <v> [...]"""
import csv, os, struct, sys, math
import numpy as np
from fractions import Fraction as Fr
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'emu15e20.py')).read().split("om, oa, v = sys.argv[1:4]")[0]
G = {'__file__': os.path.abspath(__file__)}; exec(src, G)
m = G['m']; lut = G['lut_norm_x']; f32 = G['f32']; SC = G['SC_EXACT']
F = lambda x: float(np.float32(x)); g = lambda r, o: struct.unpack_from('<f', r, o)[0]
def r24(q):
    x = np.float32(float(q)); best = None
    for y in (np.nextafter(x, np.float32(-np.inf)), x, np.nextafter(x, np.float32(np.inf))):
        key = (abs(Fr(float(y)) - q), int(np.array(y).view(np.uint32)) & 1)
        if best is None or key < best[0]: best = (key, float(y))
    return best[1]
def acos_crt24(d):
    a = F(1.0 + d); b = F(1.0 - d); p = F(a * b); s = F(math.sqrt(p)); return math.atan2(s, d)
def ang(x, z, mode):
    d = float(lut(x, z, 'f32')); d = max(-1.0, min(1.0, d))
    a = r24(Fr(acos_crt24(d)) * Fr(SC)) if mode == 'crt24' else F(math.acos(d) * SC)
    if float(f32(z)) < 0: a = -a
    while a <= 0: a = F(a + 360.0)
    return a
def err(ox, oz, lx, lz, fx, fz, mode):
    A = ang(f32(f32(lx) - f32(ox)), f32(-f32(f32(lz) - f32(oz))), mode); H = ang(f32(fx), f32(-f32(fz)), mode)
    e = F(A - H)
    while e <= 0: e = F(e + 360)
    while e > 360: e = F(e - 360)
    return e
if __name__ == '__main__':
    a = sys.argv[1:]
    while a:
        om, oa, v = a[:3]; a = a[3:]
        rows = [r for r in csv.DictReader(open(oa)) if r['v'] == v]; i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
        f0 = int(rows[i0]['frame']); O = rows[i0:]; rec = {fi - f0: r for fi, r in m.parse(om)}
        for mode in ('acos53', 'crt24'):
            ok = 0; bad = []
            for k in range(1, min(400, len(O))):
                o = O[k]
                if k - 1 not in rec or '' in (o['hist_dc'], o['hist_d8'], o['own_x'], o['look_x']): continue
                h = F(float(o['hist_dc'])) if float(o['hist_d8']) == 360 else F(float(o['hist_d8'])); r = rec[k - 1]
                e = err(*[float(o[c]) for c in ('own_x', 'own_z', 'look_x', 'look_z')], g(r, 0x948), g(r, 0x950), mode)
                if e == h: ok += 1
                else: bad.append(k)
            print(os.path.basename(om), 'car', v, mode, 'exact', ok, '/', ok + len(bad), 'miss', bad[:10])
