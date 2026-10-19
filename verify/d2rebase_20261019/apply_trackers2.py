"""d2rebase decision tracker note (re-classify, no C-level change): U-9763 note, one CHANGELOG line. usage: py -3.12 apply_trackers2.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/d2rebase_20261019/RESULT_d2rebase.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    key = '| U-9763 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    note = (' **2026-10-19 decision (Mariano, ' + R + ' section 3):** the D2 gate scores the port on the whole log with the 8-run '
            'band (`score_d2_v2.py port`). On that gate the triple, the pair and the current default all pass D2 3/3, so the '
            'conflict this row records (faithful arithmetic moving D2 out of band) came from the d81a8df6 band and the port '
            'window, not from the port.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  -  d2rebase  DECISION (Mariano): D2 port window = the whole log; gate = score_d2_v2.py port (whole log) + '
            'the 8-run band; default, pair and triple pass D2 3/3 on it. No default changed. ' + R + ' section 3')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
