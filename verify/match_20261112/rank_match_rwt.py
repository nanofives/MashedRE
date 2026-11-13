import csv, sys, collections
R = 'C:/Users/maria/Desktop/Proyectos/Mashed/'
S = sys.argv[1]
E = R + 'log/orch/rw_closure_review_20261112/'
comp = {r['rva'].lower(): r for r in csv.DictReader(open(E + 'composition_1066.tsv'), delimiter='\t')}
bodies = {r['rva'].lower(): r for r in csv.DictReader(open(S + '/RwCore_Bodies.tsv'), delimiter='\t')}
reach = {}
for r in csv.DictReader(open(R + 're/coverage/reach.tsv'), delimiter='\t'):
    reach[int(r['rva'], 16)] = r
mf = {}
for r in csv.DictReader(open(R + 'verify/equivcheck_20261112/measure_final.tsv'), delimiter='\t'):
    mf[r['rva'].lower()] = r
g2s = collections.defaultdict(list)
for r in csv.DictReader(open(E + 'g2s_census_712.tsv'), delimiter='\t'):
    g2s[r['rva'].lower()].append(r)
eqg = {}
for l in open(E + 'equiv_g2s_executed_rw_64.txt'):
    p = l.rstrip('\n').split('\t')
    if len(p) > 1:
        eqg[p[0].lower()] = p[1]
inv = {}
try:
    for r in csv.DictReader(open(R + 're/analysis/function_inventory.csv')):
        k = [c for c in r if c.lower() in ('rva', 'address', 'entry')]
        if k:
            try: inv[int(r[k[0]], 16)] = r.get('body_size') or ''
            except: pass
except Exception as e:
    print('inv', e, file=sys.stderr)

def isrw(c):
    g = c['group']
    return g.startswith('RW (') or 'RW core' in g or 'RW plugins' in g or 'd3dx/RW' in g
rows = []
for rva, c in comp.items():
    if c['exec'] != 'executed' or not isrw(c):
        continue
    a = int(rva, 16)
    k = c['kind']
    best = ''; src = ''; cls = None; detail = ''
    if k == 'provisional':
        m = mf.get(rva)
        if not m or not m['verdict'].startswith('NOT-PROVEN'):
            continue
        cls = 'prov-NOT-PROVEN'; best = m['best_pct']; src = m['source']; detail = m['kind']
    elif k in ('lift', 'missing'):
        gs = g2s.get(rva, [])
        if not gs:
            continue
        accs = [float(g['acc']) for g in gs if g['acc'] not in ('', '-1')]
        strict = any(g['strict'] for g in gs)
        if strict:
            continue
        if not accs:
            cls = 'g2s-compile-fail'; best = '-1'
        else:
            v = eqg.get(rva, '')
            if not v.startswith('NOT-PROVEN') or rva == '0x004c3d90':
                continue
            cls = 'g2s-NOT-PROVEN'
            bg = max(gs, key=lambda g: float(g['acc']))
            best = '%.1f' % float(bg['acc']); src = 'g2s:' + bg['dir']
        detail = k
    else:
        continue
    rr = reach.get(a, {})
    race = 'y' if (rr.get('reached_race_load') == '1' or rr.get('reached_race_run') == '1' or rr.get('reached_race') == '1') else 'n'
    stage = 'race_run' if rr.get('reached_race_run') == '1' else 'race_load' if rr.get('reached_race_load') == '1' else 'menu'
    size = c['sz'] if c['sz'] not in ('', 'None') else (c['size'] or inv.get(a, ''))
    rows.append(dict(rva=rva, size=size, executed='y', race=race, stage=stage, best=float(best) if best not in ('','-') else -2, cls=cls, src=src, detail=detail, label=c['subsystem']))
rows.sort(key=lambda r: (0 if r['race'] == 'y' else 1, -r['best']))
def band(b):
    if b >= 95: return 'A95'
    if b >= 80: return 'A80'
    if b >= 50: return 'B50'
    if b >= 0: return 'B<50'
    return 'B-cf'
cnt = collections.Counter(r['cls'] for r in rows)
print(len(rows), cnt, file=sys.stderr)
print('rank\trva\tsize\texecuted\trace_path\tstage\tbest_pct\tclass\tsource\tband\tequiv_kind')
for i, r in enumerate(rows, 1):
    print('%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s' % (i, r['rva'], r['size'], r['executed'], r['race'], r['stage'], ('%.1f' % r['best']) if r['best'] >= 0 else 'compile-fail', r['cls'], r['src'] or '-', band(r['best']), r['detail']))
