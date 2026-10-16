# PREREG camribbon, addendum 2 (committed BEFORE the first frame capture)

Addendum 1 gates all ran and are reported in RESULT_camribbon.md. This addendum adds the backbuffer comparison, so
"the camera there, before and after" is also shown as images and not only as pose numbers.

## B1. Recipe

- Original: `camera_probe.py --frame --early --track 12 --force-prog 25.5 --rows 1100 --shots-race 30,100 --nav-wait 4`
  on City. Needs the d3d9 shim (`original/d3d9.dll`). It writes `orig_row_<K>.bmp` next to the trace CSV, where K is the
  trace row the shot fired at (printed as `fired at [...]`). Same pin as the gated pose capture, a NEW capture (the
  racing length differs per run), so the replay below uses THIS capture's trace.
- Standalone: `run_cam.py dump <new exe> <arm> --track City --trace <that trace> --shots K-60,K,K+60,...`, once default
  (after), once `MASHED_NO_CAM_RIBBON_FIX=1` (before). `stand_row_<R>.bmp` is the frame whose camera consumed row R
  (d1r2 section 6).
- Score: `score_d1r2.py frame` (re/tools/imgdiff.py mean-abs over the frame).

## B2. Gates

| gate | criterion | prediction |
|---|---|---|
| **G-FRAME-AFTER** | d1r2's G-FRAME on the after arm: for each shot K, imgdiff(orig@K, after@K) < imgdiff(orig@K, after@K-60) and < imgdiff(orig@K, after@K+60) | PASS on every K |
| **G-FRAME-BEFORE-WORSE** | for each shot K, imgdiff(orig@K, before@K) > imgdiff(orig@K, after@K) | holds on every K |

Only the camera is matched: the standalone's cars, shadows and HUD are its own (d1r2 section 3), so absolute numbers
stay at d1r2's level (15 to 28). The criteria are the two inequalities. If G-FRAME-BEFORE-WORSE fails on a K, that K is
reported as "the image metric does not separate the arms there", and the pose gate stays the evidence.

## B3. Collateral (after the build, before the decision)

`MASHED_RACE_DEMO=1` on City, new exe, default: exit 0 and the in-race frames come from the director. A 12-float
`MASHED_CAM_POSE` still overrides (`src = 3`). `MASHED_SUBSTATE MASHED_MODE5_RESET`: no crash.
