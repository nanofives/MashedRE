"""d2ref flip transaction (re-classify, no C-level change): U-9212, U-9720, U-9751 RESOLVED (MASHED_D2LAT_REFRESH,
MASHED_GRID_DIR, MASHED_A5_RACE_TIMER default ON by user decision), one CHANGELOG line. Writes with newline="" so
CRLF/LF stay as found. usage: py -3.12 apply_trackers_flip.py <default trio> <opt-out trio>"""
import os, sys

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEF, OPT = sys.argv[1], sys.argv[2]
NOTE = (' **2026-10-17 (d2ref flip):** default ON by user decision, together with the other two members (refresh, grid frame, '
        'A5 race timer); opt-outs `MASHED_NO_D2LAT_REFRESH=1`, `MASHED_NO_GRID_DIR=1`, `MASHED_NO_A5_RACE_TIMER=1`. G-CTRL default `'
        + DEF + '` x2, the three opt-outs `' + OPT + '`. Accepted trade: regime-B car 3 leaves its envelope (`c1#` 52 [17, 46]) '
        'as regime-B car 2 enters its own.')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def resolve(s, nl, uid, old_type, label):
    key = '| ' + uid + ' |'
    assert s.count(key) == 1, uid
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    old = '| ' + uid + ' | ' + old_type + ' |'
    assert row.count(old) == 1, (uid, old_type)
    row = row.replace(old, '| ' + uid + ' | ~~' + old_type + '~~ **RESOLVED 2026-10-17** (' + label + ') |', 1)
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + NOTE + ' |' + s[j:]


def uncert(s, nl):
    s = resolve(s, nl, 'U-9212', 'semantic', 'd2ref flip: the speed refresh ported and default ON')
    s = resolve(s, nl, 'U-9720', 'structural', 'd2ref flip: FUN_00408b00 grid frame default ON')
    s = resolve(s, nl, 'U-9751', 'structural', 'd2ref flip: A5 reads DAT_007f0ff8, default ON')
    return s


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-17  0046ddb0  d2ref flip  MASHED_D2LAT_REFRESH + MASHED_GRID_DIR + MASHED_A5_RACE_TIMER default ON (user '
            'decision, accepting the regime-B car 3 / car 2 trade); opt-outs MASHED_NO_*; no C-level change; U-9212 / U-9720 / '
            'U-9751 RESOLVED. G-CTRL default ' + DEF.replace(' ', '') + ' x2, opt-outs ' + OPT.replace(' ', '') +
            '; D2 3/3, 869 rows 0 RED (verify/d2ref_20261017/RESULT_d2ref.md).')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
