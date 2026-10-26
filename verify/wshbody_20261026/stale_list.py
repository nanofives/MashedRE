"""Rebuild the D4-WSH Findings-4 stale-candidate population.

A C4 row is stale-candidate when the last commit touching its body .cpp
(git log -1 --format=%cs <rev> -- <cpp>) is later than the newest date
scraped from the row's scenario + frida_diff + notes.
Usage: py -3.12 stale_list.py <rev> <body-mode: exe|file|exe_or_file> [--tsv out]
"""
import csv, re, subprocess, sys, os

rev = sys.argv[1]
mode = sys.argv[2]
out = sys.argv[4] if len(sys.argv) > 4 and sys.argv[3] == '--tsv' else None

def rows():
    txt = subprocess.run(['git', 'show', f'{rev}:hooks.csv'], capture_output=True,
                         text=True, encoding='utf-8').stdout
    lines = [l for l in txt.splitlines() if not l.startswith('#')]
    return list(csv.DictReader(lines))

DATE = re.compile(r'(20\d\d)-?([01]\d)-?([0-3]\d)')

def newest(r):
    s = ' '.join(r.get(k) or '' for k in ('scenario', 'frida_diff', 'notes'))
    ds = []
    for y, m, d in DATE.findall(s):
        if 1 <= int(m) <= 12 and 1 <= int(d) <= 31:
            ds.append(f'{y}-{m}-{d}')
    return max(ds) if ds else ''

cache = {}
def gitdate(p):
    if p not in cache:
        cache[p] = subprocess.run(['git', 'log', '-1', '--format=%cs', rev, '--', p],
                                  capture_output=True, text=True).stdout.strip()
    return cache[p]

res = []
for r in rows():
    if r['confidence'].strip() != 'C4':
        continue
    f, e = (r['file'] or '').strip(), (r.get('exe_file') or '').strip()
    body = {'exe': e, 'file': f, 'exe_or_file': e or f}[mode]
    if not body.endswith('.cpp'):
        continue
    ev, gd = newest(r), gitdate(body)
    if gd and ev and gd > ev:
        res.append((r['rva'], r['name'], body, ev, gd))
print(len(res), 'rows', len({x[2] for x in res}), 'files')
if out:
    with open(out, 'w', encoding='utf-8', newline='') as fh:
        fh.write('rva\tname\tbody\tevidence_date\tfile_git_date\n')
        for x in res:
            fh.write('\t'.join(x) + '\n')
