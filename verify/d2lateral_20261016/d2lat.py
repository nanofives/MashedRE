"""d2lateral reducer (PREREG_d2lateral.md section 3). Per physics STEP j around the first wall impact, on BOTH sides:

  E   A6a entry 0x00467650            orig: latbracket site 0          port: A6ADUMP act.vel / act.bf / act.gc
  W1  post force integration          orig: RwV3dLength arg @ret 0x004686a9   port: D2SINK tag=w1
  B   A6a exit = clamp #6 output      orig: latbracket site 2 (A6b entry)     port: D2SINK tag=a6a_out
  S1  substep 1 entry 0x004709a0      orig: latbracket site 1 (1st)           port: D2SINK tag=sub_top sub=0
  S2  substep 2 entry                 orig: latbracket site 1 (2nd)           port: D2SINK tag=sub_top sub=1
  E'  next step's A6a entry

Lateral = horizontal signed component on the step's A6a-entry forward axis (cause2.py's split: fwd_h normalised,
lat = vx*fz - vz*fx), so every stage of step j is on one axis; `axis` = L_own(E') - L_j(E') is the body-rotation
term. Clamp #6 is inverted from its own law (0x004687f0..0x0046897b: vel -= k * (vel - dot(fwd,vel)*fwd)), with
fwd read at A6a entry (A6a writes none of +0x9d4..+0x9dc, disasm 0x00467650..0x0046897b this lane):
  k_meas = (W1 - B) . lat3(W1) / |lat3(W1)|^2, resid = |W1 - B - k_meas*lat3| / |W1 - B|.
Original l_60 = sum over wheels with an ld4 row (@ret 0x0046820f, wheel = (EDI - rec - 0x1a4)/0xc4) of
ld4 * min(le4, 1024), le4 = |arg @ret 0x004680fb| for that wheel when present (spin branch) else +0x9e4 at the ld4
call (Integrate2.cpp:425-450). G = l_60 / +0x18c * surface factor * (+0x2c, +0x34 factors) * |W1| (Integrate2.cpp
:644-675). Arm/k predicted with the bytes' constants (knee 32768.0 @0x005ce9fc, 1e7 @0x005ce9f8, 0x33d6bf95
@0x005ce9f4, 2^-15 @0x005ce9f0, LOW floor 0.1, HIGH x0.2).

usage: py -3.12 d2lat.py orig <out.msd>           (reads <out.msd>.latbracket.csv + .magprobe.csv)
       py -3.12 d2lat.py port <a6a.log> <d2sink.log>
  [--pre 2] [--n 16] [--json out.json]
"""
import csv, json, math, re, struct, sys

REC = 0x008815a0
KNEE, HI_C, HI_S = 32768.0, 10000000.0, struct.unpack('<f', struct.pack('<I', 0x33d6bf95))[0]
LO_S = 2.0 ** -15
GND4 = 4.0


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def hsplit(v, f):
    h = math.hypot(f[0], f[2]) or 1.0
    fx, fz = f[0] / h, f[2] / h
    return v[0] * fx + v[2] * fz, v[0] * fz - v[2] * fx


def lat3(v, f):
    d = v[0] * f[0] + v[1] * f[1] + v[2] * f[2]
    return [v[i] - d * f[i] for i in range(3)]


def kinv(w1, b, f):
    if w1 is None or b is None: return None, None, None
    d = [w1[i] - b[i] for i in range(3)]
    nd = math.sqrt(sum(x * x for x in d))
    if nd == 0.0: return 0.0, 0.0, 0.0
    l = lat3(w1, f); ll = sum(x * x for x in l)
    if ll == 0.0: return None, None, nd
    k = sum(d[i] * l[i] for i in range(3)) / ll
    r = math.sqrt(sum((d[i] - k * l[i]) ** 2 for i in range(3))) / nd
    return k, r, nd


def surface(g, tid, d00, m2c, m34):
    # Integrate2.cpp:652-663 (0x004686ad..0x00468707)
    if tid == -0x5f7f80: g *= 1.5
    else:
        dbl = tid == -0x557f80
        if not dbl:
            if tid in (-0x69e1a6, -0xe17f4c): g *= (3.0 if d00 == 0 else 1.5)
            if tid == -0x37e1a6: dbl = True
        if dbl: g = g + g
    if m2c: g = (m2c * 0.01 + 1.0) * g
    if m34: g = (m34 * 0.1 + 1.0) * g
    return g


def kpred(G):
    if G is None: return None, None
    if KNEE < G:
        k = max((HI_C - G) * HI_S, 0.0); return 1, k * 0.1 + k * 0.1
    return 0, max((KNEE - G) * LO_S, 0.1)


# ---------------------------------------------------------------- original
def load_orig(msd):
    lb = list(csv.DictReader(open(msd + '.latbracket.csv')))
    mp = list(csv.DictReader(open(msd + '.magprobe.csv')))
    V = lambda r, a: [float(r[a + 'x']), float(r[a + 'y']), float(r[a + 'z'])]
    steps, bad, cur = [], 0, None
    for r in lb:
        s = int(r['site'])
        if s == 0:
            if cur is not None: steps.append(cur)
            cur = dict(seq=int(r['seq']), E=r, B=None, S=[])
        elif cur is None:
            continue
        elif s == 2:
            if cur['B'] is not None or cur['S']: cur['bad'] = 1
            cur['B'] = r
        elif s == 1:
            cur['S'].append(r)
    if cur is not None: steps.append(cur)
    bymp = {}
    for r in mp:
        if 'lbseq' not in r or r['lbseq'] in (None, ''): continue
        bymp.setdefault(int(r['lbseq']), []).append(r)
    out = []
    for j, st in enumerate(steps):
        E = st['E']
        ok = (st['B'] is not None) and len(st['S']) == 2 and not st.get('bad')
        if not ok: bad += 1
        rows = bymp.get(st['seq'] + 1, [])
        w1 = [r for r in rows if r['site'] == '004686a9']
        ld = {}; le = {}; unk = 0
        for r in rows:
            if r['site'] not in ('0046820f', '004680fb'): continue
            w = (int(r['edi']) - REC - 0x1a4)
            if w % 0xc4 or not 0 <= w // 0xc4 <= 3: unk += 1; continue
            m = math.sqrt(float(r['vx']) ** 2 + float(r['vy']) ** 2 + float(r['vz']) ** 2)
            (ld if r['site'] == '0046820f' else le)[w // 0xc4] = (m, float(r['rec_speed']))
        l60 = 0.0
        for w, (ld4, sp) in ld.items():
            le4 = le[w][0] if w in le else sp
            l60 = ld4 * min(le4, 1024.0) + l60
        W1 = [float(w1[0]['vx']), float(w1[0]['vy']), float(w1[0]['vz'])] if len(w1) == 1 else None
        ka = None
        if len(w1) == 1:
            ka = (float(w1[0]['vx']) == float(w1[0]['rec_velx']) and float(w1[0]['vz']) == float(w1[0]['rec_velz']))
        g = None
        if W1 is not None:
            spd = f32(math.sqrt(sum(x * x for x in W1)))
            g = surface(l60 / float(w1[0]['rec_18c']), int(E['i1f0']), int(E['id00']), int(E['i2c']), int(E['i34'])) * spd
        out.append(dict(
            j=j, ok=ok, E=V(E, 'vel'), f=V(E, 'fwd'), gnd=float(E['gnd']),
            st=[int(E['st%d' % w]) for w in range(4)], pm=[int(E['pm%d' % w]) for w in range(4)],
            tid=int(E['i1f0']), b20=int(E['ib20']), W1=W1, ka=ka, nW1=len(w1),
            B=V(st['B'], 'vel') if st['B'] is not None else None,
            S1=V(st['S'][0], 'vel') if len(st['S']) > 0 else None, S2=V(st['S'][1], 'vel') if len(st['S']) > 1 else None,
            fS1=V(st['S'][0], 'fwd') if len(st['S']) > 0 else None, fS2=V(st['S'][1], 'fwd') if len(st['S']) > 1 else None,
            nld=len(ld), nle=len(le), unk=unk, l60=l60 if ld else 0.0, G=g, port=None))
    return out, bad


# ---------------------------------------------------------------- port
NUM = r'(-?[0-9.]+(?:e[-+]?\d+)?|-?nan\S*|-?inf)'


def load_port(a6a, sink):
    acts = []
    for line in open(a6a, errors='replace'):
        if not line.startswith('f='): continue
        g = lambda k: re.search(r'\b' + re.escape(k) + r'=([^ |]+)', line)
        vec = lambda k: [float(x) for x in g(k).group(1).split(',')]
        acts.append(dict(E=vec('act.vel'), f=vec('act.bf'), gnd=float(g('act.gc').group(1)),
                         l60=float(g('act.l60').group(1)), G=float(g('act.grip').group(1)),
                         k=float(g('act.kvel').group(1)), arm=int(g('act.arm').group(1)),
                         clamp=int(g('act.clamp').group(1)), speed=float(g('act.speed').group(1)),
                         nld=len(re.findall(r'\|w\d fired=1', line)),
                         pm3=[int(x) for x in re.findall(r's\.pm3=(-?\d+)', line)]))
    frames = {}
    for line in open(sink, errors='replace'):
        m = re.match(r'f=(-?\d+) tag=(\S+) sub=(-?\d+) pass=(-?\d+) v=\(([^)]*)\) mag=(\S+) r9e4=(\S+) r9e0=(\S+)', line)
        if not m: continue
        fr = frames.setdefault(int(m.group(1)), {})
        v = [float(x) for x in m.group(5).split(',')]
        tag, sub = m.group(2), int(m.group(3))
        key = tag if tag != 'sub_top' else 'sub_top%d' % sub
        fr.setdefault(key, v)
        if tag == 'w1': fr['w1mag'] = float(m.group(6))
    # pair act rows to sink frames by the known answer act.speed == |w1| (both are A6a's own post-W1 |v|)
    fk = sorted(frames)
    off = None
    for o in range(-3, 4):
        hits = sum(1 for i, a in enumerate(acts[:400]) if (i + o) in frames and 'w1mag' in frames[i + o]
                   and abs(frames[i + o]['w1mag'] - a['speed']) <= 1e-4 * max(1.0, a['speed']))
        if off is None or hits > off[1]: off = (o, hits)
    o = off[0]
    out, miss = [], 0
    for i, a in enumerate(acts):
        fr = frames.get(i + o, {})
        if 'w1mag' in fr and abs(fr['w1mag'] - a['speed']) > 1e-4 * max(1.0, a['speed']): miss += 1
        out.append(dict(j=i, ok=('w1' in fr and 'a6a_out' in fr and 'sub_top0' in fr and 'sub_top1' in fr),
                        E=a['E'], f=a['f'], gnd=a['gnd'], st=a['pm3'], pm=None, tid=None, b20=None,
                        W1=fr.get('w1'), ka=None, nW1=1 if 'w1' in fr else 0, B=fr.get('a6a_out'),
                        S1=fr.get('sub_top0'), S2=fr.get('sub_top1'), fS1=None, fS2=None,
                        nld=a['nld'], nle=None, unk=0, l60=a['l60'], G=a['G'] if a['clamp'] else None,
                        port=dict(k=a['k'], arm=a['arm'], clamp=a['clamp'], a6bout=fr.get('a6b_out'))))
    return out, dict(offset=o, paired=off[1], ka_miss=miss, acts=len(acts), sink_frames=len(frames))


# ---------------------------------------------------------------- table
def impact(steps):
    for s in range(1, len(steps)):
        a, b = steps[s - 1]['E'], steps[s]['E']
        sa, sb = math.hypot(a[0], a[2]), math.hypot(b[0], b[2])
        if sa > 500 and sb < 0.5 * sa: return s - 1          # step j0 = the one whose end shows the drop
    return None


def table(steps, pre, n, label):
    j0 = impact(steps)
    print(f'== {label}: impact step j0={j0}' + ('' if j0 is None else
          f" (|v_h| {math.hypot(steps[j0]['E'][0], steps[j0]['E'][2]):.1f} -> {math.hypot(steps[j0 + 1]['E'][0], steps[j0 + 1]['E'][2]):.1f})"))
    if j0 is None: return []
    print('  +k   sp_E   lat_E  ret   | dInt   dClamp  dPost   dS1     dS2     axis  | gnd  st      nld  l60      G         '
          'arm_p k_p    k_meas  resid  [port: arm k]')
    rows = []
    for j in range(max(0, j0 - pre), min(len(steps) - 1, j0 + n + 1)):
        s, nx = steps[j], steps[j + 1]
        f = s['f']
        L = lambda v: hsplit(v, f)[1] if v is not None else float('nan')
        lE, lW, lB, l1, l2, lN = L(s['E']), L(s['W1']), L(s['B']), L(s['S1']), L(s['S2']), L(nx['E'])
        lNown = hsplit(nx['E'], nx['f'])[1]
        ret = abs(lNown) / abs(hsplit(s['E'], f)[1]) if abs(lE) > 1e-6 else float('nan')
        km, kr, nd = kinv(s['W1'], s['B'], f)
        ap, kp = kpred(s['G'])
        pk = s['port']
        row = dict(k=j - j0, j=j, sp=math.hypot(s['E'][0], s['E'][2]), latE=lE, ret=ret, dInt=lW - lE, dClamp=lB - lW,
                   dPost=l1 - lB, dS1=l2 - l1, dS2=lN - l2, axis=lNown - lN, gnd=s['gnd'], st=s['st'], nld=s['nld'],
                   l60=s['l60'], G=s['G'], arm_p=ap, k_p=kp, k_meas=km, resid=kr, ok=s['ok'], ka=s['ka'],
                   port_arm=pk['arm'] if pk else None, port_k=pk['k'] if pk else None)
        rows.append(row)
        fm = lambda x, w=7, p=2: ('%*.*f' % (w, p, x)) if isinstance(x, float) and not math.isnan(x) else ' ' * (w - 2) + 'NA'
        print(f"  {row['k']:+3d} {fm(row['sp'],6,1)} {fm(lE,7,2)} {fm(ret,5,3)} | {fm(row['dInt'])} {fm(row['dClamp'])} "
              f"{fm(row['dPost'])} {fm(row['dS1'])} {fm(row['dS2'])} {fm(row['axis'],6,2)}| {s['gnd']:.1f}  "
              f"{''.join(str(x) for x in (s['st'] or []))[:4]:4s}  {s['nld']}   {fm(s['l60'],8,2)} {fm(s['G'],9,1)} "
              f"{'NA' if ap is None else ap}     {fm(kp,6,4)} {fm(km,7,4)} {fm(kr,6,4)}"
              + (f"  [{pk['arm']} {pk['k']:.4f}]" if pk else '') + ('' if s['ok'] else '  !pattern'))
    return rows


if __name__ == '__main__':
    a = sys.argv[1:]
    pre = int(a[a.index('--pre') + 1]) if '--pre' in a else 2
    n = int(a[a.index('--n') + 1]) if '--n' in a else 16
    js = a[a.index('--json') + 1] if '--json' in a else None
    if a[0] == 'orig':
        steps, bad = load_orig(a[1])
        nka = [s['ka'] for s in steps if s['ka'] is not None]
        print(f'orig steps={len(steps)} pattern_bad={bad} W1 rows/step: {sorted(set(s["nW1"] for s in steps))} '
              f'KA-1 (W1 arg == record +0x9b0/+0x9b8) {sum(nka)}/{len(nka)}  wheel-unresolved rows {sum(s["unk"] for s in steps)}')
        rows = table(steps, pre, n, 'ORIGINAL ' + a[1])
    else:
        steps, info = load_port(a[1], a[2])
        print(f'port {info}')
        rows = table(steps, pre, n, 'PORT ' + a[1])
    if js: json.dump(rows, open(js, 'w'), indent=1)
