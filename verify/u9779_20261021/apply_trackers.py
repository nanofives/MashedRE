"""u9779 tracker transaction (no default, no C-level change): a cause-found note on U-9779, new row U-9781, one CHANGELOG line.
Line endings kept."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9779_20261021/RESULT_u9779.md'
NOTE = (" u9779 2026-10-21: cause found. The only store of 7 is 0x004111a6 in FUN_00411170, reached when FUN_00410d10 (call "
        "0x0041118f) returns non-zero and FUN_00410510 (0x00411198) returns 0. On K0 FUN_00410d10's elimination block fires (zoom "
        "DAT_00898980 == 10.0 at 0x00410ee3) on the player; FUN_0040eee0 (4 participants, no teams, 3 remaining, FUN_0042f6a0 in "
        "{3,4,5,10}, DAT_007f0fd0 == 0) walks the type-2 slots and calls FUN_0040d590(3,1,2,1), which kills cars 1 / 2 (scores 4,5,7,8); "
        "the alive-count tail returns 1. Witnessed with the new `--elim-probe` (o_ep_1). The port awarded -2 and stopped "
        "(TrackRenderer::ScoreOnElimination), its AI host returns sub-state 6 always, and its UpdateRace runs after the AI tick on a "
        "mixed-time snapshot (player after this frame's physics, AI cars before), while FUN_004111c0 calls FUN_00411170 (0x004112cd) "
        "before FUN_0040fc00 (0x004112d5). Knobs, all default OFF: `MASHED_ELIM_RESOLVE` (the type-2 arms; FUN_0040d590 now one body, "
        "Frontend::RaceRankThree), `MASHED_ROUND_END7` (FUN_0040e350 = 7 after a round end, the 0x0040fe46 clock skip, FUN_00424eb0's "
        "FUN_0046c570 0.85 damp), `MASHED_RACE_TAIL_LATE` (UpdateRace at the end of the frame). With all three car 3's row 220 reads "
        "sub-state 7 / c4 0 / c5 0 and the clock stops as in the original; records still differ from k=220 (U-9781). No flip. " + R + ".")
ROW81 = ("| U-9781 | semantic | car 3's record from k=220 on the B recipe (K0, sub-state 7) | In the original car 3's position freezes "
         "from k=220 (6.0739, -22.0905) while its velocity decays (2009.32 -> 1676.83 -> 1396.51). `--watch-write 958,9b0` (o_ww_2, used "
         "for writers only): per frame after the round end the position writer 0x0046ea62 rewrites the same x, only one substep's "
         "writers appear (two while racing), and FUN_0046c570 (pc 0x0046c591, from FUN_00424eb0) scales the velocity by 0.85. "
         "FUN_00470c70 also hands A4 (0x00471076) the block 0x007f19b8 instead of the slot's when FUN_0040e350() is not 6 / 0xb / 0xa "
         "(0x00470fdb..0x00470ff9). With `MASHED_ROUND_END7` the port damps the velocity (1632.52 at 220) but keeps integrating the "
         "position and runs two substeps. " + R + " | why the original's position does not advance in sub-state 7 (which step of "
         "FUN_00470c70 / FUN_004709a0 reads the sub-state or a zero step) and what 0x007f19b8 holds [UNCERTAIN: not measured] | the "
         "original's FUN_00470c70 / FUN_004709a0 call arguments on sub-state-7 frames (entry probes) and a read of 0x007f19b8, against "
         "the port's StepCar under `MASHED_ROUND_END7` | D3 (b) car-level parity |")


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9781 |' not in s
    k = '| U-9779 |'
    assert s.count(k) == 1
    i = s.index(k); j = s.index(nl, i); row = s[i:j]
    c = row.rstrip().rfind('|')
    s = s[:i] + row[:c].rstrip() + NOTE + ' |' + s[j:]
    h = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(h) == 1
    return s.replace(h, h + ROW81 + nl, 1)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ("2026-10-21  004111a6  u9779  the B-recipe round end at 220 = FUN_00411170's 0x004111a6 DAT_0063ba8c = 7 after FUN_00410d10 "
            "kills the player and FUN_0040eee0's type-2 arm runs FUN_0040d590 on the three AI cars (scores 4,5,7,8). Knobs "
            "MASHED_ELIM_RESOLVE / MASHED_ROUND_END7 / MASHED_RACE_TAIL_LATE added default OFF; FUN_0040d590 consolidated to one body "
            "(Frontend::RaceRankThree). U-9779 cause found, new U-9781 (sub-state-7 physics). No default, no C-level change. " + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
