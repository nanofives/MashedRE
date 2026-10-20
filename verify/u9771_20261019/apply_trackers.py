"""u9771 tracker transaction (re-classify, no C-level change): U-9771 RESOLVED (car 3's call 7; car 3's call 55 and car 2's call
136 move to U-9772), new U-9772, one CHANGELOG line. Line endings kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9771_20261019/RESULT_u9771.md'
U71 = ('| U-9771 | ~~semantic~~ **RESOLVED 2026-10-19** (u9771: car 3\'s call 7 is A6a\'s gear pre-factor association) | cars 2 and 3 '
       'on `FULLRL` | **RESOLVED 2026-10-19 (u9771, ' + R + '):** the original computes `fVar4 = (c498 * 0.5) * (fVar5 * k)`, '
       '`k = _DAT_005ce1e8` (`0x0046768b..0x00467739`); the port had `((c498 * 0.5) * fVar5) * k`, which moved car 3\'s `local_cc` by '
       '1 ulp in frame 7 (`8586.67383` vs `8586.6748`) and through it `+0xb14 / +0xb1c` and the velocity. `MASHED_A6A_GEAR_ORDER` '
       '(default OFF) holds car 3 equal through call 54. Not a default: D2 slip2000 `0.2574` out of band on 3/3. Car 3\'s call 55 '
       'and car 2\'s call 136 move to U-9772. | (none) | (none) | D3 (b) car-level parity |')
U72 = ('| U-9772 | semantic | car 3\'s record call 55 and car 2\'s call 136 on `FULLRL` + `MASHED_A6A_GEAR_ORDER` | Car 3: `+0x9b0` '
       '-518.006104 (original) vs -515.64032, `+0x9b8` -1768.26038 vs -1760.0874, position, at call 55 after 0..54 equal. Car 2: '
       '`+0x9b0` -391.180573 vs -390.790558 at call 136. Both are single-call jumps of 0.2..0.5%, not ulp drift. ' + R +
       ' | Which writer of the velocity differs on that call (A5, the A6a tail, the A6a lateral write `0x0046885a`, the fixup) | '
       '`--a5-probe` / `--a6t-probe` / `MASHED_A6A_TAILLOG` / `MASHED_A6A_VZLOG` on that car (u9771 tools) | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9772 |' not in s and s.count('| U-9771 |') == 1
    i = s.index('| U-9771 |'); j = s.index(nl, i)
    return s[:i] + U72 + nl + U71 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00467650  u9771  U-9771 RESOLVED: car 3\'s call-7 split is A6a\'s gear pre-factor association '
            '(0x0046768b..0x00467739: (c498*0.5)*(fVar5*k)); MASHED_A6A_GEAR_ORDER (default OFF) holds car 3 equal through call 54; '
            'alone it keeps the (b) verdicts but D2 slip2000 0.2574 is out of band 3/3, so no default. MASHED_A5_DRAFT_LUT and '
            'MASHED_A6A_DRIVE_NORM_LUT (default OFF) are inert on this recipe. New U-9772. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
