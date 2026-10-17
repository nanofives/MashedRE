"""c2k93 flip transaction (re-classify, no C-level change): U-9750 and U-9752 RESOLVED (MASHED_AI_CLOCK_LATE and
MASHED_AI_LUT=5 default ON by user decision), one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found."""
import os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEF = '262504cc / 39fc6210 / 6797ffe5'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def resolve(s, nl, uid, old_type, label, note):
    key = '| ' + uid + ' |'
    assert s.count(key) == 1, uid
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    old = '| ' + uid + ' | ' + old_type + ' |'
    assert row.count(old) == 1, uid
    row = row.replace(old, '| ' + uid + ' | ~~' + old_type + '~~ **RESOLVED 2026-10-17** (' + label + ') |', 1)
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def uncert(s, nl):
    s = resolve(s, nl, 'U-9750', 'structural', 'c2k93 flip: the add at the end of the frame, default ON by user decision',
                ' **2026-10-17 (c2k93 flip):** default ON; `MASHED_NO_AI_CLOCK_LATE=1` opts out. G-CTRL default `' + DEF +
                '` x2 (with U-9752), both opt-outs `a34714b4 / 0f8ef6df / 852a255d`.')
    s = resolve(s, nl, 'U-9752', 'semantic', 'c2k93 flip: RW LUT lookahead default ON by user decision',
                ' **2026-10-17 (c2k93 flip):** mask 5 default; `MASHED_NO_AI_LUT=1` opts out (exact sqrt), `MASHED_AI_LUT=<mask>` '
                'overrides. With only the LUT opt-out: `dd822489 / 35f5fea0 / da12bbe0` (the clock-late arm).')
    return s


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-17  00443dc0  c2k93 flip  MASHED_AI_CLOCK_LATE and MASHED_AI_LUT=5 default ON (user decision; opt-outs '
            'MASHED_NO_AI_CLOCK_LATE=1 / MASHED_NO_AI_LUT=1), no C-level change, U-9750 / U-9752 RESOLVED. G-CTRL default '
            + DEF.replace(' ', '') + ' x2; (b) A 2/3 B 2/3 C 0/3, D2 3/3, 869 rows 0 RED (verify/c2k93_20261017/RESULT_c2k93.md).')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
