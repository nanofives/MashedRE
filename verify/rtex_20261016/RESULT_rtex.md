# RESULT rtex: two-stage world textures and the car material passes (U-9692, U-9693)

**Verdict.** Two of four fixes ship default ON and pass every pre-registered rule on all 9 frames;
one is ported but held opt-in; one is filed, not ported.

| fix | what | default | C16/C17 rows gone (9 frames) | new rows | pixels vs original |
|---|---|---|---:|---:|---|
| F-DUAL | TRAINING two-stage world = RenderWare MatFX DUAL (BSP + ROAD.DFF) | **ON** (`MASHED_LIBRW_DUAL=0` off) | 15 (5 per TRAINING frame) | 0 | TRAINING 28.52 -> 15.83, 21.30 -> 11.13, 15.76 -> 9.02; Arctic/City identical |
| F-DIRT | car dirt dual texture (dirt.piz, by part code) | **ON** (`MASHED_CAR_DIRT=0` off) | 8 (dirt TEX1, every frame that had it) | 0 | down on 7 frames, equal on 2 |
| F-ENV | car env-map gloss (single pass + CarGloss second pass) | OFF (`MASHED_CAR_ENV=1` on) | its 2 target rows go | 21 (7 RED) | Arctic 1051 +0.60, City 761 +0.23 |
| F-WET | Arctic WetRoad (game vertex + pixel shader) | not ported | 0 | -- | -- |

Default build, base `ec3301f3` -> final (`log/rtex/final/mashed_re.exe`): rows TRAINING 86/85/85 -> 80/79/79,
Arctic 81/81/81 -> 80/80/80, City 157/155/152 -> 156/154/152; 0 RED on 9/9 before and after; matched draws
and matched triangles unchanged on 9/9. G-CTRL `c2e4d60d / b50bd39b / ba90ed88` on base, on the F-DUAL
build and on the final default build (`gctrl.txt`). Pre-registration `PREREG_rtex.md` (`8e3d15be`, before
the first measuring run; amendments 1-2 committed in `29c09547` before any F-DIRT / F-ENV run).

## 1. What the original does

All of it is RenderWare's own MatFX D3D9 code plus the game's car set-up; full citations in
`PREREG_rtex.md` section 1 and `evidence/decomp_rtex.txt`, `evidence/dis_005405c0_statecalls.txt`.

- **MatFX callback** `FUN_00543500`: per mesh, effect at MatFX data `+0x30`: 2 -> `FUN_005412d0`,
  4/6 -> `FUN_005405c0`, else plain `FUN_00540340`.
- **DUAL** SRCALPHA/INVSRCALPHA, base present (`0x005408f9..0x005409af`): `C = Diffuse * lerp(T0, T1, T1.a)`
  on colour and alpha; stage-1 TEXCOORDINDEX forced to 0 only without TEXTURED2 (`0x00540c56`); a stage-1
  texture never changes blending (`FUN_004d6ce0`, `0x004d6d00`).
- **Assets** (`evidence/matfx_census.txt`, every DFF/BSP): 1518 dual materials SRCALPHA/INVSRCALPHA, 48 ZERO/ONE
  (Atmos only), 57 UVTRANSFORM, 7 ENVMAP (dump), 1 BUMPENVMAP, 1 DUALUVTRANSFORM (Storm). TRAINING: ROAD.DFF
  (62) and GRAPH.BSP material 0, two UV sets. Arctic and City worlds: none.
- **Car dirt** `FUN_0040cea0` -> `FUN_0041ecc0`: dirt.piz PNG named by course (`FUN_00404e00`) and course x
  vehicle (`FUN_00404e20`), tables built by `FUN_00404830`; wheel parts 0..3 / 0x21..0x24.
- **Car env** `FUN_00420420`: parts listed by `FUN_0041f360` get ENVMAP with `cargloss`; per frame
  `FUN_00420870` rotates the env frame, `FUN_0041f590` keeps an 8-sample shade window from `FUN_00479100`
  (collision prelit under the car), `FUN_00420290` computes
  `coef = lerp(lo, hi, shade) * (1 - clamp(d^2, 0, 100) / 100)` with `lo, hi` = COURSE.LUA
  `Vehicle_Shininess_Range` (binding `0x0047ac80`; defaults 0 / 1.0 at `0x0047a060` / `0x0047a066`) and
  `d` = part 0x13 to camera. Render `FUN_005412d0`: single pass MULTIPLYADD (ADD at coef byte 255) for an
  opaque base, else a second pass SRCALPHA/ONE, Z-write off, lighting off, fog colour 0.
- **Part code** `FUN_004b5190` reads the GEOMETRY (`atomic+0x18`) user data array 0 element 0
  (`FUN_00543d70` / `FUN_00543df0`, plugin at `geo + DAT_007dc634`): the DFF's rpUSERDATA `0.tv_part_id`.
  Live witness: 71/71 car record slots match (`evidence/partid_check_training.txt`). This answers U-9079.

Live values (`camera_probe.py --matfx`, `log/rtex/orig_matfx2_*`): `DAT_00636ad0/ad4` 0; dirt TRAINING
DirtBody/DirtAdWheel, Arctic and City GrimeBody/GrimeAdWheel (the generated tables predicted both before the
capture), 128x128 4444 no mip, filterAddressing 0x1101; shininess 0.7/1, 0.6/1.0, 0.5/1.0; the coefficient
formula reproduces the live value to about 1e-4 on stationary rows; the env frame is a pure rotation of
`rec+0x280` degrees (`envframe_check.py`).

## 2. What changed

- `Track/MatFxExt.h` (new), `TrackWorld`, `DffModel`: MatFX material extension, second UV set, part code,
  atomic frame origin.
- `LibRw/RwSceneBuild.cpp`: DUAL kept on materials, TEXTURED2 + texcoord set 1, MatFX pipeline on atomics with
  a dual material.
- librw P9 (`deps/librw/MASHED_PATCHES.md`): `matfxRender_Dual`, `matfxRender_EnvRW`, 12 shader blobs, stage>=1
  unbinds to NULL (not `whiteTex`), recorder stage-1 hook.
- `LibRw/RwCarFx`, `RwRaceSubmit` (dirt bind, env set-up, per-instance env coefficient/frame),
  `RwRasterBridge` (C4444 texture, texel alpha rule), `TrackRenderer` (dirt.piz load, Lua shininess,
  collision prelit, per-car gloss step).
- `Vehicle/CarMaterialTables.cpp` (new, both targets): `FUN_00404e00`, `FUN_00404e20`, `FUN_0041f360` moved
  unchanged for the .asi from `Util/PromoLoop_round74/66.cpp`, so each RVA keeps one body (build.bat
  rva-lint); `MASHED_STANDALONE` resolves the MASHED tables to the generated `FUN_00404830` transcription.
- `re/frida/camera_probe.py --matfx`; `DrawRec.cpp` takes librw's stage-1 name.

## 3. Rules (PREREG section 3), per fix

| rule | F-DUAL | F-DIRT | F-ENV |
|---|---|---|---|
| 1 targets gone | PASS 15/15 | PASS 8/8 | PASS (dirt-arm TEX1 env + MISSING CarGloss) |
| 2 no new rows | PASS 0 | PASS 0 | **FAIL** 21: EXTRA CarGloss (RED, 7 frames), ORDER CarGloss (C21), STATE CarGloss (amendment 2, allowed) |
| 3 matched do not drop | PASS | PASS | PASS (matched rises, e.g. 194 -> 202) |
| 4 no-op control | PASS 9/9 (`dualoff` = base) | PASS 9/9 (`carfx_off` = dual) | same control |
| 5 G-CTRL | PASS | PASS (final) | PASS (`final_allon`, `gctrl.txt`) |
| 6 pixels | PASS (all down or equal) | PASS | **FAIL** Arctic 1051 +0.60 |

Why F-ENV fails: the standalone draws every car LOD (C02, U-9079's consumer), so the env second pass also runs
on the duplicate glass atomics the original never draws. With part codes now parsed, porting
`FUN_00420870`'s per-part visibility is the unblocker (U-9693 path).

Scores: `scores.txt`; row deltas `evidence/rowdelta.txt`; per-arm summaries `evidence/summary_*.txt`; final
cited diffs `evidence/final_diffs/`. Raw runs under `log/rtex/` (not committed).

## 4. F-WET (not ported)

`FUN_0049a750` is a game render callback over one Arctic sector's instanced meshes: names `W?t` / `Ic?`
(`bl = 0x65`, `0x0049a8c8`) get vertex shader `[0x00773974]` + pixel shader `[0x0077397c]` (created
`0x0049aae0` from bytecode `0x005d0438` / `0x005d0518`) and stage-1 texture `[0x00773978]` (setter
`0x0049a740`) with TTFF PROJECTED, CLAMP. The same callback is U-9689's sector loop. Porting needs the shader
code, its constants (`0x00497060`, `FUN_004cbb00` uploads) and the 128x128 X8R8G8B8 texture source; filed as
U-9692's open half.

## 5. Found on the way

1. U-9079's slot is DFF data (above); ENDPOINTPANEL.DFF carries the same `0.tv_part_id`.
2. BuildClump gives every atomic the model's whole material list, so per-atomic state must go through
   `atomic_mat` (the first car run bound 836 dual / 994 env materials before this was fixed).
3. librw's upstream `matfx_env_VS.hlsl` never got P7's per-pixel fog.
4. The measured final exe predates four comment-only edits (census numbers in `d3d9matfx.cpp`,
   `matfx_dual_PS.hlsl`, `MatFxExt.h`, `RwSceneBuild.cpp`); no code changed after it.

## 6. Reproduce

```
py -3.12 verify\rtex_20261016\run_arm.py <exe> <arm> [--tracks training,arctic,city] [KNOB=V ...]
py -3.12 verify\rtex_20261016\rowdelta.py <before arm> <after arm>
py -3.12 verify\rtex_20261016\run_gctrl.py <exe> <name> [KNOB=V ...]
py -3.12 re\frida\camera_probe.py --frame --early --rows 1100 --matfx --track 0 --out log\rtex\orig_matfx2_training\trace.csv --static-out log\rtex\orig_matfx2_training\static.json
```
