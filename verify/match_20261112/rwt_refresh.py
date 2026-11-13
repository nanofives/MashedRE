import csv, sys, collections
R = 'C:/Users/maria/Desktop/Proyectos/Mashed/'
W = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWT-REFRESH/'
E = R + 'log/orch/rw_closure_review_20261112/'
def isrw(g): return g.startswith('RW (') or 'RW core' in g or 'RW plugins' in g or 'd3dx/RW' in g
comp = {r['rva'].lower(): r for r in csv.DictReader(open(E+'composition_1066.tsv'), delimiter='\t')}
hk = {}
for r in csv.DictReader(open(W+'hooks.csv', newline='')):
    try: hk[int(r['rva'],16)] = r
    except: pass
adm = {int(r['rva'],16) for r in csv.DictReader((l for l in open(W+'re/match/ADMITTED.tsv') if not l.startswith('#')), delimiter='\t')}
prov = {int(r['rva'],16): r['status'] for r in csv.DictReader((l for l in open(W+'re/match/PROVISIONAL.tsv') if not l.startswith('#')), delimiter='\t')}
base = None
old = {r['rva'].lower(): r for r in csv.DictReader(open(W+'verify/match_20261112/RANKED_MATCH-RWT.tsv'), delimiter='\t')}
base = set(old) | {k for k, c in comp.items() if c['exec']=='executed' and isrw(c['group']) and c['kind']=='hand'} | {'0x004c7a70'}
out = []
for k in sorted(base):
    a = int(k,16); h = hk.get(a, {})
    conf = h.get('confidence','')
    st = 'C4' if conf == 'C4' else ('admitted' if a in adm else 'open')
    out.append((k, comp[k]['kind'], conf, st, 'prov:'+prov[a] if a in prov else ''))
c = collections.Counter(o[3] for o in out)
print(len(out), c, file=sys.stderr)
w = csv.writer(open(W+'verify/match_20261112/RWT_TAIL_CENSUS.tsv','w',newline=''), delimiter='\t')
w.writerow(['rva','kind','hooks_conf','state','provisional']); w.writerows(out)
# ranked: old open rows not now C4/admitted
rows = []
for k, r in old.items():
    s = [o for o in out if o[0]==k]
    if s and s[0][3] == 'open': rows.append(r)
rows = [r for r in rows]
rows.sort(key=lambda r:(0 if r['race_path']=='y' else 1, -(float(r['best_pct']) if r['best_pct']!='compile-fail' else -2)))
w = csv.writer(open(W+'verify/match_20261112/RANKED_MATCH-RWT-REFRESH.tsv','w',newline=''), delimiter='\t')
w.writerow(['rank','rva','size','executed','race_path','stage','best_pct','class','source','band','equiv_kind','hooks_conf'])
cf = {o[0]:o[2] for o in out}
for i, r in enumerate(rows,1):
    w.writerow([i,r['rva'],r['size'],r['executed'],r['race_path'],r['stage'],r['best_pct'],r['class'],r['source'],r['band'],r['equiv_kind'],cf.get(r['rva'],'')])
print('ranked', len(rows), file=sys.stderr)
print('open in census but not in old ranked:', [o[0] for o in out if o[3]=='open' and o[0] not in old], file=sys.stderr)
