"""u9767 tracker transaction (re-classify, no C-level change): U-9767 RESOLVED, new U-9768, one CHANGELOG line. Line endings
kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9767_20261019/RESULT_u9767.md'
U67 = ('| U-9767 | ~~semantic~~ **RESOLVED 2026-10-19** (u9767: the heading assist read a stale `+0x9d4`) | car 1\'s frame-start forward '
       'at call 67 on the four-knob stack (`+0x9d4`, the unsteered wheels\' axis) | **RESOLVED 2026-10-19 (u9767, ' + R + '):** call 67 '
       'is the first steer-0 frame; `FUN_0046c7d0` rotates the pose from a heading read off `+0x9d4`, which the original refreshes '
       'during the AI tick through `FUN_0046d510` (`0x0046d53a`, the slot-0 `+0x928` `at` row) and the port wrote only at A5\'s frame '
       'start. `MASHED_HEADING_FWD_LIVE=1` (default OFF) holds car 1 equal to o_wobj2 through call 104 on the stack; not a default: '
       '(b) B 0/3 on the stack, B 2/3 alone. | (none) | (none) | D3 (b) car-level parity |')
U68 = ('| U-9768 | semantic | car 1\'s AI lookahead target at record call 104 (aistep row 105) on the five-knob stack | With '
       '`ROTATE_LUT` + `ORTHO_LUT` + `A6A_LC0` + `ROTATE_PC24` + `HEADING_FWD_LIVE` car 1\'s physics equal o_wobj2 through call 104. At '
       'aistep row 105 the original\'s look point jumps from `(-1.271637, -14.80128)` to `(-1.009332, -20.2197)` and its steer error '
       '(`0x008032d8 + v*0x14`) from `359.3607` to `357.803` (steer `c1 19`); the port keeps `(-1.267988, -14.92525)`. Own position, '
       '`curv`, `march_n / march_idx0` and the spline index are equal. ' + R + ' | Which input of the target selection differs at that '
       'row | Compare the lookahead chain (`FUN_00443dc0` and its LUT form, `MASHED_AI_LUT`) term by term at row 105 with a '
       'callee-entry probe | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9768 |' not in s and s.count('| U-9767 |') == 1
    i = s.index('| U-9767 |'); j = s.index(nl, i)
    return s[:i] + U68 + nl + U67 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  0046c7d0  u9767  U-9767 RESOLVED: car 1\'s call-67 gap is the steer-0 heading assist reading +0x9d4 before the '
            'port refreshes it (the original\'s AI tick rewrites it through FUN_0046d510); MASHED_HEADING_FWD_LIVE (default OFF) holds '
            'car 1 equal to o_wobj2 through call 104 on the five-knob stack; not a default ((b) B 0/3 on the stack, 2/3 alone); new '
            'U-9768 (AI lookahead target at call 104). No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
