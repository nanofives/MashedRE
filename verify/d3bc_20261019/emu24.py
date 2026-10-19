"""d3bc: FUN_00415e20 on the ORIGINAL's own inputs (aistep own / look, record call k-1 +0x948/+0x950), two acos forms:
acos53 = double acos (CW 0x27f), acos24 = fpatan(sqrt((1-x)(1+x)), x) with every op at float precision (PC_24). Scored against
the stored error, rows with a stored error (ai_mode 0 / 7). usage: emu24.py <orig.msd> <orig.aistep.csv> <v> [rows] ..."""
import csv, os, struct, sys, math
import numpy as np
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'emu15e20.py')).read().split("om, oa, v = sys.argv[1:4]")[0]
G = {'__file__': os.path.abspath(__file__)}; exec(src, G)
m = G['m']; f32 = G['f32']; lut = G['lut_norm_x']; SC = G['SC_EXACT']
F = lambda x: float(np.float32(x))
g = lambda r, o: struct.unpack_from('<f', r, o)[0]
def acos24(d):
    a = F(1 - d); b = F(1 + d); p = F(a * b); s = F(math.sqrt(p)); return F(math.atan2(s, d))
def ang(x, z, acf):
    d = float(lut(x, z, 'f32')); d = max(-1.0, min(1.0, d)); a = F(acf(d) * SC)
    if float(f32(z)) < 0: a = -a
    while a <= 0: a = F(a + 360.0)
    return a
def err(ox, oz, lx, lz, fx, fz, acf):
    A = ang(f32(f32(lx) - f32(ox)), f32(-f32(f32(lz) - f32(oz))), acf); H = ang(f32(fx), f32(-f32(fz)), acf)
    e = F(A - H)
    while e <= 0: e = F(e + 360)
    while e > 360: e = F(e - 360)
    return e
args = sys.argv[1:]
while args:
    om, oa, v, n = args[0], args[1], args[2], int(args[3]); args = args[4:]
    rows = [r for r in csv.DictReader(open(oa)) if r['v'] == v]; i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
    f0 = int(rows[i0]['frame']); O = rows[i0:]; rec = {fi - f0: r for fi, r in m.parse(om)}
    for nm, acf in (('acos53', math.acos), ('acos24', acos24)):
        ok = 0; bad = []
        for k in range(1, min(n, len(O))):
            o = O[k]
            if k - 1 not in rec or '' in (o['hist_dc'], o['hist_d8'], o['own_x'], o['look_x']): continue
            h = F(float(o['hist_dc'])) if float(o['hist_d8']) == 360 else F(float(o['hist_d8']))
            r = rec[k - 1]; e = err(*[float(o[c]) for c in ('own_x', 'own_z', 'look_x', 'look_z')], g(r, 0x948), g(r, 0x950), acf)
            if e == h: ok += 1
            else: bad.append(k)
        print('car', v, nm, 'exact', ok, 'miss', len(bad), bad[:10])
