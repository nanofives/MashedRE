"""syflip tracker transaction (re-classify, no C-level change): U-9755 flip note, one CHANGELOG line.
Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers_flip.py <default G-CTRL trio> <opt-out trio>"""
import os, sys

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9755_20261018/RESULT_u9755.md'
CTRL, OPT = sys.argv[1], sys.argv[2]


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def uncert(s, nl):
    key = '| U-9755 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    note = (' **Flipped 2026-10-18 (syflip, user decision, ' + R + ' section 5):** the spawn pose is default ON; '
            '`MASHED_NO_SPAWN_Y_LINE=1` opts out. Default G-CTRL `' + CTRL + '`; opt-out `' + OPT + '`.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-18  0046d400  syflip  spawn pose (FUN_004704c0: FUN_00408b00 grid point + FUN_0046d400 spawn y) default ON '
            'by user decision; opt-out MASHED_NO_SPAWN_Y_LINE=1. G-CTRL ' + CTRL + ' (opt-out ' + OPT + '); (b), D2 3/3 and 13 '
            'tracks 0 RED per the u9755 SY arms. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
