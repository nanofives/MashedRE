"""d2rebase tracker note (re-classify, no C-level change): U-9763 note, one CHANGELOG line. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/d2rebase_20261019/RESULT_d2rebase.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    key = '| U-9763 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    note = (' **2026-10-19 (d2rebase, ' + R + '):** 8-run band (same rule): slip1500 0.18396..0.20586, slip2000 0.24555..0.25557, '
            'driving-median 1893.63..1997.46; new scorer `verify/d2rebase_20261019/score_d2_v2.py`. On the register\'s arm (TRACK_SEL '
            '12, STEER_HOLD_AFTER 0) the registered `--max-lines 1080` gives the port ~70% of the original\'s driving window (774-871 '
            'samples vs 1153-1185) and every arm, the default included, fails driving-median (1855-1879); on the whole log all '
            'three arms (default, pair, triple) pass 3/3. The window choice is a user decision.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  -  d2rebase  D2 re-baseline: 8-run band (slip1500 0.18396..0.20586, slip2000 0.24555..0.25557, '
            'driving-median 1893.63..1997.46), score_d2_v2.py; registered --max-lines 1080 shortens the port window (all arms FAIL '
            'driving-median), whole log: default, pair and triple PASS 3/3; window choice pending; no default changed. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
