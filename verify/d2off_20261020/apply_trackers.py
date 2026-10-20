"""d2off tracker transaction (re-classify, no C-level change): new U-9774 (the D2 offsetting error after k = 138 on FAITH +
MASHED_COLL_LUT), one CHANGELOG line. Line endings kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/d2off_20261020/RESULT_d2off.md'
U74 = ('| U-9774 | semantic | the player\'s record on the D2 recipe, `FAITH` + `MASHED_COLL_LUT` (' + R + ') | `FAITH` (38 default-OFF '
       'faithful knobs, POS_MODEL among them) passes D2 3/3 and holds the player\'s record bit-equal to orig_solo3 through k = 63. '
       '`MASHED_COLL_LUT` (collision FastSqrt / Vec3Mag / Vec3Normalize on FUN_004c3b30 / FUN_004c3ac0 / FUN_004c39b0) moves the '
       'first material split from k 94 to k 258 but drops driving-median to 1878.85..1882.20 (< 1893.63): a further offsetting '
       'error remains. First difference on that stack: `+0x9b0` 3 ulp at k 138, then `+0xb0c / +0xb14 / +0xb1c` from k 139. | '
       'Which writer of `+0x9b0` differs at k 138 (A5, A6a, A6b, the refresh, the fixup) | `d2first.py` on `D2_FD2` + an original '
       'probe at frame 138 (`--a5-probe` / `--a6t-probe` on car 0 with the D2 argv) | D2 fidelity, default-ON of the faithful set |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9774 |' not in s and s.count('| U-9773 |') == 1
    i = s.index('| U-9773 |')
    return s[:i] + U74 + nl + s[i:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-20  004c3b30  d2off  D2 offsetting error: without MASHED_POS_TRANS the default physics runs 0.0375 high (caller y); '
            'the full faithful stack FAITH (38 default-OFF knobs incl. POS_MODEL, A5_FD0_LIVE, A5_DRAFT_LUT, FIX_5CEA44) passes D2 3/3 '
            '(0.1918 / 0.2501 / 1909.67), 13 tracks 868 rows 0 RED, (b) A 2/3 B 2/3 C 1/3; new MASHED_COLL_LUT (collision LUT leaves) '
            'moves the player split k 94 -> 258 but fails D2 (1878.85), new MASHED_A5_RING_ORDER inert; both default OFF. New U-9774. '
            'No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
