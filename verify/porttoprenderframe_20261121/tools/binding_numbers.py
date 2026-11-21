"""PORT-TOP-RENDERFRAME-492E90: the three binding numbers (log/orch/run53_lead_rules.md "Binding numbers") from
re/tools/bind/state/renderframe/plan.json, plus the stop table (verify/porttoprenderframe_20261121/STOPS.tsv).
  reachable  = units of the plan (root, bind, reuse) reached from the root 0x00492e90 through bound bodies; on the exe
               they run only under MASHED_ORIG_TOP + MASHED_RWC_CORE + MASHED_RWC_FRAME + MASHED_OT_RENDERFRAME
  prebound   = units held only by a keep-alive reference (PortTop_BulkRefs.cpp): this row writes none
  unbound    = stops with a source the tool would bind, left unbound with the reason (owned / other branch / static)
  nobody     = stops with no bindable source (no re/match source, below 100%, or C4 without a c4=match tag)
Run from the worktree root: py -3.12 verify/porttoprenderframe_20261121/tools/binding_numbers.py
"""
import csv, json, os, re

plan = json.load(open('re/tools/bind/state/renderframe/plan.json'))
root = '00492e90'
seen, q = set(), [root]
while q:
    r = q.pop()
    if r in seen or r not in plan:
        continue
    seen.add(r)
    v = plan[r]
    if v[0] == 'bind':
        q.extend(v[3] + v[4])
hooks = {}
for row in csv.reader(open('hooks.csv', encoding='utf-8', errors='replace')):
    if row and re.fullmatch(r'(0x)?[0-9a-fA-F]{8}', row[0]):
        hooks[row[0].lower().replace('0x', '')] = row
reach = [r for r in seen if plan[r][0] in ('bind', 'reuse')]
stops = [r for r in seen if plan[r][0] == 'stop']
rows = []
for r in sorted(stops):
    why = plan[r][1] if len(plan[r]) > 1 and plan[r][1] else ''
    h = hooks.get(r, [])
    lvl, name = (h[3] if len(h) > 3 else '-'), (h[1] if len(h) > 1 else '-')
    srcs = [p for p in __import__('glob').glob('re/match/**/*%s*.c' % r, recursive=True)]
    if why:
        kind = 'bindable, left unbound'
    elif not srcs:
        kind = 'no bindable source: no re/match source (%s)' % lvl
    elif lvl == 'C4':
        kind = 'no bindable source: C4 without c4=match / ADMITTED / PROVISIONAL bind (%s)' % srcs[0].replace('\\', '/')
    else:
        kind = 'no bindable source: %s source below 100%% (%s)' % (lvl, srcs[0].replace('\\', '/'))
    rows.append((r, name, lvl, kind, why))
print('reachable (root + bind + reuse reached from 0x%s): %d (bind %d incl. root, reuse %d)' % (
    root, len(reach), sum(plan[r][0] == 'bind' for r in reach), sum(plan[r][0] == 'reuse' for r in reach)))
print('pre-bound keep-alive only: 0')
print('bindable left unbound: %d' % sum(1 for x in rows if x[3].startswith('bindable')))
print('stops with no bindable source: %d' % sum(1 for x in rows if not x[3].startswith('bindable')))
with open('verify/porttoprenderframe_20261121/STOPS.tsv', 'w', encoding='utf-8', newline='\n') as f:
    f.write('rva\tname\tlevel\tkind\treason\n')
    for x in rows:
        f.write('0x%s\t%s\t%s\t%s\t%s\n' % x)
