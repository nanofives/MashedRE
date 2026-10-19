"""u9764 final tracker transaction (re-classify, no C-level change): U-9764 corrected + RESOLVED, new U-9765, one CHANGELOG
line. Supersedes the partial apply_trackers.py (8eb8c0fe0), whose U-9764 text named a withdrawn cause. Line endings kept.
usage: py -3.12 apply_trackers2.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9764_20261019/RESULT_u9764.md'
U64 = ('| U-9764 | ~~semantic~~ **RESOLVED 2026-10-19** (u9764: `RwMatrixRotate` axis normalize) | car 1\'s wheel 1 force x `+0x2d8` at '
       'call 3 (A6a block #4 addend `30.5746307` port, `30.5746327` original) | **RESOLVED 2026-10-19 (u9764, ' + R + '):** the first '
       'differing input is the spin branch\'s `le0` (port `0.0337664969`, original `0.0337665901`). `0x004c4d20` normalizes the axis '
       'through the LUT inverse sqrt (`call 0x004c3b90` at `0x004c4d5d`); the port\'s `Math/RwMatrixRotate.cpp` calls `FastInvSqrt`, '
       'which in the standalone returns `1/sqrt(x)`. `MASHED_ROTATE_LUT=1` (default OFF) makes block #4 exact through call 4 and '
       '`+0x2d8` = o_wobj2 at calls 3-4; (b) A 2/3 B 3/3 C 0/3, 13 tracks 0 RED, D2 0/3 (slip1500 `0.1826`), so not a default. The '
       'partial RESULT\'s "steered axis y" cause was a probe artifact (mid-function `Interceptor.attach` at `0x00468320`) and is '
       'withdrawn. | (none) | (none) | D3 (b) car-level parity |')
U65 = ('| U-9765 | semantic | car 1\'s body matrix at call 4 on `MASHED_ROTATE_LUT` (`+0x93c` up.y, `+0x948 / +0x950` at.x / at.z) | '
       'With `MASHED_ROTATE_LUT=1` car 1 matches o_wobj2 through call 3 outside read-time fields; at call 4 the original\'s `up.y` is '
       '`1.00000024` where the port\'s is exactly 1, and `at.x / at.z` are 1 ulp off. `MASHED_RW_LUT_ALL=1` (every `RwLutGuard` caller '
       'on the in-process LUT) leaves the record unchanged through call 7, so the gap is not a sqrt fallback. `recpos.py`: velocity '
       'differs from call 5, position from call 7. ' + R + ' | Which writer of `+0x928..+0x950` at call 4 differs, and how | '
       '`--watch-write 93c` on the original (verify the run against o_wobj2 first), then compare the writer\'s arithmetic with the '
       'port\'s | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9765 |' not in s and s.count('| U-9764 |') == 1
    i = s.index('| U-9764 |'); j = s.index(nl, i)
    return s[:i] + U65 + nl + U64 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  004c4d20  u9764  U-9764 RESOLVED: car 1\'s call-3 +0x2d8 gap is RwMatrixRotate\'s axis normalize (the original '
            'calls the LUT inverse sqrt 0x004c3b90, the standalone FastInvSqrt is exact); MASHED_ROTATE_LUT (default OFF) exact through '
            'call 4, (b) unchanged, 13 tracks 0 RED, D2 0/3, not a default; MASHED_RW_LUT_ALL (default OFF) adds nothing at car 1; the '
            'partial RESULT\'s axis-y cause withdrawn (probe artifact); new U-9765 (body matrix at call 4). No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
