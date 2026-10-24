# RESULT d1r2 stills: in-race side-by-side for Mariano's R2 review (P0-2 A)

Three stills, TRAINING Quick Battle, original left, standalone default (R2 ON) right, 1280x480 each.
They are the existing G-FRAME captures from `verify/d1r2_20261016/` (copied here, not re-captured, so the
cited numbers in `RESULT_d1r2.md` section 3 apply unchanged). No fresh boot was run on this step.

| file | race row K | phase | G-FRAME mean-abs orig@K vs standalone@K (vs @K-60 / @K+60) |
|---|---|---|---|
| `sbs_row_00884.jpg` | 884 | racing, start of the 175-row window | 27.77 (43.63 / 46.81) |
| `sbs_row_00924.jpg` | 924 | racing, mid | 21.94 (40.66 / 42.64) |
| `sbs_row_00959.jpg` | 959 | racing, end of window | 15.75 (46.00 / 38.76) |

## How they were made
Original: `re/frida/camera_probe.py --frame --early` on TRAINING Quick Battle (original backbuffer dump at row K).
Standalone: the exe replays the original's recorded camera trace into `race_cam_` and dumps its backbuffer at
row K (`verify/d1r2_20261016/run_replay.py`). Pairing: `verify/d1r2_20261016/score_d1r2.py --sbs`.
Standalone env is the default: `MASHED_NO_RACE_CAM_BASIS` unset.

## What to look for
- **Framing is the review item.** Horizon, the Supersonic/Empire boards on the left rail, the oil-tank cluster
  and the right-hand gantry and screen should sit at the same pixel positions in both halves in all three rows.
  They do, to the eye, in rows 924 and 959. Camera residual vs the original is eye median 0.00011, at-axis median 0.0005 deg
  (`scores.txt`).
- **Expected differences, not camera:** the right half has no cars (only the camera is replayed, the standalone race
  runs underneath, so the cars are elsewhere), no HUD (standings strip and badges), and different shadows and lighting.
  Transplanting the original's own row-959 frame scores 15.75, identical to the ported camera, so none of this is R2.

## Not delivered
- The `MASHED_NO_RACE_CAM_BASIS=1` contrast stills were not captured (the task marked them optional and this step
  ran text-only on existing captures). A fresh arm is cheap to add on request.
