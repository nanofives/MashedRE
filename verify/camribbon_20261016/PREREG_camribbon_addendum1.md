# PREREG camribbon, addendum 1 (committed BEFORE the first original-side City capture)

Measured so far (these already ran, against PREREG_camribbon.md section 4, and are NOT part of this addendum):
G-C3-TRAIN, G-DIR-TRAIN, G-NODE-ARCTIC, C-OPTOUT, N0, G-SCAN, G-CTRL (baseline, new default, new opt-out). Results are
in RESULT_camribbon.md. The scan found one track with nodes lacking an LED elevation: City (Course_Id 26),
nodes 25, 26, 27 (of 117). All other 12 tracks have an LED elevation on every node.

## A1. Original capture on City (G-NOLED), exact recipe

`py -3.12 re/frida/camera_probe.py --frame --early --track 12 --force-prog <P> --rows 1100 --out <csv> --static-out <json>`

- `--track 12` writes engine track index 12 (City, `scenario_launch.py --track` table) to `0x0063ba7c` and `0x0067f17c`
  before the race starts. The track that actually loaded is judged by the trace's `track_type` column
  (`DAT_00644158` = Course_Id) which must read 26 on every row, and by the static `nodecount` which must read 117.
  If either does not, the capture is discarded and NOT used.
- `--force-prog P` pins the four per-car path progress floats (`0x008a96e8 + i*0x30c`, what `FUN_00408a50` returns) to P
  for the span of each `FUN_00446520` call only (onEnter writes, onLeave restores after the trace row is built). It is the
  only instrumentation beyond reads. The trace's `c{i}prog` column records P.
- Pins: **P = 25.5** (director samples nodes 25 and 26, both no-LED) and **P = 26.5** (nodes 26 and 27, both no-LED) are the
  gated pins. P = 24.5 (24 has LED, 25 does not) and P = 27.5 (27 does not, 28 has) are report-only edges.
- The original's Quick Battle opponents and elimination are untouched, so the racing span may be short (d1r2 saw 175
  racing rows with no input). Rows that are not `comparable` (d1r2 PREREG section 3) are excluded, exactly as in d1r2.

## A2. Standalone replay

`run_cam.py dump <exe> <arm> --track City --trace <csv> --frames 3000` with the new exe, once default (after) and once
`MASHED_NO_CAM_RIBBON_FIX=1` (before). The standalone consumes one trace row per camera update (d1r2 section 6).
`score_camribbon.py pose <trace> <cam_dump.csv>` gives the d1r2 G-POSE metrics (published eye vs the original
RwFrame modelling position, at-axis and right-axis angle) on comparable racing rows (`DAT_0063ba8c == 6`).

## A3. Gates

| gate | criterion | prediction |
|---|---|---|
| **G-NODE-CITY** | the original's live ribbon (the probe's static JSON, 117 nodes, converted with `score_d1r2.py nodes`) equals the new standalone `City` nodes file bit-for-bit on `dir`, `c0`, `c3` | 117 / 117 |
| **G-NOLED-AFTER** | for each gated pin (25.5, 26.5): pose metrics within the d1r2 G-POSE tolerances (pos median <= 0.001 and p95 <= 0.01, at-axis and right-axis angle median <= 0.005 deg and p95 <= 0.05 deg) on the joined comparable racing rows | PASS |
| **G-NOLED-BEFORE** (discriminator) | same metrics, opt-out arm. Must EXCEED at least one tolerance. If it does not, the pin did not exercise `c3`, and G-NOLED is reported VACUOUS (the node-level gates stand on their own) | FAIL |
| **G-PIN-CTRL** (does the method distort?) | same capture recipe on TRAINING (`--track 0`) with `--force-prog 10.5` (both nodes have LED elevation, so c3 is unused). Both arms (after and before) must pass the tolerances. If the pin itself breaks the comparison, the City pass/fail above is not interpretable | both arms PASS |
| **G-PIN-NOOP** | the row-count and `track_type` self-checks of A1 hold on every capture used | hold |

## A4. Item 3 of the brief, decided now from what was read

`0x004481e5..0x004481f9` copies tgt to `cam+0x10..0x18` and `0x004481ff..0x00448208` is the hold-flag branch (PREREG_d1r2
section 1). The standalone's `MASHED_SUBSTATE` path is the D-11073 lane. The `DAT_007f101c` advance and the `+0x10` copy are
read by nothing that the default build executes, and `MASHED_SUBSTATE` is a default-OFF knob. Unless a reader of either
turns up in the default build while the City work runs, item 3 is FILED (tracker row via re-classify, evidence needed
named), not fixed.
