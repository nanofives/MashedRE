"""d2ref tracker transaction (re-classify, no C-level change): notes on U-9212, U-9720, U-9751, one CHANGELOG line.
Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/d2ref_20261017/RESULT_d2ref.md'
STACK = ('the faithful stack `MASHED_D2LAT_REFRESH` + `MASHED_GRID_DIR` + `MASHED_A5_RACE_TIMER` (RGX) on the 6242c84b6 default: '
         'D2 3/3 in bounds (slip 0.1939..0.1942 / 0.2545, driving-median 1960.31..1963.27, H1 398/400; original 0.1937 / 0.2498 / '
         '1943.57), 13 tracks 869 rows 0 RED, (b) A 2/3 B 2/3 C 0/3 with regime-B car 2 passing and B car 3 turning to FAIL '
         '(`c1#` 52 [17, 46]); G-CTRL `1fbc77ae / 29a08810 / d3a20f4c`. Without the race timer the pair misses slip 2000-2600 '
         '(0.2557); without the grid frame the refresh pins (H1 9/400).')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def note(s, nl, uid, text):
    key = '| ' + uid + ' |'
    assert s.count(key) == 1, uid
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + text + ' |' + s[j:]


def uncert(s, nl):
    s = note(s, nl, 'U-9212', ' **2026-10-17 (d2ref, ' + R + '):** ' + STACK + ' Default awaits the user (the (b) trade).')
    s = note(s, nl, 'U-9720', ' **2026-10-17 (d2ref, ' + R + '):** `MASHED_GRID_DIR` is the spawn-frame member of ' + STACK)
    s = note(s, nl, 'U-9751', ' **2026-10-17 (d2ref, ' + R + '):** ported behind `MASHED_A5_RACE_TIMER=1` (default OFF; '
             '`VehiclePhysicsRun.cpp` copies the exe\'s `DAT_007f0ff8` into `g_raceTimer` before each step). The timer advances '
             '50 per step in the D2 recipe (59950 at step 1200), so the ramp covers the last ~430 of ~1630 steps. Alone: slip '
             '1500-2000 0.1864 (out); with refresh + grid it closes the slip 2000-2600 overshoot (0.2557 -> 0.2545).')
    return s


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-17  0046ddb0  d2ref  U-9212 chain: refresh + grid frame + A5 race timer (new MASHED_A5_RACE_TIMER, '
            'default OFF, U-9751) passes D2 3/3 for the first time and 13 tracks 869 rows 0 RED; (b) trades regime-B car 3 for '
            'car 2. All default OFF, no C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
