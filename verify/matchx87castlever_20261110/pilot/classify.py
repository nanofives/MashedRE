"""Residue classifier for the pilot pool: run match.py --only <rva> --bytes per row and count
the differing instruction lines ('!') and how many of them are x87 (mnemonic starts with f on either side).
Output: pool_classified.tsv (rva, source, bytes_bad, bytes_n, pct, diff_lines, x87_lines, x87_share, first diffs)."""
import csv, os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
root = os.path.dirname(os.path.abspath(__file__))
repo = os.path.abspath(os.path.join(root, '..', '..', '..'))
rows = list(csv.DictReader(open(os.path.join(root, 'pool_all.tsv'), encoding='utf-8'), delimiter='\t'))

def mn(s):
    s = s.strip()
    return s.split()[0] if s else ''

def run(r):
    p = subprocess.run([sys.executable, 're/tools/match/match.py', r['source'], '--only', r['rva'], '--bytes'],
                       cwd=repo, capture_output=True, text=True)
    out = p.stdout
    pct = re.search(r': ([\d.]+)% \(regs', out)
    b = re.search(r'bytes: (\d+) B whole symbol, (\d+) differing', out)
    diffs = [l for l in out.splitlines() if l.startswith('   ! ')]
    x87 = 0
    for l in diffs:
        a, _, o = l[5:].partition(' | ')
        if mn(a).startswith('f') or mn(o).startswith('f'):
            x87 += 1
    first = ' ;; '.join(' '.join(l[5:].split()) for l in diffs[:4])
    return [r['rva'], r['source'], b.group(2) if b else '?', b.group(1) if b else '?', pct.group(1) if pct else '?',
            len(diffs), x87, f'{x87 / len(diffs):.2f}' if diffs else '-', r['source_pool'], r['conf'], first]

with ThreadPoolExecutor(6) as ex:
    res = list(ex.map(run, rows))
res.sort(key=lambda x: int(x[2]) if x[2].isdigit() else 99999)
w = csv.writer(open(os.path.join(root, 'pool_classified.tsv'), 'w', newline='', encoding='utf-8'), delimiter='\t')
w.writerow(['rva', 'source', 'bytes_bad', 'bytes_n', 'pct', 'diff_lines', 'x87_lines', 'x87_share', 'source_pool', 'conf', 'first_diffs'])
for x in res:
    w.writerow(x)
    print('\t'.join(str(v) for v in x[:10]))
