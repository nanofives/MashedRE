import csv,json,collections,pathlib
R=pathlib.Path(__file__).resolve().parents[2]
hooks={}
for r in csv.DictReader(open(R/'hooks.csv',encoding='utf-8')):
    try: hooks[int(r['rva'],16)]=r
    except: pass
g={int(o['rva'],16):o for o in csv.DictReader(open(R/'verify/portplan_20261108/survey_game.tsv',encoding='utf-8'),delimiter='\t')}
xr={int(k):v for k,v in json.load(open(R/'log/xref_mashed.json')).items()}
called={a for a,r in hooks.items() if (r['port'] or '')=='called'}
def cls(a):
    r=hooks.get(a)
    if r is None: return 'nohook'
    s=r['subsystem']
    if s.startswith('third-party'): return 'lib:'+s[len('third-party-library['):-1]
    if not r['confidence'].startswith('C4'): return 'game-'+(r['confidence'] or 'C?')
    return 'game-C4'
seen=set(called); q=collections.deque(called); reach=set(); depth={a:0 for a in called}
blockers=collections.Counter(); blk_by=collections.defaultdict(set)
while q:
    a=q.popleft()
    for c in xr.get(a,{}).get('calls',[]):
        if c not in xr or c in seen: continue
        k=cls(c)
        if k=='game-C4' and g.get(c,{}).get('match'):
            seen.add(c); reach.add(c); depth[c]=depth[a]+1; q.append(c)
        else:
            blockers[k]+=1; blk_by[k].add(c)
print('called seeds',len(called),'reachable C4-match game rows not yet called',len(reach))
print('by port',collections.Counter((hooks[a]['port'] or 'none') for a in reach))
print('by depth',sorted(collections.Counter(depth[a] for a in reach).items()))
print('by subsys',collections.Counter(hooks[a]['subsystem'] for a in reach).most_common())
print('blocker callee classes (distinct)',{k:len(v) for k,v in blk_by.items()})
# whole original from WinMain
# subtree gated by each blocker: C4-game rows reachable from blocker not in seen
allgame={a for a in g if g[a]['port'] in ('none','body')}
def sub(b):
    s=set(); q=collections.deque([b]); v={b}
    while q:
        a=q.popleft()
        for c in xr.get(a,{}).get('calls',[]):
            if c in v or c not in xr: continue
            v.add(c); q.append(c)
            if c in allgame and c not in seen: s.add(c)
    return s
rows=[]
for k,bs in blk_by.items():
    for b in bs:
        s=sub(b); rows.append((len(s),f'0x{b:08x}',k,hooks.get(b,{}).get('name','?') if b in hooks else '?'))
rows.sort(reverse=True)
for r in rows[:25]: print(r)
union=set()
for k,bs in blk_by.items():
    for b in bs: union|=sub(b)
print('gated by blockers (union)',len(union))
# roots: functions with no callers in xr
roots=[a for a,v in xr.items() if not v.get('callers')]
left=allgame-seen-union
print('remaining not under exe-called tree',len(left))
# for those, top ancestors
anc=collections.Counter()
for a in left:
    # walk up to a root
    v={a}; q=collections.deque([a]); tops=set()
    while q:
        x=q.popleft(); cs=xr.get(x,{}).get('callers',[])
        if not cs: tops.add(x)
        for c in cs:
            if c not in v: v.add(c); q.append(c)
    for t in tops: anc[t]+=1
for t,n in anc.most_common(15): print(f'root 0x{t:08x}',n,cls(t),hooks.get(t,{}).get('name','?') if t in hooks else '?', hooks.get(t,{}).get('port','') if t in hooks else '')
