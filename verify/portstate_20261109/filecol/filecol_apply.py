"""filecol_apply.py <candidates.tsv> --result <RESULT path> [--apply]

PORT-TRACKER-FILECOL, the re-classify skill's hooks.csv transaction restricted to the `file` cell (no C-level move):
for each candidate row of filecol_candidates.py (a matched_tu column that is a path), set `file` = matched_tu and
prepend one CHANGELOG line below `<!-- ENTRIES -->`:
  <date>  0x<rva>  <name>  Cn->Cn  file column only: <old> -> <new> (...); evidence: <RESULT>
Only the changed hooks.csv lines are rewritten (every other byte kept), line endings preserved. Dry run by default.
Idempotent: a row whose file already equals matched_tu is skipped.
"""
import argparse, csv, io, sys, datetime
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
ap = argparse.ArgumentParser()
ap.add_argument('tsv')
ap.add_argument('--result', required=True)
ap.add_argument('--apply', action='store_true')
a = ap.parse_args()

want = {}
for i, l in enumerate(open(a.tsv, encoding='utf-8')):
    p = l.rstrip('\r\n').split('\t')
    if i == 0 or len(p) < 7 or not p[6].startswith('mashedmod/'):
        continue
    want.setdefault(p[1], (p[0], p[6]))

raw = (REPO / 'hooks.csv').read_bytes().decode('utf-8')
lines = raw.splitlines(keepends=True)
hdr = next(csv.reader([lines[0]]))
F = hdr.index('file')
today = datetime.date.today().isoformat()
log, done = [], set()
for k, line in enumerate(lines[1:], 1):
    body = line.rstrip('\r\n')
    eol = line[len(body):]
    try:
        r = next(csv.reader([body]))
    except StopIteration:
        continue
    if not r:
        continue
    rva = r[0].lower().replace('0x', '').zfill(8)
    if rva not in want or rva in done:
        continue
    scope, new = want[rva]
    old = r[F]
    done.add(rva)
    if old == new:
        print(f'SKIP 0x{rva} already {new}')
        continue
    if not (REPO / new).exists():
        sys.exit(f'REFUSED 0x{rva}: {new} does not exist')
    r[F] = new
    buf = io.StringIO()
    csv.writer(buf, lineterminator='').writerow(r)
    lines[k] = buf.getvalue() + eol
    short = lambda s: s.replace('mashedmod/src/mashed_re/', '')
    log.append(f'{today}  0x{rva}  {r[1]}  {r[3]}->{r[3]}  file column only: {short(old)} -> {short(new)} '
               f'({scope}: the matched source is the one body in both targets, the old TU holds none, rva-lint NEW=0); '
               f'evidence: {a.result}')
    print(f'SET  0x{rva} {r[1]} {r[3]}: {old} -> {new}')
missing = set(want) - done
if missing:
    sys.exit(f'REFUSED: no hooks.csv row for {sorted(missing)}')
if not a.apply:
    print(f'dry run: {len(log)} rows would change')
    sys.exit(0)
(REPO / 'hooks.csv').write_bytes(''.join(lines).encode('utf-8'))
cl = REPO / 're' / 'analysis' / 'CHANGELOG.md'
t = cl.read_bytes().decode('utf-8')
nl = '\r\n' if '\r\n' in t[:2000] else '\n'
m = '<!-- ENTRIES -->'
i = t.index(m) + len(m)
j = t.index('\n', i) + 1
t = t[:j] + ''.join(x + nl for x in log) + t[j:]
cl.write_bytes(t.encode('utf-8'))
print(f'applied: {len(log)} hooks.csv rows, {len(log)} CHANGELOG lines')
