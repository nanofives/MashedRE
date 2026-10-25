# PREREG D3-CF-R7FLIP (2026-10-25)

Decision L-16 = 1 (Mariano, 2026-10-25): `MASHED_R7_EXIT` + `MASHED_R7_INPUT` default ON together. Code: `34438fd59` on
`orch/D3-CF-R7FLIP`, branched from `orch/integration` @ `3d0c006c8` (contains the L-13 / L-14 / L-15 deletions).
Opt-outs: `MASHED_NO_R7_EXIT=1`, `MASHED_NO_R7_INPUT=1`. Runner `run_arms.py` (copy of r7joypad's, adds `NOXI` = both opt-outs).

Predictions (from verify/r7joypad_20261025/RESULT_D3-CF-R7JOYPAD.md, arms XI):
- P1 G-CTRL `H_DEF` = `5943c5f1 / b493262b / 09551e6a`.
- P2 B `B_DEF_K0 / K1 / K26` = `3a03670c / 2516d903 / 53778664`.
- P3 opt-out `B_NOXI_K0` = `c37d4c0c` (the pre-flip B K0 ref).
- P4 D2 `D2_DEF_1..3` (`--solo`, `score_d2_v2.py port`) PASS 3/3, each motion_diag >= 1600 lines.
- P5 13 tracks (`run13.py`, no knob) vs `verify/rres_20261016/cites_rres.json`: 13/13 OK, 0 RED, score table equal to
  `orch/D3-CF-R7JOYPAD:verify/r7joypad_20261025/data/tracks_XI_score.txt`.
If P1 or P2 differs: verdict DECISION with the measured hashes, no merge request.
