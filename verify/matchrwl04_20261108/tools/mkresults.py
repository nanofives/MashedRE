"""MATCH-RWL04 results.tsv: results1.tsv (collect.py) + class/reason from the helper notes + cxx check.
Run from the worktree: py -3.12 verify/matchrwl04_20261108/tools/mkresults.py"""
import csv, re, hashlib, pathlib
V = 'verify/matchrwl04_20261108/'
D = 'D:/MashedWT/MATCH-RWL04_data/'
rows = list(csv.DictReader(open(V + 'results1.tsv'), delimiter='\t'))
cxx = {}
for l in open(V + 'cxx_check.txt', encoding='utf-8', errors='replace'):
    m = re.match(r'\W*(CXXOK|CXXFAIL) (\w+)', l)
    if m: cxx[m.group(2)] = m.group(1)
notes = {}
for h in ('h1', 'h2', 'h3'):
    p = pathlib.Path(D + 'notes/' + h + '.md')
    if not p.exists(): continue
    for l in p.read_text(encoding='utf-8', errors='replace').splitlines():
        m = re.match(r'\s*0x([0-9a-f]{8})\s+(.*)', l)
        if not m: continue
        toks = m.group(2).split()
        cl, i = '-', None
        for k, t in enumerate(toks[:7]):
            if t in ('a', 'b', 'c', '-'):
                cl, i = t, k; break
        reason = ' '.join(toks[i + 1:]) if i is not None else m.group(2)
        notes[m.group(1)] = (cl, reason.lstrip('- ').strip(), h)
out = open(V + 'results.tsv', 'w', newline='')
w = csv.writer(out, delimiter='\t')
w.writerow(['rva', 'size', 'best%', 'regs%', 'flags', 'by', 'sha256', 'class', 'reason', 'helper', 'cxx'])
for r in rows:
    va8 = r['rva'][2:]
    src = pathlib.Path('re/match/rwl04/%s.c' % va8)
    if src.exists():
        fl = src.read_text(encoding='latin-1').split('\n')[0].replace('// FLAGS:', '').strip()
        sha = hashlib.sha256(src.read_bytes()).hexdigest()[:16]
    else:
        fl, sha = '-', '-'
    cl, rs, h = notes.get(va8, ('b', 'raw converter body, not worked', 'raw'))
    if r['best%'] == '100.0':
        cl, rs = '-', 'identical'
        if va8 not in notes: h = 'raw'
    elif not src.exists():
        cl, rs, h = 'c', 'raw converter failed (NAN operand with no address), no source', '-'
    w.writerow([r['rva'], r['size'], r['best%'], r['regs%'], fl, r['by'], sha, cl, rs, h, cxx.get(va8, '-')])
