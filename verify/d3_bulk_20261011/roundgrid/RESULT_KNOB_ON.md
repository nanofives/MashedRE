# Round-restart grid Stage A, knob ON: RESULT (2026-10-11), per PREREG_STAGE_A.md

Verdict: **all three registered gates pass.**
- G-RG-ON-TRACE: the restart nodes follow the survivor and the snap.
- G-RG-ON-DET: RG_ON == RG_ON2.
- G-RG-ON-LOCAL: the first difference is the round-1 placement itself, nothing earlier.
- Round 2+ no longer starts on the start line.
- No default change (user decision).

## Arms (`verify/d3_gatefire_20261008/run_h4.ps1`)

| arm | step | gates | ptrace |
|---|---|---|---|
| RG_CTRL (knob OFF) | ae67b13a | c8317ce2 | 31db9384 |
| RG_PP (PATHPROG) | 4e250237 | 22cceec1 | dc72ab09 (= AL_PP) |
| RG_ON | c374f936 | 3caa0b26 | d92d7b56 |
| RG_ON2 | c374f936 | 3caa0b26 | d92d7b56 (identical; trace identical) |

## G-RG-ON-TRACE (`RG_ON.roundgrid.txt`)

**Start of the match:**
```
race-init node=0
round-start node=0   A=(0.020925, 1.175, -1.99996)
```

**Each round end, then the next round's start:**

| round end: survivor, progress | stored node | next round-start node |
|---|---|---|
| car 3, 21.31 | 21 | 21, A=(8, 0.59, 26): not the start line |
| car 3, 10.23 | 10 | 10 |
| car 2, 10.29 | 10 | 10 |
| car 2, 13.92 | 13 | **14** (snap 13 → 14, the same correction as the original's in o_rounds_dense) |
| car 2, 2.16 | 2 | (the run ends) |

The exe's `round_winner_` (first alive) equals the original's rule (last alive) on every round here, because
there is one survivor each time.

## G-RG-ON-LOCAL

**First difference:** `RG_PP` vs `RG_ON` ptrace line 1 (the first sim step after round 1's StartRound). The
player is at z -1.99904 against -2.00247: the node-0 anchor in place of the gate anchor. Nothing differs before
the placement.

PREREG item 3 allowed this case, because node 0's formation differs from the gate one by up to 0.0236. The node-0
formation is the one that matches the original (0.0001, RESULT_STAGE_A.md).

## Report only

**Collateral** (`--mode paired`, A = RG_PP, B = RG_ON, floor-B = RG_ON2, so the floor is 0;
`collateral_RG_PP_vs_RG_ON.csv`):
- 183 of 252 fields diverge, all from step frame 0.
- The round-1 shift makes the arms differ from the start, so this pairing cannot isolate the round-2+ effect.
- **D2 watch list:** `rec_4a4` is not divergent on any car. No D2 REOPEN CANDIDATE.

**(b) per regime** (`ai_ctrl_window.py --check-regime`):

| arm | passing | failing items |
|---|---|---|
| RG_PP | 0/3 | 6: v1 c0_distinct + abs_steer; v2 c0_distinct + steer_distinct + abs_steer; v3 abs_steer |
| RG_ON | 1/3 (v3 PASS) | 2: v1 and v2 abs_steer only |

The only input difference inside the 220-step window is the round-1 grid shift (≤ 0.0236 in x/z). So (b) moves
under a sub-0.03 change of starting position. This is not evidence of an AI improvement; it shows how sensitive
(b) is. Single deterministic run per arm.

**G-AILEAF when stacked:** not run. Its 19 mode-1 commits came from the round-2 start-line restart, which this
knob removes.

## Open

- Stage B: the car-to-slot order per round (FUN_0040e590 assignment + FUN_00408b00 verbatim).
- The tracker row (re-classify): the exe StartRound start-line anchor against the original's restart node, now
  ported behind the knob.
- The exe's knob-OFF round-1 grid is off from the original by up to 0.0236 (RESULT_STAGE_A.md). A flip to the
  node-0 anchor by default would fix it. That is a user decision.
