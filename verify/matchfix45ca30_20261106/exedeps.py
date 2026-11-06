"""Check that no file changed by the branch is in any exe or asi object's dependency set.
usage: exedeps.py <worktree> <base-commit>"""
import json, os, subprocess, sys
wt, base = sys.argv[1], sys.argv[2]
changed = subprocess.run(['git', '-C', wt, 'diff', '--name-only', base, 'HEAD'],
                         capture_output=True, text=True, check=True).stdout.split()
chg = {os.path.normcase(os.path.normpath(os.path.join(wt, c))) for c in changed}
print('changed files:', len(changed))
for c in changed:
    print('  ', c)
for tgt in ('exe', 'asi'):
    d = os.path.join(wt, 'mashedmod', 'build', 'obj', tgt)
    hits, n = {}, 0
    for f in sorted(os.listdir(d)):
        if not f.endswith('.json'):
            continue
        n += 1
        j = json.load(open(os.path.join(d, f), encoding='utf-8-sig'))
        data = j.get('Data', j)
        deps = [data.get('Source', '')] + list(data.get('Includes', []))
        for p in deps:
            q = os.path.normcase(os.path.normpath(p))
            if q in chg:
                hits.setdefault(f[:-5], []).append(p)
    print(f'{tgt}: {n} objects scanned, objects depending on a changed file: {len(hits)}')
    for k, v in hits.items():
        print('  ', k, '<-', ', '.join(os.path.basename(x) for x in v))
