"""u9763 step 1 tracker note (re-classify, no C-level change): U-9763 note, one CHANGELOG line. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9763_20261019/RESULT_u9763_step1.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    key = '| U-9763 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    note = (' **2026-10-19 (u9763 step 1, ' + R + '):** the original re-measured with the band\'s own argv (4 runs, LIVING-PC): '
            'slip1500 0.1979 / 0.1979 / 0.1953 / 0.1984 (three above the band top 0.19635), slip2000 0.2515 / 0.2515 / 0.2503 / '
            '0.2517, driving-median 1937.06 / 1937.06 / 1971.67 / 1944.28, H1 398/400; with the four band runs (MARIANO-PC) the '
            'spread is 0.1911..0.1984 / 0.2498..0.2517 / 1937.06..1971.67. The triple (0.1859 / 0.2534 / ~1990) is outside it '
            'on all three metrics. The port scoring recipe has drifted from the pre-registration (TRACK_SEL 12, HOLD_AFTER 0, no '
            '--max-lines). No band changed.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  -  u9763  U-9763 step 1 (measurement only): the original re-measured with the D2 band\'s argv (4 runs, '
            'LIVING-PC) gives slip1500 up to 0.1984, above its own band; the eight-run spread does not contain the three-knob '
            'arm on any metric; port scoring recipe differs from the d81a8df6 pre-registration. No band or default changed. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
