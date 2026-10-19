"""u9766 tracker transaction (re-classify, no C-level change): U-9766 RESOLVED, new U-9767, one CHANGELOG line. Line endings
kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9766_20261019/RESULT_u9766.md'
U66 = ('| U-9766 | ~~semantic~~ **RESOLVED 2026-10-19** (u9766: A6a `lc0` sum order + `RwMatrixRotate` x87 precision) | car 1\'s torque '
       '`+0x9c0` at call 5 on `MASHED_ROTATE_LUT` + `MASHED_ORTHO_LUT` | **RESOLVED 2026-10-19 (u9766, ' + R + '):** A6a\'s tail writes '
       '`+0x9c0` at `0x004685a2`, A6b halves it at `0x0046892b`; the first gap is wheel 0\'s block-#4 `lc0`, summed in the original\'s '
       'order by `MASHED_A6A_LC0=1` (`0x0046819f..0x004681c3`). The next, the call-27 steered axis, is `RwMatrixRotateInner`\'s '
       'inline x87 stream running at the standalone\'s default precision instead of PC_24; `MASHED_ROTATE_PC24=1` (default OFF). With '
       '`ROTATE_LUT` + `ORTHO_LUT` + `A6A_LC0` + `ROTATE_PC24` car 1 equals o_wobj2 in every physics field through call 66. Not a '
       'default: (b) B 2/3 (car 2 fails), D2 0/3. | (none) | (none) | D3 (b) car-level parity |')
U67 = ('| U-9767 | semantic | car 1\'s frame-start forward at call 67 on the four-knob stack (`+0x9d4`, the unsteered wheels\' axis) | '
       'Through call 66 both sides\' A5 Phase 0 forward equals the previous call\'s `+0x948`. At call 67 neither does: the original '
       'uses `-0.010067312`, the port `-0.00835652556`, both dumps holding `+0x948 = -0.00666628964` at the end of call 66; then 108 '
       'fields split at once. The integer AI controls stay equal until record call 98. ' + R + ' | Which writer sets the call-67 '
       'frame-start pose, and whether the chunk / substep count changes there | `--watch-write 9d4` on the original (check the run '
       'against o_wobj2 first; two-slot watches have frozen car 1), and the port\'s chunk / substep count at call 67 | D3 (b) '
       'car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9767 |' not in s and s.count('| U-9766 |') == 1
    i = s.index('| U-9766 |'); j = s.index(nl, i)
    return s[:i] + U67 + nl + U66 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00467650  u9766  U-9766 RESOLVED: car 1\'s call-5 torque gap is A6a block #4\'s lc0 sum order (MASHED_A6A_LC0), '
            'the call-27 axis gap is RwMatrixRotateInner\'s inline x87 running off PC_24 in the standalone (new MASHED_ROTATE_PC24, default '
            'OFF); with ROTATE_LUT + ORTHO_LUT + A6A_LC0 + ROTATE_PC24 car 1 equals o_wobj2 through call 66; not a default ((b) B 2/3, D2 '
            '0/3); new U-9767 (call-67 frame-start forward). No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
