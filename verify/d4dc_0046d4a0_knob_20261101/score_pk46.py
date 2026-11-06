"""Score D4-DC-INC-0046d4a0-KNOB arms (PREREG section 3) from the arms folder. Prints a markdown summary.

usage: py -3.12 score_pk46.py <arms_dir>
"""
import hashlib, json, os, sys

A = sys.argv[1]
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
mt = json.load(open(os.path.join(W, 're', 'orchestrator', 'merge_train.json')))
GREF = mt['gctrl']['ref']; BREF = mt['b']['ref']


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8] if os.path.exists(p) else 'MISSING'


def h(name):
    return [sha8(os.path.join(A, name + s)) for s in ('.step.csv', '.gates.csv', '.ptrace.log')]


def b(stack):
    return [sha8(os.path.join(A, 'B_%s_K%s.step.csv' % (stack, k))) for k in (0, 1, 26)]


res = {}
res['G-DEF'] = (h('H_DEF') == GREF and b('DEF') == BREF, 'H_DEF %s  B_DEF %s' % ('/'.join(h('H_DEF')), '/'.join(b('DEF'))))
res['G-ON'] = (h('H_PK46') == GREF and b('PK46') == BREF, 'H_PK46 %s  B_PK46 %s' % ('/'.join(h('H_PK46')), '/'.join(b('PK46'))))
res['G-WIT-INERT'] = (h('H_PK46W') == h('H_PK46') and h('H_DEFW') == h('H_DEF'),
                      'H_PK46W %s  H_DEFW %s' % ('/'.join(h('H_PK46W')), '/'.join(h('H_DEFW'))))


def rows(name):
    p = os.path.join(A, name + '.pk46.tsv')
    if not os.path.exists(p): return None
    out = []
    for ln in open(p):
        if ln.startswith('#') or not ln.strip(): continue
        c = ln.rstrip('\n').split('\t')
        out.append(dict(n=int(c[0]), knob=int(c[1]), idx=int(c[2]), nret=int(c[3]), nptr=int(c[4], 16),
                        nx=c[5:8], oret=int(c[8]), optr=int(c[9], 16), ox=c[10:13]))
    return out


for name in ('H_PK46W', 'H_DEFW'):
    r = rows(name)
    if r is None:
        print('%s: witness file missing' % name); continue
    bad = 0
    for x in r:
        if not (0 <= x['idx'] < 0x10): continue
        t_ok = (x['nptr'] - 0x00881ec8 - x['idx'] * 0xd04) % 0x40 == 0 and x['nptr'] >= 0x00881ec8 + x['idx'] * 0xd04
        if not (x['nret'] == 1 and t_ok and x['oret'] == 1 and x['optr'] != x['nptr']): bad += 1
    idxs = sorted(set(x['idx'] for x in r))
    nz_naked = sum(1 for x in r if any(v != '00000000' for v in x['nx']))
    nz_one = sum(1 for x in r if any(v != '00000000' for v in x['ox']))
    print('%s: calls=%d idx=%s G-READS bad=%d  naked xyz non-zero rows=%d  one-body xyz non-zero rows=%d'
          % (name, len(r), idxs, bad, nz_naked, nz_one))
    for x in r[:3]:
        print('   n=%d idx=%d naked ptr=%08x xyz=%s | one ptr=%08x xyz=%s' % (x['n'], x['idx'], x['nptr'], '/'.join(x['nx']), x['optr'], '/'.join(x['ox'])))
    if name == 'H_PK46W':
        res['G-EXEC'] = (len(r) >= 1, 'calls=%d' % len(r))
        res['G-READS'] = (len(r) >= 1 and bad == 0, 'bad=%d of %d' % (bad, len(r)))

print('refs: G-CTRL %s  B %s' % ('/'.join(GREF), '/'.join(BREF)))
for k, (ok, d) in res.items():
    print('%-12s %s  %s' % (k, 'PASS' if ok else 'FAIL', d))
