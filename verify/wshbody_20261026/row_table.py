"""Fold bodydiff.tsv (one line per checked copy) into one markdown row per hooks.csv row.

The row verdict is the verdict of the copy the C4 evidence ran on: the `file` copy for
dual-copy rows (file != exe_file, both .cpp), the only body otherwise. The exe copy of a
dual-copy row is reported in its own column, it carries no evidence of its own.
0042b8d0: hooks.csv `file` names Util/StateAccessors.cpp, absent on this lineage, so the
installed body (FrameDispatch.cpp, RH_ScopedInstall at :26) is the evidence copy.
Usage: py -3.12 row_table.py <bodydiff.tsv>
"""
import sys
from collections import Counter, OrderedDict

lines = [l.rstrip('\n').split('\t') for l in open(sys.argv[1], encoding='utf-8')][1:]
rows = OrderedDict()
for f in lines:
    rows.setdefault(f[0], []).append(f)


def cell(f):
    if f[7] in ('unchanged', 'FILE-MISSING'):
        return f[7]
    bits = [f[7]]
    if f[8]:
        bits.append('body ' + f[8])
    if f[10]:
        bits.append('callee ' + f[10])
    return '; '.join(bits)


out, cnt = [], Counter()
for rva, fs in rows.items():
    ev = next((f for f in fs if f[2] in ('asi', 'asi+exe', 'exe (file=.md)')), None)
    exe = next((f for f in fs if f[2] == 'exe'), None)
    if ev is None or ev[7] == 'FILE-MISSING':
        ev = exe
        exe = None
    cnt[ev[7]] += 1
    out.append(f'| `0x{rva}` | {fs[0][1]} | `{ev[3].split("mashed_re/")[-1]}:{ev[6]}` | {ev[4]} | '
               f'{cell(ev)} | {cell(exe) if exe else "-"} |')
print('| RVA | hooks.csv name | evidence copy (file:lines) | evidence date | verdict | exe copy |')
print('|---|---|---|---|---|---|')
print('\n'.join(out))
print()
print(dict(cnt), 'rows', len(rows))
