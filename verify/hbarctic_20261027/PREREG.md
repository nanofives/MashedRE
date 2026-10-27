# HB-ARCTIC PREREG (2026-10-27): does the Arctic sea still need ambient?

Committed alone, before any arm runs. Nothing below is amended after the arms.

## 0. What is already decoded (static, no arm needed)

How the course lights reach a non-`rpGEOMETRYLIGHT` atomic in the original: **they do not.**

- Course loader `0x00479330`: the `Lights_Filename` branch (`param_2[0x2640] != 0`) loads the
  light clump, and for every light sets `*(char*)(light+2) = 1` then calls
  `RpWorldAddLight` (`0x004e4810`). Byte `+2` is the RwObject flags, so each course light
  carries flag `1` only (light-atomics), never `2` (light-world). The no-file branch creates
  an ambient (`RpLightCreate(2)`) and a directional (`RpLightCreate(1)`) with the same flag `1`.
- Atomic pipeline callbacks are installed at `0x004ea970`: instance `0x004ea9a0`, reinstance
  `0x004eb350`, **lighting `0x004ea4e0`**, render `0x004ec190`.
- `0x004ea4e0`: `mov eax,[ebp+0x18]` (atomic geometry), `mov ebx,[eax+8]` (geometry flags),
  `test bl,0x20` at `0x004ea4ef`, `je 0x004ea6b5`. Also exits there when
  `[RwGlobals+4]` (current world) is NULL (`0x004ea500`). At `0x004ea6b5` it calls
  `0x004f4f10(0, 1)`; `0x004f4f10` with a zero first argument branches to `0x004f5007`, which
  tail-jumps to `0x004d5480` with `(0x89, 0)` = `D3DRS_LIGHTING` FALSE.
- So an atomic whose geometry lacks `0x20` renders with D3D lighting off: no world light,
  ambient or directional, reaches it. Its colour is texture x prelit (x material colour only
  when `0x40` is set).
- `SEA.DFF` geometry flags (`re/tools/dff_dump.py`): Arctic `0x1000f`, SuperG `0x1000f`.
  Neither has `0x20` (LIGHT) or `0x40` (MODULATEMATERIALCOLOR).
- The world-sector lighting callback `0x004eb550` (sector pipeline installer `0x004eb520`)
  only counts lights with flag `2` (`test byte ptr [eax+2],2` at `0x004eb58a`), so the course
  lights do not light the world BSP either.

Consequence: the faithful mechanism is the port's current default (fold OFF; BuildClump
never sets `rw::Geometry::LIGHT` for an unlit DFF). There is no light path to port. The
manual folds (`MASHED_LIBRW_AMBFOLD`, `MASHED_LIBRW_AMBFOLD_SEA`) have no counterpart in the
original.

## 1. The open question the arms answer

The "Arctic sea too dark" number (original 28.0, unfolded 9.1, `verify/water_refs/RESULT.md`)
was taken before two later changes:

1. The sea-tile placement fix (2026-09-30, `verify/sea_fix_20260930/RESULT.md`). Its own
   RESULT records that the old Arctic delta compared the original's ROAD against our WATER.
2. `verify/water_refs/U9064_MECHANISM.md` (2026-08-31) traced the remaining gap to the librw
   fog colour arriving at the pixel shader as black. Whether that is still true on today's
   build is not recorded.

Q: on the current `orch/integration` build, unchanged, is Arctic's sea within band of the
original with the fold OFF?

## 2. Arms

Exe: this worktree's `mashedmod/build/mashed_re.exe`, built from `orch/integration` at
`ebc7b0c36` with no source change. Private root `D:/MashedWT/HB-ARCTIC_root`.
Recipe: `MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DETERMINISTIC=1 MASHED_MUTE=1`,
`MASHED_TRACK_SEL=<sel>`, `MASHED_CAM_POSE=<orig_cambasis.txt>`, capture `race1/01_grid.bmp`.

| pose | sel | original reference | basis |
|---|---|---|---|
| arctic_s8 | 0 | `verify/arctic_ref/sea_search/s8/a.bmp` | same dir `orig_cambasis.txt` |
| arctic_s14 | 0 | `verify/arctic_ref/sea_search/s14/a.bmp` | same dir |
| superg_s14 | 7 | `verify/water_refs/superg_s14/orig.bmp` | same dir |
| forest_s8 | 3 | `verify/water_refs/forest_s8/orig.bmp` | same dir |
| forest_s14 | 3 | `verify/water_refs/forest_s14/orig.bmp` | same dir |

Per pose two arms: `OFF` (no env, shipping default) and `SEA` (`MASHED_LIBRW_AMBFOLD_SEA=1`).
Control: `arctic_s8` `OFF` run a second time (`OFF2`).
Each arm's `log/librw_race.txt` `P7 fogcolor` / `P7 fogconst` lines are kept.

## 3. Metric

Luma = 0.299 R + 0.587 G + 0.114 B per pixel. Water mask M = pixels where
`|L_SEA - L_OFF| > 6` (the fold touches only water-asset geometry, so M is the visible
water). Report mask %, mean luma over M for original / OFF / SEA, and
dOFF = |OFF - orig|, dSEA = |SEA - orig|.

## 4. Rules (scored only where mask >= 20%: arctic_s8, arctic_s14, superg_s14)

- **R0 control.** `OFF2` vs `OFF` on arctic_s8: mean |dL| over the whole frame < 0.5 and
  over M < 0.5. If R0 fails the run is noise-limited and no rule below is scored.
- **R1 (fold not needed).** dOFF <= 8 on all three scored poses. Verdict: the default is
  faithful, U-9064's Arctic gap was the placement/fog artefacts, and the fold code is dead
  weight (follow-up: delete both fold knobs).
- **R2 (Arctic residual remains).** dOFF > 8 on either Arctic pose while dOFF <= 8 on
  superg_s14. Verdict: a residual exists that is NOT lighting (section 0 rules lighting out).
  Report the Arctic `P7 fogcolor` line and file a fog follow-up, not a lighting one.
- **R3 (both wrong).** dOFF > 8 on superg_s14 as well: a shared defect on water; report it.
- Forest rows (mask < 5% historically) are reported, not scored.
- In every branch, dSEA is reported; no rule promotes the fold.

## 5. Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe must
reproduce `re/orchestrator/merge_train.json` refs (G-CTRL `5943c5f1/364350ae/52052890`,
B `3a03670c/e85ea6fb/53778664`). No default changes in this step.
