"""u9776 tracker transaction (no default, no C-level change): a note on U-9776, new row U-9777, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9776_20261020/RESULT_u9776.md'
NOTE = (" u9776 2026-10-20: car 3's call-149 ulp is FUN_004c52f0's device multiply 0x005cb2a0 (verbatim naked x87 in the port) run "
        "at the standalone's PC_53 where the original runs PC_24; `MASHED_WCS_COMBINE_PC24` / `MASHED_MATMUL_PC24` (default OFF) make "
        "the ring bit-equal (new `scenario_launch.py --tri-probe`). The comparable arm is K0 (K26 rolls `flag_a368` = 2 at row 61); on K0 "
        "with the knob cars 1 and 3 are bit-exact through call 179; (b) A 2/3 B 2/3 C 3/3 (A car 3 fails by 1), D2 3/3. The call-180 "
        "split follows the player's call-128 split (U-9777). " + R + ".")
ROW = ("| U-9777 | semantic | the player (slot 0) record at call 128 on the B recipe (K0, `MASHED_WCS_COMBINE_PC24`) | Bit-exact "
       "through call 127 against both original variants (`o_bq0_1` / `o_bq0_2`); at frame 987's first wheel classifier call the "
       "position and body up `+0x9c8` differ (up.y 0x3f7ffb82 / 0x3f7ffb9b), frame 986's second call is still equal. Through "
       "FUN_00442a60's reference (refdist 0x008989b0) and FUN_004148b0 it moves cars 1 and 3 at call 180. Not the surface-jitter RNG "
       "(`MASHED_FI_RAND` inert here) nor `MASHED_A5_RING_ORDER`. " + R + " | which step between frame 986's second classifier call and "
       "frame 987's first changes slot 0 (solver pass 2, the integrate, A5 / A6a / A6b of frame 987) | `--wheelstate-probe` / "
       "`--a5-probe` / `--a6t-probe` on `--statediff-car 0` against `MASHED_WCS_POSLOG` / `MASHED_FORCE_SPLIT_SLOT=0` / the A6a logs "
       "at that frame | D3 (b) car-level parity |")


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9777 |' not in s
    k = '| U-9776 |'
    assert s.count(k) == 1
    i = s.index(k); j = s.index(nl, i); row = s[i:j]
    c = row.rstrip().rfind('|')
    s = s[:i] + row[:c].rstrip() + NOTE + ' |' + s[j:]
    h = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(h) == 1
    return s.replace(h, h + ROW + nl, 1)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ("2026-10-20  0046f6c0  u9776  car 3's call-149 ulp = FUN_004c52f0 device multiply 0x005cb2a0 at PC_53 (port) vs PC_24; "
            "MASHED_WCS_COMBINE_PC24 / MASHED_MATMUL_PC24 / MASHED_FI_RAND added default OFF; with the combine knob (b) A 2/3 B 2/3 "
            "C 3/3, D2 3/3; new U-9777 (the player's call-128 split). No default, no C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
