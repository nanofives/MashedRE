"""x87_emu_score.py <emu json>... -> per-RVA arm-3 verdicts (stdout TSV). PORT-X87-PC24-AUDIT, PREREG section 4.
Columns: scored calls, refused calls (reasons), natural deltas (O53 != O), then per variant GREEN n / RED bad/n.
S_f32: S compared with ST0 rounded to f32 (the caller's fstp m32) instead of the raw 80-bit ST0."""
import json, sys
from collections import defaultdict

E = defaultdict(lambda: {'n': 0, 'ref': 0, 'why': defaultdict(int), 'nat': 0, 'v': defaultdict(lambda: [0, 0]),
                         'vf': defaultdict(lambda: [0, 0]), 'cw': defaultdict(int), 'ex': {}})
for f in sys.argv[1:]:
    for s in json.load(open(f))['samples']:
        e = E[s['rva']]
        e['cw']['0x%04x' % s['cw']] += 1
        for k, why in s.get('refused', {}).items():
            e['why'][k + ':' + why.split(' @')[0].split(' %')[0][:40]] += 1
        if 'eq' not in s or 'O' not in s['eq']:
            e['ref'] += 1
            continue
        e['n'] += 1
        if s['eq'].get('O53') is False:
            e['nat'] += 1
        for k, v in s['eq'].items():
            e['v'][k][0 if v else 1] += 1
        for k, v in s['eqf'].items():
            e['vf'][k][0 if v else 1] += 1
        for k, d in s.get('diff', {}).items():
            e['ex'].setdefault(k, {'n': s['n'], **d})


def g(p):
    ok, bad = p
    return '-' if ok + bad == 0 else ('GREEN %d' % ok if bad == 0 else 'RED %d/%d' % (bad, ok + bad))


print('rva\tcw\tscored\trefused\tnatural_deltas\tO24\tO53\tX53\tX24\tS\tS_f32\tX53_f32\trefusals')
for r in sorted(E):
    e = E[r]
    print('\t'.join([r, ','.join('%s x%d' % kv for kv in e['cw'].items()), str(e['n']), str(e['ref']), str(e['nat']),
                     g(e['v']['O24']), g(e['v']['O53']), g(e['v']['X53']), g(e['v']['X24']), g(e['v']['S']),
                     g(e['vf']['S']), g(e['vf']['X53']), '; '.join('%s x%d' % kv for kv in e['why'].items())]))
if '-x' in sys.argv[0:1]:
    pass
