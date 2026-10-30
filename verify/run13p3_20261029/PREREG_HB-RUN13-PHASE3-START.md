# PREREG HB-RUN13-PHASE3-START (L-59 (a), harness step before the MASHED_SUBSTATE flip)

Date 2026-10-29. Branch `orch/HB-RUN13-PHASE3-START` off `orch/integration` @ `1611794e7`. Committed alone, before any arm.

## Change

- `TrackRenderer.cpp` `SubStateCameraChain`: new knob `MASHED_CAM_REPLAY_P3` (DIAGNOSTIC, default OFF, inert without
  `MASHED_CAM_REPLAY`). On, the chain's `race_cam_.Update` call becomes `RaceCamUpdate(cc, reset)`, so each pre-race chain
  call (FUN_00446520 equivalent) consumes one trace row. A cam[7] != 0 call advances the trace one row without a camera update.
  The chain runs only under `MASHED_SUBSTATE`, so with SUBSTATE off the knob changes nothing.
- `verify/rsky_20261016/run13.py`: the recipe sets `MASHED_CAM_REPLAY_P3=1` (a knob argument `MASHED_CAM_REPLAY_P3=0`
  overrides it).
- Basis (measured on all 13 traces before writing this): row 0 is ba8c 3 with reset 1, row 1 is the same frame (ff4 repeats
  once, rows 0-1), cam[7] (c7) is 0 on every row, GO rows 554..1064. That is the port's chain call pattern on its phase-3
  entry frame (`SubStateCameraChain(true)` then `(false)`).

## Arms (exe built from this branch, `MASHED_NO_DEPLOY=1`, private root, test-slot + mashed_lock --solo)

| arm | knobs | prediction |
|---|---|---|
| A (recipe, default build) | none (recipe has P3=1, SUBSTATE off) | table byte-identical to `verify/pregocubeflip_20261029/tracks_score.txt` cf3307d2 |
| A0 (knob OFF) | `MASHED_CAM_REPLAY_P3=0` | cf3307d2 |
| B (flip candidate) | `MASHED_SUBSTATE=1 MASHED_P3_RESET_PAIR=1` (the pair D3-SUBSTATE-FLIP seeds) | fewer RED rows than run 22's flip table 580ab2ce (19 RED). [UNCERTAIN] whether 0 RED |
| B1 (optional) | `MASHED_SUBSTATE=1` only | report only |
| gates | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | merge_train.json refs 5943c5f1/306186d4/52052890, 3a03670c/9fcdbd3a/53778664 |

Fail rule: A or A0 differing from cf3307d2 means this step moves the reference. Then the RESULT records both tables and why.
B's table is committed as the candidate `run13_reference` for the flip whatever its RED count.
