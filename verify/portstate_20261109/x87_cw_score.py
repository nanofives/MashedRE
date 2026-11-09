"""x87_cw_score.py <cw json> [...] -> per-RVA CW table (stdout, TSV): key, then per window n7f/n27f/nother/last.
PORT-X87-PC24-AUDIT arm 1. Windows are differences of consecutive snapshots (boot = the first snapshot)."""
import json, sys
from collections import defaultdict

agg = defaultdict(lambda: defaultdict(lambda: [0, 0, 0, 0]))
wins = []
for f in sys.argv[1:]:
    d = json.load(open(f))
    prev = None
    for s in d['snaps']:
        tag = s['tag']
        if d['target'] == 'exe':
            tag = 'run'
        if tag not in wins:
            wins.append(tag)
        for i, key in enumerate(d['targets']):
            c = s['c'][i]
            p = prev[i] if prev else [0, 0, 0, 0]
            w = agg[key][tag]
            if d['target'] == 'exe':
                continue
            for j in range(3):
                w[j] += c[j] - p[j]
            if c[2] - p[2]:
                w[3] = c[3]
        prev = s['c']
    if d['target'] == 'exe':
        last = d['snaps'][-1]['c']
        for i, key in enumerate(d['targets']):
            w = agg[key]['run']
            for j in range(3):
                w[j] += last[i][j]
            if last[i][2]:
                w[3] = last[i][3]
    for i, key in enumerate(d['targets']):
        if d.get('attach') and d['attach'][i] != 1:
            agg[key]['attach_fail'] = [0, 0, 0, 0]
print('key\t' + '\t'.join(wins) + '\tverdict')
for key in sorted(agg):
    cols, cws = [], set()
    for w in wins:
        v = agg[key].get(w, [0, 0, 0, 0])
        cols.append('%d/%d/%d%s' % (v[0], v[1], v[2], ('/last=0x%04x' % v[3]) if v[2] else ''))
        if v[0]: cws.add('0x007f')
        if v[1]: cws.add('0x027f')
        if v[2]: cws.add('other:0x%04x' % v[3])
    verdict = 'ATTACH FAIL' if 'attach_fail' in agg[key] else (' + '.join(sorted(cws)) if cws else 'NOT REACHED')
    print(key + '\t' + '\t'.join(cols) + '\t' + verdict)
