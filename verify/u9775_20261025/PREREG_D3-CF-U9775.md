# PREREG D3-CF-U9775: the original's run-to-run split and the port's A5 race-timer onset (2026-10-25)

Branch `orch/D3-CF-U9775` from `orch/integration` `3613b185b`. Committed alone, before any arm that sets the new knob.

## 1. What is already measured (no knob, before this file)

Original captures (D2 argv: `--statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38 --poke-ctrl-slots`), 13 of
them (10 archived + o_cw1 / o_sn1 / o_sn2 today on LIVING-PC):

- x87 CW `0x007f` (PC_24) and MXCSR `0x9fb0` at entry and leave of the physics dispatcher `0x00470c70`, every call of
  3 runs (2346 / 2403 / 2396 calls). Constant: not the source.
- Physics calls are one per render tick (frame gap 1 at every call, k 300..318), so not a skipped / doubled step.
- Two captures with the same first-moving frame a0 are bit-identical over the whole capture (a0 887: orig_solo3 / orig_solo4
  on MARIANO-PC and o_w2 on LIVING-PC; a0 893: o_w3 / o_sn1; a0 885: o_cw1 / o_sn2 / o_ww9b0). Every pairwise split lands at
  the same absolute frame 1203: split k = 1203 - max(a0_A, a0_B), 12 of 12 against the trunk (`a0table_1.txt`).
- `.data` images at dispatcher entry (o_sn1 a0 893, o_sn2 a0 885): `DAT_007f0ff8` = 50 * (a0 - 1) + 50 * k at the entry of
  the call that produces record k + 1. `60000 - t` first goes negative at the entry of the call that produces each run's split
  record (o_sn1 k 309 -> record 310, o_sn2 k 317). A5 `FUN_0046ddb0` reads it (`FILD [0x007f0ff8]` at `0x0046e0a9`) for its grip
  ramp (`_DAT_005cea7c` 60000, `_DAT_005cea74` -1/30000, `_DAT_005cea78` -30000).
- Port (exe `c2b3989e`, this branch before the knob): `MASHED_A5_RACE_TIMER_LOG` gives 0 / 29900 / 59900 at slot-0 steps
  0 / 600 / 1200, and `MASHED_NO_A5_RACE_TIMER=1` first changes the player's record at k 1200. The port's onset is k 1200, the
  originals' k 307..326.

## 2. Hypothesis

H1. The U-9775 driving-median failure of the faithful player path (`CLSF4` = the default + `MASHED_COLL_LUT` + the five u9774
knobs) comes from the port's A5 ramp onset (k 1200), not from the collision path: with the onset moved to an original's, the
record follows that original past its onset and D2 passes.

## 3. The knob

`MASHED_A5_RACE_TIMER_ADD=<n>` (diagnostic, default OFF = 0, `Vehicle/VehiclePhysicsRun.cpp`): n is added to the copied
`DAT_007f0ff8` before each car's step. n = 44200 puts the port's onset at record k 316 (the trunk's, a0 887). Report-only
extremes: n = 43700 (a0 877, o_w0) and n = 44650 (a0 896, o_ww9b4).

## 4. Arms (exe = this branch with the knob, `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\D3-CF-U9775_root`, D2 arms alone)

| arm | stack | measures |
|---|---|---|
| G-NOOP | `H_DEF`, `B_DEF_K0 / K1 / K26`, knob unset | equal to `merge_train.json` refs (G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / 2516d903 / 53778664`) |
| `D2_DEF_1..3` | default, knob unset | D2 3/3 (default unchanged) |
| `D2_CLSF4A_1..3` | CLSF4 + n 44200 + record dump | **G-H1-D2**: D2 3/3 in the d2rebase band (`score_d2_v2.py port`) |
| `D2_CLSF4A_1` dump | | **G-H1-REC**: first split against orig_solo3 (d2first word set, `+0xb2c` sign residual excluded) at k > 316 |
| `D2_DEFA_1..3` | default + n 44200 | reported (D2 of the default with the onset matched) |
| `D2_CLSF4L_1`, `D2_CLSF4H_1` | CLSF4 + n 43700 / 44650 + dump | reported (split against o_w0 / o_ww9b4) |
| 13 tracks | default, knob unset | unchanged against the integration default (no code path changes with n = 0) |

## 5. Reading

- G-H1-REC and G-H1-D2 pass: H1 holds. U-9775 is resolved as "the port's race timer starts at the player's first move; the
  original's runs through the countdown". `COLL_LUT` default-ON and a faithful timer start become decision rows. No default flips here.
- G-H1-REC passes and G-H1-D2 fails: the onset was one residual, not the D2 driver; report the new first split and the scores.
- G-H1-REC fails: H1 refuted for the record; report the new first split.
- Any G-NOOP mismatch: the change is not inert. Stop and report DECISION.
