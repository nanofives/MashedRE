"""u9773 tracker transaction (re-classify, no C-level change): U-9773 RESOLVED, new U-9776, one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9773_20261020/RESULT_u9773.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9776 |' not in s and s.count('| U-9773 |') == 1
    i = s.index('| U-9773 |'); j = s.index(nl, i); row = s[i:j]
    cells = row.split(' | ')
    cells[1] = '~~semantic~~ **RESOLVED 2026-10-20** (u9773: the wheel solver\'s 3-wheel alignment was skipped and stubbed)'
    new_row = ' | '.join(cells)
    k = new_row.rstrip().rfind('|')
    new_row = new_row[:k].rstrip() + (' **RESOLVED 2026-10-20 (u9773, ' + R + '):** FUN_0046f6c0 with three grounded wheels calls '
        'FUN_0046c5f0 at 0x0046fd57 on three contact points and rotates the +0x928 ring toward that normal (CRT acos FUN_004a3384, '
        'FUN_004c52f0(ring, m, 2), gate `_DAT_005cea88` = 0.999f or `(int)+0x9e0 < 3`); the port skipped the call and stubbed acos / '
        'combine / the `_ftol`, so car 3 stayed flat at call 149. `MASHED_WCS_TRINORMAL` (default OFF) restores it: car 3 equal to '
        '1-2 ulp at 149, record to 157; (b) A 1/3 B 2/3 C 2/3 (C car 3 passes, A car 2 fails), D2 unchanged. Remainder -> U-9776. |')
    U76 = ('| U-9776 | semantic | car 3 past call 157 and A car 2 with `MASHED_WCS_TRINORMAL` | The alignment leaves 1-2 ulp in car 3\'s '
           'rotation at call 149 (`+0x92c` 0.0240422506 vs 0.0240422543), which grows to |dv| > 1e-3 at 158; with the knob A car 2 '
           'fails `c0_distinct 36 [13,35]`. ' + R + ' | Which step of the alignment (CRT acos, RwMatrixRotate 0x004c4d20, '
           'RwMatrixCombine 0x004c52f0) differs by an ulp, and why A car 2 changes | an original probe of the ring rows before / '
           'after 0x0046ffff on car 3 at call 149 against MASHED_WCS_TRILOG; car 2\'s record on K0 with the knob | D3 (b) car-level parity |')
    return s[:i] + U76 + nl + new_row + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-20  0046f6c0  u9773  U-9773 RESOLVED: car 3\'s call-149 split is the wheel solver\'s 3-wheel alignment, skipped '
            '(FUN_0046c5f0 at 0x0046fd57) and stubbed (acos, FUN_004c52f0, _ftol(+0x9e0); _DAT_005cea88 0.999f) in the port; '
            'MASHED_WCS_TRINORMAL (default OFF) restores it: car 3 to call 157, (b) A 1/3 B 2/3 C 2/3 (C car 3 passes, A car 2 fails), '
            'D2 unchanged. New U-9776. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
