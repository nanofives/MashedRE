"""ccinv tracker transaction (re-classify, no C-level change): flip notes on U-9769 and S-3442, one CHANGELOG line. Line endings
kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/ccinv_20261019/RESULT_ccinv.md'
NOTE = (' ccinv 2026-10-19 (user decision): `MASHED_CC_MATRIX_INVERT` DEFAULT ON (`MASHED_NO_CC_MATRIX_INVERT=1` opts out); '
        'G-CTRL `66d51720 / ca4284be / 1193bbd1`, (b) A 1/3 B 3/3 C 0/3, D2 3/3, 13 tracks 0 RED; ' + R + '.')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def append_last_cell(s, nl, key):
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    k = row.rstrip().rfind('|')
    return s[:i] + row[:k].rstrip() + NOTE + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  004c4dc0  ccinv  MASHED_CC_MATRIX_INVERT DEFAULT ON (user decision, accepting (b) A 2/3 -> 1/3): the '
            'car<->car spin tail calls RwMatrixInvertEntry instead of reading an uninitialised local; opt-out MASHED_NO_CC_MATRIX_INVERT '
            'restores the stub. G-CTRL 66d51720 / ca4284be / 1193bbd1 (opt-out 3b3690f6 / 9a657924 / a386f43b), B dumps d6d87a6c / '
            'b903bddb / 0d23fb1f, D2 3/3, 13 tracks 868 rows 0 RED. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', lambda s, nl: append_last_cell(s, nl, '| U-9769 |'))
rw('STUBS.md', lambda s, nl: append_last_cell(s, nl, '| ~~S-3442~~ |'))
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
