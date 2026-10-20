"""faithflip tracker transaction (re-classify, no C-level change): a flip note on every RESOLVED U-id that names a FAITH knob
(23 rows), one CHANGELOG line. Line endings kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/faithflip_20261020/RESULT_faithflip.md'
GATES = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gates_line.txt'), encoding='utf-8').read().strip()
ROWS = {
    'U-9180': ['GRID_DIR'], 'U-9212': ['AI_LUT', 'GRID_DIR'], 'U-9219': ['MODE7'], 'U-9682': ['ROUND_GRID'],
    'U-9720': ['GRID_DIR'], 'U-9747': ['AI_TICK_FIRST'], 'U-9749': ['A4_ORDER'], 'U-9752': ['AI_LUT'],
    'U-9754': ['REASSERT_KEEP_NORMAL', 'SPAWN_BASIS_RW'], 'U-9755': ['SPAWN_BASIS_RW'], 'U-9756': ['FIX_5CEA60'],
    'U-9758': ['FWD_AT', 'RW_SSE_POINTS', 'UP_TRINORMAL'], 'U-9759': ['FWD_AT', 'RW_SSE_POINTS', 'UP_TRINORMAL'],
    'U-9760': ['FWD_AT', 'RING_DBUF'], 'U-9764': ['ROTATE_LUT'], 'U-9765': ['ORTHO_LUT', 'ROTATE_LUT'],
    'U-9766': ['A6A_LC0', 'ORTHO_LUT', 'ROTATE_LUT', 'ROTATE_PC24'], 'U-9767': ['HEADING_FWD_LIVE'],
    'U-9768': ['MODE7', 'PICKUPS_ORIG', 'REGISTRY', 'REG_CALLBACKS', 'REG_CARS'], 'U-9769': ['A4_ORDER', 'CC_PC24'],
    'U-9770': ['REG_LATE'], 'U-9771': ['A6A_GEAR_ORDER'], 'U-9772': ['A5_DRAFT_LUT', 'A5_FD0_LIVE', 'A6A_GEAR_ORDER'],
}
ALREADY_ON = {'AI_COUNTDOWN_CLOCK', 'GRID_DIR', 'FIX_5CEA60', 'RW_SSE_POINTS', 'FWD_AT'}


def note(knobs):
    flipped = [k for k in knobs if k not in ALREADY_ON]
    kept = [k for k in knobs if k in ALREADY_ON]
    parts = []
    if flipped:
        parts.append(', '.join('`MASHED_%s%s`' % (k, '=7' if k == 'AI_LUT' else '') for k in flipped) + ' DEFAULT ON')
    if kept:
        parts.append(', '.join('`MASHED_%s`' % k for k in kept) + ' already default ON, now also in the FAITH set')
    return (' faithflip 2026-10-20 (Mariano\'s decision, the FAITH stack): ' + '; '.join(parts) +
            ' (opt-out `MASHED_NO_<name>=1`, all of FAITH `MASHED_NO_FAITH=1`); ' + GATES + '; ' + R + '.')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    for key, knobs in ROWS.items():
        k = '| ' + key + ' |'
        assert s.count(k) == 1, key
        i = s.index(k); j = s.index(nl, i); row = s[i:j]
        assert 'RESOLVED' in row, key
        c = row.rstrip().rfind('|')
        s = s[:i] + row[:c].rstrip() + note(knobs) + ' |' + s[j:]
    return s


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-20  (stack)  faithflip  FAITH DEFAULT ON (Mariano\'s decision, accepting (b) B car 2): the 38 knobs of the d2off '
            'full faithful stack seeded by Core/FaithDefaults.cpp (exe only); MASHED_NO_FAITH=1 restores the pre-flip default, '
            'MASHED_NO_<name>=1 per knob; MASHED_COLL_LUT / MASHED_A5_RING_ORDER stay OFF. ' + GATES + '. Flip notes on 23 RESOLVED '
            'U-ids. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
