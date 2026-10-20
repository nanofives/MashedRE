"""trinflip tracker transaction (default flip, no C-level change): a note on U-9776, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/trinflip_20261020/RESULT_trinflip.md'
NOTE = (' trinflip 2026-10-20 (Mariano\'s decision, after wheelcite C27b): `MASHED_WCS_TRINORMAL` DEFAULT ON through the FAITH seed '
        '(opt-out `MASHED_NO_WCS_TRINORMAL=1`); G-CTRL `91cc1c0b / 2d77a5d7 / ab6c9329` x2, opt-out `df5ee50e / a90bbb16 / b3cd0e5f`, '
        'B `28a76cf2 / 76a0b4cd / 040ce640`, (b) A 1/3 B 2/3 C 2/3, D2 3/3 and 13 tracks 0 RED carried from wcsflip; ' + R + '.')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    k = '| U-9776 |'
    assert s.count(k) == 1
    i = s.index(k); j = s.index(nl, i); row = s[i:j]
    c = row.rstrip().rfind('|')
    return s[:i] + row[:c].rstrip() + NOTE + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-20  0046f6c0  trinflip  MASHED_WCS_TRINORMAL default ON (Mariano\'s decision, after wheelcite C27b): G-CTRL '
            '91cc1c0b / 2d77a5d7 / ab6c9329 x2, opt-out df5ee50e / a90bbb16 / b3cd0e5f, (b) A 1/3 B 2/3 C 2/3 (A car 2 traded for C car 3), '
            'D2 3/3, 13 tracks 0 RED. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
