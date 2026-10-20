"""wheelcite tracker transaction (no C-level change): a note on U-9776 (the RED rows are wheel steer, C27b), one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/wheelcite_20261020/RESULT_wheelcite.md'
NOTE = (' wheelcite 2026-10-20 (corrects the wcsflip note): the 3 RED rows are front-WHEEL draws (96-triangle `Advantage`), not car bodies; '
        'every wheel sits at the original\'s world position (|dt| <= 4.4e-7) and only the steer differs (-18.05 / +15.54 / +14.09 deg), '
        'which `MASHED_CAR_REPLAY` does not replay (the wheel frames keep the standalone\'s AI state). New cite `C27b-CAR-WHEEL-ROT` '
        '(`max_dpos` 0.001, `tri` 96): wcsflip 13 tracks 869 rows 0 RED, the f5flip default 870 rows 0 RED (4 rows C27 -> C27b); ' + R + '.')


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
    line = ('2026-10-20  0046f6c0  wheelcite  the wcsflip 13-track RED rows are front-wheel steer (wheel frames are not replayed), not '
            'car bodies; cite C27b-CAR-WHEEL-ROT (drawlist3d max_dpos / tri predicates) puts wcsflip at 0 RED and leaves the default '
            '0 RED. Instrument change only, no default change, no C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
