# Session 94 — breadth_unmapped_0049x

**Date:** 2026-05-14  
**Pool slot:** Mashed_pool0 (pre-assigned as pool5; pool5 did not exist — pool0 used)  
**RVAs analysed:** 20

## Summary

Two distinct subsystems were found in the 0x0049xxxx lower block:

### A. LineRain system (0x00490e50 – 0x00491530)

The string `"LineRain error"` at 0x00490e50 names this subsystem. Six setter functions (`FUN_00490f50`, `0f80`, `0fb0`, `0ff0`, `1010`, `1030`) correspond directly to Lua API names found in the SESSION_MMMM sky_weather session strings: `RainSetHeadColour`, `RainSetTailColour`, `RainSetCameraScale`, `RainSetScale`, `RainSetDirection`.

The per-frame update has **two variants**:
- `FUN_004910c0` — random camera-space respawn (called by `FUN_00491490` = `RainRender`).
- `FUN_00491340` — table-driven respawn from `DAT_0077152c`; no callers found.

### B. Spark/lightning point-sprite system (0x00491b20 – 0x004921b0)

A proximity-triggered point-sprite effect centred on bolt sources registered via `FUN_00491b20`. Per-frame update (`FUN_00491f00`) spawns sparks when a game object (param_1 + 0x40) enters threshold `_DAT_005cf694`, then builds a 512-entry vertex stream.

`FUN_00491c00` (init), `FUN_00492170` (teardown), `FUN_004921b0` (render dispatch) have no cross-reference callers — possibly invoked via function pointer table or from C++ call site not yet analysed.

### C. Clump-pool helpers (0x00490000, 0x00490020)

Adjacent to `FUN_00490500` (effects_particle), using the same `DAT_0086a4ac` stride-0x14 pool:
- `FUN_00490000` — clears all 120 pool slot handles.
- `FUN_00490020` — spawns a particle into the pool (position, velocity, scale, clump).

## RVA table

| RVA | Subsystem | Description |
|-----|-----------|-------------|
| 0x00490000 | render/effects | clump-pool clear (120 entries) |
| 0x00490020 | render/effects | particle spawn into clump pool |
| 0x00490e50 | render/rain | LineRain fatal error handler |
| 0x00490e70 | render/rain | LineRain param init (defaults) |
| 0x00490f50 | render/rain | SetHeadColour (DAT_00616030..33) |
| 0x00490f80 | render/rain | SetTailColour (DAT_00616034..37) |
| 0x00490fb0 | render/rain | SetSpawnRegion (X/Z range + Y pair) |
| 0x00490ff0 | render/rain | SetCameraScale (velocity scale) |
| 0x00491010 | render/rain | SetScale (width range 3.0f..6.0f) |
| 0x00491030 | render/rain | SetRenderColours (6-float cluster) |
| 0x004910c0 | render/rain | per-frame update (random respawn) |
| 0x00491340 | render/rain | per-frame update (table respawn) |
| 0x00491530 | render/rain | teardown (free buffers) |
| 0x00491b20 | render/spark | register bolt source (max 8) |
| 0x00491bd0 | render/spark | clear spark vertex buffer (0x4000 bytes) |
| 0x00491c00 | render/spark | init (alloc + create point-sprite geometry) |
| 0x00491e50 | render/spark | spawn spark particle into free slot |
| 0x00491f00 | render/spark | per-frame update + vertex stream build |
| 0x00492170 | render/spark | teardown (free geometry + buffer) |
| 0x004921b0 | render/spark | render dispatch (vtable[0x48]) |

## Uncertainties filed

| ID | Issue |
|----|-------|
| [UNCERTAIN] 0x00490e70 | DAT_00771538/3c purpose (Y-range or other pair) |
| [UNCERTAIN] 0x00491340 | No callers found for table-respawn rain update |
| [UNCERTAIN] 0x00491b20 | No callers found (may be function-pointer invoked) |
| [UNCERTAIN] 0x00491c00 | No callers found for spark init |
| [UNCERTAIN] 0x00491e50 | Non-standard EAX/EDI calling convention |
| [UNCERTAIN] 0x00491f00 | `DAT_0077171c` meaning (frame counter or enable flag) |
| [UNCERTAIN] 0x00492170 | No callers found for spark teardown |
| [UNCERTAIN] 0x004921b0 | No callers found for spark render dispatch |
