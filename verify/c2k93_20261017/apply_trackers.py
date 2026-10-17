"""c2k93 tracker transaction (re-classify, no C-level change). New U-9752, U-9753 (newest-first under the table header), a
note on U-9212, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/c2k93_20261017/RESULT_c2k93.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROWS = [
    "| U-9753 | semantic | `Ai/AiStandalone.cpp` `SplineLookahead` (FUN_00443dc0) for car 3 | On the original's own inputs (`LR_` replay, (spline, v, own_x, own_z) of every aistep row) the port's lookahead differs from car 3's recorded target (> 0.01) on a contiguous run of mode-0 calls from window call 44 under both the exact and the LUT form (o_reg_r1 44 / 74, o_reg_r13 53 / 66, o_reg_r19 15 / 44 of ~200; `LOOK_REPLAY_FLIPS_MODE0.txt` of " + R + "). Cars 1 and 2 match with the LUT form (0..6 flips). A difference that persists over consecutive calls under both forms points at state the replay carries (the continuity index `DAT_008032d4`, the march), not at the square root. | Car 3's `DAT_008032d4` index and march state on the original at calls 40..50 | Record the index on the original (aistep column) and replay with it | D3 (b) car 3 (A, B) |",
    "| U-9752 | semantic | `Ai/AiStandalone.cpp` `AiLutMask` (exact sqrt by default; `MASHED_AI_LUT=5` = bit 0x1 `FUN_00443dc0` phases 6 / 8 through `FUN_004c3c60` / `FUN_004c3bf0`, bit 0x4 the listing's operand order `0x00443373..0x004433cf`) | The original's spline lookahead uses the RW LUT bodies (d3b `decomp_math.txt`). On the original's own inputs the LUT form reproduces its target choice on mode-0 rows far better than the exact form: flips (> 0.01) car 1 1..3 vs 38..48, car 2 0..6 vs 4..24 (o_reg_r1 / r13 / r19); car 2's first split from o_reg_r13 at window call 93 is the exact form's (LUT gives the original's (-1.002, -11.643)). On the current default + `MASHED_AI_CLOCK_LATE`: (b) A 2/3 B 2/3 C 0/3 (same pass set; the d3b / dflip A car 2 / car 3 trade is gone), D2 3/3, 13 tracks 869 rows 0 RED (3 more `C27-CAR-PART-OFFSET`, cited), G-CTRL `262504cc / 39fc6210 / 6797ffe5`. Live, car 2 still splits at 93: the choice sits within the LUT/exact difference and car 2's position is 2.2e-3 off by then (U-9212). | None for the LUT bodies | User decision on the default (with or after `MASHED_AI_CLOCK_LATE`) | the knob's default; D3 (b) |",
]


def uncert(s, nl):
    hdr = '| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |' + nl + '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) == 1
    for u in ('U-9752', 'U-9753'):
        assert ('| ' + u + ' |') not in s, u
    s = s.replace(hdr, hdr + nl.join(ROWS) + nl, 1)
    key = '| U-9212 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    note = (' **2026-10-17 (c2k93, ' + R + '):** `MASHED_D2LAT_REFRESH` on the current default + `MASHED_AI_CLOCK_LATE` cuts AI car 2\'s '
            'position drift against o_reg_r13 5x (2.18e-3 -> 0.45e-3 at window call 92; the drift starts at call 33, the first A4 frame '
            'after a contact coast) but still pins the D2 player (H1 9/400, driving-median 1812.98, 3/3 runs, with and without '
            '`MASHED_AI_LUT=5`), as d2lateral measured.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-17  00443dc0  c2k93  car 2 from call 93: the split is the lookahead\'s exact sqrt (the original uses the RW '
            'LUT, MASHED_AI_LUT=5, U-9752); with CL + LUT (b) keeps A 2/3 B 2/3 C 0/3, D2 3/3, 869 rows 0 RED; the speed refresh '
            'cuts car 2\'s drift 5x but still pins D2 (U-9212 note); car 3 lookahead state run from call 44 (U-9753). No code '
            'change, no C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
