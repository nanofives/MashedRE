"""BIND-BULK-PROFILE: check that porttop_bind.py --profile bulk reproduces RACE-BIND-C4-BULK checkpoint 1 on a tree.
usage (from the tree root): py -3.12 <this> [--index <exeidx.json>]
Builds the bulk plan (live exe index, or the frozen one), then compares without writing anything:
  plan       against verify/bindbulk_20261121/tools/plan_cp.json (dict and key order) or state/bulk/plan.json (--against state)
  listing    against verify/bindbulk_20261121/tools/last_plan.txt (rva lines; the writer's trailing 'wrote' line skipped)
  wrappers   every planned bind's text against its Match/PortTop_<rva>.cpp (CRLF ignored)
  stops      against Match/PortTop_BulkStops.cpp
  keep       the anchors against disposition_cp.json 'keep', the groups and refused roots against it
Prints one line per check and each differing item."""
import json, os, sys
sys.path.insert(0, 're/tools/bind')
import porttop_bind as pb

a = sys.argv[1:]
idxp = a[a.index('--index') + 1] if '--index' in a else None
against = a[a.index('--against') + 1] if '--against' in a else 'cp1'
T = 'verify/bindbulk_20261121/tools'
b = pb.Binder('bulk', pb.profile_roots('bulk').split(','))
if idxp:
    b.idx = {k: [tuple(x) for x in v] for k, v in json.load(open(idxp)).items()}
plan = b.build_plan()
ref_plan = json.load(open(os.path.join(T, 'plan_cp.json') if against == 'cp1' else 're/tools/bind/state/bulk/plan.json'))
mine = {k: list(v) for k, v in plan.items()}
print('plan     %s, key order %s (%d entries)' % ('IDENTICAL' if mine == ref_plan else 'DIFFERENT',
      'IDENTICAL' if list(mine) == list(ref_plan) else 'DIFFERENT', len(mine)))
for k in sorted(set(mine) | set(ref_plan)):
    if mine.get(k) != ref_plan.get(k):
        print('   plan', k, ref_plan.get(k), '->', mine.get(k))
old = [l.rstrip('\n') for l in open(os.path.join(T, 'last_plan.txt'), encoding='utf-8') if not l.startswith('wrote ')]
new = b.listing()
print('listing  %s vs last_plan.txt (%d lines)' % ('IDENTICAL' if old == new else 'DIFFERENT', len(new)))
nd = 0
for rva, p in sorted(plan.items()):
    if p[0] != 'bind':
        continue
    fn, text = b.wrapper_text(rva)
    if not os.path.exists(fn) or open(fn, 'rb').read().replace(b'\r\n', b'\n') != text.encode():
        nd += 1
        print('   wrapper', fn, 'missing' if not os.path.exists(fn) else 'differs')
print('wrappers %s (%d binds)' % ('IDENTICAL' if not nd else 'DIFFERENT %d' % nd, sum(1 for p in plan.values() if p[0] == 'bind')))
_, st = b.stops_text()
sf = '%s/%s' % (pb.SRC, b.P['stop_file'])
print('stops    %s' % ('IDENTICAL' if open(sf, 'rb').read().replace(b'\r\n', b'\n') == st.encode() else 'DIFFERENT'))
dp = os.path.join(T, 'disposition_cp.json')
if against == 'cp1' and os.path.exists(dp):
    d = json.load(open(dp))
    grp = {}
    for o, m in sorted(b.group_bound):
        grp.setdefault(o, []).append(m)
    print('keep     %s, group %s, refused %s' % ('IDENTICAL' if b.keep_list() == d['keep'] else 'DIFFERENT',
          'IDENTICAL' if grp == d['group'] else 'DIFFERENT', 'IDENTICAL' if b.refused == d['refused'] else 'DIFFERENT'))
