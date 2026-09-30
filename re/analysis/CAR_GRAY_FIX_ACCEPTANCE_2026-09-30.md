# Render fix (a) — grey car chassis: PRE-REGISTERED acceptance — 2026-09-30

Diagnosis is **not** re-derived here. Root cause, evidence and the exact fix are
`re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md` (committed `ebbc4c68`). This file is
written **before any source edit** and states, in advance, what will count as
acceptance. It is not amended after the fact: a rule that fails is reported as a
failure (the pattern the brightness child followed for its A3).

Build slot held by this session. D2 / vehicle physics untouched. The car-brightness
fix (`90bd38c6`) is accepted and is **not** revisited; its open A3 stays under U-9161.

---

## The fix under test (from the note, not re-derived)

`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp` — in `TrackRenderer::LoadCar`
(`:2217`, after `model.Parse` at `:2243`) and `TrackRenderer::LoadCarLiveries`
(`:2604`, after `model.Parse` at `:2651`), i.e. **before** the wheel heuristic
(`:2249-2293`), before `BuildDffBatches` (`:2298` / `:2653`) and before
`LibRw::RaceSubmit_RegisterModel` (`:2301`): erase every batch whose geometry
declares no texture coordinates —

```
(b.geo_flags & 0x84u) == 0        // no rpGEOMETRYTEXTURED (0x04), no TEXTURED2 (0x80)
```

`model.bbox` is **not** recomputed (the note's constraint: the locator atomics extend
it, and it feeds `car_ground_off_` / `car_len_` / `car_height_` at `:2415-2422`).
Scope is the car path only.

**Provenance of the criterion, and its status.** The criterion is *measured*, not
guessed, and the note does not leave it `[UNCERTAIN]`: `re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md`
M2/M3/M4 identify the set by parsing the assets and then confirm it by neutralising
exactly those atomics at asset level and recovering the original's car. What **is**
`[UNCERTAIN]` is the *original's own mechanism* — `FUN_00420420` @ `0x00420420`
scatters atomics into the per-player table at `DAT_0063d9e0 + player*0x2AC` by the
part code returned from `FUN_004b5190` @ `0x004b5190`, and that part code is not
derivable from the DFF (note O1, tracker row **U-9079**). So this fix is a
**measured data-driven equivalent of the original's selection, not a verbatim port**,
and is recorded as such. No `hooks.csv` row and no C-level promotion follows from it.

---

## Asset-side ground truth (re-derived this session, independent of the port)

`verify/car_gray_fix_20260930/dff_atomic_census.py` re-parses all 13 vehicle DFFs
with `re/tools/dff_dump.py`'s parser and applies the *same* `& 0x84` predicate. Run
2026-09-30, `--liveries` confirms liveries 1/2/3 match slot 0 on every car:

| piz | dff | atoms | kept | dropped | hulls | locators | dropped tri counts |
|---|---|---:|---:|---:|---:|---:|---|
| Advantag | ADVANTAGE0 | 71 | **44** | **27** | 4 | 23 | `1x23 40x1 44x1 56x1 60x1` |
| Atmos | ATMOS0 | 67 | 39 | 28 | 4 | 24 | `1x21 2x3 28x1 56x1 68x1 96x1` |
| BamBam | BAMBAM0 | 69 | 46 | 23 | 4 | 19 | `1x15 2x4 38x2 50x2` |
| Bullet | BULLET0 | 69 | 49 | 20 | 4 | 16 | `1x15 2x1 88x1 110x1 120x1 142x1` |
| Cooler | COOLER0 | 66 | 41 | 25 | 4 | 21 | `1x15 2x6 32x1 48x1 52x1 68x1` |
| Creeper | CREEPER0 | 66 | 41 | 25 | 4 | 21 | `1x17 2x4 44x2 92x2` |
| Crusader | CRUSADER0 | 69 | 43 | 26 | 4 | 22 | `1x18 2x4 40x1 48x1 64x1 72x1` |
| Formula | FORMULA0 | 70 | 45 | 25 | 4 | 21 | `1x15 2x6 56x4` |
| Kustom | KUSTOM0 | 69 | 49 | 20 | 4 | 16 | `1x15 2x1 44x1 52x2 60x1` |
| Shorty | SHORTY0 | 69 | 43 | 26 | 4 | 22 | `1x16 2x6 32x1 48x1 64x1 80x1` |
| Shuriken | SHURIKEN0 | 67 | 39 | 28 | 4 | 24 | `1x23 2x1 72x1 144x1 148x1 220x1` |
| Sputter | SPUTTER0 | 69 | 43 | 26 | 4 | 22 | `1x21 2x1 28x1 36x1 48x1 56x1` |
| Stallion | STALLION0 | 67 | 45 | 22 | 4 | 18 | `1x18 48x1 52x1 84x1 86x1` |

This reproduces the note's M4 table on all of `atoms` / `notex` / `texAt` / `hulls`
/ `1tri`. The hull-vs-locator cut is read off the data, not assumed: every dropped
atomic has either **1-2** triangles (locator quads) or **>=28** (the four car-sized
shells). Nothing lands between 3 and 27 on any of the 13 vehicles, so a cut at 10 is
unambiguous. (The note's `1tri` column counts the 1- and 2-triangle quads together;
a naive `ntris==1` split disagrees with it on 8 cars and is the wrong reading.)

The player car on TRAINING is **ADVANTAGE** (`Race/RaceSession.cpp:128`,
`kVehicles[0]`; `log/mashed_re.log` "R6 car liveries: original/TOASTART/VEHICLES/Advantag.piz
base=ADVANTAGE loaded=3/3"), so every G1 number below is scored against the
**71 / 44 / 27** row.

---

## Capture protocol (both arms identical)

`py -3.12 verify/car_gray_fix_20260930/run_race.py <exe> <outdir> 12,0`

The standalone drives its own race flow (`MASHED_RACE_DEMO=1`, `MASHED_GOTO=6`,
`exe_main.cpp:1438`), so no run takes the foreground and the capture is the real
640x480 backbuffer through `DumpBackbufferBMP`. `MASHED_DETERMINISTIC=1`, so frame N
pre-fix and frame N post-fix are the **same pose** — G4 compares identical poses,
not poses within jitter. Every run muted (`MASHED_MUTE=1`), `MASHED_WIN_POS=left-bl`,
`MASHED_TITLE` set, `MASHED_NAV_DEMO` never set, PID tracked and killed only on
timeout, capture read only after process exit.

Tracks: **12 = training** (the note's and the brightness child's track) and
**0 = Arctic** (the second track A4 was scored on). Captures per track:
`race1/{00_challengeselect,01_grid,01_inrace_track,01_action,02_back_to_menu}.bmp`.

Channels collected per run: `log/mashed_re.log` (R5/R6 car-load lines),
`./mashed_re.log`, `./mashed_re_carlight.log` (the `CARLIGHT ... body_batches:` line)
and `drawstream3d.json` (`MASHED_DBG_DRAWSTREAM3D=1` -> the camera-INVARIANT
per-category `{batches, verts, textured}` tally, `DrawStreamDump.cpp:127`).

---

## The rules

### G1 — the dropped set is exactly the set the note identifies, counted per car

A load-time log line is added by this session to `LoadCar` and `LoadCarLiveries`
reporting, per DFF: `atoms=` (batches parsed), `kept=`, `dropped=`, and `nwheels=`.

1. **G1a** — PRE-fix arm must log `atoms=71 kept=71 dropped=0` for `ADVANTAGE0.DFF`
   and for each of `ADVANTAGE1/2/3.DFF`. This is the armed-control half: a counter
   that reports 0 dropped because it never ran looks identical to one that reports
   0 dropped because there was nothing to drop (memory
   `arm-coverage-counters-before-first-run`, `absent-log-proves-nothing-run-a-control`).
   If the pre-fix arm does not print the line at all, G1 is **unmeasurable** and
   this session stops and says so.
2. **G1b** — POST-fix arm must log `atoms=71 kept=44 dropped=27` on **all four**
   ADVANTAGE DFFs — exactly the asset census row above. Any other number fails G1.
3. **G1c** — `CARLIGHT ... body_batches: tot=` must move **67 -> 40**
   (71 atomics - 27 dropped - 4 wheel atomics, which the body tally excludes).
   Pre-fix value 67 is already on record (`verify/car_bright_fix_20260930/post/12_training/mashed_re_carlight.log`).
4. **G1d** — `drawstream3d.json` `"cars"."batches"` must fall pre->post, on every
   captured frame, by the same proportion the load counts predict.

### G2 — the grey chassis pixels disappear

Instrument: `verify/car_gray_fix_20260930/gray_frac.py`. GREY = achromatic
(`max-min <= 14`) with luma in `[30,130]` — the hull material is `(102,102,102)`,
R==G==B, and every lighting term in both renderers is a per-channel scale by the same
lit-vertex colour, so a hull pixel is achromatic under any light and its luma lands in
`102 x [ambient .. 1.0] = [51 .. 102]`. PAINT = the selector
`verify/car_bright_20260930/body_ratio.py` already uses (`R>=32, R>1.6G, R>1.6B`), so
the counts are directly comparable with the brightness child's numbers.
`grey_frac = n_grey / (n_grey + n_paint)` is a ratio of two car-surface classes, which
is why it survives the two sides framing the car differently.

Scored on TRAINING `race1/01_grid.bmp`, car box **`(225,290,420,480)`** — fixed from
the PRE-fix frame and reused unchanged on the post-fix frame (the pose is identical
under `MASHED_DETERMINISTIC=1`). Terrain does not enter the GREY class there: the box
corners measure `(136,130,97)` / `(146,135,92)` / `(117,106,72)`, all with
`max-min >= 39`.

Pre-fix values, measured **before** the edit exists, on the HEAD build's TRAINING
`01_grid` (`verify/car_bright_fix_20260930/post/12_training/race1/01_grid.bmp`,
exe `E24D9C28...`): `n_box=37050  n_grey=21327  n_paint=3801  grey_frac=0.8487`,
grey dominant exactly `(102,102,102)` at **n=1999**, paint dominant `(236,52,60)`
= texel x 1.0000 at n=1536.

1. **G2a** — post-fix `grey_frac <= 0.30` on that box (from 0.8487).
2. **G2b** — the count of pixels **exactly** `(102,102,102)` in that box falls from
   **1999** to **<= 50**. This is the literal hull tone: the untextured material
   colour at full lit-vertex scale.
3. **G2c** (cross-side band) — the ORIGINAL, `verify/car_gray_20260929/orig_bb_a.png`
   (in-race TRAINING, 4 cars, backbuffer through the d3d9 shim), player-car box
   **`(276,323,345,400)`** — the tight bbox of its paint mask, `(278,325,342,397)`,
   dilated 2-3 px — measures `n_box=5313  n_grey=400  n_paint=2341
   grey_frac=0.1459`; its grey is the windscreen/rear-window glass, which the fix
   keeps (the `Glass` material is textured). Post-fix `grey_frac` must land in
   **`[0.00, 0.30]`**, i.e. in the original's regime rather than the pre-fix regime.

   **Stated limitation, up front:** the two poses are **NOT** matched. The original
   capture is a wider 4-car chase view at a different heading; `race1/01_grid` is a
   close rear chase. A matched-pose original TRAINING capture is exactly what
   **U-9161** (note item O6 of `CAR_BRIGHTNESS_2026-09-30.md`) already owes. G2c is
   therefore a **class-fraction band, not a pixel diff**, and it cannot be tightened
   below `0.30` until that capture exists. It is reported with that caveat attached,
   not as a matched-pose measurement.

### G3 — no textured car batch is lost, and the car still renders at the post-brightness level

1. **G3a** — the load log's `kept=` equals the asset census `kept` (**44**) on all
   four ADVANTAGE DFFs, and `drawstream3d.json` `"cars"."textured"` is **identical**
   pre->post on every captured frame.
2. **G3b** — `nwheels=4` post-fix (it is 4 pre-fix). The wheel heuristic at `:2249`
   runs on the filtered set, so this is the rule that catches a wheel atomic being
   taken with the hulls.
3. **G3c** — A2-style brightness check, same instrument and the **same box** the
   brightness child's A2 used: `verify/car_bright_20260930/body_ratio.py` on the
   up-facing rear-deck box **`(272,315,372,336)`** of TRAINING `01_grid.bmp`. Post-fix
   dominant ratio-to-texel must be **>= 0.80**. Pre-fix at HEAD is **0.8517** at
   `n_body=1329` (commit `8f065625`). `n_body` is expected to RISE post-fix (paint
   the shell was covering becomes visible); a rise is not a failure, a dominant
   ratio below 0.80 is.

### G4 — terrain, sea and props do not move

Instrument: `verify/car_bright_fix_20260930/a4_scope.py` at `--thr 0` (the poses are
bit-identical, so there is no jitter floor to absorb), plus
`verify/car_bright_fix_20260930/terrain_region.py` region aggregates on the boxes A4
already used.

Pre-fix vs post-fix, same deterministic pose, on TRAINING `01_grid` / `01_action` /
`01_inrace_track` and Arctic `01_grid` / `01_action`:

1. **G4a** — every differing pixel must lie **inside the union of the car regions of
   the PRE-fix frame**. Those regions are declared per capture from the pre-fix arm
   *before* the post-fix binary exists, as the bounding box of each connected
   component of the pre-fix `GREY u PAINT` mask, dilated by 4 px. This is a sharp
   rule, not a judgement call: removing the hull can only *uncover* pixels that the
   pre-fix car silhouette already covered, so a differing pixel outside that
   silhouette is by construction not attributable to the fix.
   `a4_scope.py --car <box> ...` reports the count; **`outside` must be 0**.
2. **G4b** — the A4 terrain/sea region aggregates (TRAINING left/right/mid-far
   terrain and the RIGHT-TERRAIN band; the three Arctic ice-water boxes) must be
   **bit-identical** pre->post: 0 differing pixels and region means equal to three
   decimals.
3. **G4c** — the frontend capture `00_challengeselect.bmp` must be bit-identical
   pre->post (the fix is in a race-load path; the menu must not move at all).

---

## What makes this stop rather than pass

* Any rule that cannot be measured with the channel it names -> **stop and report**,
  do not substitute another channel and do not relax the rule.
* Any rule that fails -> reported as a failure, and the source edit is **reverted**
  unless the failure can be shown to lie outside the fix's scope; if the edit is kept
  despite a failure, the RESULT block says so explicitly with the reason.
* No tracker row is mutated except through the `re-classify` skill.

## Artefacts

| path | what |
|---|---|
| `verify/car_gray_fix_20260930/dff_atomic_census.py` | asset-side ground truth (table above) |
| `verify/car_gray_fix_20260930/gray_frac.py` | the G2 GREY/PAINT counter |
| `verify/car_gray_fix_20260930/run_race.py` | the capture driver |
| `verify/car_gray_fix_20260930/bin/` | both pinned exes + `SHA256SUMS.txt` (exes gitignored) |
| `verify/car_gray_fix_20260930/pre/`, `post/` | the two arms' captures and logs |
