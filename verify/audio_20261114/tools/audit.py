"""Audit BINDINGS.tsv kind=matched rows against hooks.csv C4 tags, BYTECHECK, promotions, ADMITTED, PROVISIONAL.
usage: audit.py <wt> <bytecheck.tsv>"""
import csv, glob, os, re, sys
wt, bc = sys.argv[1], sys.argv[2]
os.chdir(wt)
hooks = {}
with open('hooks.csv', encoding='utf-8', errors='replace') as f:
    for r in csv.reader(f):
        if r and re.fullmatch(r'[0-9a-fA-F]{8}', r[0]):
            hooks[r[0].lower()] = r
def tsv(p, key=0):
    d = {}
    with open(p, encoding='utf-8') as f:
        for r in csv.reader((l for l in f if not l.startswith('#')), delimiter='\t'):
            if r and r[0].startswith('0x'):
                d[r[0][2:].lower()] = r
    return d
adm = tsv('re/match/ADMITTED.tsv')
prov = tsv('re/match/PROVISIONAL.tsv')
byt = {}
with open(bc, encoding='utf-8') as f:
    for r in csv.DictReader(f, delimiter='\t'):
        byt[r['rva'][2:].lower()] = r
pend = {}
for p in glob.glob('re/promotions/*.tsv'):
    with open(p, encoding='utf-8') as f:
        for r in csv.DictReader((l for l in f if not l.startswith('#')), delimiter='\t'):
            if r.get('rva', '').startswith('0x'):
                pend[r['rva'][2:].lower()] = (os.path.basename(p), r.get('route'))
n = 0
out = []
with open('verify/porttop_20261108/BINDINGS.tsv', encoding='utf-8') as f:
    for r in csv.reader(f, delimiter='\t'):
        if len(r) < 5 or r[2] != 'matched' or not r[0].startswith('0x'):
            continue
        n += 1
        rva = r[0][2:].lower()
        h = hooks.get(rva)
        notes = h[8] if h else ''
        tags = sorted(set(re.findall(r'c4=([a-z-]+)', notes)))
        if 'match' in tags or 'libid' in tags:
            continue
        b = byt.get(rva, {}).get('result', '-')
        notetag = 'c4=match' if '(c4=match)' in r[4] else ('L-126 admitted' if 'L-126 admitted' in r[4] else
                  ('100%' if '100%' in r[4] else 'other'))
        pv = prov.get(rva)
        out.append([r[0], h[3] if h else 'no-row', ','.join(tags) or '-', notetag, b,
                    '%s:%s' % pend[rva] if rva in pend else '-', 'ADMITTED' if rva in adm else '-',
                    pv[10] if pv and len(pv) > 10 else '-', r[3], r[4][:110]])
print('matched rows', n, 'without c4=match/libid', len(out))
print('\t'.join(['rva', 'level', 'hooks_tags', 'note_tag', 'bytecheck', 'promotion', 'admitted', 'provisional', 'exe_symbol', 'note']))
for o in out:
    print('\t'.join(o))
