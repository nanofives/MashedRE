"""d2lateral (PREREG amendment 1, A1.6): replay 0x0046ef70's VELOCITY path, transcribed from the port's
mashedmod/src/mashed_re/Collision/ContactFixup.cpp (lines cited), on a fixup-probe row's own inputs, in float32
(numpy) with the x87 double chains of :170-174 / :205 held in float64, and compare with the measured output (the
next row's velocity: a retry's 0x0046f6c0 entry, A9 writes no velocity).
Also prints each input-sensitivity: the output when ONE port-side input is substituted (m, +0x9e4, depth n/a,
arm, normal), so the share of each differing input in the output gap is read, not assumed.
usage: py -3.12 fixup_replay.py orig <out.msd>            (first fixup with |vel| > 500)
       py -3.12 fixup_replay.py port <DFX.wc.log>          (first line; port inputs from the wc log)
"""
import csv, math, re, sys
import numpy as np
F = np.float32


def mag(v):
    return F(math.sqrt(float(v[0]) ** 2 + float(v[1]) ** 2 + float(v[2]) ** 2))


def norm(v):
    m = mag(v)
    return [F(v[i] / m) for i in range(3)] if m != 0 else [F(0)] * 3


def fixup_vel(vel, slots, sp, states, cfc, key0, gnd4, ax):
    """slots: list of (i, depth, n[3], scale, arm[3], m). Returns (vel_out, damp, local_74, local_54)."""
    c8 = sum(1 for s in states if s != 0)                                      # :98-101
    l54 = [F(0)] * 3; l60 = [F(0)] * 3; l74 = F(1)
    for (i, depth, n, sc_, arm, m) in slots:
        n = [F(x) for x in n]; m = F(m)
        l34 = m                                                                # :122
        if i < 4:
            l34 = l34 * F(-1)
            if n[1] < F(0.9): l34 = l34 * F(4)                                 # :125 (pfVar7[0] = n.y)
        elif c8 == 4: l34 = l34 * F(-1)                                         # :127
        else: l34 = l34 * F(-0.5)                                               # :129
        f2 = F(m / F(sp))                                                      # :133
        if f2 < 0:
            f2 = -f2
            if F(1) < f2: f2 = F(1)
            l74 = F(1) - f2                                                    # :137
        if cfc != 0 and l34 < F(500): l34 = F(500)                              # :140
        l34 = l34 * F(0.75)                                                    # :141
        l3c = [l34 * n[0], l34 * n[1], l34 * n[2]]
        if cfc == 0: s = F(90)
        else: s = F(500)
        l48v, l44v, l40v = n[0] * s, n[1] * s, n[2] * s                        # :149-158
        l54 = [l54[0] + (l48v + l3c[0]), l54[1] + (l44v + l3c[1]), l54[2] + (l40v + l3c[2])]
        l60 = [l60[0] + (l48v + l3c[0]), l60[1] + (l44v + l3c[1]), l60[2] + (l40v + l3c[2])]
        if 3 < i:                                                              # :167
            a24 = norm([F(x) for x in arm])
            dotv = float(a24[0]) * float(n[0]) + float(a24[1]) * float(n[1]) + float(a24[2]) * float(n[2])
            f9 = float(mag(l3c)) * dotv
            lc = [F(float(a24[k]) * f9) for k in range(3)]
            l54 = [l54[k] + lc[k] for k in range(3)]                           # :196-198
    inv = F(1) / F(len(slots))                                                 # :217 (+0x9ec = live count)
    l54 = [x * inv for x in l54]
    v0 = F(vel[0]); v = [l54[0] + v0, l54[1] + F(vel[1]), l54[2] + F(vel[2])]  # :263-266
    damp = None
    if key0 != -2:                                                             # :292
        damp = l74 * F(3)
        if F(0.9) < damp: damp = F(0.9)
        v = [v[0] * damp, v[1] * damp, v[2] * damp]                            # :312-314
    if gnd4:                                                                   # :319
        sp2 = mag(v)
        if F(1e-4) < sp2:
            d = norm(v)
            k = (d[2] * F(ax[2]) + d[0] * F(ax[0]) + d[1] * F(ax[1])) * F(0.1) * sp2
            v = [v[0] - k * F(ax[0]), v[1] - k * F(ax[1]), v[2] - k * F(ax[2])]
    return v, damp, l74, l54


def orig_case(msd):
    rows = list(csv.DictReader(open(msd + '.fixupprobe.csv')))
    i = next(i for i, r in enumerate(rows) if r['site'] == '0'
             and math.sqrt(sum(float(r[k]) ** 2 for k in ('velx', 'vely', 'velz'))) > 500)
    r, nx = rows[i], rows[i + 1]
    slots = []
    for s in r['slots_all'].split('|'):
        q = s.split(':')
        slots.append((int(q[0]), float(q[1]), [float(x) for x in q[2:5]], float(q[5]), [float(x) for x in q[6:9]], float(q[9])))
    case = dict(vel=[float(r[k]) for k in ('velx', 'vely', 'velz')], slots=slots, sp=float(r['speed']),
                states=[int(r['st%d' % w]) for w in range(4)], cfc=int(r['icfc']), key0=int(r['key0']),
                gnd4=float(r['gnd']) == 4.0, ax=[float(r[k]) for k in ('ax9c8', 'ax9cc', 'ax9d0')])
    out = [float(nx[k]) for k in ('velx', 'vely', 'velz')]
    return case, out, f"seq {r['seq']} -> next site {nx['site']} seq {nx['seq']}"


def port_case(wc, fixlog=None):
    line = open(wc).readline()
    m = re.search(r'vel=\(([^)]*)\)->\(([^)]*)\)', line)
    vin = [float(x) for x in m.group(1).split(',')]; vout = [float(x) for x in m.group(2).split(',')]
    slots = []
    for s in re.finditer(r'\| s(\d+) d=(\S+) n=\(([^)]*)\) m=(\S+) arm=\(([^)]*)\) sl=(\S+)', line):
        slots.append((int(s.group(1)), float(s.group(2)), [float(x) for x in s.group(3).split(',')], float(s.group(6)),
                      [float(x) for x in s.group(5).split(',')], float(s.group(4))))
    return dict(vel=vin, slots=slots, sp=None, states=[1, 1, 1, 1], cfc=0, key0=-1, gnd4=True, ax=[0.0, 1.0, 0.0]), vout


if __name__ == '__main__':
    if sys.argv[1] == 'orig':
        case, out, where = orig_case(sys.argv[2])
        print('ORIGINAL', where)
        print('  inputs: vel', case['vel'], 'sp(+0x9e4)', case['sp'], 'states', case['states'], 'cfc', case['cfc'],
              'key0', case['key0'], 'gnd4', case['gnd4'], 'ax', case['ax'])
        for s in case['slots']: print('  slot', s)
        v, damp, l74, l54 = fixup_vel(**case)
        print(f'  replay  out=({v[0]:.4f},{v[1]:.4f},{v[2]:.4f}) |v|={mag(v):.4f}  damp={damp} l74={l74} l54={[float(x) for x in l54]}')
        print(f'  measured out=({out[0]:.4f},{out[1]:.4f},{out[2]:.4f}) |v|={mag(out):.4f}')
        rel = max(abs(float(v[k]) - out[k]) for k in range(3)) / float(mag(out))
        print(f'  max|replay - measured| / |measured| = {rel:.3e}')
    else:
        print('port mode: use the DFX fixup log + wc log (see RESULT); not automated')
