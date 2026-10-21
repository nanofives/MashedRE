"""fflip tracker transaction (default flip + canonical recipe swap, no C-level change): one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/fflip_20261021/RESULT_fflip.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ("2026-10-21  0042f6a0  fflip  canonical recipe F (MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10, was rule 4) + MASHED_MODE_LIVE / "
            "MASHED_SLOT_TYPE_LIVE default ON (Mariano's decision): G-CTRL ce087436 / b0a2973b / c4f7a000 x2, opt-outs on F 2e01286e / "
            "c634bbac / 26a6e4ad, old recipe + opt-outs 15594ec8 / 7285d708 / 26a6e4ad, B 7ab70c5f / 15262e4a / d1b4bc64 unchanged, "
            "(b) A 3/3 B 2/3 C 3/3 (IL 9/9), D2 3/3, 13 tracks 873 rows 0 RED. No gate reads the rule-4 finish-order consumer. "
            "No C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
