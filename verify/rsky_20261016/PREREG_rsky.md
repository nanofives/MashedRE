# PREREG rsky: sky slots (U-9714) and the Forest object BSP (U-9715)

Written before the first measuring run of this lane. Base `1fa71b277` (race/first-frame-parity), branch `work/rsky`,
worktree `.worktrees/rsky`. Orchestrated child (TAG rsky), model claude-sonnet-5-5.

## Question

r10 (`verify/r10_20261016/RESULT_r10.md`) mapped two small render defects of the default standalone:

1. **U-9714 / C23**, `Sky_Filename`: the parse keeps the last of two slots, and the one-argument form (Roundabout) is not parsed.
   Rows: 12 on Neustein, Roundabout, Warzone.
2. **U-9715 / C23b**, `Object_Bsp_Filename(0, "Trees.bsp")` (Forest only) is unknown to the standalone; Forest is the lowest-matched
   track (68.5 to 70.1 % of the original's triangles). Rows: 20.

Does porting the original's behaviour turn those cited rows GREEN on the default build, without adding a row anywhere in the 13 tracks?

## What the original does (decoded this lane, headless Ghidra, read-only)

- `Sky_Filename` handler `0x0047a1e0`: arg count `n` = `FUN_004b6fc0` (`(top - base) >> 4`, `0x004b6fc0`). For `n != 0` the slot is
  `ftol(FUN_004b7090(L, n-1))` and the name is `FUN_004b70d0(L, n)`; the name goes to `names + (slot+1)*0x40`
  (`0x0047a248`..). `FUN_004b7090` returns `_DAT_005d1008` for a non-number (0x005d1008 = 0.0, read from the file), so the one-argument form
  lands in slot 0. [UNCERTAIN U-9719: index 0 is `top + 0`, a stack cell past the arguments; its tag is taken as non-number. Evidence for
  the claim: Roundabout's original draws `Sky3`/`Sky4`.]
- `Course::CreateFromDescription` (`0x00479330`) loads sky slots 0..3 (`0x00479330` loop at the `"Loading SkyDome [%s] into sky[%d]"` string): name at
  `names + 0x40*(i+1)`, clump into `course + 0x100f0 + 4*i` (= `0x00656f48`..`0x00656f54`).
- Draw: `FUN_004270f0` calls `FUN_00426d20` (per slot, identity, rotate Y by `DAT_0066d720`, translate to the camera position) then `FUN_00426d90`
  (per slot ascending, `FUN_004e6680` = RpClumpRender, return `0x00426da2`). Slot 3 of the Arctic and Roundabout records (`0x00449010`) is the same pair.
- `Object_Bsp_Filename` handler `0x0047a610`: slot `ftol(arg1)`, name at `names + (slot+0xe)*0x40`, count `course_desc + 4`++.
  CreateFromDescription loads each non-empty name with the world-BSP loader `0x0042a640` into `course + 0x10040 + 4*i` (flag `course + 0x10330`).
- Forest's hook record (table `0x005f33f8`, Course_Id 34): slot 0 `0x0044dea0` = `0x0044dde0` (CamMan clones) + `0x0044dc00` (six tree sets);
  slot 2 `0x0044df00` = RpClumpRender(clump 0 `Water.dff`, `course + 0x10110`) then `0x0044dce0`: end camera update, remove the camera from the main world
  and add it to the object world (`0x004e4350`, `0x004e4320`), begin update, cull mode NONE (`RwRenderStateSet(0x14, 1)`), RpWorldRender(object world)
  (`0x004e5660`), cull mode 2, end update, swap the worlds back. `0x0044dc00` = six sets of `FUN_00479f30`(first atomic of a clump, Mts count, Mts matrices):
  (clump 2, Mts 0), (3, 1), (3, 6), (4, 4), (5, 3), (5, 5).
- Order in the original's Forest frame (`verify/r10_20261016/bt/09_Forest`): main world sectors, `water05` (clump 0), then six groups
  `bark, Bush2, Tree'sBack, midtree, OakTreeSprite3` (sector + tied atomics), then the cars.

## Method (nothing re-tuned from d1r1 / r10)

- Instrument: d1r1's `run_replay3d.py` recipe (one deterministic round, `MASHED_CAM_REPLAY_BASIS=1`, `MASHED_CAR_REPLAY=1`, `MASHED_DBG_D3DREC=1`, no opt-in
  knobs) and `drawlist_diff.py` 3D mode. **Isolation** (orchestrator rule 3): the d1r1 / r10 drivers hard-code `MASHED_ROOT` and the CWD to the main
  checkout, where `mashed_re.log` and `log/librw_race.txt` are shared with every other session. This lane uses `verify/rsky_20261016/run_replay_priv.py`, the
  same recipe with `MASHED_ROOT` and the CWD = a private game root (a robocopy of `original\`, `original\` inside it, checked by
  `Test-Path <root>\original\TOASTART\Common\Frontend.piz`). Recorded in the RESULT with the file count.
- Tracks (13): the ten r10 capfix tracks (originals `verify/capfix_20261016`, cites `verify/r10_20261016/cites_r10_v3.json`) and TRAINING, Arctic, City
  (originals `log/d1r1/orig_*`, same cite file: it is the union of d1r1's and the later lanes' cites). Same trace rows, same shots as r10 / d1r1.
- Baseline: a clean build of the base commit in this worktree, `log/rsky/exe_base.exe`, replayed on all 13 tracks with the same drivers. The AFTER arm is the
  build with the fixes. Row identity for "new row": (kind, texture, original triangle count, standalone triangle count) in a frame.
- G-CTRL (`run_gctrl.py` recipe, private root): default build expected `2b4d47bb / 356821c8 / f7a155e1` (memory `default-ctrl-hashes-after-b2-flip`),
  baseline measured first in this lane, the AFTER build compared to the baseline measured here.

## Gates (same as r10: I-COVER, I-FRAME, I-NAME original side; I-SYNC, I-NAME standalone side)

A frame failing a gate is re-run (piz-collision crashes are re-runs); never relaxed in this lane. Neustein +105 I-NAME (73 of 77, one `#64x64:21` runtime
texture) is the gate miss r10 reported on the original side: see "Neustein" below.

## Acceptance (decided before the run)

- A1. Neustein, Roundabout, Warzone: every C23 row (the sky rows) is gone from the AFTER scores, and matched draws do not fall on any frame of any of the three.
- A2. Forest: the C23b rows are gone or reduced to rows with a named residual (ORDER or MOVED of a sector, not MISSING of a whole texture); matched triangles rise
  on all 3 frames. Anything not gone keeps its cite and a U-row; no new cite group.
- A3. No new row anywhere: the AFTER set of row identities on the 13 tracks (39 frames) is a subset of the baseline set, except rows that exist only because a
  newly drawn object now exists (these are reported one by one and are an A3 FAIL unless they carry an existing cite).
- A4. The default build's G-CTRL hashes equal the baseline measured in this lane (the sky of TRAINING / City / Arctic has one slot, Forest's change is
  render-only; nothing on the physics or AI path is touched).
- A5. 0 RED rows on the 39 frames with `cites_r10_v3.json` unchanged (a cite may be removed for a fixed group, never added).

## Predictions

- Pr-1: the sky change moves only Neustein, Roundabout and Warzone; the 10 other tracks' row sets are identical to baseline.
- Pr-2: Forest rises from 68.5 to 70.1 % to at least 80 % of matched triangles; ties in the object world follow the main world's rule (RESULT_rstates)
  and I expect residual ORDER rows from U-9709 (tie order not decoded).
- Pr-3: the G-CTRL hashes do not move.
- Pr-4: if the `#64x64:21` texture can be named from its creator without a new original-side capture, the Neustein +105 gate passes; if it needs a new
  capture of the original, I report the finding and leave the gate as r10 left it (no relaxation).

## Defaults

A faithful port that passes A1 to A5 is default ON with an opt-out knob (`MASHED_NO_SKYSLOTS=1`, `MASHED_NO_OBJBSP=1`); otherwise OFF with a recommendation.
