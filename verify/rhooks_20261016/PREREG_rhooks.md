# PREREG rhooks: per-track hook-slot draw model for excluded clumps (U-9713), scenario objects (U-9716), Dump train (U-9717)

Written 2026-10-16, before the first measuring run of any ported code. Base `1fa71b277` (`race/first-frame-parity`),
branch `work/rhooks`, worktree `.worktrees/rhooks`.

**Already run before this file (controls, no ported code):** clean build of the base
(`verify/rhooks_20261016/base_exe/mashed_re.exe`, SHA-256 `a19dc61f7cbbba6ce494622464443f089bbeb347c0cc68f0abb2865aed091a33`),
its ten capfix replays (`base/`, 10 of 10 first attempt) scored with `cites_r10_v3.json` (`base_*.txt`). They reproduce r10's
per-frame numbers (e.g. Warzone 584: 175 of 364 draws, 87.3 % triangles; Roundabout 1166: 197 of 250, 88.6 %). G-CTRL on the
base exe runs in parallel with this file.

## What the original does (static decode, this lane, before any measurement)

Hook table `0x005f33f8` (13 records x 0x48, slot k reached through thunk `0x0041e8b0 + 0x10*k`). Render order in
`FUN_00410b30`: world `FUN_004270f0` (slot 2 at `0x004272df` right after the world), then FOGENABLE 1 and the switch table
`FUN_004725c0` (`0x00691500`, 10 x 0x10: clump at +0xc drawn when the flag at +8 is set), then the cars, the copters
`FUN_00425e40`, then slot 10 (`FUN_00426640` -> thunk `0x0041e950`).

- Mts instance group (`FUN_00479f30` create: `FUN_00479e80` clones the clump's FIRST atomic (`FUN_004b3f90`) count times,
  refusing count > 0x30, one fresh unparented frame per copy set to the Mts record (`FUN_00479f10` -> `FUN_004c1480` REPLACE);
  `FUN_00479f70` draws copies 0..n-1 through each atomic's render callback (no frustum test).
- LOD hook (`FUN_004758d0` -> `FUN_00475830`, RpLODAtomic): LOD slots 0..4 = H geometry, 5..9 = the L clump's first atomic's
  geometry, range from `DAT_006915fc`. Select `FUN_00557a60`: `lod = round(dot(atomic LTM pos - camera LTM pos, camera at) *
  (10.0 [0x005cc55c] / range))`, below 0 -> 0, above 9 -> 9. Three call sites in the whole image: rouabout 17<-18 r60
  (`0x0044d325`), rouabout 19<-20 r30 (`0x0044d348`), Warzone 20<-21 r60 (`0x00452aba`).
- Warzone: slot 0 `0x004527f0` groups A (OakTrunk, Mts 2), B (OakTrunk, Mts 3), C (CrossTree, Mts 0), D (Fence_2, Mts 6);
  slot 2 `0x00452d10` draws clumps 19, 20, group D, (boat `0x00452130`), clump 0, (chunks, inactive at load); slot 10
  `0x00452da0` draws groups A, B, C. Craters 9..16, Chunk 17 base, OakTrunk / CrossTree / Fence_2 bases, Pylon_L: no draw site.
- Neustein: slot 0 `0x0044ac70` groups Trunk x Mts 0 / 2 / 5; slot 2 `0x0044af10` (read from the bytes: the decompiler drops
  two calls) draws clumps 5, 6, `0x0044a930`, then the three groups. Trunk base: no draw site.
- Sands: slot 2 `0x00450910` draws clumps 14, 15, 16, 5, 1 (+ Debris clones, timer-gated); switch table set by `0x00450300`
  (entry 0 BridgeWhole 20 ON, entries 1..3 Bridgehafbroke 18, BridgeBroke 19, Jump 7 OFF). Debris 10 base, 9: no draw site.
- Roundabout: slot 2 `0x0044d6f0` draws clumps 3, 0, 5, 11, 9, 8, 19, 17 (unchanged) with the two LOD hooks; Pylon_L 18,
  Frame_L 20, railcar 2 base, Hut01 10: no draw site (the railcar clones join the world, `0x0044c8b0`, and move: not in this step).
- Highway: every excluded clump (0..38, 43, 44, 46, 47, 48, 50) is drawn only from the 80-entry traffic array in slot 2
  `0x0044e470` or the gated Jet (`DAT_005f9748`). The traffic simulation is not ported in this step.

## Port (step A, render only)

New TU `Render/TrackHookSlots.{h,cpp}` holding the tables above; `TrackRenderer` draws an excluded clump of a covered track
only through them. Opt-out `MASHED_NO_HOOKSLOTS=1` restores the base behaviour exactly. Tracks not covered keep the base
behaviour.

## Acceptance and predictions (sample A = capfix captures, scored with cites_r10_v3.json merged with any sibling cites)

- **Pr-1 (C22):** C22 rows fall from 50 to at most 20 over the 30 frames: Warzone 18 -> at most 3, Sands 6 -> 0, Highway 6 -> 3
  (one MISSING traffic3 per frame), Roundabout 20 -> at most 14 (railcar and crane rows remain).
- **Pr-2 (no new rows):** no row on any of the ten tracks that the base scoring does not have, other than a C22 row changing kind
  (MOVED -> MISSING) where a placed copy is removed; Egypt, SuperG, Storm, Forest, Dump: identical draw lists.
- **Pr-3 (Neustein):** the C29 untextured MOVED row at the origin (3 rows) disappears; matched-draw count rises.
- **Pr-4 (LOD):** every Roundabout / Warzone Pylon and Frame copy matches the original's H/L choice on all 9 frames of the two tracks.
- **Pr-5 (G-CTRL):** unchanged against the base exe (render-only change).
- Gate failures, piz-collision loads: re-run; a frame never passing a gate is left unscored (r10's rule).

Defaults: ON with the opt-out if Pr-1, Pr-2 and Pr-5 hold; otherwise OFF with a recommendation.

## Addendum 1 (written after arm A / arm B were scored, before any train measurement)

Arm B results are in RESULT_rhooks.md. Added after them, decoded before measuring:

- **Warzone craters (correction to the decode above).** Clumps 9..16 ARE drawn: switch-table entries 0..7, all ON, set by
  FUN_00452620 @ 0x00452620 (`add edi,0x10134` 0x00452628, from slot 0 via 0x00452720 and from slots 6/7). The first linear
  capstone sweep desynchronised and missed that instruction; the byte-anchored sweep (`dispref.py`) found it. Arm A hid the
  craters (3 new C01c rows on Warzone); arm B draws them.
- **Slot render states.** Arm B carries each slot's cull / zwrite (`RaceSubmit_AddInstanceStates`).
- **Dump train (U-9717), arm C.** SetClumpAnim (handler FUN_0047ad40) records: clump = arg n-1, anim = arg n, type = arg 1
  (LINEAR is not a global of the image, so nil -> 0 -> sampler FUN_004752f0, the copters' CopterAnimMatrix). Tick
  FUN_00426700 (t += units * [0x005cc948], wrap at the duration) runs from FUN_0040fc00 at 0x0040fc94, before the copter tick
  0x0040fcb9 with the same units. **Pr-6:** both Dump train rows (C28, rows 735 and 775) stop being MISSING: the train is
  drawn either matched or as a MOVED row no further off than the copters' C14 MOVED rows (0.04 to 0.20); no other Dump row
  changes; G-CTRL unchanged.
