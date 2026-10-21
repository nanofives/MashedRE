"""substep tracker transaction (new default-OFF knob MASHED_SUBSTEP_OUTER, no C-level change): one CHANGELOG line, one new
uncertainty U-9782 (the car-car retry counter). Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/substep_20261021/RESULT_substep.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ("2026-10-21  004709a0  substep  VehiclePhysics_StepCar split into phase functions (StepBegin / StepChunkHead / "
            "StepSubstep / StepChunkTail / StepEnd, per-call StepCtx, ContactScratch per owner); new MASHED_SUBSTEP_OUTER=1 "
            "(default OFF) drives them in FUN_00470c70's order (all A4 heads, then substep rounds with cars 0..15 inside as "
            "FUN_004709a0 0x00470c47..0x00470c53, then all tails). Knob off unchanged (G-CTRL ce087436 / b0a2973b / c4f7a000, "
            "B 7ab70c5f / 15262e4a / d1b4bc64). Knob on = the u9778 fiber form on every arm: G-CTRL f78de789 / 43bbe0fb / "
            "161b1a6a, B 9bd86ffe / 433abcd2 / 1a1ff727, (b) 9/9, D2 3/3, 13 tracks 877 rows 0 RED (captures byte-identical). "
            "U-9782 opened. No C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


def uncertainties(s, nl):
    hdr = ('## Active uncertainties' + nl + nl + '| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |' + nl +
           '|----|------|-------|-----------|------------------|--------------------|--------|' + nl)
    assert s.count(hdr) == 1
    assert '| U-9782 |' not in s
    row = ("| U-9782 | semantic | FUN_004709a0's retry counter vs `StepSubstep` (Vehicle/VehiclePhysicsRun.cpp) | FUN_004709a0 "
           "counts world retries and car-car contacts in one register: `inc ebp` at 0x00470b0a (world retry) and 0x00470bde (per "
           "car-car contact), loop test `cmp ebp,2 / jge 0x470c47` at 0x00470ab0, which skips the selector swap 0x00470c31. Two "
           "car-car contacts in one pass 0 reach ebp 2, so the original leaves the substep with no retry and no swap. The port's "
           "retry loop advances `pass` by 1 per retry (its `ccPass` only feeds the contact's third argument), so it runs pass 1. "
           "Static reading. On the knob-off B arms K0 / K1 / K26 (`MASHED_CARCAR_LOG`, 860 calls, 36 ret 1) no pass-0 loop has two "
           "ret-1 contacts, so (b) cannot see it. " + R + " | whether G-CTRL or any other recipe has two car-car contacts with "
           "ret 1 in one pass 0 of one substep [UNCERTAIN: not counted on G-CTRL] | count those loops in `MASHED_CARCAR_LOG` on "
           "G-CTRL (the log caps at 8000 lines); if "
           "any, a default-OFF knob that advances the retry counter by the contact count, measured on those arms | D3 (b) "
           "car-level parity on multi-car contacts |")
    return s.replace(hdr, hdr + row + nl, 1)


rw('re/analysis/CHANGELOG.md', changelog)
rw('UNCERTAINTIES.md', uncertainties)
print('applied')
