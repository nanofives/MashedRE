"""Union of the three parked x87 pools, minus C4 rows and the MATCH-RACEPATH-NEAR list."""
import csv, os, sys
root = os.path.dirname(os.path.abspath(__file__))
repo = os.path.abspath(os.path.join(root, '..', '..', '..'))
V = os.path.join(repo, 'verify')
near = set('0x00488e70 0x0040aef0 0x0048aa50 0x00411ae0 0x0048ab70 0x0040ca00 0x004128f0 0x0040be50'.split())
conf = {}
for r in csv.DictReader((l for l in open(os.path.join(repo, 'hooks.csv'), encoding='utf-8') if not l.startswith('#')), ):
    conf['0x' + r['rva'].lower()] = r['confidence']
# C4 on orch/integration (newer than this base): git show orch/integration:hooks.csv
for l in open(os.path.join(root, 'c4_integration.txt')):
    conf[l.strip()] = 'C4'
# rows already worked with the levers in another MATCH-RACEPATH slice (X87B auto rows stay in scope)
racepath = {}
for l in open(os.path.join(root, 'racepath_worked.tsv')):
    rva, d = l.split()
    if d != 'x87b':
        racepath[rva] = d
def rd(p):
    return list(csv.DictReader(open(p, encoding='utf-8'), delimiter='\t'))
cand = {}
# C4R-NEARMISS: every non-closed row (miss type not recorded there)
for r in rd(os.path.join(V, 'c4rnearmiss_20261107', 'results.tsv')):
    if r['verdict'] != 'IDENTICAL':
        cand.setdefault(r['rva'].lower(), (r['source'], '?', 'c4rnearmiss'))
# MATCH-NEARMISS-2 POOL: miss_type x87 (none of these were worked in pilot/A/B)
worked = set()
for s in ('pilot', 'slice_A', 'slice_B'):
    for r in rd(os.path.join(V, 'matchnearmiss2_20261109', s, 'results.tsv')):
        if r['verdict'] == 'CLOSED':
            worked.add(r['rva'].lower())
for r in rd(os.path.join(V, 'matchnearmiss2_20261109', 'POOL.tsv')):
    if r['miss_type'] == 'x87' and r['rva'].lower() not in worked:
        cand[r['rva'].lower()] = (r['source'], 'x87', 'matchnearmiss2')
# X87B auto x87
for r in rd(os.path.join(V, 'matchracepath_20261109', 'x87b', 'results.tsv')):
    if r['worked'] == 'auto' and r['miss_type'] == 'x87':
        cand[r['rva'].lower()] = (r['source'], 'x87', 'x87b-auto')
out = csv.writer(open(os.path.join(root, 'pool_all.tsv'), 'w', newline='', encoding='utf-8'), delimiter='\t')
out.writerow(['rva', 'source', 'miss_type_prior', 'source_pool', 'conf'])
b = csv.writer(open(os.path.join(root, 'pool_dryrun_in.tsv'), 'w', newline='', encoding='utf-8'), delimiter='\t')
b.writerow(['rva', 'route', 'evidence'])
n = 0
for rva, (src, mt, pool) in sorted(cand.items()):
    c = conf.get(rva, '?')
    if rva in near or c == 'C4':
        continue
    if rva in racepath and pool != 'x87b-auto':
        print('skip, worked in race-path slice', racepath[rva], rva); continue
    if not os.path.exists(os.path.join(repo, src)):
        print('missing source', rva, src); continue
    out.writerow([rva, src, mt, pool, c]); b.writerow([rva, 'match', src]); n += 1
print(n, 'candidates of', len(cand))
