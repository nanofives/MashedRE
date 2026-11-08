import csv,collections,pathlib,re,json
R=pathlib.Path(__file__).resolve().parents[2]
g=list(csv.DictReader(open(R/'verify/portplan_20261108/survey_game.tsv',encoding='utf-8'),delimiter='\t'))
hooks={}
for r in csv.DictReader(open(R/'hooks.csv',encoding='utf-8')):
    try: hooks[int(r['rva'],16)]=r
    except: pass
cache={}
def kind(o):
    fs=[f for f in o['exe_file'].split('|') if f]
    if not fs: return 'nobody'
    ks=set()
    for f in fs:
        if f not in cache:
            p=R/f; cache[f]=p.read_text(encoding='utf-8',errors='replace').lower() if p.exists() else ''
        ks.add('M' if re.search(r're/match/[^"\n]*'+o['rva'][2:],cache[f]) else 'H')
    return 'dual' if len(ks)==2 else ('matched' if ks=={'M'} else 'handport')
out=[]
for o in g:
    o['kind']=kind(o)
    if not o['match']: continue
    if o['port']=='called' and o['kind'] in('handport','dual'): lane='A-swap-called'
    elif o['port']=='body' and o['kind']=='dual': lane='A-dual'
    elif o['port'] in('none','body') and o['callers_called']!='0': lane='B-frontier'
    else: continue
    tu=sorted({f.split('/mashed_re/')[-1].split('/')[0] for f in o['exe_file'].split('|') if f}) or ['(new)']
    hn=hooks[int(o['rva'],16)]['notes'].lower()
    flag='NOTSAFE' if 'not-safe' in hn or 'not safe' in hn else ''
    out.append(dict(lane=lane,rva=o['rva'],sub=o['sub'],port=o['port'],kind=o['kind'],tu_dir='+'.join(tu),exe_file=o['exe_file'],match=o['match'],flag=flag,name=o['name']))
w=csv.DictWriter(open(R/'verify/portplan_20261108/lanes.tsv','w',newline='',encoding='utf-8'),fieldnames=list(out[0]),delimiter='\t')
w.writeheader(); w.writerows(sorted(out,key=lambda x:(x['lane'],x['sub'],x['rva'])))
c=collections.Counter((o['lane'],o['sub']) for o in out)
for k,v in sorted(c.items()): print(k,v)
print('notsafe',sum(1 for o in out if o['flag']))
print(collections.Counter((o['lane'],o['tu_dir']) for o in out if o['lane']=='A-swap-called').most_common(30))
