# PRE-REGISTRATION: regime C under MASHED_WIRE_B2 (U-9204 step 1), committed before the arms run

Date 2026-10-09. Follows `RESULT_BREGIME_C.md` §1. `FUN_004148b0` (the leader timer, branch 2 of
ControlStep) returns 0 at once when `DAT_0089a368 == 2` (`AiLeaderTimer.cpp:170`, `LT_EXIT(91)`;
original `cmp eax,2` at `0x004148b0`). So with branch 2 wired, regime C can differ from regime A only
through the branch-2 firings that A has and C suppresses.

Arms (`run_b2c.ps1`): the `run_bscore.ps1` recipe + the full `$B2` knob set of `run_h4.ps1`
(`MASHED_A364_RESET`, `MASHED_REFDIST`, `MASHED_RACEPCT_BRIDGE`, `MASHED_NO_ELIM`, `MASHED_WIRE_B2`),
plus `MASHED_U9186_GATES` (per-call `lt_ran`, `lt_ret`, `lt_fire`, `mode368`). K = 0, 1, 26.

| # | prediction | refuted if |
|---|---|---|
| W1 | labels unchanged from the default build: K0 A, K1 B, K26 C (the B2 knobs add no ring draw before the roll) | any label differs (report the new map) |
| W2 | **logic check:** port C's windows differ from port A's IF AND ONLY IF branch 2 fires (`lt_fire` > 0) inside regime A's 220-call window for some car | C != A with no A-window firing, or C == A with firings |
| W3 | (report) the C-envelope score of the K26 arm, and the A-envelope score of the K0 arm, next to the default build's (A 2/3, C 0/3) | n/a |

W2's outcome (whether branch 2 fires in the window at all) is NOT predicted. RESULT_WIRE2 measured
103 firings over a whole no-round race; the window is the first 220 calls of a round-mode race.
