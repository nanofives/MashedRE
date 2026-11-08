"""PORT-lead survey 2026-11-08: C4 rows x port x matched source x caller/callee port state."""
import csv, json, re, pathlib, collections, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
rows = list(csv.DictReader(open(ROOT/'hooks.csv', encoding='utf-8')))
by = {}
for r in rows:
    try: by[int(r['rva'],16)] = r
    except Exception: pass
src = collections.defaultdict(list)
for p in (ROOT/'re/match').rglob('*.c'):
    for m in re.findall(r'(?<![0-9a-f])(00[4-5][0-9a-f]{5})(?![0-9a-f])', p.name.lower()):
        src[int(m,16)].append(p.relative_to(ROOT).as_posix())
xr = {int(k): v for k, v in json.load(open(ROOT/'log/xref_mashed.json')).items()}
def port(a):
    r = by.get(a); return (r['port'] or 'none') if r else 'nohook'
out = []
for a, r in sorted(by.items()):
    if not r['confidence'].startswith('C4'): continue
    lib = r['subsystem'].startswith('third-party')
    x = xr.get(a, {})
    callers = x.get('callers', []); callees = [c for c in x.get('calls', []) if c in xr]
    cp = collections.Counter(port(c) for c in callers)
    ep = collections.Counter(port(c) for c in callees)
    out.append(dict(rva=f'0x{a:08x}', sub=r['subsystem'], lib=int(lib), port=r['port'] or 'none',
        exe_file=r['exe_file'], match=';'.join(src.get(a, [])[:2]), nsrc=len(src.get(a, [])),
        callers=len(callers), callers_called=cp['called'], callers_body=cp['body'],
        callees=len(callees), callees_none=sum(v for k,v in ep.items() if k in('none','nohook')),
        callees_lib=sum(1 for c in callees if by.get(c) and by[c]['subsystem'].startswith('third-party')),
        name=r['name']))
w = csv.DictWriter(open(ROOT/'verify/portplan_20261108/survey.tsv','w',newline='',encoding='utf-8'), fieldnames=list(out[0]), delimiter='\t')
w.writeheader(); w.writerows(out)
g = [o for o in out if not o['lib']]
print('C4 game', len(g), 'lib', len(out)-len(g))
print('game port', collections.Counter(o['port'] for o in g))
print('game with match src', sum(1 for o in g if o['match']))
def cat(o):
    m = bool(o['match'])
    if o['port']=='called': return 'called/'+('match' if m else 'nomatch')
    if o['port']=='body':
        return 'body/'+('match' if m else 'nomatch')+('/caller-called' if o['callers_called'] else '/no-called-caller')
    k='none/'+('match' if m else 'nomatch')
    if o['callers_called']: k+='/caller-called'
    elif o['callers_body']: k+='/caller-body'
    elif o['callers']==0: k+='/no-static-caller'
    else: k+='/callers-none'
    if o['callees_none']==0: k+='/callees-closed'
    return k
c = collections.Counter(cat(o) for o in g)
for k,v in sorted(c.items()): print(f'{k:60s} {v}')
t = collections.defaultdict(collections.Counter)
for o in g: t[o['sub']][cat(o).split('/callees')[0]]+=1
for s in sorted(t, key=lambda s:-sum(t[s].values())): print(s, dict(t[s]))
