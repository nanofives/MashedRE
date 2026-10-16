"""d2lateral per-wheel l_60 split (PREREG_d2lateral.md 4.2 H-K). For steps j0-1..j0+N on both sides prints, per wheel,
ld4 (|arg| @ret 0x0046820f / port a.ld4), le4 (post-cap: |arg| @ret 0x004680fb when the spin branch ran, else +0x9e4;
port a.le4) and ld4*le4, plus |W1| and G. Wheel identity on the original from EDI (wheel block = rec + 0x1a4 + w*0xc4).
usage: py -3.12 d2lat_wheels.py <orig.msd> <port a6a.log> [<port d2sink.log>] [--n 9]"""
import csv, math, re, sys
sys.path.insert(0, __import__('os').path.dirname(__import__('os').path.abspath(__file__)))
import d2lat

a = sys.argv[1:]
N = int(a[a.index('--n') + 1]) if '--n' in a else 9
msd, a6a = a[0], a[1]


def orig_wheels(msd):
    steps, _ = d2lat.load_orig(msd)
    lb = list(csv.DictReader(open(msd + '.latbracket.csv')))
    seq0 = [int(r['seq']) for r in lb if r['site'] == '0']
    mp = {}
    for r in csv.DictReader(open(msd + '.magprobe.csv')):
        if r.get('lbseq'): mp.setdefault(int(r['lbseq']), []).append(r)
    out = []
    for j, s in enumerate(steps):
        rows = mp.get(seq0[j] + 1, [])
        w = {}
        for r in rows:
            if r['site'] not in ('0046820f', '004680fb'): continue
            k = (int(r['edi']) - d2lat.REC - 0x1a4) // 0xc4
            m = math.sqrt(float(r['vx']) ** 2 + float(r['vy']) ** 2 + float(r['vz']) ** 2)
            w.setdefault(k, {})['ld4' if r['site'] == '0046820f' else 'le4raw'] = m
            if r['site'] == '0046820f': w[k]['sp'] = float(r['rec_speed'])
        for k, d in w.items():
            d['spin'] = 'le4raw' in d
            d['le4'] = min(d.get('le4raw', d.get('sp', float('nan'))), 1024.0)
        out.append(w)
    return steps, out


def port_wheels(a6a):
    out = []
    for line in open(a6a, errors='replace'):
        if not line.startswith('f='): continue
        w = {}
        for m in re.finditer(r'\|w(\d) fired=(\d) spin=(\d).*? a\.le4raw=(\S+) a\.le4=(\S+) .*? a\.ld4=(\S+)', line):
            if m.group(2) == '1':
                w[int(m.group(1))] = dict(spin=m.group(3) == '1', le4raw=float(m.group(4)), le4=float(m.group(5)),
                                          ld4=float(m.group(6)))
        out.append(w)
    return out


ost, ow = orig_wheels(msd)
pw = port_wheels(a6a)
pst = None
if len(a) > 2 and not a[2].startswith('--'):
    pst, _ = d2lat.load_port(a6a, a[2])
oj0 = d2lat.impact(ost)
pj0 = d2lat.impact(pst) if pst else None
print(f'orig j0={oj0}  port j0={pj0}')
for k in range(-1, N + 1):
    oj, pj = oj0 + k, (pj0 + k if pj0 is not None else None)
    o = ow[oj]; p = pw[pj] if pj is not None else {}
    so = ost[oj]; sp = pst[pj] if pst else None
    wo = math.sqrt(sum(x * x for x in so['W1'])) if so['W1'] else float('nan')
    wp = math.sqrt(sum(x * x for x in sp['W1'])) if sp and sp['W1'] else float('nan')
    print(f"+{k:<2d} |W1| orig {wo:8.2f} port {wp:8.2f}  l60 orig {so['l60']:8.2f} port {sp['l60'] if sp else float('nan'):8.2f}"
          f"  G orig {so['G'] or 0:10.1f} port {(sp['G'] or 0) if sp else float('nan'):10.1f}")
    for w in range(4):
        x, y = o.get(w, {}), p.get(w, {})
        f = lambda d, kk: d.get(kk, float('nan'))
        print(f"     w{w} ld4 {f(x,'ld4'):.5f} / {f(y,'ld4'):.5f}   le4 {f(x,'le4'):8.2f}{'s' if x.get('spin') else ' '} / "
              f"{f(y,'le4'):8.2f}{'s' if y.get('spin') else ' '}   prod {f(x,'ld4')*f(x,'le4'):8.2f} / {f(y,'ld4')*f(y,'le4'):8.2f}")
