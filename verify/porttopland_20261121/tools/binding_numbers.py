"""PORT-TOP-LAND-06-RF: the three binding numbers for the raceframe and renderframe profiles on the merged head.
  reachable = plan units (bind, reuse) reached from the root through bound bodies, plus file-local members of bound
              sources that are not plan keys (PORT-TOP-06 counted these, 31 at its branch)
  prebound  = /include anchors in Match/PortTop_BulkRefs.cpp (bodies with no bound caller, keep-alive only)
  unbound   = stops that carry a reason (bindable source left unbound: cpp-unclean, owned, static, link)
  nosource  = stops with no reason (no matched / admitted source)
Run from the worktree root: py -3.12 verify/porttopland_20261121/tools/binding_numbers.py [--tsv <out>]"""
import collections, json, re, sys

out = sys.argv[sys.argv.index('--tsv') + 1] if '--tsv' in sys.argv else None
rows = []
for p, root in (('raceframe', '004111c0'), ('renderframe', '00492e90')):
    pl = json.load(open('re/tools/bind/state/%s/plan.json' % p))
    seen, q = set(), [root]
    while q:
        r = q.pop()
        if r in seen or r not in pl:
            continue
        seen.add(r)
        if pl[r][0] == 'bind':
            q.extend(pl[r][3] + pl[r][4])
    loc = set()
    for r in seen:
        if pl[r][0] == 'bind':
            loc |= set(pl[r][4])
    loc -= set(pl)
    c = collections.Counter(pl[r][0] for r in seen)
    stops = sorted(r for r in seen if pl[r][0] == 'stop')
    unb = [r for r in stops if len(pl[r]) > 1]
    nos = [r for r in stops if len(pl[r]) == 1]
    why = collections.Counter(re.split(r'[:(]', pl[r][1])[0].strip() for r in unb)
    print('%s: reachable %d (bind %d + file-local members %d) + reuse %d; unbound with reason %d %s; no source %d'
          % (p, c['bind'] + len(loc), c['bind'], len(loc), c['reuse'], len(unb), dict(why), len(nos)))
    for r in stops:
        rows.append((p, '0x' + r, pl[r][1] if len(pl[r]) > 1 else 'no matched / admitted source'))
refs = open('mashedmod/src/mashed_re/Match/PortTop_BulkRefs.cpp', encoding='utf-8').read().count('/include:')
print('prebound keep-alive (Match/PortTop_BulkRefs.cpp /include anchors): %d' % refs)
if out:
    with open(out, 'w', encoding='utf-8', newline='\n') as f:
        f.write('profile\trva\treason\n')
        for r in rows:
            f.write('\t'.join(r) + '\n')
