"""wcsflip tracker transaction (no flip, no C-level change): a note on U-9776 (the blocked flip), one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/wcsflip_20261020/RESULT_wcsflip.md'
NOTE = (' wcsflip 2026-10-20: the default-ON flip of `MASHED_WCS_TRINORMAL` (Mariano\'s decision) was NOT applied: 13 tracks 3 RED '
        '(Roundabout 1091 / 1131, Training 959, all `MOVED Advantage`, car-body AABB moved by up to 0.026); D2 3/3, (b) A 1/3 B 2/3 '
        'C 2/3, G-CTRL `91cc1c0b / 2d77a5d7 / ab6c9329`; ' + R + '.')


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
    line = ('2026-10-20  0046f6c0  wcsflip  MASHED_WCS_TRINORMAL default-ON flip NOT applied: 13 tracks 3 RED (MOVED Advantage, car-body '
            'pose moved ~0.02 on Roundabout 1091 / 1131 and Training 959); D2 3/3, (b) A 1/3 B 2/3 C 2/3, G-CTRL 91cc1c0b / 2d77a5d7 / '
            'ab6c9329; the default stays df5ee50e / a90bbb16 / b3cd0e5f. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
