"""negctl re-earn judge (2026-10-11): apply the keep criterion to re-earn negctl runs.

KEEP iff, for the registry entry that backs the row:
  real      GREEN                       (the normal diff; DEGENERATE does not count)
  baseline  GREEN                       (original vs original)
  null      RED
  ret       RED, or SKIP (void signature)
  field_*   RED or NA, or void when field_none is CTRL-RED (freeze latency perturbs the vector)
heap_extra / import_extra are not value mutants and are not held against the row.

Usage:
  py -3.12 re/analysis/negctl/reearn_judge.py --tsv <negctl tsv>... --rows <rva=hook,...> --batch <name>
      [--old <rva=C3,...>] [--out decisions.tsv]
For each KEEP it copies log/diff_<hook>.csv (run_diff.py must have run) to
re/analysis/negctl/reearn_20261011/evidence/ and the negctl TSVs to .../runs/<batch>/.
"""
import argparse, csv, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BASE = ROOT / 're' / 'analysis' / 'negctl' / 'reearn_20261011'


def load(tsvs):
    jobs = {}
    for t in tsvs:
        for r in csv.DictReader(open(t, encoding='utf-8'), delimiter='\t'):
            jobs[(r['hook'], r['mode'])] = r          # later TSVs win (re-runs)
    return jobs


def judge(jobs, hook):
    v = lambda m: (jobs.get((hook, m)) or {}).get('verdict', 'MISSING')
    why = []
    if v('real') != 'GREEN': why.append(f'real {v("real")}')
    if v('baseline') != 'GREEN': why.append(f'baseline {v("baseline")}')
    if v('null') != 'RED': why.append(f'null {v("null")}')
    if v('ret') not in ('RED', 'SKIP'): why.append(f'ret {v("ret")}')
    if v('field_none') != 'CTRL-RED':
        for m in ('field_first', 'field_last'):
            if v(m) not in ('RED', 'NA', 'MISSING'): why.append(f'{m} {v(m)}')
    summ = ' '.join(f'{m}={v(m)}' for m in ('real', 'baseline', 'null', 'ret', 'field_first', 'field_last'))
    return (not why), why, summ


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--tsv', nargs='+', required=True)
    ap.add_argument('--rows', required=True, help='rva=hook,...')
    ap.add_argument('--old', default='', help='rva=C3,... (default read from hooks.csv)')
    ap.add_argument('--batch', required=True)
    ap.add_argument('--out')
    a = ap.parse_args()
    jobs = load(a.tsv)
    hk = {r['rva'].lower(): r for r in csv.DictReader(open(ROOT / 'hooks.csv', encoding='utf-8'))}
    runs = BASE / 'runs' / a.batch
    runs.mkdir(parents=True, exist_ok=True)
    for t in a.tsv:
        shutil.copy2(t, runs / Path(t).name)
    ev = BASE / 'evidence'
    ev.mkdir(parents=True, exist_ok=True)
    out = []
    for pair in a.rows.split(','):
        rva, hook = pair.split('=')
        r8 = rva.lower().replace('0x', '').zfill(8)
        old = hk[r8]['confidence']
        ok, why, summ = judge(jobs, hook)
        cite = f'{(runs / Path(a.tsv[-1]).name).relative_to(ROOT).as_posix()} [{hook}] {summ}'
        if ok:
            src = ROOT / 'log' / f'diff_{hook}.csv'
            if not src.exists():
                sys.exit(f'{hook}: {src} missing, run run_diff.py first')
            shutil.copy2(src, ev / src.name)
            out.append([f'0x{r8}', hook, 'keep', old, old, (ev / src.name).relative_to(ROOT).as_posix(), cite, ''])
        else:
            out.append([f'0x{r8}', hook, 'FAIL', old, '', '', cite, '; '.join(why)])
        print(f'{r8} {hook:40s} {"KEEP" if ok else "FAIL"}  {summ}' + ('' if ok else f'   <- {"; ".join(why)}'))
    if a.out:
        with open(a.out, 'w', encoding='utf-8', newline='') as f:
            w = csv.writer(f, delimiter='\t', lineterminator='\n')
            w.writerow(['rva', 'hook', 'action', 'old', 'new', 'evidence', 'negctl', 'why'])
            w.writerows(out)


if __name__ == '__main__':
    main()
