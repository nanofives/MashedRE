"""a6flip tracker transaction (re-classify, no C-level change): U-9760 / U-9761 / U-9762 flip notes, one CHANGELOG line.
usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/a6flip_20261019/RESULT_a6flip.md'
CTRL = '3b3690f6 / 9a657924 / a386f43b'; OPT = '726b8fab / b048d6f0 / ecc20282'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def append(s, nl, key, note):
    assert s.count(key) == 1, key
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def uncert(s, nl):
    for key, knob in (('| U-9760 |', '`MASHED_A6A_LUT` (opt-out `MASHED_NO_A6A_LUT=1`)'),
                      ('| U-9761 |', '`MASHED_A6A_F32` (opt-out `MASHED_NO_A6A_F32=1`)'),
                      ('| U-9762 |', '`MASHED_A5_SUSP_LUT` (opt-out `MASHED_NO_A5_SUSP_LUT=1`)')):
        s = append(s, nl, key, ' **Flipped 2026-10-19 (a6flip, user decision, ' + R + '):** ' + knob + ' is default ON together '
                   'with the other two A5 / A6a knobs; (b) A 2/3 B 3/3 C 0/3, D2 3/3 on the new gate, 13 tracks 0 RED. Default G-CTRL `'
                   + CTRL + '`; all three opt-outs `' + OPT + '`.')
    return s


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00467650  a6flip  MASHED_A6A_LUT + MASHED_A6A_F32 + MASHED_A5_SUSP_LUT default ON by user decision (A6a LUT '
            'lengths, A6a chain at float precision, A5 original arithmetic); opt-outs MASHED_NO_*. G-CTRL ' + CTRL + ' (all opt-outs '
            + OPT + '); (b) A 2/3 B 3/3 C 0/3; D2 3/3 on the d2rebase gate; 13 tracks 870 rows 0 RED. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
