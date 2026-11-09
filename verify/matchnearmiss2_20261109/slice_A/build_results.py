"""Rebuild slice_A/results.tsv: re-score each row's committed source (and its best variant, when one exists) with
match.py --show --bytes --only, take sha256 of the committed source. before_* columns come from slice_A.tsv / the
base-commit score (unchanged sources score the same). Run from the repo root."""
import csv, hashlib, pathlib, re, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[3]
D = pathlib.Path(__file__).resolve().parent
rows = list(csv.DictReader(open(D.parent / 'slice_A.tsv'), delimiter='\t'))
notes = {r['rva']: r for r in csv.DictReader(open(D / 'hand_log.tsv'), delimiter='\t')}


def score(rva, src):
    out = subprocess.run([sys.executable, 're/tools/match/match.py', '--show', '--bytes', '--only', rva, str(src)],
                         capture_output=True, text=True, cwd=ROOT).stdout
    m = re.search(r': ([\d.]+)% ', out)
    if 'BYTE-IDENTICAL' in out:
        return m.group(1), '0'
    b = re.search(r'(\d+) differing unmasked bytes', out)
    return (m.group(1) if m else '?'), (b.group(1) if b else '?')


cols = ['rva', 'source', 'subsystem', 'miss_type', 'before_pct', 'after_pct', 'before_bad_bytes', 'after_bad_bytes',
        'verdict', 'detail', 'compiles', 'best_variant', 'sha256_after']
with open(D / 'results.tsv', 'w', newline='') as f:
    w = csv.writer(f, delimiter='\t', lineterminator='\n')
    w.writerow(cols)
    for r in rows:
        n = notes[r['rva']]
        src = ROOT / r['source']
        scored = src
        if n['verdict'] == 'PORT-INCLUDED':
            scored = D / 'port_included' / (r['source'].split('/')[-1])
        cur_pct, cur_bad = score(r['rva'], scored)
        best = n['best_variant']
        if best != '-':
            a_pct, a_bad = score(r['rva'], D / best)
        else:
            a_pct, a_bad = cur_pct, cur_bad
        sha = hashlib.sha256(src.read_bytes()).hexdigest()
        w.writerow([r['rva'], r['source'], r['subsystem'], r['miss_type'], r['live_pct'], a_pct, r['bytes_bad'], a_bad,
                    n['verdict'], n['detail'], n['compiles'], best, sha])
        print(r['rva'], n['verdict'], r['live_pct'], '->', a_pct, r['bytes_bad'], '->', a_bad, flush=True)
