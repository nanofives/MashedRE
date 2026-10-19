"""u9762 tracker transaction (re-classify, no C-level change): U-9762 RESOLVED, new U-9763, one CHANGELOG line.
Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9762_20261019/RESULT_u9762.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9763 | semantic | the player\'s D2 metrics against the faithful A5 / A6a arithmetic, and car 1\'s next fields | With '
       '`MASHED_A6A_LUT` + `MASHED_A6A_F32` + `MASHED_A5_SUSP_LUT` car 1 is bit-exact against o_wobj2 through call 4 (position '
       'and velocity, `recpos.py`), but D2 leaves its band: slip1500 0.1859 (< 0.18855) and driving-median 1989.05-1990.80 '
       '(> 1982.44). Each faithful arithmetic fix so far moved the player\'s D2 (U-9757 `_DAT_005cea44`, U-9761 the A6a lengths '
       'alone, here A5). Car 1\'s first differing fields with the triple: wheel 1\'s force x `+0x2d8` at call 3 (-28.2014942 / '
       '-28.2014961), velocity from call 5 (dvz 1.2e-4). ' + R + ' | Whether the D2 band was fitted against a port with these '
       'arithmetic deviations (a compensating deviation elsewhere on the player\'s path), or the player has a separate gap | '
       'Re-measure the D2 reference metrics on the original with the same recipe and compare the port\'s per-substep player '
       'record with all three knobs; watch `+0x2d8` on the original for car 1 | D2 bounds; D3 (b) car-level parity |')
N9762 = (' **RESOLVED 2026-10-19 (u9762, ' + R + '):** `+0x460` is wheel 3\'s force x; its call-2 gap is A5\'s arithmetic '
         '(split log `MASHED_FORCE_SPLIT_WHEEL=3`): with `MASHED_A5_SUSP_LUT` the A5 output equals the original\'s and car 1 is '
         'bit-exact through call 4 with the u9761 pair. The triple fails D2 (slip1500 0.1859, driving-median ~1990), so the pair '
         'stays the candidate. Next U-9763.')


def uncert(s, nl):
    key = '| U-9762 | semantic |'
    assert s.count(key) == 1 and '| U-9763 |' not in s
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    row = row.replace(key, '| U-9762 | ~~semantic~~ **RESOLVED 2026-10-19** (u9762: A5 arithmetic, wheel 3) |', 1)
    row = row.rstrip()[:-1].rstrip() + N9762 + ' |'
    return s[:i] + ROW + nl + row + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  0046ddb0  u9762  U-9762 RESOLVED: +0x460 (wheel 3 force x) at call 2 is A5\'s arithmetic; with '
            'MASHED_A5_SUSP_LUT + the u9761 pair car 1 is bit-exact through call 4 (recpos.py), but the triple fails D2 '
            '(slip1500 0.1859, driving-median ~1990); new U-9763. Diagnostic MASHED_FORCE_SPLIT_WHEEL. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
