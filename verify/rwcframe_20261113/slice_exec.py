"""RWC-FRAME-492E90: the executed slice of CLOSURE_492E90.tsv: members reachable from 0x00492e90 through members the
INT3 census marks reached_menu=1 only (a member executed at the menu only from callers outside the closure is left out).
usage: py -3.12 verify/rwcframe_20261113/slice_exec.py [menu|race]   writes SLICE_EXEC_<col>.tsv"""
import csv, os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
col = 'reach_menu' if (sys.argv[1:] or ['menu'])[0] == 'menu' else 'reach_race'
rows = list(csv.DictReader(open(os.path.join(HERE, 'CLOSURE_492E90.tsv'), encoding='utf-8'), delimiter='\t'))
by = {r['rva']: r for r in rows}
kids = {}
for r in rows:
    for c in filter(None, r['callers'].split(',')):
        kids.setdefault(c, []).append(r['rva'])
keep, st = [], ['00492e90']
seen = set()
while st:
    a = st.pop(0)
    if a in seen: continue
    seen.add(a)
    r = by[a]
    if r[col] != '1': continue
    keep.append(a)
    st += kids.get(a, [])
with open(os.path.join(HERE, 'SLICE_EXEC_%s.tsv' % col), 'w', encoding='utf-8', newline='\n') as f:
    f.write('\t'.join(rows[0].keys()) + '\n')
    for a in keep: f.write('\t'.join(by[a].values()) + '\n')
g = [by[a] for a in keep if by[a]['depth'] != 'RW']
w = [by[a] for a in keep if by[a]['depth'] == 'RW']
p = lambda r: float(r['pct'] or 0)
print('%s slice: game %d (100%% %d, below %d, no source %d), RW-band %d (100%% %d)' % (col, len(g), sum(p(r) == 100 for r in g),
      sum(r['source'] != '' and p(r) < 100 for r in g), sum(r['source'] == '' for r in g), len(w), sum(p(r) == 100 for r in w)))
for r in g:
    if p(r) < 100: print('  below/none', r['rva'], r['pct'], r['source'], 'callers', r['callers'])
