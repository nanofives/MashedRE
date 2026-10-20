"""u9774 tracker transaction (re-classify, no C-level change): U-9774 RESOLVED (the call-63 / 113 / 138 chain behind COLL_LUT's D2
failure), new U-9775 (D2 driving-median of the fully faithful stack past the original's own run-to-run split), one CHANGELOG line.
Line endings kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9774_20261020/RESULT_u9774.md'
G = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gates_line.txt'), encoding='utf-8').read().strip()


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9775 |' not in s and s.count('| U-9774 |') == 1
    i = s.index('| U-9774 |'); j = s.index(nl, i); row = s[i:j]
    cells = row.split(' | ')
    cells[1] = '~~semantic~~ **RESOLVED 2026-10-20** (u9774: four float-order / precision sites on the player\'s contact path)'
    c = row.rstrip().rfind('|')
    new_row = ' | '.join(cells)
    k = new_row.rstrip().rfind('|')
    new_row = new_row[:k].rstrip() + (' **RESOLVED 2026-10-20 (u9774, ' + R + '):** four sites, each a default-OFF knob: '
        '`MASHED_A6A_SPIN_ORDER` (A6a spin factor grouping 0x00468077..0x004680ae, call 63), `MASHED_TER_SPIN_F32` (contact-normal '
        'spin term at float precision 0x0046906b..0x004690a1) + `MASHED_FIXUP_F32` (fixup hull-slot block 0x0046f1c2..0x0046f291, '
        'call 113), `MASHED_FIXUP_SUM_ORDER` (fixup per-contact sums 0x0046f0d8..0x0046f1a8, call 138); `MASHED_FIXUP_SLIDE_ORDER` '
        '(0x0046f631..0x0046f656) is faithful and inert here. With them and `MASHED_COLL_LUT` the player\'s record equals every '
        'original capture until that capture departs from the others (k 307..326). ' + G + '. Remainder moves to U-9775. |')
    U75 = ('| U-9775 | semantic | the D2 driving-median of the fully faithful player path (`COLL_LUT` + the five u9774 knobs) | '
           'Bit-exact against the original through its deterministic window, yet driving-median 1890.05 x3 (< 1893.63); without '
           '`COLL_LUT` 1963..1965 (in). The original captures part from each other at k 307..316 with every record word equal at '
           'the call before (two runs: equal at 308, 62 words differ at 309; A5\'s random impulse `+0xb00..b08` is 0 throughout), so '
           'the source is outside the record. ' + R + ' | What in the original varies run to run at k ~309 (a substep / chunk count, '
           'an input-timing word, a timer), and whether the port\'s one realisation is inside the original\'s distribution | a '
           '`--peek` / write watch on the chunk count and timers across two original runs at k 300..320; then D2 on the port with '
           'that source matched | D2 fidelity, default-ON of `COLL_LUT` |')
    return s[:i] + U75 + nl + new_row + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-20  0046ef70  u9774  U-9774 RESOLVED: COLL_LUT\'s D2 failure was four float-order / precision sites on the '
            'player\'s contact path (A6a spin grouping, contact-normal spin term, fixup hull block, fixup sums); five default-OFF '
            'knobs (MASHED_A6A_SPIN_ORDER, MASHED_TER_SPIN_F32, MASHED_FIXUP_F32, MASHED_FIXUP_SUM_ORDER, MASHED_FIXUP_SLIDE_ORDER). '
            'With COLL_LUT the player\'s record is bit-exact against every original capture until the originals part (k 307..326). '
            + G + '. New U-9775. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
