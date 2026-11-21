"""PORT-TOP-LAND-06-RF: compare this tree's ddkobj check against the run 54 head check.
  - ddkobj totals, refusals, dual-body count, head vs here
  - newly DIFFERENT rvas (DIFFERENT here, not DIFFERENT at the head), each classified: ADMITTED (re/match/ADMITTED.tsv),
    PROVISIONAL (re/match/PROVISIONAL.tsv, with its verdict), replay-C4 (hooks.csv c4=frida-replay), else UNCLASSIFIED
  - units that fail or are skipped here (VS2022 body) whose rvas are byte-matched (hooks.csv c4=match): the
    "listed unit -> 100% neighbour that fell back to its VS2022 body" pairs; a unit already failing at the head is marked
Run from the worktree root: py -3.12 verify/porttopland_20261121/tools/headcmp.py <headcheck dir> [--out <md>]"""
import csv, json, os, re, sys

H = sys.argv[1]
out = sys.argv[sys.argv.index('--out') + 1] if '--out' in sys.argv else None
B = 'mashedmod/build/ddkobj'


def bc(p):
    d = {}
    for r in csv.DictReader(open(p, encoding='utf-8'), delimiter='\t'):
        d[r['rva'].lower()] = r
    return d


hb, nb = bc(os.path.join(H, 'BYTECHECK.tsv')), bc(os.path.join(B, 'BYTECHECK.tsv'))
hs, ns = json.load(open(os.path.join(H, 'ddkobj_summary.json'))), json.load(open(os.path.join(B, 'ddkobj_summary.json')))
hu = json.load(open(os.path.join(H, 'ddkobj_build.json')))['units']
nu = json.load(open(os.path.join(B, 'ddkobj_build.json')))['units']
adm = {l.split('\t')[0].lower() for l in open('re/match/ADMITTED.tsv', encoding='utf-8') if l.startswith('0x')}
prov = {}
for l in open('re/match/PROVISIONAL.tsv', encoding='utf-8'):
    if l.startswith('0x'):
        f = l.rstrip('\r\n').split('\t')
        prov[f[0].lower()] = (f[2], f[5], f[6][:140])
hooks = {}
for row in csv.reader(open('hooks.csv', encoding='utf-8', errors='replace')):
    if row and re.fullmatch(r'(0x)?[0-9a-fA-F]{8}', row[0]):
        hooks['0x' + row[0].lower().replace('0x', '')] = row
L = []
p = L.append
p('| | head (log/orch/run54_headcheck) | here |')
p('|---|---|---|')
for k in ('identical', 'different', 'not_in_map', 'functions', 'units', 'units_linked', 'provisional_units_skipped'):
    p('| %s | %s | %s |' % (k, hs.get(k), ns.get(k)))
p('| refusals | %s | %s |' % (hs.get('refusals'), ns.get('refusals')))
p('| dual body rvas | %d | %d |' % (len(hs.get('dual_body_rvas', [])), len(ns.get('dual_body_rvas', []))))
gone = sorted(set(hs.get('dual_body_rvas', [])) - set(ns.get('dual_body_rvas', [])))
new_d = sorted(set(ns.get('dual_body_rvas', [])) - set(hs.get('dual_body_rvas', [])))
p('')
p('Dual body: left %s, new %s.' % (' '.join(gone) or 'none', ' '.join(new_d) or 'none'))
p('')
nd = sorted(r for r, v in nb.items() if v['result'] == 'DIFFERENT' and (r not in hb or hb[r]['result'] != 'DIFFERENT'))
p('### Newly DIFFERENT (%d)' % len(nd))
p('')
p('| rva | at head | src | diff bytes / size | class |')
p('|---|---|---|---|---|')
cnt = {}
for r in nd:
    row = hooks.get(r)
    if r in adm:
        c = 'ADMITTED (re/match/ADMITTED.tsv)'
    elif r in prov:
        c = 'PROVISIONAL: %s %s' % (prov[r][1], prov[r][2])
    elif row and 'c4=frida-replay' in row[8] and 'c4=match' not in row[8]:
        c = 'replay-C4 body (hooks.csv c4=frida-replay)'
    else:
        c = 'UNCLASSIFIED (hooks.csv %s %s)' % (row[3] if row else '-', (row[8][:60] if row else ''))
    cnt[c.split(' ')[0]] = cnt.get(c.split(' ')[0], 0) + 1
    p('| %s | %s | %s | %s / %s | %s |' % (r, hb[r]['result'] if r in hb else 'not checked', nb[r]['src'], nb[r]['diff_bytes'],
                                         nb[r]['size'], c))
p('')
p('Tally: %s' % cnt)
p('')
hfail = {u['src'] for u in hu if u['status'] != 'ok'}
p('### Listed / refused unit -> 100% neighbour on its VS2022 body')
p('')
p('| unit (src) | status | rvas in the unit with c4=match (fell back) | listed / provisional rvas in the unit | at head |')
p('|---|---|---|---|---|')
for u in nu:
    if u['status'] == 'ok':
        continue
    m = [r for r in u['rvas'] if r.lower() in hooks and 'c4=match' in hooks[r.lower()][8]]
    lp = [r for r in u['rvas'] if r.lower() in prov]
    if not m:
        continue
    if u['status'] == 'skip:provisional' and set(m) <= set(lp):
        continue   # the unit's only byte-matched rvas are themselves listed: no neighbour falls back
    p('| %s | %s | %s | %s | %s |' % (u['src'], u['status'], ' '.join(m), ' '.join(lp) or '-',
                                      'same' if u['src'] in hfail else 'NEW'))
txt = '\n'.join(L) + '\n'
print(txt)
if out:
    open(out, 'w', encoding='utf-8', newline='\n').write(txt)
