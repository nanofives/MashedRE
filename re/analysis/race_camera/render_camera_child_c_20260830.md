# Race render camera: verbatim race_cam vs the invented chase rig (child-C, 2026-08-30)

Branch `race/camera`, worktree `.worktrees/race-camera`, based on c779cbf3.
All measurements on TRAINING (MASHED_TRACK_SEL=12), Quick Battle / MASHED_RACE_DEMO.

## What was done

The in-race render eye/target was overridden by an invented "WS-E s3 GROUND CHASE"
rig (TrackRenderer.cpp:4097-4123) with hand-tuned kBack/kUp/kAhead multipliers on
`car_len_`. This session wired the render to the VERBATIM race-camera director
instead: `race_cam_.pos()` / `race_cam_.target()` (the FUN_00446520 port,
RaceCamera.cpp) now drive `eye`/`at` when `car_ready_` and `MASHED_CHASE_RIG` is
unset (default ON). Old rig kept behind `MASHED_CHASE_RIG=1` for A/B.
`MASHED_CAM_POSE` (the parity harness) is untouched — it still overrides afterward.
Added `MASHED_DBG_CAM=1` → `log/cam_re.txt` dumping the director pos/target and the
resolved eye/at each frame. Build clean (`mashed_re.exe`; the `.asi`-to-`original\`
deploy step fails by design — no `original\` junction in this worktree, per the hard
rule).

## Can race_cam drive the render?

YES, mechanically. `UpdateCar`->`UpdateRace`->`race_cam_.Update` (:3488) runs before
`Render` (exe_main.cpp:2797/2810 -> :2921), so `race_cam_.pos()/target()` hold the
current-frame director output. Confirmed live: `use_race_cam=1`, cars framed, tracking
view down the road (verify/camera_racecam/race1/01_inrace_track.png).

One caveat found: during the pre-race grid/countdown the director returns early
(`RaceCamera::Update` line 242 `if (!nodes_ || node_count_ <= 0) return;`) so
`pos()/target()` stay {0,0,0} for the first ~181 frames of the standalone; the new
branch falls through to the chase-rig geometry until the director primes. The first
LIVE director frame is f181.

## Numeric basis comparison (the primary signal)

race_cam first LIVE output (f181, tight grid, reqzoom=2.04):
  pos = (0.03191, 2.65027, 4.35754)   target = (0.03191, 0.54052, -1.38182)

Original reference, two DIFFERENT representations captured from the same frame:
  A) CONTROLLER pose (DAT_00897fe0 +0x40 eye / +0x4c at):
       eye = (0.04454, 3.15413, 5.00376)   at = (-0.05675, 0.48602, -1.45778)
  B) RwCamera FRAME basis (*(cam+0x84)+4 LTM), WITH ROLL:
       pos   = (1.42528, 2.89283, 19.67259)
       right = (-0.87664, 0.47269, -0.08985)   (right.y=0.473 => ~26deg roll)
       up    = ( 0.48115, 0.86122, -0.16370)
       at    = (-0.00000, -0.18674, -0.98241)

| comparison | eye/pos dist | forward-dir angle |
|---|---|---|
| race_cam vs CONTROLLER pose (A) | **0.82** | **2.40 deg** |
| race_cam vs FRAME basis (B) | 15.38 | 9.42 deg |
| invented chase rig vs CONTROLLER (A) | 4.68 | — |
| CONTROLLER (A) vs FRAME (B) | 14.74 | — |

race_cam reproduces the original's race-DIRECTOR output (the CONTROLLER pose A) to
~0.8 units / 2.4 deg at a comparable early-race moment. It is a faithful port of
FUN_00446520. The invented chase rig does not track the director (4.68 off).

## Pixel diff vs orig_race.bmp (backstop; 640x480, grid 8x6)

Transplanted via MASHED_CAM_POSE (removes car-config / timing confounds, isolates the
camera pose), diffed against the reference render:

| camera pose | mean abs diff | % over threshold 16 |
|---|---|---|
| FRAME basis B (12-float, rolled, z=19.67) | **20.52** | **48.22%** |
| CONTROLLER pose A (6-float, level, z=5.0) — what race_cam produces | 58.41 | 88.72% |
| invented chase rig (current shipping, at grid) | 71.12 | 92.83% |

Ranking (lower better): **FRAME basis (20.5) >> race_cam/CONTROLLER (58.4) > chase rig (71.1)**.

The reference `orig_race.bmp` was rendered with the ROLLED, DISTANT frame basis B: the
image horizon is tilted ~26deg and the cars sit small near the vanishing point
(~19 units away). The controller pose A (level, close, z=5) reproduces the reference
far worse (88.7% vs 48.2%). race_cam, as ported, produces pose A — so wiring it into
the render (roll-free Y-up LookAt) lands FARther from the reference than the frame
basis, though still better than the invented rig it replaces.

## Verdict on the :4101 "high orbit looking down" claim

**REFUTED.** The claim justifying the invented rig was that the shared 4-car race
camera "sits in a high orbit looking down — wrong for the single-car standalone view."
Neither the render camera nor the director is a high orbit:
- FRAME basis B: eye height y=2.89, forward pitched DOWN only 10.8deg
  (at.y=-0.187) — a low, near-horizontal view down the road, as the reference image
  plainly shows (cars ahead on the road, not below).
- CONTROLLER/director pose A: y=3.15, ~22deg down — also low, not overhead.
- The race_cam-live standalone frame is a sensible behind-the-pack tracking shot down
  the road (verify/camera_racecam/race1/01_inrace_track.png), NOT an orbit.

So the invented rig was built on a false premise, and it also scores WORST of the
three (71/92%). The "high orbit" only occurs transiently at max zoom (fully spread
pack), and even then it is a pulled-back tracking view, not a top-down orbit —
race_cam itself reaches pos ~(15,-1,-20), ~20 units out, at spread-pack frames
(cam_re.txt f391), confirming the director produces the distant framing when the pack
strings out. The reference frame IS such a spread-pack moment (cars at z=-2..1.3).

## The real remaining gap: ROLL (not framing)

The port's `RaceCamera::Update` (RaceCamera.cpp:482-495) writes only pos_out_/tgt_out_
and explicitly defers the frame build to a Y-up LookAt with **roll = 0**. But the
reference render has ~26deg of roll (right.y=0.473), and the pixel diff shows the
rolled frame basis (20.5) beats every level pose (58-71) by ~2.8x. Roll is the
dominant reproducible error, exactly the class the parent fixed in c779cbf3.

Where the original's roll comes from — cited, not the port's punt:
- Camera::Apply (0x00441760, 00441760.md l.19-22) rebuilds the frame with THREE
  rotations: yaw about world-Y by cam[+0x38], pitch about the frame right-axis by
  cam[+0x34], and a THIRD rotation about the frame at/forward axis by **cam[+0x3c]**
  (`FUN_004c4d20(iVar2+0x30, *(param_1+0x3c), 2)`). That third rotation IS a roll.
- The director FUN_00446520 (00446520.md l.31-32) writes angles param_1[0xd/0xe/0xf]
  = +0x34/+0x38/**+0x3c** — i.e. it computes and stores a roll angle [0xf].
- The port omits [0xf] entirely, so `race_cam_` cannot carry roll and the render is
  built with a level LookAt.

## Conclusion / handoff

- race_cam CAN drive the render and its pos/target faithfully reproduce the original
  race-director output (~0.8 units / 2.4deg). Framing (distance/pitch/pack-tracking)
  is correct; the invented "high orbit" justification at :4101 is false and the rig it
  motivated scores worst.
- BUT the ported director outputs a roll-free look-at pair, and the original render
  camera is ~26deg rolled. Until the port computes cam[+0x3c] and publishes the full
  rolled basis through RaceSceneState (last_right_/last_up_/last_atdir_/
  last_basis_valid_, added in c779cbf3), driving the render from race_cam scores 58/88%
  — worse than the frame-basis transplant (20/48%), better than the chase rig (71/92%).
- NEXT STEP (needs the FUN_00446520 tail in Ghidra, parent session): transcribe the
  angle write-out for [0xd/0xe/0xf] (via FUN_004a3620 yaw / FUN_004c3b30 and whatever
  sets the roll [0xf]) into RaceCamera::Update, emit right/up/at, and have the render
  build the view from the basis (MatViewFromBasis) instead of MatLookAtLH.
- [UNCERTAIN] Exact-distance parity of race_cam vs the frame basis for the SAME car
  config was not proven — the standalone's AI/physics gives a different pack spread at
  any given time, so a same-config race_cam-live pixel match is confounded. Missing
  evidence: a frame where the standalone pack matches the original's (z=-2..1.3) with
  race_cam live, or a Frida read of DAT_00897fe0+0x40 and the +0x84 frame LTM in the
  same original frame to confirm the frame basis == director-pos + roll (not a second
  camera). The distance-in-range check passed (race_cam reaches ~20 units), the roll
  formula is the un-transcribed piece.

## Repro

```
# race_cam driving (default), with per-frame camera dump:
$env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_DETERMINISTIC="1"
$env:MASHED_TRACK_SEL="12"; $env:MASHED_WIN_POS="left-bl"
$env:MASHED_ROOT="C:\Users\maria\Desktop\Proyectos\Mashed"
$env:MASHED_DBG_CAM="1"; $env:MASHED_VERIFY_OUT="<wt>\verify\camera_racecam"
mashedmod\build\mashed_re.exe   # wait for exit
# A/B: MASHED_CHASE_RIG=1 reverts to the invented rig.
# transplant either reference pose with MASHED_CAM_POSE (6- or 12-float).
py -3.12 re\tools\imgdiff.py verify\parity_race_20260830\orig_race.bmp <cap>\race1\01_grid.bmp --grid 8x6
```
