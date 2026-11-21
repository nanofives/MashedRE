"""PORT-TOP-06 merge-bar check: match.py --bytes and relocheck.py on every source the raceframe plan binds (kind match,
plus the replay / admitted ones for the record). One line per source: [N/M] <src> <kind> <match verdict> <relocheck>.
Run from the worktree root; writes verify/porttop06_20261121/match_bound.tsv."""
import json, re, subprocess, sys

p = json.load(open('re/tools/bind/state/raceframe/plan.json'))
srcs = {}
for k, v in p.items():
    if v[0] == 'bind':
        srcs.setdefault(v[1], (v[2], []))[1].append(k)
out = ['src\tkind\trvas\tmatch_bytes\trelocheck']
items = sorted(srcs.items())
for i, (s, (kind, rvas)) in enumerate(items, 1):
    m = subprocess.run([sys.executable, 're/tools/match/match.py', s, '--bytes'], capture_output=True, text=True, errors='replace').stdout
    byt = re.findall(r'bytes: .*', m)
    mv = 'BYTE-IDENTICAL' if byt and all('BYTE-IDENTICAL' in b for b in byt) else ('; '.join(b[7:80] for b in byt) or 'no bytes line')
    r = subprocess.run([sys.executable, 're/tools/match/relocheck.py', s], capture_output=True, text=True, errors='replace').stdout
    fl = re.findall(r'^0x([0-9a-fA-F]{8})\t(\w+)', r, re.M)
    bad = [a + ':' + v for a, v in fl if v != 'OK']
    rv = ('OK %d/%d' % (len(fl), len(fl))) if fl and not bad else ('NOT OK ' + ' '.join(bad)[:70] if fl else 'no output')
    out.append('\t'.join([s, kind, ','.join(sorted(rvas)), mv, rv]))
    print('[%d/%d] %s %s %s | %s' % (i, len(items), s, kind, mv[:40], rv[:40]), flush=True)
open('verify/porttop06_20261121/match_bound.tsv', 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print('match BYTE-IDENTICAL: %d of %d match-kind sources; relocheck OK: %d' % (
    sum(1 for l in out[1:] if l.split('\t')[1] == 'match' and l.split('\t')[3] == 'BYTE-IDENTICAL'),
    sum(1 for l in out[1:] if l.split('\t')[1] == 'match'),
    sum(1 for l in out[1:] if l.split('\t')[1] == 'match' and l.split('\t')[4].startswith('OK'))))
