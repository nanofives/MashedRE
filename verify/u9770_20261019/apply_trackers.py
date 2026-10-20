"""u9770 tracker transaction (re-classify, no C-level change): U-9770 RESOLVED (car 1's part; cars 2 / 3 move to U-9771),
new U-9771, one CHANGELOG line. Line endings kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9770_20261019/RESULT_u9770.md'
U70 = ('| U-9770 | ~~semantic~~ **RESOLVED 2026-10-19** (u9770: car 1\'s row 149 is the pickup tick\'s frame order) | the next '
       'per-car divergences on the full stack | **RESOLVED 2026-10-19 (u9770, ' + R + '):** the original\'s car 1 holds the '
       'pickup at row 149 (held word `0x0088fc88 + v*0xb4` = `0x5f9b58`, so FUN_00414490 and FUN_00414c30 return 0), written at '
       '`0x0045bfb5` from FUN_00485070 (`0x0040fd07`), which FUN_0040fc00 runs after FUN_00425a40 (`0x0040fc6c`) like FUN_0040da50 '
       '(`0x0040fc75`) and FUN_0045bba0 (`0x0040fcd0`); the port ran them before the AI tick. `MASHED_REG_LATE` (default OFF) on the '
       'full stack: car 1 equals o_reg_r19 on all 196 C rows (regime C PASS) and o_reg_r1 through row 179. Cars 2 / 3 move to '
       'U-9771. The stack-dependence note of this row was closed by the ccinv flip. | (none) | (none) | D3 (b) car-level parity |')
U71 = ('| U-9771 | semantic | cars 2 and 3 on `FULLRL` (u9770: d3bc full stack + M4 mode-7 set + `MASHED_CC_PC24` + `MASHED_A4_ORDER` + '
       '`MASHED_REG_LATE`) | Car 2: record call 136, `+0x9b8` -3311.94531 (original) vs -3308.72559, position and the wheel-ring '
       'fields `+0x194 / +0x214..+0x21c / +0x2d8..` differ, calls 0..135 equal apart from one-call one-ulp blips; commands equal '
       'through row 138. Car 3: record call 7, `+0x9b8` -1964.98584 vs -1964.98596 and nothing else; commands through row 57. ' + R +
       ' | The term of each car\'s velocity that differs on that call | `MASHED_FORCE_SPLIT` on slot 3 (call 7) and slot 2 (call 136) '
       'against `--a6a-probe-wheel` original captures of those cars; `--fixup-probe` if the fixup writes it | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9771 |' not in s and s.count('| U-9770 |') == 1
    i = s.index('| U-9770 |'); j = s.index(nl, i)
    return s[:i] + U71 + nl + U70 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00485070  u9770  U-9770 RESOLVED: car 1\'s row-149 split is the pickup collection one frame late in the '
            'port (FUN_0040fc00 runs FUN_0040da50 / FUN_0045bba0 / FUN_00485070 after FUN_00425a40; the port ran them before the AI '
            'tick); MASHED_REG_LATE (default OFF) on the full stack makes car 1 pass regime C (196/196 rows equal) and A. New U-9771 '
            '(car 2 call 136, car 3 call 7). No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
