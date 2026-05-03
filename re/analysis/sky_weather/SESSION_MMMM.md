# Session MMMM — Sky / Weather
**Date:** 2026-05-03  
**Pool slot:** Mashed_pool8 (read-only)  
**SHA-256 anchor:** BDCAE093… ✓  
**Anchor strategy:** String-based → found "Loading SkyDome [%s]" @ 0x005cecc4 immediately

---

## Anchor outcome: FOUND via Strategy 1

Key strings confirmed:
| Address     | Value                                  |
|-------------|----------------------------------------|
| 0x005cdf4c  | `Sky_Filename`                         |
| 0x005cecc4  | `Loading SkyDome [%s] into sky[%d]\n`  |
| 0x005cde4c  | `Modify_Fog`                           |
| 0x005cde58  | `Setup_Fog`                            |
| 0x005cdb28  | `RainEnable`                           |
| 0x005cdb14  | `RainSetHeadColour`                    |
| 0x005cdab8  | `RainSetDirection`                     |
| 0x005cdacc  | `RainSetCameraScale`                   |
| 0x005cdae0  | `RainSetScale`                         |
| 0x005cdaf0  | `RainSetPosition`                      |
| 0x005cdb00  | `RainSetTailColour`                    |
| 0x005ce5f5  | `.:>exp_cloud2`                        |
| 0x005cf4dc  | `exp_cloud`                            |

No SKY / WEATHER / FOG directories in TOASTART — sky content is per-track DFF files.

---

## SKY_FN

**`0x004492b0`** — `SkyDomeRender` (unanalyzed in Ghidra; ~378 bytes, ends at RET 0x0044942a)

Called via function pointer stored at `0x005f34ac` (training/view vtable table).

### Execution:
1. Sets camera-follow color (R=0x50 G=0x58 B=0x60 A=0xff) for screen clear
2. Gets camera via `FUN_004840d0(0)` and sets up fog color `FUN_004c1bb0`
3. `FUN_00426060(camera)` → `FUN_004e4320` — raster/camera setup
4. Calls `FUN_004491e0(course_ptr)` — positions all sky clumps at camera position scaled 180 units
5. `FUN_004840d0(0)` → `FUN_004c1a00` — `RwCameraBeginUpdate`
6. If begin-update succeeds:
   - Sets render states: zwrite=OFF, blend=ON (mode 5), z-test=ON (mode 6), fog=OFF, zbuf-write=OFF
   - `MOV EAX, [ESI + 0x100f0]` → `CALL FUN_004e6680(sky_clump[0])` — **`RpClumpRender`**
   - `CALL FUN_00489240` — vtable render on secondary object
   - Render state: (0x14, 2)
   - `MOV ECX, [ESI + 0x10130]` → `CALL FUN_004e6680` — renders another clump
   - Restores render states: z-test, alpha-blend, zwrite=ON, fog=ON, zbuf=ON
   - `FUN_004c19f0(camera)` — `RwCameraEndUpdate`
   - `FUN_004e4350` — `RwCameraShowRaster`

Sky domes are NOT added to the RpWorld — rendered explicitly by this function.

---

## Sky dome data structures

| Global          | Value at game-run                             |
|-----------------|-----------------------------------------------|
| `DAT_006bf1c8`  | Sky filename array base (64 bytes/slot)        |
| `DAT_006bf1d4`  | Sky dome count (incremented by SkyFilename cmd)|
| `DAT_00646e58`  | Course object base                            |
| `+0x100f0`      | Sky clump[0..7] (RpClump*) — loader uses 0..3 |
| `+0x10210`      | Sky atomic[0..3] (RpAtomic*) — from loader    |

Up to 4 sky domes loaded per track (COURSE.LUA `Sky_Filename` command, 0-indexed).  
Sky clumps are positioned at camera origin, scaled 180.0 units (`FUN_004491e0`).  
UV animations updated each frame via `FUN_00426780` (already C1).

---

## Fog system

### Script commands registered at `0x00440bc0` block (via `FUN_0047b980` = script registrar):

| Command             | Handler RVA  | Notes                              |
|---------------------|--------------|------------------------------------|
| `Sky_Filename`      | `0x47a1e0`   | Writes filename to array slot      |
| `Setup_Fog`         | `0x47ab30`   | Writes fog near/far/RGBA           |
| `Modify_Fog`        | `0x47abd0`   | Writes 3 floats + RGBA fog variant |
| `Ambient_RGB`       | `0x47aad0`   | Course ambient color               |
| `Lights_Filename`   | `0x47aaa0`   | Course light DFF path              |

### Fog struct layout (base = `DAT_006bf1cc` = course description struct):
```
+0x1e8  fog_color_R      (byte) — Setup_Fog
+0x1e9  fog_color_G      (byte)
+0x1ea  fog_color_B      (byte)
+0x1eb  fog_alpha        (byte) = 0xff hardcoded
+0x1ec  fog_near         (float)
+0x1f0  fog_far          (float)
+0x1f4  modify_fog_R     (byte) — Modify_Fog
+0x1f5  modify_fog_G     (byte)
+0x1f6  modify_fog_B     (byte)
+0x1f7  modify_fog_alpha (byte) = 0xff hardcoded
+0x1f8  modify_fog_f0    (float)
+0x1fc  modify_fog_f1    (float)
+0x200  modify_fog_f2    (float)
```

`FUN_004924e0` (0x4924e0, 5 bytes) — returns `&DAT_006147b4` (runtime fog RGB, read back in render states via vtable[+0x20]).

Fog color packed into render state in `FUN_004270f0`:
`((bVar2 | (bVar1 | 0x4000) << 8) << 8) | bVar3` (standard RW RGBA encoding with bias bit).

---

## Rain system

### Init path (called from track loader `FUN_00426e10` → `FUN_00491780`):
- **`0x00491780`** — [C1 2026-05-03] — gate: if `DAT_00771534 == 0` return; else call `FUN_00491590`
- **`0x00491590`** — [C1 2026-05-03] — **RainParticleInit**: allocs `DAT_00771530` (0x7000 bytes = 896×32) and `DAT_0077152c` (0x2a00 bytes). Randomizes drop positions/velocities using `FUN_00472650` (range RNG). Calls `FUN_00491070`.
- **`0x00491070`** — **RainColorInit**: fills byte at `+0x1d` every 0x20 bytes in rain buffer with RGBA from `DAT_00616030..33`. 4 passes × 0xe0 = 896 drops total.

### Script commands (registered near `0x00440f4a`):
| Command           | Handler RVA  |
|-------------------|--------------|
| `RainEnable`      | `0x4917a0`+  |
| `RainSetHeadColour` | `0x4917c0` |
| `RainSetTailColour` | — (follows)|
| `RainSetPosition` | —            |
| `RainSetScale`    | —            |
| `RainSetCameraScale` | —         |
| `RainSetDirection` | —           |

`RainEnable` handler: the instruction at `0x4917a0` is `CALL 0x00490e70` — handler starts just before 0x4917a0. `DAT_00771534` = rain-enabled flag (handler sets this to 1). [UNCERTAIN: exact handler start RVA — U-1767]

### Per-frame rain:
- **`0x00491490`** — [C1 2026-05-02] — update dispatcher: calls `FUN_00491340` or `FUN_004910c0` based on `DAT_007f108b`
- **`0x004914b0`** — **RainRender**: guards on `DAT_00771534`; on first render sets blend states (fog=1, zbuf=1, blending=1, blend_src=2); calls `FUN_00499d90(DAT_00771530, 0x380)` = renders 896 particles
- `DAT_00771540` = rain render states initialized flag

### Rain globals:
| Global          | Role                              |
|-----------------|-----------------------------------|
| `DAT_00771530`  | Rain particle buffer (0x7000 B)   |
| `DAT_0077152c`  | Rain secondary buffer (0x2a00 B)  |
| `DAT_00771534`  | Rain enabled flag                 |
| `DAT_00771538`/`0x3c` | Velocity range x/y parameters |
| `DAT_00771540`  | Render states init flag           |

---

## Per-frame render pipeline

**`FUN_004270f0`** (0x4270f0, 689 bytes) — `CourseRenderFrame`:
1. Render-state disable (0xe, 0x8, 0x6)
2. `FUN_0041e9b0` — camera select
3. `FUN_0041ea00` check: if 0 → frontend path; if non-zero → `FUN_0041e8e0` (vtable sky dispatch)
4. Render states: fog packed from `FUN_004924e0` result, fog enable (0x10,1)
5. `FUN_00478cd0` → `FUN_00484580` → `FUN_004844a0` — BSP world render (uses `RpWorldForAllAtomics`)
6. **`FUN_004914b0()`** — rain particle render
7. `FUN_0041e9b0` + `FUN_0041e9f0` → `FUN_0041e8d0` — second vtable camera dispatch

---

## Functions identified this session

| RVA        | Name                       | Subsystem | Confidence | Notes                                    |
|------------|----------------------------|-----------|------------|------------------------------------------|
| 0x004492b0 | SkyDomeRender              | render    | C1         | SKY_FN; unanalyzed in Ghidra; S-1760     |
| 0x004491e0 | SkyDomeUpdatePos           | render    | C1         | Positions sky clumps at camera, scale 180|
| 0x0047ab30 | SetupFog_Handler           | render    | C1         | Script cmd; near/far + RGB → +0x1ec     |
| 0x0047abd0 | ModifyFog_Handler          | render    | C1         | Script cmd; 3 floats + RGB → +0x1f4     |
| 0x0047a1e0 | SkyFilename_Handler        | render    | C1         | Script cmd; stores DFF filename in array |
| 0x004914b0 | RainRender                 | render    | C1         | 896 particles; FUN_00499d90              |
| 0x00491070 | RainColorInit              | render    | C1         | Sets RGBA per drop from DAT_00616030..33 |
| 0x004270f0 | CourseRenderFrame          | render    | C1         | Per-frame: sky+world+rain                |
| 0x00478cf0 | CourseAssetDestroy         | render    | C1         | Frees sky clumps + world + TXD           |
| 0x00474fd0 | GetFirstAtomicFromClump    | render    | C1         | RW helper; 49 bytes; RpClumpForAllAtomics |
| 0x004e6680 | RpClumpRender              | render    | C1         | Standard RW3; iterates atomics, calls renderCallBack |
| 0x004e5680 | RpWorldForAllAtomicsCallback| render   | C1         | FUN_004f0900 visibility gate; calls atomic+0x48 |
| 0x0047b980 | RegisterScriptCommand      | track     | C1         | Takes (fn_ptr, name_str); builds command table |
| 0x004924e0 | FogColorGetter             | render    | C1         | 5 bytes; returns &DAT_006147b4           |

Already in Ghidra as C1 (may be in hooks.csv from parallel sessions today):
- `0x00491590` [C1 2026-05-03] RainParticleInit
- `0x00491780` [C1 2026-05-03] RainInitGate
- `0x0047c0b0` [C1 2026-05-03] CourseStateReset
- `0x0047c0f0` [C1 2026-05-03] AnimClumpInit

---

## Uncertainties

**U-1767** — `RainEnable` handler exact start: instruction at 0x4917a0 is mid-function (`CALL 0x00490e70`). Handler likely begins at 0x491796 (just after FUN_00491780 ends) or earlier. Need: disassemble 0x491760..0x4917a0 to find entry.

**U-1768** — `SkyDomeRender` (0x004492b0) dispatch mechanism: stored as function pointer at `0x005f34ac`. Vtable structure of the training/view object not fully mapped. Need: read `0x005f34ac` and surrounding table to understand all vtable methods.

**U-1769** — `FUN_00489240` called after sky clump render: `(**(DAT_007030ac + 0x48))(DAT_007030ac)` — vtable call on unknown object. Possibly a particle system or secondary sky effect.

**U-1770** — `course + 0x10130` second clump: rendered alongside sky in `SkyDomeRender`. Offset 0x10130 is `0x100f0 + 0x40` = 16 sky-clump slots in. May be a cloud layer or separate skydome zone loaded from a different track section.

---

## STUBs

**S-1760** — `SkyDomeRender` (0x004492b0): Ghidra has no function record. Needs `function_create` in writable session before C2 work can proceed.

**S-1761** — `FUN_00499d90` (rain particle draw): called as `FUN_00499d90(DAT_00771530, 0x380)` in `RainRender`. Not yet analyzed. Likely a low-level particle vertex-buffer flush.

**S-1762** — `FUN_00491340` / `FUN_004910c0` (rain update variants): dispatched by `FUN_00491490` based on `DAT_007f108b`. Not analyzed.

**S-1763** — `FUN_0047a034` (COURSE.LUA executor that writes `DAT_006bf1cc`): one WRITE ref to the course description base pointer. Writes it then all 47 handler reads follow. Not yet analyzed.

---

## cap_count = 0 (no early finish)

## Deferred (D=5200..5259 bucket `sky_weather-cont1`)

D-5200 — `SkyDomeRender` full disasm + function creation in writable session  
D-5201 — `FUN_00499d90` rain particle renderer (low-level vertex buffer)  
D-5202 — Rain script command handlers (RainSetDirection, RainSetPosition, etc.) — addresses ~0x491796..  
D-5203 — `FUN_00489240` second-sky vtable object  
D-5204 — `course + 0x10130` clump identification  
D-5205 — `FUN_0047a034` COURSE.LUA executor that initializes the fog/sky struct  
D-5206 — Full mapping of `FUN_004270f0` (CourseRenderFrame) callers — find main game loop integration
