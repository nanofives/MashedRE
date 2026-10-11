"""negctl re-earn transaction (re-classify leg) -- 2026-10-11.

Applies one batch of keep/demote decisions for the at-risk rows of
re/analysis/negctl/atrisk_20261011.tsv to hooks.csv and re/analysis/CHANGELOG.md, in one transaction.

Decision TSV columns: rva  hook  action  old  new  evidence  negctl  why
  action   keep | demote
  old/new  C-levels (keep: new == old)
  evidence keep: the new path1 CSV (frida_diff is set to it). demote: '' (frida_diff is left as is)
  negctl   keep: the re-earn negctl TSV proving null/value mutants RED.
           demote: the negctl job proving the blindness, e.g. 'negctl_jobs_20261011.tsv null BLIND-DEGENERATE'
  why      one line

Gates (refuses the whole batch on any failure):
  - at most 25 rows (user rail)
  - every rva is a row in hooks.csv whose confidence == old
  - keep rows: old == new, evidence + negctl present
  - demote rows: C3->C2, C4->C3 or C4->C2 only, negctl present
  - CHANGELOG marker matched as an exact line (never a substring)

Usage: py -3.12 re/analysis/negctl/reearn_apply.py <decisions.tsv> [--dry-run]
"""
import csv, io, sys
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HOOKS = ROOT / 'hooks.csv'
CHANGELOG = ROOT / 're' / 'analysis' / 'CHANGELOG.md'
MARKER = '<!-- ENTRIES -->'
ALLOWED_DEMOTIONS = {('C3', 'C2'), ('C4', 'C3'), ('C4', 'C2')}
TODAY = date(2026, 10, 11).isoformat()


def die(msg):
    sys.exit(f'REFUSED: {msg}')


def main():
    if len(sys.argv) < 2:
        die(__doc__)
    dry = '--dry-run' in sys.argv
    dec = list(csv.DictReader(open(sys.argv[1], encoding='utf-8'), delimiter='\t'))
    if not dec:
        die('empty decision file')
    if len(dec) > 25:
        die(f'{len(dec)} rows > 25 per transaction')

    raw = HOOKS.read_bytes()
    nl = '\r\n' if b'\r\n' in raw else '\n'
    rows = list(csv.reader(io.StringIO(raw.decode('utf-8'), newline='')))
    hdr = rows[0]
    ix = {k: hdr.index(k) for k in ('rva', 'name', 'confidence', 'frida_diff', 'notes')}
    by_rva = {}
    for r in rows[1:]:
        if r and r[0]:
            by_rva.setdefault(r[0].lower(), []).append(r)

    log = []
    seen = set()
    for d in dec:
        rva = d['rva'].lower().replace('0x', '').zfill(8)
        if rva in seen:
            die(f'{rva} twice in one batch')
        seen.add(rva)
        hit = by_rva.get(rva)
        if not hit or len(hit) != 1:
            die(f'{rva}: {0 if not hit else len(hit)} hooks.csv rows')
        r = hit[0]
        old, new, act = d['old'], d['new'], d['action']
        if r[ix['confidence']] != old:
            die(f'{rva}: hooks.csv says {r[ix["confidence"]]}, decision says {old}')
        if not d.get('negctl'):
            die(f'{rva}: no negctl citation')
        if act == 'keep':
            if old != new or not d.get('evidence'):
                die(f'{rva}: keep needs old == new and an evidence CSV')
            r[ix['frida_diff']] = d['evidence']
            r[ix['notes']] += (f' | negctl re-earn {TODAY}: {old} KEPT. {d["why"]} '
                               f'Evidence {d["evidence"]}; negctl {d["negctl"]}')
            arrow = f'{old}->{new} (kept, re-earned)'
        elif act == 'demote':
            if (old, new) not in ALLOWED_DEMOTIONS:
                die(f'{rva}: demotion {old}->{new} not allowed')
            r[ix['confidence']] = new
            r[ix['notes']] += (f' | negctl re-earn {TODAY}: {old}->{new} DEMOTED. {d["why"]} '
                               f'Blindness: {d["negctl"]}')
            arrow = f'{old}->{new}'
        else:
            die(f'{rva}: action {act!r}')
        log.append(f'{TODAY}  {rva}  {r[ix["name"]]}  {arrow}  negctl re-earn ruling (2026-10-11): '
                   f'{d["why"]} negctl: {d["negctl"]}' + (f'; evidence {d["evidence"]}' if act == 'keep' else ''))

    cl = CHANGELOG.read_bytes().decode('utf-8')   # keep CRLF as is
    lines = cl.split('\n')
    hits = [i for i, l in enumerate(lines) if l.rstrip('\r') == MARKER]
    if len(hits) != 1:
        die(f'CHANGELOG marker matched {len(hits)} exact lines')
    eol = '\r' if lines[hits[0]].endswith('\r') else ''
    new_cl = '\n'.join(lines[:hits[0] + 1] + [l + eol for l in log] + lines[hits[0] + 1:])
    if len(new_cl) <= len(cl):
        die('CHANGELOG would not grow')

    out = io.StringIO(newline='')
    csv.writer(out, lineterminator=nl).writerows(rows)
    if dry:
        print('\n'.join(log))
        print(f'DRY RUN: {len(dec)} rows OK')
        return
    HOOKS.write_bytes(out.getvalue().encode('utf-8'))
    CHANGELOG.write_bytes(new_cl.encode('utf-8'))
    k = sum(d['action'] == 'keep' for d in dec)
    print(f'applied {len(dec)} rows: {k} kept, {len(dec) - k} demoted')


if __name__ == '__main__':
    main()
