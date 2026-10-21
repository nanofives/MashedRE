"""rbflip tracker transaction (default flip, no C-level change): a note on U-9777, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/rbflip_20261021/RESULT_rbflip.md'
NOTE = (" rbflip 2026-10-21 (Mariano's decision): `MASHED_A5_RUBBER_LIVE` DEFAULT ON through the FAITH seed (opt-out "
        "`MASHED_NO_A5_RUBBER_LIVE=1`); G-CTRL `15594ec8 / 7285d708 / 26a6e4ad` x2, opt-out `b9fb3f25 / 8740221b / b2e4b612`, B "
        "`7ab70c5f / 15262e4a / d1b4bc64`, (b) A 3/3 B 2/3 C 3/3, D2 3/3, 13 tracks 873 rows 0 RED; " + R + ".")


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    k = '| U-9777 |'
    assert s.count(k) == 1
    i = s.index(k); j = s.index(nl, i); row = s[i:j]
    c = row.rstrip().rfind('|')
    return s[:i] + row[:c].rstrip() + NOTE + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ("2026-10-21  00442c80  rbflip  MASHED_A5_RUBBER_LIVE default ON (Mariano's decision): G-CTRL 15594ec8 / 7285d708 / "
            "26a6e4ad x2, opt-out b9fb3f25 / 8740221b / b2e4b612, NO_FAITH 66d51720 unchanged, (b) A 3/3 B 2/3 C 3/3 (A car 3 now "
            "passes, no car lost), D2 3/3, 13 tracks 873 rows 0 RED. No C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
