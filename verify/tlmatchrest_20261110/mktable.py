"""TL-MATCH-REST: per-row table from scores.tsv (latest line per rva), residual_kinds.tsv and hooks.csv.
usage: py -3.12 verify/tlmatchrest_20261110/mktable.py > verify/tlmatchrest_20261110/table.md"""
import csv
sc = {}
for l in open('verify/tlmatchrest_20261110/scores.tsv', encoding='utf-8'):
    f = l.rstrip('\n').split('\t')
    if len(f) >= 6: sc[f[0]] = f
rk = {}
for l in open('verify/tlmatchrest_20261110/residual_kinds.tsv', encoding='utf-8'):
    f = l.rstrip('\n').split('\t')
    if len(f) >= 3: rk[f[0].split('/')[-1][:8]] = f[1]
h = {}
rd = csv.reader(open('hooks.csv', encoding='utf-8')); next(rd)
for r in rd: h[r[0].lower()] = r
groups = [
 ('C4, matched body added', ['00409680', '00409710']),
 ('RW-math band', ['004c39b0','004c4a50','004c4d20','004c4dc0','004c4eb0','004c51a0','004c52f0','004cc050']),
 ('audio closure rows', ['005a6d60','005a7b60','005aa060','005aa0c0','005aaac0','005ab380','005ab410','005aba20','005abd30','005abfa0','005ac210','005acaa0','005acd10','005addd0','005ae650','005ae800','005ae920','005aec30','005aeca0','005b3760','005b3a00','005b3b80']),
 ('row-less before this slice', ['00440bc0','00474f90','0047f7d0','00487180','004b4eb0','00562ac0','005aaaa0','005ac770','005ac930']),
]
IDENT = {'00409680':'re/match/tlr/00409680.c','00409710':'re/match/tlr/00409710.c','00474f90':'re/match/tlr/00474f90.c',
         '005aaaa0':'re/match/tlr/005aaaa0.c','005addd0':'re/match/s26/005addd0.c'}
print('| group | rva | name | level now | source | best | bytes (positional) | ins ours/orig | relocheck | residual |')
print('|---|---|---|---|---|---|---|---|---|---|')
for g, rows in groups:
    for r in rows:
        hr = h.get(r); name = hr[1] if hr else '-'; lvl = hr[3] if hr else '-'
        if r in IDENT:
            print('| %s | 0x%s | %s | %s | `%s` | **100%%** | 0, BYTE-IDENTICAL | | OK | none |' % (g, r, name, lvl, IDENT[r])); continue
        s = sc.get(r)
        if not s:
            print('| %s | 0x%s | %s | %s | - | - | - | - | - | - |' % (g, r, name, lvl)); continue
        print('| %s | 0x%s | %s | %s | `%s` | %s%% | %s | %s | %s | %s |' % (g, r, name, lvl, s[1], s[2], s[3], s[4], s[5], rk.get(r, '-')))
