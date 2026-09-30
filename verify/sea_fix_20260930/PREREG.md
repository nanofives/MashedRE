# Arctic sea-tile fix — PRE-REGISTERED acceptance (2026-09-30)

Written and committed **before any edit under `mashedmod/src`**. Diagnosis is not
re-derived here: it is `re/analysis/SEA_LEVEL_2026-09-29.md`, and the fix is
`verify/sea_level_20260929/sea_tile.patch` (verified to apply cleanly at HEAD
`0166167e`, `git apply --check` exit 0).

## What is being changed

`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1711-1720`, the
`Clump_Filename` prop loop, which today pushes one identity instance per clump.
For Arctic (`course_id_ == 0`) clump index 2 (`sea.dff`) it will instead push the
25 instances the original places from its per-track hook.

RVAs the fix reproduces (all from `SEA_LEVEL_2026-09-29.md` §4, re-read this
session and carried verbatim into the source comment):

| RVA | what it is |
|---|---|
| `0x005f33f8` | per-track hook table, stride `0x48` = `{char name[0x10]; u32 Course_Id; void* slot[13]}` |
| `0x005f3488` | the record's name, ASCII `"arctic"` |
| `0x005f3498` | that record's `Course_Id` = 0 |
| `0x005f349c` | slot 0 of that record = `0x00448940` |
| `0x0041e870` | selector; matches `Course_Id` into `DAT_0063d7e4` (store at `0x0041e895`), called at `0x00426eaf` |
| `0x0041e8b0` | thunk `jmp [rec+0x14]` (slot 0), invoked at `0x0042709f` after `push 0x646e58` (the course object) |
| `0x00448940` | the Arctic post-load hook itself |
| `0x0044899d` | `mov ecx,[esi+0x10118]` — course clump[2] == `Clump_Filename(2,"sea.dff")` |
| `0x004489bb` | `mov [esp+0x4c], 0xc0833333` — **translation.y = −4.1** |
| `0x00448a47` | `call 0x004e6ab0` ×24 — `RpClumpClone`; array `0x008963e0..0x00896440` = 1 base + 24 clones |
| `0x00448a65` / `0x00448a80` | `mov [esp+0x10]/[esp+0x18], 0xc3160000` — X start / Z start = **−150.0** |
| `0x00448a6f` / `0x00448a88` | `mov ebp,5` / `mov ebx,5` — **5×5** |
| `0x00448aa2` | `call 0x004c1340(frame,&v,0)` — `RwFrameTranslate`, combine op 0 = `rwCOMBINEREPLACE` |
| `0x00448aab` / `0x00448ac1` | `fadd [0x005cc728]` — **step = +60.0** |
| `0x00449030` (slot 2) / `0x004490d0` (slot 5) | render / destroy-the-24-clones; neither transforms a frame (§5b) |

So the original's placement is: 25 tiles, `X, Z ∈ {−150, −90, −30, +30, +90}`,
`Y = −4.1` for all 25.

## Interaction checks done before pre-registering

- **Water-fold keying** (`mashedmod/src/mashed_re/LibRw/RwSceneBuild.cpp:497-513`,
  `ModelIsWaterAsset`, `strncmp` on `SEA`/`WATER`/`LAKE`/`RIVER`, ANDed with
  `BatchIsWaterClass` at `:591-598`). That decision is taken **per model/batch**
  at scene-build time, from the DFF's source name and geometry flags. The fix
  changes only `Prop::instances` (a per-instance world matrix list consumed at
  `TrackRenderer.cpp:5369`/`:5373`), so it cannot change which batches are
  classified as water. No interaction.
- **Terrain ambient fill.** The memory names `TrackRenderer.cpp:228`; that line
  is now inside `MakeAtomicLight` (the line number has drifted). Either way it
  is a shading path keyed on prelit/lit flags, not on instance count or world
  matrix. No interaction.
- **Second live copy of the sea/track-hook code.** `grep` for the prop-instance
  loop (`MatIdentity(&id)` / `s_skip_excluded`) across `mashedmod/src/mashed_re`
  returns exactly one site, `TrackRenderer.cpp:1712-1717`. `scripts/lint_rva_bodies.py`
  at HEAD: `122 finding(s) ... allowlisted=122 NEW=0`.

## Binaries

| arm | path | SHA-256 | size |
|---|---|---|---|
| pre-fix (HEAD `0166167e`) | `verify/sea_fix_20260930/bin/mashed_re_prefix.exe` | `5327511B605A8DE4A580974AF15011DFE7A13C56BB71EDEE1030065A334F71F3` | 1 961 472 |
| post-fix | `verify/sea_fix_20260930/bin/mashed_re_postfix.exe` | *(pinned after the fix builds)* | |

## Capture protocol (identical for every arm)

`verify/sea_fix_20260930/run_race.py` — the standalone's own race-flow driver:
`MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DETERMINISTIC=1 MASHED_MUTE=1
MASHED_WIN_POS=left-bl MASHED_TITLE=<label> MASHED_TRACK_SEL=<idx>`, optional
12-float `MASHED_CAM_POSE`. Waits for process **exit** before reading any BMP
(memory `race-capture-wait-for-exit`), kills only the pid it spawned, and only on
timeout. Frame measured is always `<out>/<NN>_<name>/race1/01_grid.bmp`
(`exe_main.cpp:1559`, 1800 deterministic ms after InRace).

Original-side references reused (no new original capture is needed for any rule
below): `verify/arctic_ref/sea_search/s8/a.bmp` + `orig_cambasis.txt`, and the
same pair under `s14/`.

## Region policy

**Every measured region is a fixed pixel rectangle** in the 640×480 backbuffer,
defined once in `verify/sea_fix_20260930/boxstats.py` and identical across the
original arm, the pre-fix arm and the post-fix arm. **No region is declared by a
colour class.** Two earlier children of this lane lost a rule that way: Arctic's
tinted night lighting moves every luma/achromatic threshold, so a colour-defined
"sea class" or "car class" stops selecting the same surfaces between arms
(memory `achromatic-selector-breaks-on-tinted-light`). The boxes were placed by
overlaying them on the **original** and on the **pre-fix** captures and checking
by eye that each `ROAD_*` box is car-free and prop-free in *both* arms
(`boxes_s8.png`, `boxes_s14.png`, `boxes_port_s8.png`, `boxes_port_s14.png`).

Discriminating statistic: texture detail, not colour.
`g` = mean |Sobel gradient| of luma over the box; `L` = mean luma;
`g/L` = brightness-invariant relative gradient; `n` = pixels in the box.
Road is textured (snow streaks, painted lines, falling snow); a flat water sheet
is smooth. This is the statistic `SEA_LEVEL_2026-09-29.md` §2 already measured
globally (original 5.729 vs standalone 0.469), here scoped to fixed boxes.

### Measured baselines (ORIGINAL vs PRE-FIX), fixed before the fix exists

| pose | box | n | orig g | orig g/L | pre g | pre g/L | g separation |
|---|---|---|---|---|---|---|---|
| s8 | ROAD_UL | 14 000 | 0.8229 | 0.04190 | 0.6421 | 0.03425 | **1.28×** |
| s8 | ROAD_UC | 11 700 | 2.3452 | 0.06276 | 0.4530 | 0.03047 | 5.18× |
| s8 | ROAD_MC | 12 000 | 2.0901 | 0.11399 | 0.1827 | 0.02320 | 11.44× |
| s8 | ROAD_LC | 10 500 | 2.8433 | 0.14923 | 0.0947 | 0.02031 | 30.02× |
| s14 | ROAD_FG | 13 200 | 2.9679 | 0.09224 | 0.1331 | 0.02243 | 22.30× |
| s14 | ROAD_MID | 12 000 | 2.6370 | 0.07472 | 0.4184 | 0.03534 | 6.30× |
| s14 | ROAD_R | 10 200 | 2.8027 | 0.12856 | 0.1764 | 0.02292 | 15.89× |

`ROAD_UL` is **NON-GATING** (`boxstats.NON_GATING`): the original is itself
near-featureless there, so the box cannot tell road from water in either
direction. It is still measured and reported. Dropped on these baseline numbers
alone, before the fix existed.

Controls (rule 4), same baselines: s8 `CAR_A` n=2 250, `CAR_B` n=2 250,
`CAR_C` n=3 300 — tight boxes strictly inside a car silhouette in the **port**
arm. s14 `CTRL_BLDG` n=12 825 (lit building facade, screen left),
`CTRL_PIPE` n=7 200 (silo/pipe cluster, upper right).

---

# THE RULES

A rule passes only on the criterion as written here. If a rule turns out to be
unmeasurable it is reported as such, not amended.

## Rule 1 — exactly 25 sea instances at the original's positions

From an Arctic run's `mashed_re.log` (the standalone logs through
`TrackRenderer::Load`'s `FILE* log`, opened from `kLogPath = "mashed_re.log"`,
`exe_main.cpp:446`, `:2137`), the fix must print **one line per instance**.

**PASS** iff all of:
1. instance count for `sea.dff` (clump idx 2) on `MASHED_TRACK_SEL=0` is exactly **25**;
2. the 25 logged translations are exactly the set
   `{(−150 + 60·ix, −4.100000, −150 + 60·iz) : ix ∈ 0..4, iz ∈ 0..4}` — grid
   origin −150, step 60, `Y = −4.1` on all 25, X and Z each taking
   {−150, −90, −30, +30, +90};
3. every one of the 25 is listed in the RESULT block;
4. on a non-Arctic track (`MASHED_TRACK_SEL=12`, training) the branch does not
   fire: **0** sea-tile instances logged.

## Rule 2 — Arctic, matched pose, against the original

### 2a — the sea no longer covers the road (6 gating boxes, 2 poses)

At pose s8 and pose s14, in **every** gating `ROAD_*` box:

**PASS** iff `post g ≥ 3 × pre g` **AND** `post g/L ≥ 0.5 × orig g/L`.

Both halves are required. The first says the surface in that box gained texture
relative to the pre-fix water sheet; the second says it reached at least half the
original's relative detail, so a merely-noisier water sheet cannot pass. Derived
thresholds, fixed now:

| pose | box | needs post g ≥ | needs post g/L ≥ |
|---|---|---|---|
| s8 | ROAD_UC | 1.3590 | 0.03138 |
| s8 | ROAD_MC | 0.5481 | 0.05700 |
| s8 | ROAD_LC | 0.2841 | 0.07462 |
| s14 | ROAD_FG | 0.3993 | 0.04612 |
| s14 | ROAD_MID | 1.2552 | 0.03736 |
| s14 | ROAD_R | 0.5292 | 0.06428 |

Reported alongside (not gating): per-box pre→post differing-pixel count and
percent at threshold 16, and the non-gating `ROAD_UL`.

### 2b — edge placement

The sea's world placement in the port must equal the original's, **read live from
the original**: `verify/sea_level_20260929/orig_sea_matrix.json` records the
original's `clump[2]` frame with modelling == LTM == `translate(−150.0000,
−4.1000, −150.0000)`, identical across 4 samples over 6 s.

**PASS** iff the port's tile-0 instance translation equals
`(−150.0, −4.1, −150.0)` and the 25-tile grid spans `X, Z ∈ [−180, +150]`
(each 60×60 tile spanning ±30 about its anchor).

**Why the screen-horizon form of this rule is not used, stated before the fix:**
the horizon is not in frame at either committed original Arctic pose, so there is
no waterline on screen to place. From the 12-float bases, camera forward pitch is
`asin(at.y)` = **−61.83°** at s8 (`at = (−0.29348, −0.88127, −0.37044)`) and
**−33.73°** at s14 (`at = (−0.11019, −0.55525, +0.82435)`); half-FOV is 24.23°
(`orig_lens.json`, `fovy 48.455°`), and s14's `right = (0.99119, 0, 0.13248)` has
zero Y so its top edge is the extreme, at −9.5°. Both frames end below the
horizon. 2b is therefore evaluated in world units against the original's own
live-read matrix, which is a stronger witness than a screen edge, not a weaker
substitute for one.

## Rule 3 — the tiling is Arctic-only

For every `MASHED_TRACK_SEL ∈ {1,…,12}` (the other 12 tracks), at the same
deterministic pose (driver default, no `MASHED_CAM_POSE`), compare post-fix
`01_grid.bmp` against pre-fix.

**PASS** iff, for all 12 tracks, `re/tools/imgdiff.py` reports
`pixels over threshold 16: 0` **and** `mean abs diff (all 0.000)`.

**Prerequisite control (must pass first, or rule 3 is void):** a pre-fix→pre-fix
repeat run on one track must itself be 0-diff. Without that baseline a 0 result
proves determinism, not the gate (memory `verifier-needs-passing-baseline`).

**Two-boot rule:** any non-zero verdict is re-run once before it counts; animated
water and rotors have produced transient boots in this lane
(memory `shadow-lane-failure-windows`).

## Rule 4 — Arctic cars and props outside the sea region are unchanged

Between the pre-fix and post-fix Arctic captures at the same pose:

**PASS** iff **0** pixels over threshold 16 inside each of
s8 `CAR_A`, `CAR_B`, `CAR_C` and s14 `CTRL_BLDG`, `CTRL_PIPE`.

Plus the gate check: post-fix Arctic run with `MASHED_NO_SEA_TILE=1` must be
0-diff against the pre-fix Arctic capture at the same pose (whole frame,
`pixels over threshold 16: 0`, `mean abs diff 0.000`). That proves the new branch
is the only thing that changed and that its A/B switch really restores the old
behaviour.

---

## What is explicitly NOT claimed by any rule here

- **U1b** (`SEA_LEVEL_2026-09-29.md` §7): the original renders the 25 tiles from a
  dedicated Arctic pass (`0x00449030`) that also issues eight state pairs through
  `RwGlobals+0x20`; those pairs' meaning, and therefore the sea's draw order and
  blend mode relative to our generic prop pass, remain `[UNCERTAIN]`. Placement
  is what is being fixed and measured.
- **U2**: the original clones 24 clumps and world-registers each
  (`0x004e45b0` at `0x00448a1f`); the port expresses the same result as 25
  instances of one model. Whether that per-clone world registration changes
  culling or draw order is not established.
- Car livery colour (all four cars render red in the port) and the base-pose
  copter/rotor prop are pre-existing defects visible in these captures and are
  out of this fix's scope.
