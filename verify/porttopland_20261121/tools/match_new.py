"""PORT-TOP-LAND-06-RF: match.py --bytes and relocheck.py on every source a profile's plan binds as kind `match` that
the plan at <ref> did not bind (sources the branches already checked are listed by them: PORT-TOP-06 match_bound.tsv,
RENDERFRAME RESULT section 4). Also reports bound sources whose text differs between <ref> and the worktree.
py -3.12 verify/porttopland_20261121/tools/match_new.py <profile> <ref> <out.tsv>"""
import json, re, subprocess, sys

prof, ref, outp = sys.argv[1:4]
path = 're/tools/bind/state/%s/plan.json' % prof
old = json.loads(subprocess.run(['git', 'show', '%s:%s' % (ref, path)], capture_output=True, text=True, check=True).stdout)
new = json.load(open(path))
oldsrc = {v[1] for v in old.values() if v[0] == 'bind'}
srcs = {}
for k, v in new.items():
    if v[0] == 'bind' and v[2] == 'match' and v[1] not in oldsrc:
        srcs.setdefault(v[1], []).append(k)
changed = []
for s in sorted({v[1] for v in new.values() if v[0] == 'bind'} & oldsrc):
    d = subprocess.run(['git', 'diff', '--quiet', ref, '--', s]).returncode
    if d:
        changed.append(s)
out = ['src\trvas\tmatch_bytes\trelocheck']
items = sorted(srcs.items())
for i, (s, rvas) in enumerate(items, 1):
    m = subprocess.run([sys.executable, 're/tools/match/match.py', s, '--bytes'], capture_output=True, text=True, errors='replace').stdout
    byt = re.findall(r'bytes: .*', m)
    mv = 'BYTE-IDENTICAL' if byt and all('BYTE-IDENTICAL' in b for b in byt) else ('; '.join(b[7:80] for b in byt) or 'no bytes line')
    r = subprocess.run([sys.executable, 're/tools/match/relocheck.py', s], capture_output=True, text=True, errors='replace').stdout
    fl = re.findall(r'^0x([0-9a-fA-F]{8})\t(\w+)', r, re.M)
    bad = [a + ':' + v for a, v in fl if v != 'OK']
    rv = ('OK %d/%d' % (len(fl), len(fl))) if fl and not bad else ('NOT OK ' + ' '.join(bad)[:70] if fl else 'no output')
    out.append('\t'.join([s, ','.join(sorted(rvas)), mv, rv]))
    print('[%d/%d] %s %s | %s' % (i, len(items), s, mv[:50], rv[:40]), flush=True)
open(outp, 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print('%s: %d new match-kind sources, BYTE-IDENTICAL %d, relocheck OK %d; bound sources changed since %s: %s' % (
    prof, len(items), sum(1 for l in out[1:] if l.split('\t')[2] == 'BYTE-IDENTICAL'),
    sum(1 for l in out[1:] if l.split('\t')[3].startswith('OK')), ref, ' '.join(changed) or 'none'))
