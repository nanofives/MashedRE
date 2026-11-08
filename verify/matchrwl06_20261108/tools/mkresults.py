"""results.tsv from results1.tsv (collect.py), helper notes (class/reason per row) and the C++ check log."""
import csv, re, hashlib, pathlib
V = 'verify/matchrwl06_20261108/'
D = 'D:/MashedWT/MATCH-RWL06_data/'
rows = list(csv.DictReader(open(V + 'results1.tsv'), delimiter='\t'))
cxx = {}
for l in open(D + 'cxx_all2.txt', encoding='utf-8', errors='replace'):
    m = re.match(r'(CXXOK|CXXFAIL) (\w+)', l)
    if m: cxx[m.group(2)] = m.group(1)
C = {}
for h in ('h1', 'h2', 'h3'):
    for l in open(D + 'notes/%s.md' % h, encoding='utf-8', errors='replace'):
        m = re.match(r'(0x[0-9a-f]{8})\s+\d+\s+[\d.]+\s+[\d.]+\s+cxx:\S+\s+(\S+)\s+(.*)', l.strip())
        if m: C[m.group(1)[2:]] = (m.group(2), m.group(3).strip(), h)
ASM = 'hand-written asm (pushes eax/ebx/ecx/edx after a bare ebp frame, or wait/fnstcw/fldcw on 0x00915000/4); not compiled C'
for v in ('005cb000', '005cb07f', '005cb0ef', '005cb404', '005cb42b'):
    C[v] = ('c', ASM, 'lead')
w = csv.writer(open(V + 'results.tsv', 'w', newline=''), delimiter='\t')
w.writerow(['rva', 'size', 'best%', 'regs%', 'flags', 'by', 'sha256', 'class', 'reason', 'cxx'])
miss = []
for r in rows:
    va8 = r['rva'][2:]
    src = pathlib.Path('re/match/rwl06/%s.c' % va8)
    if src.exists():
        fl = src.read_text(encoding='latin-1').split('\n')[0].replace('// FLAGS:', '').strip()
        sha = hashlib.sha256(src.read_bytes()).hexdigest()
    else:
        fl, sha = '-', '-'
    best = r['best%']
    if best == '100.0':
        cl, rs = '-', 'identical, relocheck OK'
        by = C.get(va8, (None, None, 'raw'))[2]
    else:
        cl, rs, by = C.get(va8, ('b', 'raw converter body, not worked', 'raw'))
        if va8 not in C: miss.append(va8)
    w.writerow([r['rva'], r['size'], best, r['regs%'], fl, by if best == '100.0' or va8 in C else 'raw', sha, cl, rs, cxx.get(va8, '-')])
print('missing', miss)
