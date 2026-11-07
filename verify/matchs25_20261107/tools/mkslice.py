import csv
def v(s):
    n=int(s,16); return n+0x400000 if n<0x400000 else n
lo,hi=0x005a5910,0x005ac980
libid={v(x['rva']):x for x in csv.DictReader(open('verify/matchscope_20261031/libidentity_functions.tsv'),delimiter='\t')}
d={}
for x in csv.DictReader(open('hooks.csv',encoding='utf-8')):
    if x['rva'].startswith('#'): continue
    d.setdefault(v(x['rva']),x)
out=[]
for a in sorted(d):
    if not lo<=a<=hi: continue
    x=d[a]
    if x['subsystem'].startswith('third-party'): continue
    l=libid.get(a)
    if l and l['status'].startswith('identical'): continue
    out.append((a,x))
D='D:/MashedWT/MATCH-S25_data/'
with open(D+'slice_rows.tsv','w') as f:
    f.write('rva\tsub\tconf\texe_file\n')
    for a,x in out: f.write(f"0x{a:08x}\t{x['subsystem']}\t{x['confidence']}\t{x['exe_file']}\n")
open(D+'slice_rvas.txt','w').write(''.join(f'0x{a:08x}\n' for a,_ in out))
print(len(out))
