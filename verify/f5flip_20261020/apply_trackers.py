"""f5flip tracker transaction (re-classify, no C-level change): flip note on U-9774, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/f5flip_20261020/RESULT_f5flip.md'
G = ('G-CTRL `df5ee50e / a90bbb16 / b3cd0e5f` (five opt-outs `826277f2 / 250c1569 / 2c8164b2`, MASHED_NO_FAITH=1 `66d51720 / ca4284be / '
     '1193bbd1`), B `c165d2e5 / c01e164a / 2e937af7`, (b) A 2/3 B 2/3 C 1/3 (unchanged), D2 3/3 (0.1900 / 0.2550 / 1965.35), 13 tracks '
     '870 rows 0 RED')
NOTE = (' f5flip 2026-10-20 (Mariano\'s decision): `MASHED_A6A_SPIN_ORDER`, `MASHED_TER_SPIN_F32`, `MASHED_FIXUP_F32`, '
        '`MASHED_FIXUP_SUM_ORDER`, `MASHED_FIXUP_SLIDE_ORDER` DEFAULT ON in the FAITH seed (opt-out `MASHED_NO_<name>=1`, all of FAITH '
        '`MASHED_NO_FAITH=1`); `MASHED_COLL_LUT` stays OFF (U-9775); ' + G + '; ' + R + '.')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    k = '| U-9774 |'
    assert s.count(k) == 1
    i = s.index(k); j = s.index(nl, i); row = s[i:j]
    c = row.rstrip().rfind('|')
    return s[:i] + row[:c].rstrip() + NOTE + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-20  0046ef70  f5flip  u9774\'s five knobs DEFAULT ON (Mariano\'s decision) via the FAITH seed; MASHED_COLL_LUT '
            'stays OFF. ' + G + '. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
