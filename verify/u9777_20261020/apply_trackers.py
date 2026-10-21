"""u9777 tracker transaction (no default, no C-level change): a note on U-9777, new row U-9778, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9777_20261020/RESULT_u9777.md'
NOTE = (" u9777 2026-10-20: cause found. The first diverging operation is A5 (FUN_0046ddb0) Phase 3: FUN_00442c80 returns 1 when "
        "`[car*4 + 0x008989b0] > 7.0` (0x00442ca2 / 0x00442ca9, 0x005cc9b8 = 7.0) and A5 halves local_70; the player's refdist passes "
        "7.0 at call 128. The port's exe copies of FUN_00442c80 / FUN_00442ce0 read `g_rubberBand` / `g_rubberRefCar` (zero, no "
        "writer) instead of 0x008989b0 / 0x008989c8 (0x00442d0f, 0x00442d2f), which Race/SpectatorDistances.cpp fills. Knob "
        "`MASHED_A5_RUBBER_LIVE` (default OFF): the player's A5 equal for 136 calls (was 85); on K0 the player and car 1 are bit-exact "
        "through call 219, cars 2 and 3 through 203 (U-9778); (b) A 3/3 B 2/3 C 3/3, D2 3/3, 13 tracks 873 rows 0 RED, G-CTRL knob-off "
        "unchanged, knob on `15594ec8 / 7285d708 / 26a6e4ad`. No flip (user decision). " + R + ".")
ROW = ("| U-9778 | semantic | cars 2 and 3 at call 204 on the B recipe (K0, `MASHED_A5_RUBBER_LIVE`) | With the knob cars 2 and 3 are "
       "bit-exact in position and velocity through call 203 against `o_c2_2` / `o_c2_3` and `o_tq_2`, and split together at 204 "
       "(car 2 dvz -2.31, car 3 dvz +4.39), one call before the K0 contact schedule's 2+3 contact (205). Car 3's refdist first passes "
       "7.0 at row 665, so it is not the FUN_00442c80 gate. The player and car 1 follow at 220; the player's A5 entry already "
       "differs by 1 ulp in a denormal vy at port frame 180. " + R + " | which step of call 204 first differs for cars 2 / 3 (the "
       "car-car pair 0x00469df0, the contact scan 0x00469aa0, or the solver) | `--carcar-probe` / `--batch-probe` on "
       "`--statediff-car 2` and 3 against `MASHED_CARCAR_LOG` / `MASHED_BATCH_DUMP_SLOT` at calls 203..204 | D3 (b) car-level parity |")


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9778 |' not in s
    k = '| U-9777 |'
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
    line = ("2026-10-20  00442c80  u9777  the player's call-128 split = A5's rubber-band gate FUN_00442c80 (refdist 0x008989b0 > 7.0 "
            "halves local_70); the port's exe leaves read a zero stand-in. MASHED_A5_RUBBER_LIVE added default OFF (also FUN_00442ce0's "
            "0x008989b0 / 0x008989c8 reads): (b) A 3/3 B 2/3 C 3/3, D2 3/3, 13 tracks 0 RED; new U-9778 (cars 2 / 3 at call 204). "
            "No default, no C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
