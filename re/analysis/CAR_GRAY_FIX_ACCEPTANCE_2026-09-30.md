# Render fix (a) — grey car chassis: PRE-REGISTERED acceptance — 2026-09-30

> **STATUS 2026-09-30, after the run — see `# RESULTS` at the bottom.**
> **G1 PASS. G2 PASS. G3 PASS. G4b/G4c PASS.**
> **G1d UNMEASURABLE as written** (the `"cars"` drawstream3d record does not exist on
> the default librw build); the named channel was measured on the legacy-renderer arm
> instead and is reported there, labelled, not as a substitute.
> **G4a FAILS as written** on the two Arctic captures (879 + 22 differing pixels outside
> the declared regions). The failure is shown to be in G4a's own region-declaration
> procedure, not in the fix: every one of those 901 pixels is a hull pixel lying inside
> a car's single differing-pixel component. **The edit is KEPT**; the reason is stated
> in RESULTS, rule G4a.
> Nothing below this banner was edited after the fix existed.

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

---

# RESULTS — 2026-09-30

Fix commit `5ddc0384`. Pre-registration commit `2179ed1a` (written before the fix
existed; **not amended** — the failing rule below is reported as a failure).

## Binaries

| role | SHA-256 | size |
|---|---|---|
| `bin/prefix_head_mashed_re.exe` — clean HEAD `2179ed1a`, no edits | `795A1927A788052DB4D6251BCE81E964D2A411DB922E14FC63FF4ED0080261B6` | 1 960 960 B |
| `bin/prefix_instr_mashed_re.exe` — **the PRE arm**: HEAD + the load-time census log only, no filter | `4D206591FAB74E15CC21685DA96301B3AF865B37E42A4AE2BE2D96E51F13A3E4` | 1 961 472 B |
| `bin/postfix_mashed_re.exe` — **the POST arm**: + `CarDropNonRenderAtomics` | `F77055CD397946AC7554013B0AF0D21A5C9299828C1BC427C2A9B7B47E8C2099` | 1 961 472 B |

The MSVC link embeds a PE timestamp, so two builds of identical sources do **not**
hash equal. No claim of binary identity is made from these hashes.

**The census log is render-inert, measured not assumed:** clean HEAD vs the instrument
build is **0 differing pixels on all 8 captures** (TRAINING and Arctic, race and
frontend). So `prefix_instr` is a legitimate pre-fix arm and G1a's `dropped=0` comes
from a channel that is present and firing, not from an absent one.

**Two boots per arm, as required before believing any capture-level verdict** (memory
`shadow-lane-failure-windows`): pre boot1 vs boot2 = **0** on 6/6; post boot1 vs boot2
= **0** on 6/6. One anomalous boot was caught by this and is recorded below under
"transient boot".

## Rule-by-rule

| rule | measured | n | verdict |
|---|---|---|---|
| **G1a** armed pre-fix control | `atoms=71 kept=71 dropped=0 untex_kept=27` on ADVANTAGE0/1/2/3, both tracks | 4 DFFs x 2 tracks | **PASS** |
| **G1b** post-fix dropped set | `atoms=71 kept=44 dropped=27 untex_kept=0` on ADVANTAGE0/1/2/3, both tracks — exactly the asset census row | 4 DFFs x 2 tracks | **PASS** |
| **G1c** `CARLIGHT body_batches: tot=` | **67 -> 40**, both tracks (71 − 27 dropped − 4 wheel atomics) | 2 tracks | **PASS** |
| **G1d** `drawstream3d "cars".batches` falls | record ABSENT on the default build | — | **UNMEASURABLE as written** |
| **G2a** `grey_frac` on the car box | **0.8487 -> 0.0782** (rule <= 0.30) | n_box=37 050; n_grey 21 327 -> 1 515; n_paint 3 801 -> 17 853 | **PASS** |
| **G2b** exact hull tone `(102,102,102)` | **1999 -> 0** (rule <= 50) | same box | **PASS** |
| **G2c** cross-side band | post **0.0782** in `[0.00, 0.30]`; ORIGINAL **0.1459** (n_box=5313, n_grey=400, n_paint=2341) | — | **PASS** (band, not a matched-pose diff) |
| **G3a** `kept` = asset census | `kept=44` on all four ADVANTAGE DFFs, both tracks | 8 loads | **PASS** (load-log half; the drawstream3d half is G1d's absent record) |
| **G3b** wheels still identified | `wheels=4` both tracks, **identical pivots and radii** pre and post: `[-0.18,0.09,-0.35 r=0.09] [0.18,0.09,-0.35 r=0.09] [-0.17,0.09,0.27 r=0.09 F] [0.17,0.09,0.27 r=0.09 F]` | 2 tracks | **PASS** |
| **G3c** A2 deck box `(272,315,372,336)` | dominant `(201,44,51)` = **0.8517** pre AND post (rule >= 0.80); `n_body` 1329 -> **1767** as predicted | n_body=1767 | **PASS** |
| **G4a** 0 differing px outside the declared pre-fix car regions | TRAINING 01_grid **0**, 01_action **0**, 01_inrace_track **0**; **Arctic 01_grid 879**, **Arctic 01_action 22** | 5 captures | **FAIL as written** — cause pinned below |
| **G4b** A4 terrain/sea boxes bit-identical | **0 differing pixels in all NINE boxes**, means equal to 3 decimals | 52 580 + 50 190 + 18 000 + 24 000 + 76 800 + 16 800 + 76 160 + 28 640 + 23 100 px | **PASS** |
| **G4c** frontend bit-identical | `00_challengeselect` and `02_back_to_menu`, both tracks: **n_diff = 0** | 4 captures | **PASS** |

## G1d — UNMEASURABLE as written, and what the named channel says elsewhere

`MASHED_DBG_DRAWSTREAM3D` emits no `"cars"` record on the **default** build. The tally
is written from `RenderCarsRelit` (`TrackRenderer.cpp:4856`), which is reached only
through the `else if (relit_cars)` arm at `TrackRenderer.cpp:5705`; the default librw
path takes the preceding branch and submits the cars through
`LibRw::RaceSubmit_AddInstance` (`:5703`) instead. The default arm's JSON carries only
`sky`, `world`, `props`, `copters` — and `world`/`props` read 0 there for the same
reason. G1d as written therefore has no channel, and per this document's stop rule it
is **not** rescored against a substitute.

What the **named channel** says when it is made to speak, on the legacy-renderer arm
(`MASHED_RENDER_LIBRW=0`, TRAINING, both binaries, frames 60/61/62 — reported as a
**separate arm**, not as G1d):

| category | pre | post |
|---|---|---|
| **cars** | `batches=284  verts=184296  textured=176` | `batches=176  verts=181620  textured=176` |
| sky | `1 / 258 / 1` | `1 / 258 / 1` |
| world | `24 / 34407 / 21` | `24 / 34407 / 21` |
| props | `107 / 7389 / 100` | `107 / 7389 / 100` |
| copters | `12 / 5988 / 12` | `12 / 5988 / 12` |

284 = 71 x 4 cars; 176 = 44 x 4; the difference is **108 = 27 x 4**, and `textured` is
**identical at 176**, i.e. not one textured car batch was lost and every kept batch is
textured. Every other category is unchanged on all three frames.

## G4a — FAIL as written. The fix is KEPT, and here is why the failure is the instrument's

G4a declares the allowed region as the bbox of each connected component of the
**PRE-fix `GREY u PAINT` mask**, dilated 4 px. That procedure is wrong in two ways,
both visible in the numbers:

1. It is far too **permissive** on the terrain side — desaturated rock, snow and sky
   also satisfy the GREY class, so the declaration yields 22-82 components per frame
   instead of one per car. Read literally, a `0 outside` on TRAINING is a weaker
   statement than intended.
2. It is too **restrictive** on the car side under a tinted light. TRAINING's sun is
   white and its ambient grey, so the hull renders achromatic and the class catches it.
   Arctic's ambient is `(0.200,0.300,0.300)` and its sun `(0.600,0.700,0.700)`, so the
   same hull renders **tinted**: `(21,31,31)` ambient-only and `(55,70,70)` lit. The
   first misses the `luma >= 30` floor by 1 (luma 29.0) and the second misses the
   `max-min <= 14` saturation cut by 1 (15). The declaration therefore omits part of
   the Arctic car, and the pixels the fix removes there fall "outside".

Direct evidence that the 901 pixels are car, not terrain:

* **Arctic `01_grid`, 879 px.** The whole frame's diff is **ONE connected component**,
  bbox `(225,299,416,479)`. All 879 lie inside that bbox. Their PRE-fix colours are
  **exactly two values** — `(21,31,31)` x538 and `(55,70,70)` x341 — which are the
  untextured hull material `(102,102,102)` under Arctic's ambient-only and lit terms.
  Post-fix they become car interior `(3,4,4)`, paint `(47,16,18)` and brake-light red
  `(248,43,43)`.
* **Arctic `01_action`, 22 px.** Bbox `(479,266,483,271)`, entirely inside the third AI
  car's diff component `(393,244,485,296)`. Their PRE-fix colours are the same hull
  gradient, `(25,35,36)` through `(55,70,70)`, 22 distinct values one pixel each.
* Independently, **G4b's nine fixed terrain/sea boxes differ by 0 pixels** with means
  equal to three decimals, including the two copter-bearing boxes that the *brightness*
  fix did move (111 and 461 px there, **0** here).
* The differing-pixel overlays (`G4_overlay_*.png`) show every magenta pixel on a car
  and none on terrain, sea, sky, props, billboards or copters.

So the failure is located in G4a's region-declaration procedure and not in the change
under test. **The edit is kept.** G4b and G4c carry the "terrain, sea and props do not
move" claim in a pre-registered, measurable form, and both pass exactly.

## Whole-frame differing-pixel inventory (pre vs post, bit-identical poses)

| capture | n_diff | components | largest component |
|---|---:|---:|---|
| TRAINING `01_grid` | 21 219 | **1** | `(225,299,416,479)` = the player car |
| TRAINING `01_action` | 26 427 | 14 | `(179,270,424,479)` n=25 961, rest 6-276 px on the same car |
| TRAINING `01_inrace_track` | 2 375 | 16 | 1487 / 583 / 242 px = the three visible cars |
| TRAINING `00_challengeselect` | **0** | — | identical |
| TRAINING `02_back_to_menu` | **0** | — | identical |
| Arctic `01_grid` | 21 239 | **1** | `(225,299,416,479)` = the player car |
| Arctic `01_action` | 8 751 | 55 | 3496 / 2865 / 2018 px = the three AI cars |
| Arctic `00_challengeselect` | **0** | — | identical |

## Transient boot, recorded

The **first** boot of `prefix_head` produced Arctic and frontend frames differing from
every other boot (`n_diff` 161 515 / 293 098 / 45 803; mean abs 0.99/255 and 0.25/255,
localised on the animated water surface and the copter rotors). Its second boot is
**bit-identical to the instrument build on 8/8 captures**, and same-binary controls on
both acceptance arms are 0/6. So that first boot was a transient, not a build
difference — the two-boot requirement is what caught it, and a single boot would have
produced a confident and wrong "adding an fprintf changed the Arctic render" claim.

## Still open

* **U-9079** — the original's per-atomic PART CODE (`FUN_004b5190` @ `0x004b5190`) is
  not derivable from the DFF, so this fix stays a measured equivalent rather than a
  verbatim port. Unchanged by this session.
* **U-9161** — no matched-pose original TRAINING capture exists, which is why G2c is a
  class-fraction band and not a pixel diff. Unchanged by this session.
* Note items **O3/O4/O5** of `CAR_GRAY_CHASSIS_2026-09-29.md` (duplicate low/high
  detail sets both drawn; `MASHED_RPLIGHT=0` renders the car black; props not swept for
  the same over-draw) are **NOT addressed here** and remain open.
* `[UNCERTAIN]` — whether the four hulls should instead feed our collision path the way
  the original's part codes `0x3b..0x3e` feed `FUN_0053d400` @ `0x0053d400`. This fix
  only stops them being *drawn*; `col_tris_` is built elsewhere and was not touched.
  Not investigated.

## Evidence files

| file | what |
|---|---|
| `verify/car_gray_fix_20260930/G2_orig_pre_post.png` | ORIGINAL vs PRE vs POST player car, the G2 panel |
| `verify/car_gray_fix_20260930/G4_overlay_tr_action.png` | TRAINING `01_action`, differing pixels in magenta over the post frame |
| `verify/car_gray_fix_20260930/G4_overlay_tr_track.png` | TRAINING `01_inrace_track`, three cars, terrain clean |
| `verify/car_gray_fix_20260930/G4_overlay_ar_action.png` | Arctic `01_action`, three AI cars, ice/sea/buildings clean |
| `verify/car_gray_fix_20260930/SHA256SUMS.txt` | the three exes and every scored capture |
