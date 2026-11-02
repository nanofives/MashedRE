"""Summarise --valsrc-probe (original) and exe_vsprobe.py (exe) outputs into small committed CSVs.

usage: py -3.12 summarize.py <data dir> <out dir> name [name ...]
  o_*: <name>.msd.vs.csv + <name>.msd.vsw.csv     x_*: <name>.vs.csv + <name>.vsw.csv
Writes SUMMARY_<name>.csv (calls grouped by tuple, ret ignored for x_), WATCH_<name>.csv (watch hits grouped by pc and the
dword that changed, with old -> new) and SEQ_<name>.csv (every 0x0063ba8c change in hit order).
"""
import csv, os, sys
from collections import OrderedDict

data, out = sys.argv[1], sys.argv[2]
for name in sys.argv[3:]:
    orig = name.startswith('o_')
    base = os.path.join(data, name + ('.msd' if orig else ''))
    rows = list(csv.DictReader(open(base + '.vs.csv')))
    g = OrderedDict()
    for r in rows:
        if orig:
            k = (r['ret'], r['ba8c'], r['c0'], r['c1'], r['c2'], r['c3'], r['ffc'], r['rt'], r['fd0'])
            t0, t1 = int(r['tick0']), int(r['tick1'])
        else:
            k = (r['kind'], r['ba8c'], r['re7'], r['c0'], r['c1'], r['c2'], r['c3'], r['ffc'], r['rt'], r['fd0'])
            t0, t1 = int(r['call0']), int(r['call1'])
        e = g.setdefault(k, [0, 0, t0, t1])
        e[0] += int(r['ncalls']); e[1] += 1; e[2] = min(e[2], t0); e[3] = max(e[3], t1)
    hdr = (['ret', 'ba8c', 'c0', 'c1', 'c2', 'c3', 'ffc', 'rt', 'fd0'] if orig else
           ['kind', 'ba8c', 're7', 'c0', 'c1', 'c2', 'c3', 'ffc', 'rt', 'fd0']) + ['calls', 'runs',
           'first_tick' if orig else 'first_call', 'last_tick' if orig else 'last_call']
    with open(os.path.join(out, f'SUMMARY_{name}.csv'), 'w', newline='') as f:
        w = csv.writer(f); w.writerow(hdr)
        for k, e in g.items(): w.writerow(list(k) + e)
    hits = list(csv.DictReader(open(base + '.vsw.csv')))
    cols = ['ba8c', 'ffc', 'c0', 'rt']
    prev = None; wg = OrderedDict(); seq = []
    for h in hits:
        cur = [h[c] for c in cols]
        ch = [c for i, c in enumerate(cols) if prev is not None and prev[i] != cur[i]] or ['same']
        pc = h['pc'] if orig else h['pc_va']
        site = pc if orig else h['sym']
        for c in ch:
            old = prev[cols.index(c)] if (prev is not None and c != 'same') else ''
            new = h[c] if c != 'same' else '/'.join(cur)
            key = (site, c, old, new)
            e = wg.setdefault(key, [0, h['tick'] if orig else h['call'], '', h.get('bt', '')])
            e[0] += 1; e[2] = h['tick'] if orig else h['call']
            if c == 'ba8c':
                seq.append([h['hit'], h['tick'] if orig else h['call'], site, old, new])
        prev = cur
    with open(os.path.join(out, f'WATCH_{name}.csv'), 'w', newline='') as f:
        w = csv.writer(f); w.writerow(['site', 'changed', 'old', 'new', 'hits', 'first', 'last', 'bt_first'])
        for k, e in wg.items(): w.writerow(list(k) + e)
    with open(os.path.join(out, f'SEQ_{name}.csv'), 'w', newline='') as f:
        w = csv.writer(f); w.writerow(['hit', 'tick' if orig else 'call', 'site', 'old', 'new']); w.writerows(seq)
    print(name, 'tuples', len(g), 'runs', len(rows), 'hits', len(hits), 'ba8c changes', len(seq))
