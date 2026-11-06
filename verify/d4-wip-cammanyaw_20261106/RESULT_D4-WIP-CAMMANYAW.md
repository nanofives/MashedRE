# RESULT D4-WIP-CAMMANYAW (2026-11-06)

Verdict: **knob OFF GREEN (byte-identical to base), knob ON mixed.** `MASHED_CAMMANYAW=1` brings the TRAINING
camera-man to within 0.020 deg of the original on all three rows. On Arctic 1051 it moves the yaw 29.453 deg away from
the original (the default table yaw is 1.735 deg off). The knob stays default OFF. It is safe to merge as an opt-in, and
it is not ready to flip. No tracker row moved.

## Code under test

`orch/D4-WIP-CAMMANYAW@dc50ff712` (base `a4fa5e5aa`): `CamEntryType1Step441d40` (0x00441d40) stepped at the
original's own site in `CameraEntryDispatchAll` (type 1, `0x004464e8..0x004464f5`) behind `MASHED_CAMMANYAW=1`,
`UpdateCamMen` posing from entries 1..N (`0x00448dc0`, getters `FUN_00442420` / `FUN_00442410`). The build note is in the
commit message. This step adds only the RESULT, the scorer fix below and the evidence.

## Recipe

`verify/rsky_20261016/run13.py run --tracks 3,13` (TRAINING and Arctic d1r1 captures `log/d1r1/orig_{training,arctic}`,
shots 884/924/959 and 1051/1091/1126): recipe F, `MASHED_CAM_REPLAY_BASIS=1`, `MASHED_CAR_REPLAY=1`,
`MASHED_DBG_D3DREC=1`, `MASHED_CAM_REPLAY_P3=1`. Private root `D:\MashedWT\D4-WIP-CAMMANYAW_root`, one test slot,
each boot inside `mashed_lock run --label D4-WIP-CAMMANYAW`, one game at a time (`drive_cy.sh`).

The handoff's own runner (`verify/rprops_20261016/run_replay3d.py`, `MASHED_ROUND_RULE=4`, no P3) wrote **no shots** on
this base: with `MASHED_SUBSTATE` default ON (since the 2026-10-30 flip) trace rows are consumed from the phase-3 entry
only with `MASHED_CAM_REPLAY_P3=1` (`TrackRenderer.cpp:8458`), so `cam_replay_row()` never reached row 884 inside 1300
frames. That first pass (6 arms, 09:08..09:11) is void and was discarded.

| arm | exe | knobs |
|---|---|---|
| `yaw` | `wip-cammanyaw` `mashed_re.exe` @ `dc50ff712` | `MASHED_CAMMANYAW=1` |
| `off` | same exe | none |
| `base` | `wip-panelsort` `mashed_re.exe` @ `2a3b259fa` (base `a4fa5e5aa` plus an `.asi`-only TU, standalone identical to base) | none |

All six runs: exit 0, first attempt, 6/6 shots at `actual_row == K`.

## Knob ON: yaw against the handoff witness (`camman_yaw.py`, yaw = atan2(w[2], w[0]) of the draw record)

| frame | original | knob OFF (table yaw) | knob ON | handoff i1 (step every frame, old base) |
|---|---|---|---|---|
| TRAINING 884 | 4.398 | 0.000 (-4.398) | 4.378 (**-0.020**) | 4.972 (+0.574) |
| TRAINING 924 | 4.398 | 0.000 (-4.398) | 4.378 (**-0.020**) | 6.552 (+2.155) |
| TRAINING 959 | 4.398 | 0.000 (-4.398) | 4.378 (**-0.020**) | 9.143 (+4.746) |
| Arctic 1051 | -177.085 | -178.820 (-1.735) | -147.632 (**+29.453**) | -177.047 (+0.038) |

Translation d_pos 0.0000 and 177/177 triangles on every pairing. Arctic 1091 / 1126 record no camera-man on either side.
`camman_yaw_yaw_off.txt`.

- TRAINING: the yaw is constant over the three state-6 rows, as in the original. The i1 drift (stepping while racing) is
  gone. The 0.020 deg residual is constant.
- Arctic 1051: 29.453 deg wrong. This is not explained here. [UNCERTAIN U-9699] which input to the type-1 step differs on
  Arctic (the step count before row 1051, the car-pair target from `CamCarMidpoint441c80`, or the entry's initial
  `+0xb0`). Evidence missing: a per-call dump of entry `+0xb0` and the step's target on both sides for Arctic.
  U-9699 stays open. Its row is not edited by this step.

## Knob OFF: identical to default

`off` vs `base`, byte compare (`cmp`) of every output: 16/16 files identical (per track: 3 `.bmp`, 3 `.draws.json`,
`cam_dump.csv`, `librw_dump.csv`). `PROP-CAMMAN course_id=30 clump=1 helper=0x00451220 instances=3` is unchanged.

Knob ON vs OFF: exactly one draw event differs per camera-man row (TRAINING 884/924/959 `CamManHot`, Arctic 1051
`CamManCold`, out of 162..225 events). Arctic 1091 / 1126 are byte-identical. The knob touches the camera-man and nothing
else on these frames.

## Expected gate effect

None from the default build: knob OFF is byte-identical to base on the replay outputs, and the `.asi` hook for
0x00441d40 is `L2_CamEntryType1Step441d40` (installs only with `MASHED_HOOK_ONLY` / `MASHED_HOOK_LANE2=1`). The merge
train's G-CTRL / B gates are the confirmation and were not run in this step.

## Harness change

`verify/rprops2_20261016/camman_yaw.py`: `draws()` returns no records for a missing file, so an arm without City scores
(the script crashed on `yaw_city`). Scoring of present files is unchanged.

## Raw data (not committed)

`D:\MashedWT\D4-WIP-CAMMANYAW_data\r13_{yaw,off,base}\{03_Arctic,13_Training}\stand\` (draws, BMPs, cam/librw dumps,
ptrace). SHA-256 of all 18 `.draws.json`: `draws_sha256.txt`.

| file | SHA-256 |
|---|---|
| `wip-cammanyaw/mashedmod/build/mashed_re.exe` (@ `dc50ff712`, `mashedmod/` clean) | `3f735ca7ee694aeb460fece64df7c4a7bdbf421f78fb9bf715b4c6a11f4da00d` |
| `wip-panelsort/mashedmod/build/mashed_re.exe` (base arm) | `2014e16d295397fc1daeefde221c4b9adad9b286ecdd3c243a8fb7daf9eb85b5` |
