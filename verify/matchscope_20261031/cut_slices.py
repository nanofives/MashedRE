import csv,os,math
def v(s):
    n=int(s,16)
    return n+0x400000 if n<0x400000 else n
hk=[x for x in csv.DictReader(open('hooks.csv',encoding='utf-8')) if not x['rva'].startswith('#')]
libid={v(x['rva']):x for x in csv.DictReader(open('verify/matchscope_20261031/libidentity_functions.tsv'),delimiter='\t')}
d={}
for x in hk: d.setdefault(v(x['rva']),x)
rows=sorted(d.items(),key=lambda t:t[0])
print('rows',len(hk),'unique',len(d))
def isl(a,x):
    if x['subsystem'].startswith('third-party'): return True
    l=libid.get(a); return bool(l and l['status'].startswith('identical'))
seq=[(a,isl(a,x),x) for a,x in rows]
print('game',sum(1 for s in seq if not s[1]),'lib',sum(1 for s in seq if s[1]))
BIG=40
blocks=[];cur=[];run=0
for a,l,x in seq:
    if not l:
        if run>=BIG and cur: blocks.append(cur);cur=[]
        run=0;cur.append((a,x))
    else: run+=1
if cur: blocks.append(cur)
slices=[]
for b in blocks:
    n=len(b);k=max(1,round(n/125));sz=n/k
    for i in range(k):
        slices.append(b[round(i*sz):round((i+1)*sz)])

def merge(sl):
    out=[]
    for x in sl:
        if out and (len(x)<30 or len(out[-1])<30) and len(out[-1])+len(x)<=150: out[-1]=out[-1]+x
        else: out.append(x)
    return out
slices=merge(slices)

out=['id\trva_first\trva_last\tgame_fns\tlib_fns_inside\thooked\tc2plus\tnotes']
import bisect
for i,s in enumerate(slices,1):
    f,l=s[0][0],s[-1][0]
    inside=sum(1 for a,li,_ in seq if li and f<a<l)
    hooked=sum(1 for a,x in s if x['status'].strip()=='hooked')
    c2=sum(1 for a,x in s if x['confidence'] in ('C2','C3','C4'))
    out.append(f"MATCH-S{i:02d}\t0x{f:08x}\t0x{l:08x}\t{len(s)}\t{inside}\t{hooked}\t{c2}\t")
open('verify/matchscope_20261031/slices.tsv','w').write('\n'.join(out)+'\n')
print('\n'.join(out)); print(len(slices),sum(len(s) for s in slices))
import collections;print(collections.Counter(x['status'] for _,x in rows).most_common(8))
