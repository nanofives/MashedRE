"""D3-CF-U9780 tracker transaction (no default, no C-level change): U-9780 RESOLVED, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9780_20261024/RESULT_D3-CF-U9780.md'
KIND = "~~numeric~~ **RESOLVED 2026-10-25** (D3-CF-U9780: removed by dotflip's A6a dot order, not by the fiber deletion)"
NOTE = (" **RESOLVED 2026-10-25 (D3-CF-U9780, " + R + "):** on the default exe `7c78d294` (SUBSTEP_OUTER seeded) car 2's position, "
        "velocity and angular velocity equal `o_c2_2` and `o_c2_3` on all 220 common k (0..219), first difference k=220 (the round end, "
        "U-9779 / U-9781). On the soflip exe `50558a73` (fiber code deleted, SUBSTEP_OUTER seeded) the row reproduces: k=218 vx "
        "0x44f06b06 / 0x44f06b07 (1 ulp, 1923.34), with angular velocity 1..8 ulp from k=174. `MASHED_NO_A6A_DOT_ORDER=1` alone on the "
        "current exe gives the soflip record line for line over k 0..219, and the dcphys2 opt-outs (`NOP2`) leave it exact, so dotflip "
        "`2d9cd2b2f` (A6a 0x00467650's dots 0x00467be9..0x00467c0c / 0x0046837d..0x00468394 / 0x00468771..0x00468793 in the original's "
        "association) removed it. Gates unchanged: G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a.")


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    k = '| U-9780 | numeric |'
    assert s.count(k) == 1 and 'RESOLVED 2026-10-25** (D3-CF-U9780' not in s
    i = s.index(k); j = s.index(nl, i); row = s[i:j]
    row = row.replace('| numeric |', '| ' + KIND + ' |', 1)
    c = row.rstrip().rfind('|')
    return s[:i] + row[:c].rstrip() + NOTE + ' |' + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ("2026-10-25  00467650  D3-CF-U9780  U-9780 (car 2's 1-ulp vx at k=218, B K0) RESOLVED: does not reproduce on the default "
            "(car 2 equal to o_c2_2 / o_c2_3 on k 0..219); reproduces on the soflip exe and with MASHED_NO_A6A_DOT_ORDER=1, so dotflip "
            "removed it, not the fiber deletion. No default, no C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
