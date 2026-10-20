"""u9772 tracker transaction (re-classify, no C-level change): U-9772 RESOLVED (A5's drafting term, DAT_007f0fd0 read as the
car count), new U-9773 (car 3's call 149 / car 2's call 150 position split), one CHANGELOG line. Line endings kept.
usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9772_20261020/RESULT_u9772.md'
U72 = ('| U-9772 | ~~semantic~~ **RESOLVED 2026-10-20** (u9772: A5\'s drafting term) | car 3\'s record call 55 and car 2\'s call 136 '
       'on `FULLRL` + `MASHED_A6A_GEAR_ORDER` | **RESOLVED 2026-10-20 (u9772, ' + R + '):** A5 (FUN_0046ddb0) resets `local_70` to 1 '
       'when `DAT_007f0fd0` is 4 / 9 (0.75 on 8 and car != 0), and FUN_00442ce0 / FUN_00442c80 test the same word. The original '
       'holds 0 on this recipe (`--peek`); the port read `g_playerCount` = the car count (4), so it cancelled car 3\'s draft behind '
       'car 1 from call 55 (`local_70 = 0.85 * 0.4251`). `MASHED_A5_FD0_LIVE` (default OFF) reads the live word; with '
       '`MASHED_A5_DRAFT_LUT` car 3 is equal through call 148 and car 2 through call 172. Alone on the default it drops (b) to '
       'A 0/3 B 1/3 C 0/3, so no default. Next split moves to U-9773. | (none) | (none) | D3 (b) car-level parity |')
U73 = ('| U-9773 | semantic | car 3\'s record call 149 and car 2\'s call 150 (K26) / 173 (K0) on `FULLGO` + `MASHED_A5_FD0_LIVE` + '
       '`MASHED_A5_DRAFT_LUT` | Position differs first with velocity bit-equal (car 3: `+0x958` -3.289e-05, `+0x960` -2.880e-04). '
       'Not the original\'s A/C split: five original car-3 captures (o_c3, o_a3w0..2, o_b3w0) agree there and the port differs '
       'from all of them in both K0 and K26. ' + R + ' | Which writer of `+0x958 / +0x960` differs on that call (the integrate '
       'step, a contact position fixup) | `--watch-write` on car 3\'s `+0x958` around call 149, then the matching port '
       'writer | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9773 |' not in s and s.count('| U-9772 |') == 1
    i = s.index('| U-9772 |'); j = s.index(nl, i)
    return s[:i] + U73 + nl + U72 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-20  0046ddb0  u9772  U-9772 RESOLVED: car 3\'s call-55 and car 2\'s call-136 splits are A5\'s drafting term, '
            'cancelled in the port because its DAT_007f0fd0 tests (A5, FUN_00442ce0, FUN_00442c80) read the car count (4) instead '
            'of the live word (0); MASHED_A5_FD0_LIVE (default OFF), with MASHED_A5_DRAFT_LUT, holds car 3 equal through call 148 '
            'and car 2 through 172; alone it drops (b) to A 0/3 B 1/3 C 0/3, so no default. New U-9773. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
