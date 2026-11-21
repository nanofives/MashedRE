"""PORT-TOP-LAND-06-RF: diff a profile's plan.json against a git ref (default HEAD).
py -3.12 verify/porttopland_20261121/tools/plan_diff.py <profile> [ref]"""
import json, subprocess, sys

p = sys.argv[1]
ref = sys.argv[2] if len(sys.argv) > 2 else 'HEAD'
path = 're/tools/bind/state/%s/plan.json' % p
old = json.loads(subprocess.run(['git', 'show', '%s:%s' % (ref, path)], capture_output=True, text=True, check=True).stdout)
new = json.load(open(path, encoding='utf-8'))
def k(v):
    return v[0] if v else '-'
c = {}
for r in sorted(set(old) | set(new)):
    a, b = old.get(r), new.get(r)
    if a == b:
        continue
    key = '%s -> %s' % (k(a) if a else 'absent', k(b) if b else 'absent')
    c.setdefault(key, []).append(r)
    det = ''
    if a and b and a[0] == b[0] == 'bind' and a[1:3] == b[1:3]:
        det = ' (callees/locals only)'
    print('%s %s: %s -> %s%s' % (r, key, json.dumps(a)[:110] if a else '-', json.dumps(b)[:110] if b else '-', det))
print('summary', {kk: len(v) for kk, v in c.items()})
print('counts old', {t: sum(1 for v in old.values() if v[0] == t) for t in ('bind', 'reuse', 'stop')},
      'new', {t: sum(1 for v in new.values() if v[0] == t) for t in ('bind', 'reuse', 'stop')})
