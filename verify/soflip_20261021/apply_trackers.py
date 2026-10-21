"""soflip tracker transaction (default flip + deletion of u9778's fiber form, no C-level change): one CHANGELOG line. Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/soflip_20261021/RESULT_soflip.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    assert '  soflip  ' not in s
    line = ("2026-10-21  004709a0  soflip  MASHED_SUBSTEP_OUTER default ON (Mariano's decision; opt-out MASHED_NO_SUBSTEP_OUTER=1) "
            "and u9778's fiber form deleted (SubstepInterleave.cpp, MASHED_SUBSTEP_INTERLEAVE, IlYield, g_ilScratch; snapshot / "
            "restore + g_cSlot kept as VehiclePhysics_OuterSnapshot / OuterRestore, ORDER=fiber kept as a control). Deletion "
            "changed no hash (knob off ce087436 / b0a2973b / c4f7a000, knob on f78de789 / 43bbe0fb / 161b1a6a). New default "
            "G-CTRL f78de789 / 43bbe0fb / 161b1a6a x2, opt-out ce087436 / b0a2973b / c4f7a000, NO_FAITH 997f4117 / e6288905 / "
            "a63bdcfa, B 9bd86ffe / 433abcd2 / 1a1ff727, (b) 9/9 (B car 2 gained), D2 3/3, 13 tracks 877 rows 0 RED, 0 io "
            "mismatch. No C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
