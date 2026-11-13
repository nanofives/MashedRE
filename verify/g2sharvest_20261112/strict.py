import csv,sys,subprocess,collections,json,pathlib
S=sys.argv[1]
def load(d):
    out={}
    for r in csv.DictReader(open(f'{S}/{d}/results.tsv'),delimiter='\t'):
        out[r['rva'].lower()]=r
    return out
res={}
for pair in (('g2sall_ddk','g2sall_rw9044'),('g2scr_ddk','g2scr_rw9044')):
    a=load(pair[0]); b=load(pair[1])
    for k in a:
        best=None
        for d,t in ((pair[0],a.get(k)),(pair[1],b.get(k))):
            if t is None: continue
            acc=float(t['acc']) if t['acc'] not in ('',None) else -1
            if best is None or acc>best[1]: best=(d,acc,t['stage'],t['note'])
        res[k]=best
out=[]
for k,(d,acc,stage,note) in sorted(res.items()):
    st='compile-fail' if acc<0 else ('100' if acc>=100 else ('>=95' if acc>=95 else '80-95' if acc>=80 else '50-80' if acc>=50 else '<50'))
    strict=''
    if acc>=100:
        src=f'{S}/{d}/{k[2:]}.c'
        m=subprocess.run([sys.executable,'re/tools/match/match.py',src,'--bytes'],capture_output=True,text=True).stdout
        byteok='BYTE-IDENTICAL' in m
        rc=subprocess.run([sys.executable,'re/tools/match/relocheck.py',src],capture_output=True,text=True).stdout
        relok=all(l.split('\t')[1]=='OK' for l in rc.strip().splitlines() if l.startswith('0x') and l.split('\t')[0].lower()==k) and k in rc.lower()
        strict=('C4-strict' if byteok and relok else ('bytes-only' if byteok else 'not-bytes'))+('' if relok else ' reloc:'+rc.strip().replace('\t',' ')[:140])
    out.append((k,d,acc,st,strict))
with open(S+'/g2s_census.tsv','w') as f:
    f.write('rva\tdir\tacc\tbucket\tstrict\n')
    for o in out: f.write('\t'.join(map(str,o))+'\n')
print(collections.Counter(o[3] for o in out)); print(collections.Counter(o[4].split(' ')[0] for o in out if o[3]=='100'))
