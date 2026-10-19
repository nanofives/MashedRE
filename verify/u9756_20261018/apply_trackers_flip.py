"""f60flip tracker transaction (re-classify, no C-level change): U-9756 flip note, one CHANGELOG line.
Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers_flip.py <default G-CTRL trio> <opt-out trio>"""
import os, sys

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9756_20261018/RESULT_u9756.md'
CTRL, OPT = sys.argv[1], sys.argv[2]


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def uncert(s, nl):
    key = '| U-9756 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    note = (' **Flipped 2026-10-18 (f60flip, user decision, ' + R + ' section 5):** `_DAT_005cea60` = 0.2778f is default ON; '
            '`MASHED_NO_FIX_5CEA60=1` opts out. Default G-CTRL `' + CTRL + '`; opt-out `' + OPT + '`.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-18  0046f6c0  f60flip  _DAT_005cea60 = 0.2778f (kWTorque, kProbeScale) default ON by user decision; opt-out '
            'MASHED_NO_FIX_5CEA60=1. G-CTRL ' + CTRL + ' (opt-out ' + OPT + '); B dumps unchanged; (b), D2 3/3 and 13 tracks 0 RED '
            'per the u9756 F60 arms. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
