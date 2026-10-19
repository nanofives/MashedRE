"""fwdsse tracker transaction (re-classify, no C-level change): U-9758 and U-9759 flip notes, one CHANGELOG line.
Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py <new default trio> <both opt-outs trio>"""
import os, sys

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/fwdsse_20261019/RESULT_fwdsse.md'
CTRL, OPT = sys.argv[1], sys.argv[2]


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def append(s, nl, key, note):
    assert s.count(key) == 1, key
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def uncert(s, nl):
    s = append(s, nl, '| U-9758 |', ' **Flipped 2026-10-19 (fwdsse, user decision, ' + R + '):** `MASHED_FWD_AT` is default ON '
               'together with `MASHED_RW_SSE_POINTS`; `MASHED_NO_FWD_AT=1` opts out. Default G-CTRL `' + CTRL + '`; both opt-outs `'
               + OPT + '`.')
    return append(s, nl, '| U-9759 |', ' **Flipped 2026-10-19 (fwdsse, user decision, ' + R + '):** `MASHED_RW_SSE_POINTS` is '
                  'default ON together with `MASHED_FWD_AT`; `MASHED_NO_RW_SSE_POINTS=1` opts out. Default G-CTRL `' + CTRL +
                  '`; both opt-outs `' + OPT + '`.')


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  004dc370  fwdsse  MASHED_FWD_AT (record +0x9d4 = basis at row) and MASHED_RW_SSE_POINTS (slot +0xc order of '
            '0x004dc370) default ON by user decision, one combined gate run; opt-outs MASHED_NO_FWD_AT=1 / MASHED_NO_RW_SSE_POINTS=1. '
            'G-CTRL ' + CTRL + ' (both opt-outs ' + OPT + '); (b) pass set kept, D2 3/3, 13 tracks 0 RED. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
