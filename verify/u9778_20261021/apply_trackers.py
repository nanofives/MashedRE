"""u9778 tracker transaction (no default, no C-level change): a note on U-9778, new rows U-9779 / U-9780, one CHANGELOG line.
Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9778_20261021/RESULT_u9778.md'
NOTE = (" u9778 2026-10-21: cause found. The first diverging operation is VehicleCarCarContact 0x00469df0 for cars 2 / 3 in frame "
        "204's substep 2: at its entry car 3's selector +0x9a8 is 1 in the original (one substep of frame 204 done) and 0 in the port, "
        "so the proximity test 0x00470b44..0x00470b70 and the body read car 3's pose one substep behind. FUN_00470c70 calls FUN_004709a0 "
        "once per substep (0x00471137) and FUN_004709a0 steps cars 0..15 inside (0x00470c47..0x00470c53), while the port's StepCar runs "
        "one car's substeps at a time. Knob `MASHED_SUBSTEP_INTERLEAVE` (default OFF, fibers yielding per substep): car 2 bit-exact "
        "through 217, car 3 through 219, player / car 1 through 219 (unchanged); (b) A 3/3 B 3/3 C 3/3, D2 3/3, 13 tracks 877 rows 0 RED, "
        "G-CTRL knob-off unchanged, knob on `af98e82c / 32e75acc / bdb3e07b`. Next: U-9779, U-9780. No flip. " + R + ".")
ROW79 = ("| U-9779 | semantic | the round end at call 220 on the B recipe (K0) | In the original the round ends at k=220: aistep `substate` "
         "6 -> 7, cars 1 and 2 stop producing rows, car 3's command goes to c4 0 / c5 0, the AI clock `clk_0ff4` stops (56050 at 220 and "
         "221) and every car slows (car 3 |v| x0.8345 per frame). The port stays in substate 6. With `MASHED_SUBSTEP_INTERLEAVE` this is "
         "the first split of the player, car 1 and car 3 (all bit-exact through 219). " + R + " | which code writes the original's 6 -> 7 "
         "at 220 and which inputs it reads [UNCERTAIN: writer RVA not identified] | a `--watch-write` on the "
         "sub-state word for 6 -> 7 on `o_c3`-regime captures against the port's race state at k 215..220 | D3 (b) car-level parity |")
ROW80 = ("| U-9780 | numeric | car 2's vx at call 218 on the B recipe (K0, `MASHED_SUBSTEP_INTERLEAVE`) | With the knob car 2 is bit-exact "
         "through 217 against `o_c2_2` / `o_c2_3`; at 218 vx differs by 1.221e-04 (1 ulp at 1923.34), position equal. No car<->car call "
         "in k 206..221 on either side. " + R + " | which step of frame 218 first differs for car 2 | `--a5-probe` / `--wheelstate-probe` "
         "on `--statediff-car 2` against `MASHED_D2SINK` (slot 2 support needed) | D3 (b) car-level parity |")


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9779 |' not in s and '| U-9780 |' not in s
    k = '| U-9778 |'
    assert s.count(k) == 1
    i = s.index(k); j = s.index(nl, i); row = s[i:j]
    c = row.rstrip().rfind('|')
    s = s[:i] + row[:c].rstrip() + NOTE + ' |' + s[j:]
    h = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(h) == 1
    return s.replace(h, h + ROW80 + nl + ROW79 + nl, 1)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ("2026-10-21  00469df0  u9778  cars 2 / 3 split at 204 = car 3 one substep behind at the 2-3 contact: FUN_004709a0 steps "
            "cars 0..15 per substep (0x00471137, 0x00470c47..0x00470c53), the port one car's substeps at a time. "
            "MASHED_SUBSTEP_INTERLEAVE added default OFF: (b) A 3/3 B 3/3 C 3/3, D2 3/3, 13 tracks 0 RED; new U-9779 (round end at 220), "
            "U-9780 (car 2 1 ulp at 218). No default, no C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
