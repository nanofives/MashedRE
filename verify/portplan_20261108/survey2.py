import csv,collections,re,pathlib,sys
R=pathlib.Path(__file__).resolve().parents[2]
rows=list(csv.DictReader(open(R/'verify/portplan_20261108/survey.tsv',encoding='utf-8'),delimiter='\t'))
cache={}
def txt(f):
    p=R/f
    if p not in cache:
        try: cache[p]=p.read_text(encoding='utf-8',errors='replace').lower()
        except Exception: cache[p]=None
    return cache[p]
def kind(o):
    fs=[f.strip() for f in o['exe_file'].split('|') if f.strip()]
    if not fs: return 'nobody'
    a=o['rva'][2:]; ks=set()
    for f in fs:
        t=txt(f)
        ks.add('M' if t and re.search(r're/match/[^"\n]*'+a,t) else 'H')
    return {frozenset('M'):'matched',frozenset('H'):'handport',frozenset('MH'):'dual'}[frozenset(ks)]
c=collections.Counter(); groups=collections.defaultdict(list)
for o in rows:
    if o['lib']=='1': continue
    o['kind']=kind(o)
    m='match' if o['match'] else 'nomatch'
    cc='cc' if int(o['callers_called']) else ('cb' if int(o['callers_body']) else '-')
    k=(o['port'],o['kind'],m,cc); c[k]+=1; groups[k].append(o)
for k,v in sorted(c.items()): print(k,v)
w=csv.DictWriter(open(R/'verify/portplan_20261108/survey_game.tsv','w',newline='',encoding='utf-8'),fieldnames=list(rows[0].keys()),delimiter='\t')
w.writeheader(); w.writerows([o for o in rows if o['lib']=='0'])
