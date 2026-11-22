# RACE-LOAD-LUA-2: per-rva table of this row's 77 binds (roots it added): hooks.csv name / subsystem (class) / C level,
# wrapper kind (.c C unit or .cpp), ddkobj unit status (ddkobj_build.json) and BYTECHECK result, PROVISIONAL.tsv listing.
#   py -3.12 verify/racelua2_20261122/unit_table.py > verify/racelua2_20261122/unit_table.tsv
import csv, json, os, re
from pathlib import Path

W = Path(__file__).resolve().parents[2]
roots = []
on = False
for l in open(W / 're/tools/bind/state/raceload/roots.txt', encoding='utf-8'):
    if l.startswith('# RACE-LOAD-LUA-2'):
        on = True
    if on and re.match(r'^[0-9a-f]{8}\s*$', l):
        roots.append(l.strip())
hk = {}
for row in csv.reader(open(W / 'hooks.csv', encoding='utf-8', errors='replace')):
    if row and re.match(r'^[0-9a-f]{8}$', row[0]):
        hk[row[0]] = row
bc = {}
p = W / 'mashedmod/build/ddkobj/BYTECHECK.tsv'
for row in csv.DictReader(open(p, encoding='utf-8'), delimiter='\t'):
    bc[row['rva'][2:]] = row['result']
st = {}
j = json.load(open(W / 'mashedmod/build/ddkobj/ddkobj_build.json'))
for u in j['units']:
    for r in u.get('rvas', []):
        st[r[2:]] = u.get('status')
prov = set()
pv = W / 'PROVISIONAL.tsv'
for cand in [W / 'PROVISIONAL.tsv', W / 're/match/PROVISIONAL.tsv', W / 're/tools/match/PROVISIONAL.tsv']:
    if cand.exists():
        for l in open(cand, encoding='utf-8', errors='replace'):
            m = re.match(r'^(?:0x)?([0-9a-fA-F]{8})', l)
            if m:
                prov.add(m.group(1).lower())
M = W / 'mashedmod/src/mashed_re/Match'
print('rva\tname\tclass(subsystem)\tlevel\twrapper\tddkobj_unit\tbytecheck\tprovisional')
for r in roots:
    h = hk.get(r, [r, '?', '?', '?'])
    wk = '.c' if (M / ('PortTop_%s.c' % r)).exists() else ('.cpp' if (M / ('PortTop_%s.cpp' % r)).exists() else '-')
    print('0x%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s' % (r, h[1], h[2], h[3], wk, st.get(r, '-'), bc.get(r, '-'), 'yes' if r in prov else 'no'))
