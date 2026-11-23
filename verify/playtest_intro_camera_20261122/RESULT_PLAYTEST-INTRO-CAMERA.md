# PLAYTEST-INTRO-CAMERA: the intro camera does not end on the race start (Timgidski)

Mariano's playtest of the default `mashed_re.exe`: "on timgidski que camera at the beginning is not ending on the race start".

## Track

"Timgidski" is a literal ENGLISH.DAT string (Font36.piz). It is the place name of `Arctic.piz`, Course_Id 0
(`re/analysis/cup_place_names_REmap_20260616.md:95`, `re/analysis/SEA_LEVEL_2026-09-29.md`). The original's
camera_probe capture opened `TOASTART\TRACKS\ARCTIC.PIZ` with track_type 0.

## Measurement

Original: `re/frida/camera_probe.py --frame --early --track N --rows 1500`. The rendered basis is the RwCamera
frame LTM (`L_*`), plus the sub-state `DAT_0063ba8c` and the hold flag `DAT_00897fe0`. These are the 12-float basis
rows, not the 6-float CAM_POSE. Port: `MASHED_CAM_DUMP` on the standalone with `MASHED_TRACK_VIEW=<track>
MASHED_CAR=1 MASHED_ROUND=1 MASHED_DETERMINISTIC=1`. Scorer: `cmp_intro_cam.py` computes the angle between the
rendered view directions per frame, aligned two ways: from phase-3 start, and on the release frame. Phase 3's length
follows the QPC clock on the original (903 / 736 / 552 renders here against the port's 916 / 761 / 596), so the run
lengths differ by design.

## What was wrong

The eye path already matched (eye max 0.008 over the first 100 frames). The view direction did not. On Arctic the
port's intro turned toward +x and ended 108.7 deg away from the race camera, so the view snapped at the handover.
The original's jump there is 0.43 deg. `FUN_00445aa0`'s look target `t` had two unwritten inputs.

1. **`FUN_00441c80`** (car-pair midpoint) reads each car's record ring row through `FUN_0046d4a0`, at
   `+0x928 + sel*0x40`, fields `+0x30..+0x38`. The original fills that row with the grid pose before phase 3.
   - The writer is `FUN_0040e590` → `FUN_004704c0` (translation `0x004705fe` / `0x0047060a`, Y rotation
     `0x0047061c` / `0x0047062e`).
   - It runs in two places: in `FUN_004111c0`'s state-2 tail, and on phase 3's first frame in `FUN_004102f0`,
     which calls `FUN_00448700(0,0)`, `FUN_004430a0(1)`, `FUN_0040e590()` and then `FUN_0040d470(1)`.
   - `FUN_0046baa0` (the per-frame hold) writes nothing in `0x00881ec8..0x00881f48`.

   In the exe, that row stays (0,0,0) until the first physics substep after GO, which is where
   `SyncContactRingMatrix` writes it. The u9747 countdown seed (`VehiclePhysics_SeedSpawnPose`) was the only
   pre-GO writer. The sub-state path made it unreachable, and `20efede42` (D5-DEAD-COUNTDOWN-STANDIN) deleted it.
   As a result the intro aimed at the world origin. Arctic's grid sits near x = -26, z = 15, so the origin was
   about 90 deg to the side.

2. **cam `+0x10..+0x18`**: `FUN_00446520`'s tail stores the target point there at `0x004481e5..0x004481f9`
   (`[ebp-0x10]`, `[ebp-0xc]`, `[ebp-8]`, the same value it stores at `+0x97c`). When entry 0 is within 8 units,
   `FUN_00445aa0` pulls `t` toward that point (`0x00445e9b..0x00445f0e`). `SubStateCameraChain` copied only
   `+0x40` → `+4..+0xc` (`0x004481c9`). With the target slot left at 0, the intro turned to the origin again for
   its last ~500 frames.

## Fix

- `TrackRenderer::SubStateCameraChain`: copies `RaceCamera::target()` to `0x00897ff0`, as `0x004481e5` does.
- `CamEntryHost.car_pos`, bound in `SubStateCameraChain`, which `FUN_00441c80` reads through `CarPos`. It returns
  each car's held grid pose in the record convention: x/z from the exe's car, and y = `s_spawnY` (the
  `FUN_004704c0` spawn y), else `pos.y - car_ground_off_`.

The record ring itself is unchanged; see the decision below.

## Results (rendered view angle vs the original)

`cmp_results.txt` has the full output. The fix rows below are from the merged tree (`ea4e6f6f2` + fix).

| track | window | base c553380f3 | fix |
|---|---|---|---|
| Arctic (Timgidski) | phase-3 frames 0..99 | 20.74 | 0.00 |
| | frames 450..599 | 115.81 | 3.63 |
| | last 100 intro frames | 110.45 | 1.89 |
| | handover jump (orig 0.43) | 108.70 | 0.46 |
| Training (control) | frames 0..99 / handover (orig 0.40) | 0.97 / 11.26 | 0.00 / 0.40 |
| Egypt | frames 0..99 / handover (orig 0.48) | 19.38 / 40.83 | 0.00 / 0.47 |

The defect hit every track, scaled by how far the grid sits from the world origin. Training's grid is near the
origin, which is why it passed unnoticed. After GO the residual is 2.6 to 4.3 deg (k 1..59) on base and fix alike.
That is the racing camera's existing gap, untouched here.

The frontend route (`MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_TRACK_SEL=0 MASHED_DETERMINISTIC=1`, Challenge Select
into Arctic) runs the same path. Base gives 20.74 / 27.84 deg on frames 0..99 / 100..299, and the fix gives
0.00 / 0.18. The demo returns to the menu before the release, so the handover itself is measured on the
`MASHED_TRACK_VIEW` route.

## Gates (H_DEF, B_DEF_K0/K1/K26, `verify/savelink_20261023/run_arms.py`, sha256[:8] as merge_train.ps1)

| build | G-CTRL | B |
|---|---|---|
| base c553380f3 (LAN release exe) | 8284be46 / 67931834 / 31fe043e | 1d5bf4f9 x3 |
| fix on c553380f3 | 8284be46 / 67931834 / 31fe043e | 1d5bf4f9 x3 |
| fix merged with ea4e6f6f2 | 8284be46 / 67931834 / 31fe043e | 1d5bf4f9 x3 |
| ring-seed form (e1748857e, superseded) | ffcfed29 / 4eb7f944 / 04eca0ad | 48b84108 x3 |

## Open: the record ring is (0,0,0) before GO (decision for Mariano)

The faithful form of item 1 seeds the ring itself at both `FUN_0040e590` sites (commit `e1748857e`). It moves both
gates (last table row), and D3 (b) falls from 3/3 to 2/3 on B_DEF_K0 and H_DEF. Car 1 fails regime A:
`c1_distinct` 16 against [21,70], `steer_distinct` 28 against [33,96], and `abs_steer_median` 4.0 against [7,23].
The cause is that the AI's pre-GO tick reads the same rows (for example `AiLeafTargets.cpp:332/379`,
`AiTargetClosest.cpp:479`), and so does the countdown path progress (`FUN_0040da50`, u9747). The original reads the
grid pose there. Since `20efede42`, the port's AI reads the origin before GO. Restoring it is a gate-moving flip,
so it stays out of this fix.
