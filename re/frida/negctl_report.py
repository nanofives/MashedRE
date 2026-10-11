#!/usr/bin/env py -3.12
"""negctl_report.py — turn run_negctl.py TSVs into the per-handler blindness table and the
at-risk C3/C4 row list.

  py -3.12 re/frida/negctl_report.py <negctl_*.tsv> [more.tsv ...] [--md OUT.md] [--rows OUT.tsv]

A later TSV overrides an earlier one for the same (hook, mode), so a re-run after a handler
fix replaces the old verdict.

Per row, a mutant class counts against the row only when it applies:
  null / ret / field_*   always (field_* only where the kit flipped a byte: verdict NA otherwise)
  heap_extra / import_extra
                         only when re/coverage/standin_census_d2.tsv lists a heap / import call
                         within depth 2 of the row's RVA (a wrong heap call in a function that
                         makes none is not a defect a port can have)
Independent evidence is read from the row's hooks.csv frida_diff + notes text. It is a
keyword match, listed with the tags it hit, for a human to confirm.
"""
import csv, re, sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS        # noqa: E402
import run_diff                         # noqa: E402


def _main_checkout():
    d = ROOT
    for _ in range(4):
        if (d / 'original' / 'MASHED.exe').exists():
            return d
        d = d.parent
    return ROOT


EVIDENCE_ROOT = _main_checkout()   # stored evidence CSVs live in the main checkout's log/


def stored_evidence(hook_name, hrow):
    """'trivial' | 'nontrivial' | 'missing' for the row's stored path1 CSV (frida_diff column, else
    log/diff_<hook>.csv). 'trivial' = every observation on both sides carries no information (run_diff's
    non-degeneracy rule), i.e. a null port would have produced the same CSV."""
    ev = (hrow.get('frida_diff') or '').strip()
    p = EVIDENCE_ROOT / ev if ev.startswith('log/') and ev.endswith('.csv') else EVIDENCE_ROOT / 'log' / f'diff_{hook_name}.csv'
    if not p.exists():
        return 'missing'
    with p.open(encoding='utf-8', errors='replace') as fh:
        vals = list(csv.DictReader(fh))
    if not vals or 'original' not in vals[0]:
        return 'missing'
    return 'trivial' if all(run_diff.is_trivial(v.get('original')) and run_diff.is_trivial(v.get('reimpl'))
                            for v in vals) else 'nontrivial'

MUTANTS = ('null', 'ret', 'field_first', 'field_last', 'heap_extra', 'import_extra')
VALUE_MUTANTS = ('null', 'ret', 'field_first', 'field_last')
EVIDENCE = [
    ('C4-scenario', r'c4-lift|canonical[- ]scenario|hook installed'),
    ('path2', r'path2|verify_hook|install(ed)? (jmp|verif)|inline-jmp'),
    ('replay', r'replay|veccap'),
    ('shadow', r'shadow'),
    ('statediff', r'statediff|state[- ]lane|scenario[_ ]batch'),
    ('race', r'\brace\b|race_[a-z]'),
]
CENSUS_HEAP = ('crt_alloc', 'crt_free', 'rw_alloc', 'rw_free', 'imp_heap')
CENSUS_IMPORT = ('imp_heap', 'imp_file', 'imp_other')


def load_tsvs(paths):
    v = {}
    for p in paths:
        with open(p, encoding='utf-8') as fh:
            for r in csv.DictReader(fh, delimiter='\t'):
                v[(r['hook'], r['mode'])] = r
    return v


def load_hooks_csv():
    out = {}
    with open(ROOT / 'hooks.csv', encoding='utf-8') as fh:
        for r in csv.DictReader(fh):
            try:
                out[int(r['rva'], 16)] = r
            except (ValueError, TypeError):
                pass
    return out


def load_census():
    out = {}
    p = ROOT / 're' / 'coverage' / 'standin_census_d2.tsv'
    with p.open(encoding='utf-8') as fh:
        for r in csv.DictReader(fh, delimiter='\t'):
            out[int(r['rva'], 16)] = r
    return out


def evidence_tags(row):
    t = (row.get('frida_diff', '') + ' ' + row.get('notes', '')).lower()
    return [k for k, p in EVIDENCE if re.search(p, t)]


def main(argv):
    md_out = rows_out = None
    paths = []
    it = iter(argv[1:])
    for a in it:
        if a == '--md':
            md_out = next(it)
        elif a == '--rows':
            rows_out = next(it)
        else:
            paths.append(a)
    v = load_tsvs(paths)
    hc, census = load_hooks_csv(), load_census()

    by_hook = defaultdict(dict)
    for (h, m), r in v.items():
        by_hook[h][m] = r
    per_at = defaultdict(lambda: {'hooks': set(), 'rows': set(), 'c4': set(), 'modes': defaultdict(Counter)})
    at_risk = []
    for h, modes in by_hook.items():
        hook = HOOKS[h]
        at = hook.get('arg_type')
        rva = hook['rva']
        hrow = hc.get(rva, {})
        conf = hrow.get('confidence', '').strip()
        P = per_at[at]
        P['hooks'].add(h); P['rows'].add(rva)
        if conf == 'C4':
            P['c4'].add(rva)
        for m, r in modes.items():
            P['modes'][m][r['verdict']] += 1
        real = modes.get('real', {}).get('verdict')
        base = modes.get('baseline', {}).get('verdict')
        cen = census.get(rva, {})
        heap_exp = [c for c in CENSUS_HEAP if (cen.get(c) or '').strip()]
        imp_exp = [c for c in CENSUS_IMPORT if (cen.get(c) or '').strip()]
        stored = stored_evidence(h, hrow)
        # A mutant verdict only says something about the row when the real run discriminated, or when it was
        # degenerate AND the row's own stored evidence is degenerate too (then the row cannot see a null port).
        # real DEGENERATE with non-trivial stored evidence = this lane lacked the state: inconclusive here.
        interpretable = real == 'GREEN' or (real == 'DEGENERATE' and stored == 'trivial' and not hook.get('degenerate_ok'))
        field_ctrl_red = modes.get('field_none', {}).get('verdict') == 'CTRL-RED'
        blind = []
        for m in MUTANTS:
            if not interpretable:
                break
            if m in ('field_first', 'field_last') and field_ctrl_red:
                continue
            r = modes.get(m)
            if not r or not r['verdict'].startswith('BLIND'):
                continue
            if m == 'heap_extra' and not heap_exp:
                continue
            if m == 'import_extra' and not imp_exp:
                continue
            blind.append(m)
        if blind or real in ('RED', 'ERROR') or base == 'RED' or (real == 'DEGENERATE' and stored != 'nontrivial'):
            at_risk.append({
                'rva': f'0x{rva:08x}', 'hook': h, 'arg_type': at, 'confidence': conf,
                'real': real or '', 'baseline': base or '', 'stored_evidence': stored,
                'blind_value': ','.join(m for m in blind if m in VALUE_MUTANTS),
                'blind_heap_import': ','.join(m for m in blind if m not in VALUE_MUTANTS),
                'census_heap_import': ','.join(sorted(set(heap_exp + imp_exp))),
                'independent_evidence': ','.join(evidence_tags(hrow)),
                'field_flip': (modes.get('field_first', {}).get('stats', '') or '')[:80],
                'name': hrow.get('name', ''),
            })

    lines = ['| handler | C3/C4 rows run | of which C4 | real GREEN | baseline GREEN | null | ret | field_none (ctrl) | field_first | field_last | heap_extra | import_extra |',
             '|---|---|---|---|---|---|---|---|---|---|---|---|']

    def cell(c):
        if not c:
            return '-'
        order = ['RED', 'BLIND', 'BLIND-DEGENERATE', 'NA', 'SKIP', 'ERROR', 'GREEN', 'DEGENERATE', 'CTRL-GREEN', 'CTRL-RED']
        return ' '.join(f'{k}:{c[k]}' for k in order if c.get(k)) or '-'
    for at, P in sorted(per_at.items(), key=lambda kv: -len(kv[1]['rows'])):
        md = P['modes']
        lines.append(f"| `{at}` | {len(P['rows'])} | {len(P['c4'])} | {md['real'].get('GREEN', 0)}/{sum(md['real'].values())} "
                     f"| {md['baseline'].get('GREEN', 0)}/{sum(md['baseline'].values())} | "
                     + ' | '.join(cell(md[m]) for m in ('null', 'ret', 'field_none') + MUTANTS[2:]) + ' |')
    table = '\n'.join(lines)
    print(table)
    print(f'\nat-risk rows: {len(at_risk)}')
    cols = ['rva', 'hook', 'name', 'arg_type', 'confidence', 'real', 'baseline', 'stored_evidence', 'blind_value',
            'blind_heap_import', 'census_heap_import', 'independent_evidence', 'field_flip']
    if rows_out:
        with open(rows_out, 'w', encoding='utf-8', newline='') as fh:
            w = csv.DictWriter(fh, fieldnames=cols, delimiter='\t')
            w.writeheader()
            for r in sorted(at_risk, key=lambda r: (r['arg_type'], r['rva'])):
                w.writerow(r)
    if md_out:
        Path(md_out).write_text(table + '\n', encoding='utf-8')
    c = Counter((r['arg_type'], bool(r['blind_value']), bool(r['independent_evidence'])) for r in at_risk)
    for k, n in sorted(c.items(), key=lambda kv: -kv[1]):
        print(k, n)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
