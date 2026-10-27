r"""D4-FX-REBASE: re-baselined FX rate table. Original reps (orig_fx_rebase.py <prefix>.A.json, last snapshot) against
the port's fx_debug.txt. Per car: emits per 1000 active frames (all / alive / dead), per rep, the rep band (min..max)
and mean, and port / mean, port / min, port / max. Active frames = calls[non-skid] + calls[skid] // 3 (the D4-FX-GATES
denominator, verify/fxgates_20261026/fx_score.py); alive = the same on rec+0x4 != 0 calls (fx_split_score.py).
Also prints the skid share of calls and emits per call, the two other fx_score.txt columns.
usage: py -3.12 rebase_score.py <port fx_debug.txt> <rep>.A.json [<rep>.A.json ...]"""
import json, re, sys

def rate(e, a): return 1000.0 * e / a if a else 0.0

port = {}
for ln in open(sys.argv[1], errors='replace'):
    m = re.match(r'\[s(\d)\].*og_frames=(\d+) og_calls=(\d+)/(\d+) og_emits=(\d+)/(\d+) ig=([\d,]+)', ln)
    if m:
        ig = [int(x) for x in m.group(7).split(',')]
        c0, c1, e0, e1 = (int(m.group(i)) for i in (3, 4, 5, 6))
        port[int(m.group(1))] = dict(af=c0 + c1 // 3, em=e0 + e1, aa=ig[4] + ig[14], ae=ig[13], calls=c0 + c1, skc=c1)
reps = []
for path in sys.argv[2:]:
    r = json.load(open(path))['rows'][-1]
    cars = {}
    for c in range(4):
        ca, em = r['calls'][2 * c:2 * c + 2], r['emits'][2 * c:2 * c + 2]
        aca, aem = r['acalls'][2 * c:2 * c + 2], r['aemits'][2 * c:2 * c + 2]
        cars[c] = dict(af=ca[0] + ca[1] // 3, em=sum(em), aa=aca[0] + aca[1] // 3, ae=sum(aem), calls=sum(ca), skc=ca[1])
    reps.append((path.replace('\\', '/').split('/')[-1].replace('.A.json', ''), r['frames'], cars))

print('PORT %s' % sys.argv[1])
print('ORIGINAL reps: ' + ', '.join('%s (%d frames)' % (n, f) for n, f, _ in reps))
for kind, ka, ke in (('all', 'af', 'em'), ('alive', 'aa', 'ae'), ('dead', None, None)):
    print('\n[%s] emits per 1000 active frames (active frames in brackets)' % kind)
    print('  car | port | ' + ' | '.join(n for n, _, _ in reps) + ' | orig min..max (mean) | port/mean | port/min | port/max')
    for c in range(4):
        def get(d):
            if kind == 'dead': return d['af'] - d['aa'], d['em'] - d['ae']
            return d[ka], d[ke]
        pa, pe = get(port[c]); pr = rate(pe, pa)
        orr = [rate(get(cars[c])[1], get(cars[c])[0]) for _, _, cars in reps]
        oa = [get(cars[c])[0] for _, _, cars in reps]
        lo, hi = min(orr), max(orr); mean = sum(orr) / len(orr)
        def q(x): return '%.2f' % (pr / x) if x else '  - '
        print('  %d | %.1f (%d) | %s | %.1f..%.1f (%.1f) | %s | %s | %s' % (c, pr, pa,
              ' | '.join('%.1f (%d)' % (v, a) for v, a in zip(orr, oa)), lo, hi, mean, q(mean), q(lo), q(hi)))
print('\n[calls] skid share of calls / emits per call')
print('  car | port | ' + ' | '.join(n for n, _, _ in reps))
for c in range(4):
    def f(d): return '%.3f / %.3f' % (d['skc'] / d['calls'] if d['calls'] else 0, d['em'] / d['calls'] if d['calls'] else 0)
    print('  %d | %s | %s' % (c, f(port[c]), ' | '.join(f(cars[c]) for _, _, cars in reps)))
