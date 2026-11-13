"""RO-CONTROL-DRIFT: over every pair of RO-MENU-REPLAY checkpoint-0 dumps (one private root, both sides of the
2026-11-12 12:24 reboot), is block 0x00773c00 equal? Split by dword 0x00773d60 (kernel32 proc pointer) and 0x006147d8.
    py -3.12 verify/rocontroldrift_20261113/menu_replay_pairs.py   (reads CRTWORDS.tsv)"""
import csv,itertools,collections
LO=0x5ea000
rows=[r for r in csv.DictReader(open('verify/rocontroldrift_20261113/CRTWORDS.tsv',encoding='utf-8'),delimiter='\t') if 'RO-MENU-REPLAY' in r['dump']]
def blk(f):
    with open(f,'rb') as h:
        h.seek(0x773c00-LO); return h.read(1024)
res=collections.Counter()
for a,b in itertools.combinations(rows,2):
    same_k=a['dword_0x00773d60']==b['dword_0x00773d60']; same_t=a['dword_0x006147d8']==b['dword_0x006147d8']
    eq=blk(a['dump'])==blk(b['dump'])
    res[(('k same' if same_k else 'k differs'),('toggle same' if same_t else 'toggle differs'),('block equal' if eq else 'block differs'))]+=1
for k,v in sorted(res.items()): print(v,k)
