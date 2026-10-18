"""c3k44 tracker transaction (re-classify, no C-level change): U-9753 RESOLVED (the premise was a probe artifact), a note on
U-9684 (regime-B car 3), one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/c3k44_20261017/RESULT_c3k44.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def note(s, nl, uid, text, retype=None):
    key = '| ' + uid + ' |'
    assert s.count(key) == 1, uid
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    if retype:
        assert row.count(retype[0]) == 1, uid
        row = row.replace(retype[0], retype[1], 1)
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + text + ' |' + s[j:]


def uncert(s, nl):
    s = note(s, nl, 'U-9753',
             ' **RESOLVED 2026-10-17 (c3k44, ' + R + '):** the premise is a probe artifact. From window call 44 the original\'s probe '
             'records no lookahead target for car 3 (empty `look_x` / `look_z`): `FUN_00414a70` returns 2 on calls 44..57 and '
             '`FUN_00416250` takes the immediate return at `0x004163f6..0x00416405` (ctrl[4] = 0, ctrl[5] = 0xff), so there is no '
             'target to compare. The real regime-B car 3 divergence is the `FUN_00414a70` chain (U-9684).',
             retype=('| U-9753 | semantic |', '| U-9753 | ~~semantic~~ **RESOLVED 2026-10-17** (c3k44: probe artifact; the cause is U-9684) |'))
    s = note(s, nl, 'U-9684',
             ' **2026-10-17 (c3k44, ' + R + '):** regime-B car 3 needs this chain too. On o_reg_r13 its `FUN_00414a70` returns 1 on '
             'calls 39..41 (mode 3: steer 178 / 227 / 255, brake 255) and 2 on 44..57 (brake 255, speed 2710 -> 1734); the default '
             'never calls it, and car 3 accelerates (B fails `c1#` 52). With `MASHED_PATHPROG` + `MASHED_TGT3` alone the car ahead '
             'is car 2 until call 52 (car 2 runs 0.34 ahead in z), so return 2 starts at 53 and mode 3 never fires; (b) A 1/3 B 0/3. '
             'With d3bB2\'s `S` stack car 3 brakes one call late (mode 3 on 40..41, return 2 from 45; position within 0.003 to call '
             '43): at call 39 car 2 is 0.0007 nodes ahead of car 3 on `0x008a96e8`, so the car-ahead choice is knife-edge there; '
             '(b) `S` 0/3 in every regime. The step dump\'s own_x/own_z freeze on return-2 calls (stale `FUN_00416250` locals); the '
             'record position keeps moving.')
    return s


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-17  00414a70  c3k44  regime-B car 3: U-9753 RESOLVED as a probe artifact; the divergence is the missing '
            'FUN_00414a70 brake chain (mode 3 at call 39, return 2 on 44..57, U-9684 note); the chain on today\'s default is one '
            'call late at best (S stack) and worsens (b); no code change, no C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
