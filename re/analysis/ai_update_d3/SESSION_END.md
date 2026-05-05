# Session End: ai_update_d3-20260505

## Session summary

Depth-3 pass on the `ai_update` subsystem. Decompiled and analyzed all 20 functions deferred by parent session ai_update_d2 (D-4180..D-4199, bucket ai_update_d2-cont1).

Pool slot: Mashed_pool0. SHA-256 anchor verified.

## Functions analyzed

| RVA | Name | Notes |
|-----|------|-------|
| 0x00408a50 | FUN_00408a50 | Race-progress float getter (DAT_008a96e8 stride 0x30c, field +0xa8) |
| 0x00442cc0 | FUN_00442cc0 | Progress variant getter (DAT_008989b0 stride 4; 0.0 = last-place) |
| 0x00414300 | FUN_00414300 | Pursuit-lead calculator (perp foot + velocity dot + radius gate) |
| 0x00484c70 | FUN_00484c70 | World-objects array getter (count DAT_006e70d8; base DAT_006dccb8) |
| 0x00443d10 | FUN_00443d10 | Track tile-type lookup (128×128 coarse + 8×8 sub-cell; x87 args) |
| 0x00443dc0 | FUN_00443dc0 | Spline lookahead target finder (nearest-pt binary-search + 16-step wall-checked walk) |
| 0x00417730 | FUN_00417730 | Race-angle float getter (DAT_0089a880 stride 4) |
| 0x0046cc10 | FUN_0046cc10 | Vehicle state-code getter (bounds check ≤0xf; FPU pop; caller needs code==4) |
| 0x00443300 | FUN_00443300 | Catmull-Rom spline interpolation (4 control pts; t∈[0,1]; XZ output) |
| 0x0046c7b0 | FUN_0046c7b0 | Entity alive/present flag getter (DAT_008815a4 stride 0x341) |
| 0x0046cbb0 | FUN_0046cbb0 | Spinout-state getter (DAT_00881f90 stride 0x341 + DAT_00881f94 stride 0xd04) |
| 0x004233e0 | FUN_004233e0 | Heading angle atan2 → game-angle units (fpatan + scale/offset + wrap) |
| 0x004c3df0 | FUN_004c3df0 | Velocity transform RW vtable dispatch (slot +0x14 via DAT_007d3ff8+3ffc) |
| 0x00415200 | FUN_00415200 | Vehicle-0 last-place guard (FUN_00442cc0(0)==0.0) |
| 0x0045a0f0 | FUN_0045a0f0 | Vehicle powerup-state field getter (DAT_0068ba00 stride 0x16) |
| 0x00415190 | FUN_00415190 | Track progress range check (last-place vehicle; active scan; [min,max) gate) |
| 0x00455b40 | FUN_00455b40 | Powerup table getter (DAT_006885e0 stride 0x2c; no bounds check) |
| 0x0041f030 | FUN_0041f030 | Trigger struct reader (DAT_0063dc38 stride 0x2ac; 4 DWORDs at +0x38) |
| 0x0048a630 | FUN_0048a630 | Sphere-vs-AABB overlap test (param_1=AABB float[6]; param_2=center+radius) |
| 0x00414490 | FUN_00414490 | Velocity-gated alt targeting (bomb/DM mode exclusion; dot threshold) |

## Tracker changes

- hooks.csv: +20 rows (ai, C1, mapped)
- DEFERRED.md: cleared D-4180..D-4199 (20 rows); added D-5560..D-5564 (5 depth-4 rows, bucket ai_update_d3-cont1)
- UNCERTAINTIES.md: added U-1887..U-1894 (8 rows)
- STUBS.md: no changes (no new stubs; external imports not stubs per session directive)

## Key structural findings

- `DAT_008a96e8` stride `0x30c` (field +0xa8 from FUN_00407a40 base `0x8a9640`): race-progress float per vehicle.
- `DAT_008989b0` stride `4`: separate per-vehicle progress float array (4 entries); 0.0 = last-place sentinel.
- `DAT_0089a880` stride `4`: per-vehicle race-angle float array.
- Entity alive-flag base `0x8815a4` stride `0x341` (field 0); vs position sub-struct base `0x881ec8` (offset `+0x924` from alive-base).
- Spinout flag: `DAT_00881f90` stride `0x341` (field in entity struct); second state: `DAT_00881f94` stride `0xd04`.
- Catmull-Rom spline layout confirmed: `spline[i].x = *(float*)(base + i*8)`, `.z = *(float*)(base+4+i*8)`, count = `*(int*)(base+0x200)`.
- Track tile two-level lookup inlined in FUN_00443dc0 (identical to FUN_00443d10): coarse grid `0x7f1a9c`, sub-cell `0x7f9a9c`.
- Powerup activation system in FUN_00415220 uses several per-vehicle arrays: `0x68ba00` (stride 0x16), `0x6885e0` (stride 0x2c), `0x8989b0` (stride 4); semantics of first two uncertain.
- FUN_004c3df0: RW vtable dispatch through `DAT_007d3ff8+DAT_007d3ffc+0x14` — exact RW type unknown (U-1891).

## Depth-4 deferred (D-5560..D-5564)

| D-ID | RVA | Description |
|------|-----|-------------|
| D-5560 | 0x004c3b30 | Fast sqrt/inv-sqrt (lookup table; from FUN_00414300) |
| D-5561 | 0x00416230 | Vehicle lookahead-index writer (from FUN_00443dc0; ~17 bytes) |
| D-5562 | 0x004b55a0 | Render submit/draw call (from FUN_00443dc0; ~0x1A4 bytes) |
| D-5563 | 0x004c3bf0 | 2D vector distance/magnitude (from FUN_00443dc0) |
| D-5564 | 0x004c3c60 | 2D vector normalizer in-place (from FUN_00443dc0) |

## Pool slot

Mashed_pool0 — releasing after commit.
