# PREREG kfdot (2026-10-22): 0x00532b80 RpHAnimStdKeyFrameInterpolate, one body + knob

Written before any `MASHED_KF_INTERP=1` run. Lane `work/kfdot`, base `6ddf1e1d5`. Mariano picked this pair 2026-10-22.

## 1. What the original does (`original/MASHED.exe.unpatched`, `re/tools/disasm_fn.py`)

- `0x00532b80` is `void __cdecl (out, a, b, time)`: out q `+0x08`, t `+0x18`; key time `+0x04`, q `+0x08`, t `+0x18`.
- Dot `((b.x*a.x + a.y*b.y) + b.z*a.z) + a.w*b.w`, x87 left to right (`0x00532b8f..0x00532bab`).
- Negative dot: b's quaternion negated in place (`0x00532bfd..0x00532c1c`). Dot >= `[0x005cea88]` (0.999): plain lerp
  (`je 0x00532f41`). Else acos by branches, and for dot > 0.5 the root is `FUN_004c3b30` (`0x00532dc9`), the RW LUT sqrt
  (`0x004c3b40..0x004c3b79`: table at `*DAT_007d3ff8 + DAT_007d3ffc`), not an exact sqrt.
- Callers seen: `FUN_004b47e0` at `0x004b4863` (the SetClumpAnim type != 0 sampler) and the RtAnim interpolator (Warzone boat).

## 2. The port before this lane

- `Track/WarzonePontoon.cpp:KfInterpolate`: transcribed from the listing (both points above as the original).
- `D3d9Render/TrackRenderer.cpp:LinearAnimMatrix` (Dump train, default ON): dot summed reversed `((w + z) + y) + x`, and the
  acos (`AnimAcos`) takes `std::sqrt` for dot > 0.5. Two departures, not one (the allowlist note named only the dot order).
- Dump reaches `LinearAnimMatrix`: the 13 `COURSE.LUA` files (extracted from a private copy) hold one live `SetClumpAnim`,
  `dump/COURSE.LUA:135 SetClumpAnim( LINEAR,0,3)`, with `local LINEAR = 1` at line 133; the port's parser resolves locals
  (`TrackRenderer.cpp` `lua_locals`), so type 1. The registration comment that said nil -> 0 was stale (corrected).
- The clump-anim matrices feed only the prop instance matrix (`TrackRenderer.cpp` the `clump_anims_` loop before R6): render
  only, no physics reader.

## 3. Change

- ONE body `Track/KfInterpolate.cpp:KfInterpolateFields` (moved from the pontoon file), in BOTH rsp files.
- The boat calls it (unchanged arithmetic). `LinearAnimMatrix` calls it when `MASHED_KF_INTERP=1`; default OFF keeps the
  reversed / `std::sqrt` code, so the allowlist line `DUP-IN-TARGET:00532b80` stays (the scanner now reads the knob branch as
  a call-through and stops counting it: a lint false negative while the knob is OFF, recorded here).
- The .asi installs it over `0x00532b80` (export `KfInterpolate532b80`) with the in-process A/B (`MASHED_LEAF_SELFTEST`,
  `_SWEEP` for synthetic keys over every branch).

## 4. Predictions

| id | arm | prediction |
|---|---|---|
| P0 | knob OFF, new exe | G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6` x2, B `f9deb9b9 / d3601fcc / 9da9a26a`, (b) 9/9, (e) 3/3 launch 1425.7 / 2052.5 / 2055.0, D2 3/3, 13 tracks table identical to dcstream's |
| P1 | .asi A/B on the original, Dump (track 10) and Warzone (track 11), hooks live | every natural call noo 0, ndiff 0; > 0 calls on Dump (the train) ; sweep 48 cases noo 0 ndiff 0 |
| P2 | knob ON, gates | G-CTRL / B / D2 identical to P0: all three recipes run on Training, which has no live SetClumpAnim |
| P3 | knob ON, 13 tracks | 12 tracks identical; 10_Dump 0 RED. The row-level score may not move: the pose change is small next to the matcher's tolerances. That is NOT evidence against ON; P1 is the evidence (the same body equals the original on Dump's own keys, in-process) |

## 5. Decision rule (for Mariano)

Recommend flipping the default to ON when P0, P1 and P2 hold and P3 shows 0 RED. A P1 mismatch blocks the flip and the body
goes back to the listing. A P2 move means a reader of the clump matrices exists that this note missed: stop and report.
After a flip the legacy branch becomes an opt-out (`MASHED_NO_KF_INTERP`) or is deleted, Mariano's call.
