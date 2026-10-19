"""u9765 tracker transaction (re-classify, no C-level change): U-9765 RESOLVED, new U-9766, one CHANGELOG line. Line endings
kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9765_20261019/RESULT_u9765.md'
U65 = ('| U-9765 | ~~semantic~~ **RESOLVED 2026-10-19** (u9765: the `0x004c4680` re-orthonormalize) | car 1\'s body matrix at call 4 on '
       '`MASHED_ROTATE_LUT` (`+0x93c` up.y, `+0x948 / +0x950` at.x / at.z) | **RESOLVED 2026-10-19 (u9765, ' + R + '):** '
       '`FUN_0046e9e0` calls `0x004c4680(ebx, ebx)` at `0x0046ef5e`, which scales each row by the LUT inverse sqrt `0x004c3b90`, so '
       'the original\'s rows are not unit (o_w93c: up.y `1.00000012` after the step, `1.00000024` after the ortho). The port\'s '
       '`BodyOrientationIntegrate.cpp` `MatrixOrthoNormalize` uses `1/std::sqrt`. `MASHED_ORTHO_LUT=1` (default OFF, `0x004c4680` as '
       'disassembled) puts car 1\'s first velocity difference at call 21 (was 5) and position at call 34 (was 7); not a default: '
       '(b) regime B car 3 fails, D2 0/3 (slip2000 `0.2618`, drvmed `1788.53` out). | (none) | (none) | D3 (b) car-level parity |')
U66 = ('| U-9766 | semantic | car 1\'s torque `+0x9c0` at call 5 on `MASHED_ROTATE_LUT` + `MASHED_ORTHO_LUT` | With both knobs car 1\'s '
       'record equals o_wobj2 outside read-time fields until `+0x9c0` at call 5 (`-0.000898641185` original, `-0.000898640312` port), '
       'then `+0x9e8` at call 16, velocity x at 21, the body matrix at 28 (`firstdiff.py`). Separately, the same knobs make (b) '
       'regime B car 3 fail and D2 drive slower, so a second infidelity offsets the exact normalize. ' + R + ' | Which writer of '
       '`+0x9c0` at call 5 differs, and how | `--watch-write 9c0` on the original (verify the run against o_wobj2 first), then '
       'compare the writer\'s arithmetic with the port\'s | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9766 |' not in s and s.count('| U-9765 |') == 1
    i = s.index('| U-9765 |'); j = s.index(nl, i)
    return s[:i] + U66 + nl + U65 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  004c4680  u9765  U-9765 RESOLVED: car 1\'s call-4 body matrix gap is the re-orthonormalize (0x004c4680 scales '
            'rows by the LUT inverse sqrt, the port used 1/std::sqrt); MASHED_ORTHO_LUT (default OFF) holds car 1\'s velocity to call 21 '
            'and position to call 34 (was 5 / 7); not a default: (b) regime B car 3 fails, D2 0/3; new U-9766 (torque +0x9c0 at call 5). '
            'No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
