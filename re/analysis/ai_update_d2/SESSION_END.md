# Session End: ai_update_d2-20260503-1322

## Session summary

Depth-2 pass on the `ai_update` subsystem. Decompiled and analyzed all 28 functions deferred by parent session W (D-1120..D-1147, bucket ai_update-cont1).

## Functions analyzed

| RVA | Name | Notes |
|-----|------|-------|
| 0x00408af0 | FUN_00408af0 | Heading/velocity float3 getter (offset +0x9c, stride 0x30c) |
| 0x00414030 | FUN_00414030 | AI spline-bank timer reset (DAT_008032d4 stride-5; -1 = all) |
| 0x00414570 | FUN_00414570 | Ahead-in-race targeting (progress diff > 0; acos angle gate) |
| 0x004148b0 | FUN_004148b0 | Leader-ranking timer (last-place catch-up accumulator) |
| 0x00414a70 | FUN_00414a70 | Closest-vehicle targeting (returns 1 or 2 for inactive target) |
| 0x00414c30 | FUN_00414c30 | Obstacle-avoidance targeting (object types 0x7/0xA/0xD/0x15) |
| 0x00414f00 | FUN_00414f00 | Powerup-seek targeting (DAT_00684dac+0x30 target position) |
| 0x00415020 | FUN_00415020 | Frustration timer (72000-frame threshold for mode-5 gate) |
| 0x004150e0 | FUN_004150e0 | Track lateral-zone query (tile grid DAT_007f1a9c; type 0/3 = wall) |
| 0x00415220 | FUN_00415220 | AI powerup-activation (switch on DAT_0088fc88 type; 13 cases) |
| 0x00415880 | FUN_00415880 | Ram-from-behind targeting (latch DAT_0089a4dc; state-4 gate) |
| 0x00415d00 | FUN_00415d00 | Wall-ahead trajectory check (2× velocity ray-march) |
| 0x00415e20 | FUN_00415e20 | Steering angle calculator (acos bearing vs heading; returns ST0) |
| 0x00416060 | FUN_00416060 | Line-of-sight check (ray-march A→B; returns 0 if blocked) |
| 0x004161e0 | FUN_004161e0 | Spline target-point init (wraps FUN_00443dc0) |
| 0x00417cf0 | FUN_00417cf0 | Angle-gated targeting (mode-8 variant; FUN_00417730 dead-zone) |
| 0x00443080 | FUN_00443080 | Getter: DAT_00897ffc (mode-6 gate flag; U-1432) |
| 0x00443440 | FUN_00443440 | Spline progress + curvature (binary-search + lookahead walk) |
| 0x0046d4a0 | FUN_0046d4a0 | Vehicle struct pointer (base 0x881ec8 stride 0x341; +0x30/+0x38 = x/z) |
| 0x0046d510 | FUN_0046d510 | Vehicle velocity getter (matrix-transformed +0xac float3) |
| 0x0046d570 | FUN_0046d570 | Vehicle forward-angle projection (dot heading vs velocity; acos) |
| 0x0046d6a0 | FUN_0046d6a0 | Physics scalar getter (base 0x8820ac stride 0xd04; U-1434) |
| 0x0046d6d0 | FUN_0046d6d0 | Vehicle spline-progress rate (field +0xbc stride 0x341) |
| 0x00452160 | FUN_00452160 | Powerup target position (DAT_00684dac+0x30) |
| 0x00452ea0 | FUN_00452ea0 | Per-vehicle powerup-active flag (DAT_0088ff50[v]) |
| 0x00452eb0 | FUN_00452eb0 | Powerup pursuit range (DAT_00684de0) |
| 0x00472650 | FUN_00472650 | Random float [min,max) (PRNG via FUN_00534870) |
| 0x004c3ac0 | FUN_004c3ac0 | Fast 3-vector magnitude (two-level sqrt lookup table) |

## Tracker changes

- hooks.csv: +28 rows (ai, C1, mapped)
- STUBS.md: cleared S-0403..S-0410 (8 rows); added S-1420..S-1426 (7 new stubs)
- DEFERRED.md: cleared D-1120..D-1147 (28 rows); added D-4180..D-4199 (20 depth-3 rows, bucket ai_update_d2-cont1)
- UNCERTAINTIES.md: added U-1427..U-1436 (10 rows)

## Key structural findings

- Vehicle data lives in two distinct structs: `0x881ec8` stride `0x341` (position/velocity/heading) and `0x8820ac` stride `0xd04` (physics scalars).
- Track tile system: 128×128 grid of 2-byte cell IDs at `0x7f1a9c`; 8×8 sub-cell byte map at `0x7f9a9c`; tile types 0 and 3 = wall/boundary.
- AI control output slot: `0x7f1038 + (&0x7f1a14)[v*4] * 0x4c`; byte at +3 = fire-button.
- Powerup activation data: `DAT_0088fc88[v * 0x2d]` pointer to active powerup struct; type switch has 13 cases (IDs 7–0x13).
- Spline structures: count int at `spline+0x200`; float2 points at `spline[0..n-1*8]`.
- All probabilistic AI decisions go through `FUN_00472650(0, 0.0f, 1.0f)` for random [0,1).

## Lua-glue check

No `lua_pcall` / `luaL_callmeta` callees found in any of the 28 functions. AI logic is pure C code, not Lua-glued.

## Depth-3 deferred (D-4180..D-4199)

Filed 20 depth-3 callee entries in bucket `ai_update_d2-cont1`:
- Key: FUN_00408a50 (race progress), FUN_00442cc0 (progress variant), FUN_00414300 (steer target), FUN_00443d10 (track tile type), FUN_00443300 (spline interpolation), FUN_004233e0 (heading angle), FUN_0046cc10 (vehicle state code), FUN_00417730 (race angle).

## Pool slot

Mashed_pool3 — lock held. Release after commit.
