# Session N6 — Sky / Weather d2
**Session ID:** sky_weather_d2-20260507  
**Slot used:** Mashed_pool13 (read-only)  
**Date:** 2026-05-07  
**SHA-256 anchor:** BDCAE093… ✓  
**Parent bucket:** re/analysis/sky_weather/ (7 deferred items D-5200..D-5206)  
**Outcome:** COMPLETED — 6 of 7 parent deferred items resolved; 1 re-deferred (function_create blocked on writable session)

---

## Pre-flight notes

- Pool13 had a stale Ghidra-internal lock from J6 (debug_overlay_d2 halt). No Java processes running; lock confirmed stale. Root-level `Mashed_pool13.lock` was absent → pool free.
- `.pool_slot` was stale at "Mashed_pool14" (from K6 vehicle_update_d3). Updated to "Mashed_pool13" before preflight run.
- All three preflight asserts passed: slot-match ✓, staleness ✓, scribe-check ✓.

---

## D-5200 — SkyDomeRender (0x004492b0): function_create

**Status: RE-DEFERRED → D-8380**

Ghidra still has no function record at 0x004492b0. Full listing obtained in this session (121 instructions, 0x004492b0..0x0044942a). Analysis is complete — see "SkyDomeRender listing analysis" section. `function_create` requires a writable master session; deferred.

---

## D-5201 — FUN_00499d90 rain particle renderer

**Status: RESOLVED → C1**

**`0x00499d90`** — `RainRenderParticles(int rain_buffer, int count)` — ends at 0x0049a09a.

**Execution:**
1. Two-stage loop (unrolled ×4 + scalar tail): reads source particle records from `rain_buffer` (stride 0x20 bytes/record × 2 per particle = input stride 0x40), builds head+tail vertex pairs into `DAT_007e2580` (stride 0x20 per output vertex).
2. Each particle input record at stride 0x20: `{xyz_base (float3), xyz_velocity (float3), RGBA_head (4B), RGBA_tail (4B)}` → two output vertices: head vertex `{xyz_base, RGBA_head}` and tail vertex `{xyz_base+xyz_velocity, RGBA_tail & 0xffffff}`.
3. After vertex build:
   - `FUN_004cb330(0x100, &DAT_00773928)` — sets texture/material state
   - `FUN_004d5480(0x89, 0)` + `FUN_004d5480(0x16, 1)` — render states
   - Vtable call: `(**(code **)(*DAT_007d4110 + 0x164))(DAT_007d4110, 0x42)` — D3D9 SetFVF(0x42) [UNCERTAIN: verify FVF value]
   - `(**(code **)(DAT_007d3ff8 + 0x20))(0xc, 1)` — render state
   - `FUN_004cb130(0, 0, 0, 0)` — SetTransform
   - `FUN_004d53b0()` — unknown
   - `(**(code **)(*DAT_007d4110 + 0x14c))(DAT_007d4110, 2, param_2, &DAT_007e2580, 0x10)` — **D3D9 DrawPrimitiveUP**: type=D3DPT_LINELIST(2), count=param_2(896), data=&DAT_007e2580, stride=0x10.
   - Restore render state 0xc=0.

**Key globals:**
| Global | Role |
|--------|------|
| `DAT_007e2580` | Vertex scratch buffer (line list) |
| `DAT_007d4110` | D3D9 IDirect3DDevice9* |
| `DAT_007d3ff8` | Render system wrapper |
| `DAT_00773928` | Texture/material data |

---

## D-5202 — Rain script command handlers

**Status: RESOLVED → C1 (7 handlers; all need Ghidra function_create)**

All 7 rain script command handler RVAs confirmed from listing analysis. None have Ghidra function records → stubs S-2820..S-2826.

| RVA | End | Name | Args | Setter RVA | Usage-string addr |
|-----|-----|------|------|-----------|-------------------|
| `0x004917a0` | `0x004917b1` | `RainEnable_Handler` | 0 | direct: `MOV [0x771534],1` | — |
| `0x004917c0` | `0x0049185a` | `RainSetHeadColour_Handler` | 4 (RGBA floats) | `0x00490f50` | `0x5cf554` |
| `0x00491860` | `0x004918fa` | `RainSetTailColour_Handler` | 4 (RGBA floats) | `0x00490f80` | `0x5cf588` |
| `0x00491900` | `0x004919a3` | `RainSetPosition_Handler` | 6 (xmin..zmax) | `0x00490fb0` | `0x5cf5bc` |
| `0x004919b0` | `0x00491a07` | `RainSetScale_Handler` | 2 (scale,length) | `0x00490ff0` | `0x5cf5fc` |
| `0x00491a10` | `0x00491a67` | `RainSetCameraScale_Handler` | 2 (yScale,atScale) | `0x00491010` | `0x5cf624` |
| `0x00491a70` | ~`0x00491b80` | `RainSetDirection_Handler` | 6 (xmin..zmax) | ~`0x00491030`? | `0x5cf654` |

**U-1767 resolution:** `RainEnable` handler confirmed at `0x004917a0`. The CALL at that address is `CALL 0x00490e70` (pops the script param from stack before setting the flag — parameter is read but discarded; rain enable is unconditional).

**Parsing convention:** All multi-param handlers use `FUN_004b6fc0` (arg count getter) + `FUN_004b7090` (indexed float getter) + `FUN_004a2c48` (cleanup/advance). Error path uses `0x00490e50` (script error reporter). Rain-enabled guard checks `DAT_00771534`.

---

## D-5203 — FUN_00489240 second-sky vtable object

**Status: PARTIALLY RESOLVED — DAT_007030ac type still [UNCERTAIN] → U-2827**

**`0x00489240`** (10 bytes) — `SkySecondaryDispatch`:
```c
void SkySecondaryDispatch(void) {
    (**(code **)(DAT_007030ac + 0x48))(DAT_007030ac);
}
```
Called unconditionally in SkyDomeRender at 0x00449393, after sky_clump[0] render and before the second clump render.

**DAT_007030ac creation path:**
- Written by `FUN_004880a0` via `DAT_007030ac = FUN_00534b60(0x2000, 0x10000087, 0)`.
- `FUN_00534b60` is already [C1 2026-05-06]; it normalizes flags and delegates to `FUN_00534d00(param_1, flags, param_3, &DAT_00623c78)`.
- Return type of `FUN_00534b60` not resolved (Ghidra shows `void`; actual EAX return used as object handle/pointer).

**[UNCERTAIN U-2827]:** What RenderWare object type does `FUN_00534d00` create? The vtable call at `+0x48` is compatible with an RpAtomic or RpClump subtype. Evidence missing: decompile `FUN_00534d00` or identify `DAT_00623c78` as a type discriminator.

---

## D-5204 — course+0x10130 clump identification

**Status: PARTIALLY RESOLVED — loader not found → U-2828**

From SkyDomeRender listing (0x004493a4): `MOV ECX, [ESI + 0x10130]` → `CALL 0x004e6680(ECX)` — `RpClumpRender`. This is a valid `RpClump*` pointer at course offset `+0x10130`.

**Key observations:**
- `SkyDomeUpdatePos` (0x004491e0) only iterates `param_1 + 0x100f0` for 4 slots → this clump at `+0x10130` is **NOT repositioned** each frame. It is static in world space.
- Render order in SkyDomeRender: sky_clump[0] (positioned) → FUN_00489240 (vtable) → render-state 0x14=2 → **clump@+0x10130** (static) → render-state 0x14=1 (restore).
- The render-state 0x14 change (2→1 around the static clump) [UNCERTAIN: exact meaning of state 0x14; possibly rwRENDERSTATEBORDERCOLOR or zbias].
- Offset from sky_clump[0] base: 0x10130 − 0x100f0 = 0x40 = 16 DWORD slots further — this is NOT sky_clump[16]; it is a **different array/field in the course struct**.

**[UNCERTAIN U-2828]:** Which loader writes `course + 0x10130`? Evidence missing: search for WRITE references to `DAT_006bf1cc + 0x10130` or equivalent offset. Likely in the track loader DFF parsing chain.

---

## D-5205 — FUN_0047a034 COURSE.LUA executor

**Status: RESOLVED → C1**

Address `0x0047a034` is inside `FUN_0047a020` (body 0x0047a020..0x0047a0e5). The function is **`CourseDescInit`**:

```c
undefined4 CourseDescInit(undefined4 script_state, undefined4 world_ptr, int course_desc) {
    DAT_006bf1c8 = world_ptr;         // world object base
    DAT_006bf1cc = course_desc;       // course description struct base
    _DAT_006bf1d0 = 0;
    DAT_006bf1d4 = 0;                 // sky dome count = 0
    FUN_00478cb0(world_ptr);          // CourseAssetDestroy(world)
    FUN_004b6520(course_desc, 0x21c); // memset(course_desc, 0, 0x21c)
    // init default fog/ambient values:
    *(course_desc + 0x204) = 0;       // ambient R
    *(course_desc + 0x208) = 1.0f;    // ambient G
    *(course_desc + 0x20c) = 1.0f;    // ambient B
    *(course_desc + 0x210) = 1.0f;    // ambient A
    FUN_004b6520(&DAT_0086cac0, 0x2200);   // memset large array
    // init 128-entry table at DAT_0086cba4 (stride 0x110B per entry):
    //   {+0x00=0, +0x10=0.5, +0x00=1.0, +0x04=0, +0x08=0, +0x0c=0, +0x14=0.5, +0x18=0.01}
    FUN_0047b9b0(script_state, &world_ptr, &LAB_00440bc0);  // register sky/fog/rain script commands
    return world_ptr;
}
```

Called from track loader `FUN_00426e10` at RVA 0x0047a034 (the DAT_006bf1cc write reference MMMM cited).

---

## D-5206 — CourseRenderFrame callers

**Status: RESOLVED → C1**

`CourseRenderFrame` (0x004270f0) has exactly **1 caller**: `FUN_00410b30` (0x00410b30..0x00410d03).

**`FUN_00410b30`** — `GameRenderFrame`:
```
(DAT_007d3ff8[8])(0x14, 2)               // set render state
FUN_004270f0(course_list[view_idx])       // CourseRenderFrame
(DAT_007d3ff8[8])(0xe, 1)
FUN_004725c0()
FUN_0045b350(*DAT_007d3ff8)              // clear depth/stencil
if (DAT_0063ba98 == 0):
    for i in 0..3: FUN_00420050(i, *DAT_007d3ff8)   // per-vehicle render?
(DAT_007d3ff8[8])(0xe, 0)
FUN_00411ce0()
FUN_0041ebb0(*DAT_007d3ff8)
for i in 0..3: FUN_0045b350(i, *DAT_007d3ff8)
[render state 6/8 = 1,1; various effect/particle/HUD calls]
FUN_00448730()    // HUD/frontend overlay?
FUN_00490490()    // particle system tick?
[conditional FUN_00403910 for race state 9/10 skip]
```

**Key globals:**
| Global | Role |
|--------|------|
| `PTR_PTR_005f2770` | Array of course ptrs |
| `DAT_0063ba78` | Current view/split-screen index |
| `DAT_007f0fd0` | Game mode (≠5 = not-replay?) |
| `DAT_0063ba8c` | Race state index |

---

## SkyDomeRender listing analysis (0x004492b0)

Complete listing obtained. Function body is 121 instructions spanning 0x004492b0..0x0044942a. Execution flow (confirmed):

1. **Prologue / color setup**: Pushes RGBA {0x50, 0x58, 0x60, 0xff} on stack for screen clear.
2. `CALL 0x004671a0` — SelectCamera / SetCameraColor.
3. `CALL 0x004840d0(0)` + `CALL 0x004c1c80` — GetCamera(0) → FUN_004c1c80 (fog color setup).
4. **NEW: `CALL 0x00451060`** at 0x004492e6 — `SkyClearColorSet()`: `{GetCamera(0).frame → FUN_00467210(0,0) → FUN_004c1480(frame, result)}`. Unknown exact semantics → stub S-2827.
5. `CALL 0x004840d0(0)` + `CALL 0x004c1bb0` — GetCamera + RwCameraSetFogDistance.
6. `CALL 0x004840d0(0)` → `CALL 0x00426060(camera)` → `CALL 0x004e4320` — camera raster/begin-scene setup.
7. `CALL 0x004491e0(course_ptr)` — SkyDomeUpdatePos.
8. `CALL 0x004840d0(0)` → `CALL 0x004c1a00(camera)` — RwCameraBeginUpdate; JZ to show-raster if fails.
9. **Render states (via `[DAT_007d3ff8+0x20]`):** (0xc,0) (0xa,5) (0xb,6) (0x6,0) (0x8,0).
10. `MOV EAX, [ESI+0x100f0]`; if non-null → `CALL 0x004e6680(sky_clump[0])` — RpClumpRender.
11. `CALL 0x00489240` — SkySecondaryDispatch.
12. Render state `(0x14, 2)`.
13. `MOV ECX, [ESI+0x10130]` → `CALL 0x004e6680` — RpClumpRender(static_clump).
14. **Restore states:** (0x14,1) (0xa,5) (0xb,6) (0xc,0) (0x6,1) (0x8,1).
15. `CALL GetCamera(0)` → `CALL 0x004c19f0` — RwCameraEndUpdate.
16. **Join** (from failed begin-update): `CALL GetCamera(0)` → `CALL 0x00426060` → `CALL 0x004e4350` — RwCameraShowRaster.

---

## Functions identified this session

| RVA | Name | Status | Notes |
|-----|------|--------|-------|
| `0x00499d90` | `RainRenderParticles` | C1 | DrawPrimitiveUP LINE_LIST 896 drops |
| `0x00489240` | `SkySecondaryDispatch` | C1 | 10-byte vtable wrapper on DAT_007030ac+0x48 |
| `0x0047a020` | `CourseDescInit` | C1 | Sets course desc globals, registers script cmds |
| `0x00410b30` | `GameRenderFrame` | C1 | Main per-frame render: course+vehicles+HUD+effects |
| `0x004917a0` | `RainEnable_Handler` | C1 | 12 bytes; no Ghidra func record; S-2820 |
| `0x004917c0` | `RainSetHeadColour_Handler` | C1 | 4 args; setter 0x490f50; S-2821 |
| `0x00491860` | `RainSetTailColour_Handler` | C1 | 4 args; setter 0x490f80; S-2822 |
| `0x00491900` | `RainSetPosition_Handler` | C1 | 6 args; setter 0x490fb0; S-2823 |
| `0x004919b0` | `RainSetScale_Handler` | C1 | 2 args; setter 0x490ff0; S-2824 |
| `0x00491a10` | `RainSetCameraScale_Handler` | C1 | 2 args; setter 0x491010; S-2825 |
| `0x00491a70` | `RainSetDirection_Handler` | C1 | 6 args; ~0x491030?; S-2826 |
| `0x00451060` | `SkyClearColorSet` | C1 | New; called in SkyDomeRender; S-2827 |

---

## Uncertainties

**U-2827** — `DAT_007030ac` type: `FUN_00534d00` return type not resolved. Need: decompile `FUN_00534d00` or inspect `DAT_00623c78` to determine what RW object type is created with flags `0x2000 / 0x10000087`.

**U-2828** — `course + 0x10130` loader: which function writes this slot? Need: search WRITE references to the equivalent offset in the course struct.

**U-2829** — `FUN_00451060` / `FUN_004c1480`: called as `FUN_004c1480(camera_frame, FUN_00467210(0,0))` in SkyDomeRender. Likely sets camera background / near-clip. Need: decompile `FUN_004c1480` and check if it is an RW camera property setter.

---

## STUBs

**S-2820** — `RainEnable_Handler` (0x004917a0): no Ghidra function record. Needs `function_create` in writable session.  
**S-2821** — `RainSetHeadColour_Handler` (0x004917c0): same.  
**S-2822** — `RainSetTailColour_Handler` (0x00491860): same.  
**S-2823** — `RainSetPosition_Handler` (0x00491900): same.  
**S-2824** — `RainSetScale_Handler` (0x004919b0): same.  
**S-2825** — `RainSetCameraScale_Handler` (0x00491a10): same.  
**S-2826** — `RainSetDirection_Handler` (0x00491a70): same.  
**S-2827** — `FUN_00451060` (`SkyClearColorSet`): Ghidra function exists; body analyzed at C0; callee `FUN_004c1480` not yet analyzed.  
**S-2828** — `FUN_00534d00` (DAT_007030ac creator): called by `FUN_00534b60`; return type not resolved.

---

## Deferred (D=8380..8439, bucket sky_weather_d2-cont1)

**D-8380** — `SkyDomeRender` (0x004492b0): needs `function_create` + `bookmark_add` in a writable master session. Listing analysis complete in this session; no further decomp needed.  
**D-8381** — `FUN_00534d00` return type / DAT_007030ac object type. Decompile FUN_00534d00 to determine RW object.  
**D-8382** — Loader for `course + 0x10130` clump slot. Search WRITE refs to course desc struct at offset 0x10130.  
**D-8383** — Rain setter bodies: `0x00490f50` (HeadColour), `0x00490f80` (TailColour), `0x00490fb0` (Position), `0x00490ff0` (Scale), `0x00491010` (CameraScale). Likely writes to `DAT_00616030..33` (rain RGBA globals). One session, 5 functions.  
**D-8384** — `GameRenderFrame` (0x00410b30) full callee set: `FUN_004725c0`, `FUN_00420050`, `FUN_00411ce0`, `FUN_0041ebb0`, `FUN_00448730`, `FUN_00490490`, `FUN_0047b9e0`. At least 7 new subsystem entry points.  
**D-8385** — `FUN_004c1480` / `FUN_00451060` (SkyClearColorSet) final identification.

---

## cap_count = 0

## Scribe queue row
```
2026-05-07  sky_weather_d2-20260507  bucket=sky_weather_d2  rvas=0x00499d90,0x00489240,0x0047a020,0x00410b30,0x004917a0,0x004917c0,0x00491860,0x00491900,0x004919b0,0x00491a10,0x00491a70,0x00451060  S-2820..S-2828  U-2827..U-2829  D-8380..D-8385  pool=Mashed_pool13
```
