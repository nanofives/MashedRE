"""cpcflip tracker transaction (default flip, no C-level change): a note on U-9776, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/cpcflip_20261020/RESULT_cpcflip.md'
NOTE = (" cpcflip 2026-10-20 (Mariano's decision): `MASHED_WCS_COMBINE_PC24` DEFAULT ON through the FAITH seed (opt-out "
        "`MASHED_NO_WCS_COMBINE_PC24=1`); G-CTRL `b9fb3f25 / 8740221b / b2e4b612` x2, opt-out `91cc1c0b / 2d77a5d7 / ab6c9329`, B "
        "`c4c2ae26 / b0f27763 / 6c35e33b`, (b) A 2/3 B 2/3 C 3/3, D2 3/3, 13 tracks 868 rows 0 RED; " + R + ".")


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
    line = ("2026-10-20  004c52f0  cpcflip  MASHED_WCS_COMBINE_PC24 default ON (Mariano's decision): G-CTRL b9fb3f25 / 8740221b / "
            "b2e4b612 x2, opt-out 91cc1c0b / 2d77a5d7 / ab6c9329, (b) A 2/3 B 2/3 C 3/3 (A car 3 traded for A cars 1, 2 and C car 2), "
            "D2 3/3, 13 tracks 0 RED. No C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
