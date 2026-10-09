"""Leg 2 offline model: the POSITION path of FUN_00445aa0 (type-0 camera entry), flag != 0 arm,
float32, transcribed from leg2_445aa0.asm / leg2_decomp.txt (0x00445aa0..0x00445ea6 translate).
Orientation (+0xac..+0xc8, the +0x4c matrix) is not modelled: nothing on the release path reads it.

usage: py -3.12 leg2_model.py orig            validate on o_cadence_nopress.csv's recorded target
       py -3.12 leg2_model.py port <csv>      predict on a port target trace (columns f,tx,ty,tz)
"""
import csv, sys
import numpy as np
f = np.float32
DT = f(f(50) * f(0.00033333332976326346))          # fild [0x7f1008] * [0x5cc948]

def step(e, tgt, flag, inputs_zero=True):
    """One FUN_00445aa0 call. e: dict px,py,pz,vx,vy,vz,a4,a8. tgt: (x,y,z) = cam +4/+8/+0xc.
    Returns (flag_after, dist_used)."""
    px, py, pz = e['px'], e['py'], e['pz']
    if flag:                                                    # 0x00445ad3
        tx, tz = f(tgt[0]), f(tgt[2])
        dx, dz = f(px - tx), f(pz - tz)
        L = f(np.sqrt(f(dx * dx + dz * dz)))                    # 0x004c3bf0
        if L > f(4.0): L = f(4.0)                               # 0x00445b03 (fcom; >4 -> 4)
        L3 = f(L * f(0.3333333432674408))
        e['a4'] = f(tgt[1])                                     # 0x00445b28 +0xa4 = cam +8
        tx = f(tx + L3); tz = f(L3 + tz)
    else:
        raise NotImplementedError("flag == 0 arm (FUN_00441c80 car midpoint) not modelled")
    h = f(py - e['a4'])                                         # 0x00445b5c
    if h < f(0.05): e['vy'] = f(f(DT * f(3.0)) + e['vy'])
    if h > f(0.05): e['vy'] = f(e['vy'] - f(DT * f(3.0)))
    one_m = f(f(1.0) - DT)
    k = one_m
    if h < f(1.0) and h > f(-1.0): k = f(f(1.0) - f(DT + DT))
    e['vy'] = f(k * e['vy'])
    dx, dz = f(px - tx), f(pz - tz)
    dist = f(np.sqrt(f(dx * dx + dz * dz)))                     # 0x004c3bf0 (after 0x004c3c60)
    nx, nz = (f(dx / dist), f(dz / dist)) if dist != 0 else (f(0), f(0))   # 0x004c3c60
    released = False
    if e['a8'] == 0:
        p2 = f(0.0)
        if dist < f(0.019999999552965164):                      # site (a), _DAT_005ce18c
            flag = 0; released = True
        if not inputs_zero:                                      # site (b): 0x007f1042 + k*0x4c
            flag = 0
        g = f(6.0)
        f6 = f(dist - p2)
        if flag and f6 < f(0.75): f6 = f(0.75)
        pdist = dist if dist >= f(0.0) else f(0.0)
        f9 = f(0.0)
        if f(0.0) <= f6:
            f9 = f6 if not (f(5.0) < f6) else f(5.0)
        s = f(f(f(f9 * DT) * g) * f(0.2))
        e['vx'] = f(e['vx'] - f(nx * s)); e['vz'] = f(e['vz'] - f(nz * s))
    else:
        raise NotImplementedError("+0xa8 != 0 arm (LAB_00445d74) not reached with flag set")
    k2 = one_m
    if pdist < f(10.0) and e['a8'] == 0: k2 = f(f(1.0) - f(DT + DT))
    e['vx'] = f(k2 * e['vx']); e['vz'] = f(k2 * e['vz'])
    e['px'] = f(e['px'] + f(e['vx'] * DT)); e['py'] = f(e['py'] + f(e['vy'] * DT))
    e['pz'] = f(e['pz'] + f(e['vz'] * DT))                      # 0x004c51a0 (combine 2)
    return flag, dist, released

def run(targets, first_double=True):
    """targets: list per phase-3 frame of (x,y,z) cam +4..+0xc as read by that frame's step(s).
    Returns per-frame (frame, px, pz, dist, flag) and the release frame."""
    e = dict(px=f(1.5), py=f(9.7), pz=f(34.5), vx=f(0), vy=f(0), vz=f(0), a4=f(12.0), a8=0)
    flag, out = 1, []
    for i, t in enumerate(targets):
        n = 2 if (i == 0 and first_double) else 1
        for _ in range(n):
            flag, dist, rel = step(e, t, flag)
            if rel: break
        out.append((i, float(e['px']), float(e['pz']), float(dist), flag))
        if flag == 0: return out, i
    return out, None

if __name__ == '__main__':
    if sys.argv[1] == 'orig':
        rows = [r for r in csv.DictReader(open('o_cadence_nopress.csv')) if r['substate'] == '3']
        # one row per frame expected; t step 1/60 -> frame index from t
        t0 = float(rows[1]['t'])
        tg = {}
        for r in rows[1:]:
            fr = round((float(r['t']) - t0) * 60)
            tg[fr] = (float(r['tgt_fe4']), float(r['tgt_fe8']), float(r['tgt_fec']))
        F = max(tg) + 1
        seq, last = [], None
        for i in range(F):
            last = tg.get(i, last); seq.append(last)
        out, rel = run(seq)
        print('orig samples', len(rows), 'frames', F, 'model release frame', rel)
        obs = {round((float(r['t']) - t0) * 60): (float(r['e0_x']), float(r['e0_z'])) for r in rows[1:]}
        for k in (60, 120, 180, 240, 300, 360, 420, 480, 540, 600):
            if k < len(out) and k in obs:
                m = out[k]; o = obs[k]
                print('  f%4d model (%.4f, %.4f) obs (%.4f, %.4f)' % (k, m[1], m[2], o[0], o[1]))
        print('  orig: release seen at frame', F - 1, '(last phase-3 sample)')
