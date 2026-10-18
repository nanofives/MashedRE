"""u9747 flip transaction (re-classify, no C-level change): a note on U-9747 (MASHED_COUNTDOWN_SPAWN default ON by user
decision), one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found.
usage: py -3.12 apply_trackers_flip.py <default trio> <opt-out trio>"""
import os, sys

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEF, OPT = sys.argv[1], sys.argv[2]


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def uncert(s, nl):
    key = '| U-9747 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    note = (' **2026-10-18 (u9747 flip):** `MASHED_COUNTDOWN_SPAWN` default ON by user decision; `MASHED_NO_COUNTDOWN_SPAWN=1` '
            'opts out. G-CTRL default `' + DEF + '` x2, opt-out `' + OPT + '`.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-18  00418860  u9747 flip  MASHED_COUNTDOWN_SPAWN default ON (user decision; opt-out '
            'MASHED_NO_COUNTDOWN_SPAWN=1), no C-level change. G-CTRL default ' + DEF.replace(' ', '') + ' x2, opt-out '
            + OPT.replace(' ', '') + '; (b) A 2/3 B 2/3 C 0/3, D2 3/3, 869 rows 0 RED (verify/u9747_20261018/RESULT_u9747.md).')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
