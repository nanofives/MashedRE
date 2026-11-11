"""No-chain BINDINGS.tsv row helper (HB-GENFILES-NOCONFLICT, 2026-11-10).

The update_bindings_*.py chain (each script runs its predecessor, then its own rows) is FROZEN at
update_bindings_clumptexread.py. New rows do not chain. A row's script edits BINDINGS.tsv in place with only
its own delta, idempotently:

    import os, sys; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from bindings_rows import apply
    apply('MYROW: ', FIXED, APPEND)

FIXED  {rva: (name or None, kind, exe_symbol, note, level, caller)}  sets the whole row (appended when new).
APPEND {rva: ' MYROW: ...'}  adds the suffix to the note once (skipped when the tag is already in the note).

Two rows that do this merge cleanly through scripts/genfiles_merge.py (merge=genbindings in .gitattributes):
row-level union, note suffixes concatenated, a real disagreement on the same field conflicts loudly.
"""
import os
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(os.path.dirname(HERE), 'porttop_20261108', 'BINDINGS.tsv')


def apply(tag, fixed, append, tsv=TSV, quiet=False):
    lines = open(tsv, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in fixed.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    missing = []
    for rva, extra in append.items():
        r = idx.get(rva)
        if r is None:
            missing.append(rva)
        elif tag not in r[4]:
            r[4] += extra
    open(tsv, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    if not quiet:
        for rva in missing:
            print('note target missing:', hex(rva))
        print(len(rows), dict(Counter(r[2] for r in rows)))
    return missing
