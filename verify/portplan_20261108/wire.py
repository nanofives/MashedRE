import csv,json,collections,re,pathlib
R=pathlib.Path(__file__).resolve().parents[2]
hooks={}
for r in csv.DictReader(open(R/'hooks.csv',encoding='utf-8')):
    try: hooks[int(r['rva'],16)]=r
    except: pass
g=list(csv.DictReader(open(R/'verify/portplan_20261108/survey_game.tsv',encoding='utf-8'),delimiter='\t'))
xr={int(k):v for k,v in json.load(open(R/'log/xref_mashed.json')).items()}
stat=collections.Counter(); ex=collections.defaultdict(list); n=0
for o in g:
    if o['port'] not in('none','body') or not o['match'] or o['callers_called']=='0': continue
    n+=1
    a=int(o['rva'],16); h=f'{a:08x}'
    for c in xr[a]['callers']:
        r=hooks.get(c)
        if not r or (r['port'] or '')!='called': continue
        how=set()
        for f in [f for f in r['exe_file'].split('|') if f]:
            p=R/f
            t=p.read_text(encoding='utf-8',errors='replace').lower() if p.exists() else ''
            m=re.search(r'#define\s+fun_'+h+r'\b([^\n]*)',t)
            if m: how.add('define-abs' if '0x'+h in m.group(1) else 'define-name')
            elif '0x'+h in t: how.add('abs-literal')
            elif 'fun_'+h in t: how.add('fun-name')
            else: how.add('absent')
        k=(o['port'],'+'.join(sorted(how))); stat[k]+=1; ex[k].append((o['rva'],f'0x{c:08x}',r['exe_file'][:90]))
print('frontier rows',n)
for k,v in stat.most_common(): print(k,v, ex[k][:2])
