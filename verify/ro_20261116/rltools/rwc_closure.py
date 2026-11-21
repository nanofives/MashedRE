"""Closure of an RW core body through the RwCore TU bindings (#define FUN_<rva> <symbol>).
usage: py -3.12 rwc_closure.py <worktree> <root_rva> [<root_rva> ...] [--tsv out.tsv]
Kind of each node: from verify/rwccorerun_20261111/RwCore_Bodies.tsv when listed, else from the bound symbol prefix.
Also prints hooks.csv status and owner-list membership (log/orch/run53_bindlists)."""
import os, re, sys, csv, glob
wt = sys.argv[1]
args = [a for a in sys.argv[2:] if not a.startswith('--')]
out = sys.argv[sys.argv.index('--tsv') + 1] if '--tsv' in sys.argv else None
roots = [a.lower().replace('0x', '') for a in args if a != out]
rc = os.path.join(wt, 'mashedmod', 'src', 'mashed_re', 'RwCore')
tu = {}
for f in glob.glob(os.path.join(rc, 'RwCore_*.cpp')):
    m = re.search(r'RwCore_([0-9a-f]{8})\.cpp$', f)
    if m: tu[m.group(1)] = f
kinds = {}
with open(os.path.join(wt, 'verify', 'rwccorerun_20261111', 'RwCore_Bodies.tsv'), encoding='utf-8') as fh:
    for r in csv.DictReader(fh, delimiter='\t'):
        kinds[r['rva'].lower().replace('0x', '')] = (r['kind'], r.get('best_pct', ''), r.get('source', ''))
hooks = {}
with open(os.path.join(wt, 'hooks.csv'), encoding='utf-8', errors='replace') as fh:
    for row in csv.reader(fh):
        if row and re.fullmatch(r'[0-9a-f]{8}', row[0]):
            hooks[row[0]] = (row[3] if len(row) > 3 else '', row[-1] if row else '')
lists = {}
bl = os.path.join(os.path.dirname(os.path.dirname(wt)), 'log', 'orch', 'run53_bindlists')
for name in ('race_owned.txt', 'audio_owned.txt', 'bulk_cp1.tsv'):
    p = os.path.join(bl, name)
    if os.path.exists(p):
        lists[name] = set(re.findall(r'(?:0x)?([0-9a-fA-F]{8})', open(p, encoding='utf-8', errors='replace').read().lower()))
def edges(rva):
    f = tu.get(rva)
    if not f: return []
    res = []
    for m in re.finditer(r'#define\s+FUN_([0-9a-f]{8})\s+(\w+)', open(f, encoding='utf-8', errors='replace').read()):
        if m.group(1) != rva: res.append((m.group(1), m.group(2)))
    return res
seen, sym, order = {}, {}, []
stack = [(r, 'RwCore_' + r) for r in roots]
while stack:
    r, s = stack.pop()
    if r in seen: continue
    seen[r] = True; sym[r] = s; order.append(r)
    if s.startswith('RwCore_') and r in tu:
        stack.extend(edges(r))
rows = []
for r in sorted(order):
    s = sym[r]
    k = kinds.get(r, ('', '', ''))
    if s.startswith('RwCoreMissing_'): kind = 'missing-stop'
    elif s.startswith('RwCoreLift_'): kind = 'lifted'
    elif s.startswith('RwCore_') and r in tu: kind = k[0] or 'rwcore'
    elif s.startswith('RwCore'): kind = 'rwcore-other:' + s.split('_')[0]
    else: kind = 'exe:' + s
    own = ','.join(n.split('.')[0] for n, v in lists.items() if r in v)
    rows.append((r, s, kind, k[1], hooks.get(r, ('', ''))[0], own))
from collections import Counter
print('nodes', len(rows))
print('by kind', dict(Counter(x[2] if not x[2].startswith('exe:') else 'exe-binding' for x in rows)))
print('hooks status', dict(Counter(x[4] for x in rows)))
print('owned elsewhere', [(x[0], x[5]) for x in rows if x[5] and x[5] != 'race_owned'])
if out:
    with open(out, 'w', encoding='utf-8', newline='') as fh:
        fh.write('rva\tbound_symbol\tkind\tbest_pct\thooks_status\towner_lists\n')
        for x in rows: fh.write('\t'.join(x) + '\n')
