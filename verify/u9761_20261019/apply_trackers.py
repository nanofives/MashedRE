"""u9761 tracker transaction (re-classify, no C-level change): U-9761 RESOLVED, new U-9762, one CHANGELOG line.
Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9761_20261019/RESULT_u9761.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9762 | semantic | car 1\'s record `+0x460` at call 2, and the A4 torque ring\'s index | With `MASHED_A6A_LUT` + '
       '`MASHED_A6A_F32` car 1\'s record equals o_wobj2 at call 1 except the A4 torque ring index (`rec + 0x1ac / 0x270 + '
       '(DAT_007f101c & 0xf)*4`: the call-1 value at index 13 in the original, 10 in the port, `g_torqueRingPhase`), pad dwords '
       'and the forward\'s one-call dump lag; the first physics field that differs is `+0x460` at call 2 (-0.04996714 / '
       '-0.0499671251), and the call-21 drift is 1.164e-4. ' + R + ' | Which writer sets `+0x460` and which input differs; '
       'whether the torque ring\'s consumer reads it by index (then the phase matters) | Write-watch car 1 `+0x460` on the '
       'original, compare with the port at call 2; find the reader of `rec + 0x1ac..` | D3 (b) car-level parity before the first '
       'contacts |')
N9761 = (' **RESOLVED 2026-10-19 (u9761, ' + R + '):** `MASHED_A6A_F32` (default OFF) computes A6a\'s torque / grip chain in float '
         '(x87 PC_24); with `MASHED_A6A_LUT` too, car 1\'s omega `+0x9c0` at call 1 equals o_wobj2, and the D2 conflict resolves: '
         'F32 alone slip1500 0.1863 OUT, LUT alone 0.1877 OUT, both 0.1887 in. Pair: (b) A 2/3 B 3/3 C 0/3, D2 3/3, 13 tracks 0 '
         'RED, G-CTRL `bbe1bfd5 / a054388e / 9f599ae9` (default candidate). Next U-9762.')


def uncert(s, nl):
    key = '| U-9761 | semantic |'
    assert s.count(key) == 1 and '| U-9762 |' not in s
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    row = row.replace(key, '| U-9761 | ~~semantic~~ **RESOLVED 2026-10-19** (u9761: A6a chain at float precision) |', 1)
    row = row.rstrip()[:-1].rstrip() + N9761 + ' |'
    return s[:i] + ROW + nl + row + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00467650  u9761  U-9761 RESOLVED: MASHED_A6A_F32 (default OFF) puts A6a\'s torque/grip chain at float '
            'precision; with MASHED_A6A_LUT car 1\'s call-1 omega is exact and D2 slip1500 returns to the band (0.1887); pair '
            'candidate: (b) B 3/3, D2 3/3, 13 tracks 0 RED. New U-9762 (+0x460 at call 2, torque ring index). No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
