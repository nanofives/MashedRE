# PREREG PORT-TOP-KNOBON-CAPFLAKE (2026-11-09, written before any arm ran)

## Observation (already measured, not an arm)

- The 04G f2 and 04J p1 outliers (navdemo_knob1, first nav run after the gate arms) are the **same capture set**:
  walk_02 sha256/16 `2b5487e980ea0a9f`, walk_12 `27d967126f477737` in both. walk_01 equals the 04E list in both.
- Diff against knob run 2 (same set): walk_02 7588 pixels, max 3; walk_03..12 27084 to 33048 pixels, max 9. Bounding
  box x 0..509, y 12..479 in every one of them. walk_01 (title, no video backdrop) has no diff.
- x 0..509 is the frontend.mpg backdrop quad (512x512 video drawn 1:1 at the origin, exe_main.cpp backdrop draw). The
  PORT-BATCH-TOOL flake (memory navdemo-captures-flake-under-load) covered the same x range.
- The driver order was knob, hand, hand, knob in every set, so "first run after the gates" was always a knob-ON run.

## Hypothesis

H0 (cause): `MpegVideoTexture::Update` (D3d9Render/MpegVideoTexture.cpp) freezes the backdrop in
`MASHED_DETERMINISTIC` mode on whatever buffer `ISampleGrabber::GetCurrentBuffer` holds at the first `Update()`. The
graph runs in real time from `Open()` (asset load) and the first `Update()` comes at the main menu, so the frozen frame
is a function of wall-clock, not of the TOP knob. The outlier is a second frame.

## Change under test (default OFF, nothing changes without the env var)

- Probe (log only, deterministic mode): `F1 frontend.mpg det freeze: live|pinned pos=<100ns> hash=<fnv1a>` once, after
  the one upload. `pos` = `IMediaSeeking::GetCurrentPosition` at the freeze (live) or the pinned sample start time.
- Knob `MASHED_DET_MPG_PIN=<ms>` (deterministic mode, looping graphs only = the frontend backdrop): the grabber's
  BufferCB copies the first sample with start time >= `<ms>`, the graph runs with no reference clock
  (`IMediaFilter::SetSyncSource(NULL)`, so no sample is late or dropped), `Update()` uploads that copy and stops the graph.

## Arms (driver `verify/porttop_20261108/capflake_runs.py`, every run inside test-slot + `mashed_lock.py run`)

- **A1 probe, pin OFF:** 12 nav runs, TOP knob ON, OFF, OFF, ON x3. Record freeze pos / hash and capture class
  (E = 04E list, X = the outlier list, ? = neither).
  Prediction: every run with the same freeze hash has the same capture class, ON or OFF. `pos` varies run to run.
- **A2 pin map:** pinned runs (TOP knob OFF) at `0` ms and at every frame start across the range of A1 `pos` values,
  plus one frame on each side (cap 12 runs). Prediction: one pinned frame gives class E and its hash equals the A1
  hash of the class-E runs. If A1 shows an X run, its frame pinned gives class X (the outlier reproduced on demand).
  If A1 shows no X run, the frames next to the E frame are checked for class X.
- **A3 fix:** pin at the A2 frame that gives class E, TOP knob ON, OFF, OFF, ON x3 sets. Pass: 12/12 class E in all 12 runs.
- **Gates:** `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the new exe, pin unset. Pass: G-CTRL
  `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (merge_train.json refs).

## Decision rule

- A1 + A2 support H0 and A3 passes and gates are unchanged: GREEN. The knob ships default OFF. Turning it on for the
  12-capture check (the harness env, not a game default) is the lead's call. It keeps the 04E list if the pinned frame
  is the class-E frame.
- A3 fails, or A2 finds no frame that gives class E: RED, report the data. H0 refuted (same freeze hash, different
  classes, or a class split by TOP knob at equal hash): RED, the cause is elsewhere and is traced from the draw stream.
