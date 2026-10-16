# PREREG rtex: two-stage world textures and the car material passes (U-9692, U-9693)

Committed before the first measuring run. Base: `race/first-frame-parity` `ec3301f3` (d1r1 merged).
Branch `work/rtex`. Instrument: d1r1's matched-pose per-draw recorder and `re/tools/drawlist_diff.py`
3D mode (`verify/d1r1_20261016/RESULT_d1r1.md` section 1 and 7), on d1r1's 9 frames: TRAINING
884/924/959, Arctic 1051/1091/1126, City 761/801/836. The original side is d1r1's committed records
(`verify/d1r1_20261016/evidence/*_orig_row_*.draws.json`) and traces (`log/d1r1/orig_*/trace.csv`);
they are reused unchanged, so every row delta is a standalone-side change.

## 1. What the original does (static, read before measuring)

All RVAs in `MASHED.exe` (anchor in CLAUDE.md). Decomp via `re/tools/decomp_pc.py`, disassembly via
`re/tools/disasm_fn.py`; extracts in `evidence/`.

**A. MatFX dispatch.** `FUN_00543500` is the MatFX D3D9 atomic render callback. Per instanced mesh
(resEntry `+0x58`, stride `0x24`) it reads the material's MatFX data `*(material + DAT_007dc5dc)`,
effect type at `+0x30`, and calls 2 ENVMAP `FUN_005412d0`, 3 BUMPENV `FUN_00541d40`, 4 DUAL /
6 DUALUVTRANSFORM `FUN_005405c0`, else `FUN_00540340`.

**B. DUAL, SRCALPHA/INVSRCALPHA, base texture present, `DAT_007dc624 >= 3`** (`0x005408cf`..
`0x005409b9`, disassembly): `SetTexture(dual, 1)` `0x005408f9`; stage 0 COLOROP SELECTARG1 / ARG1
TEXTURE / ALPHAOP SELECTARG1 / ALPHAARG1 TEXTURE (`0x00540904`..`0x00540925`); stage 1 COLOROP 13
BLENDTEXTUREALPHA / ARG1 TEXTURE / ARG2 CURRENT, ALPHAOP 13 / ARG1 TEXTURE / ARG2 CURRENT
(`0x00540930`..`0x0054096a`); stage 2 COLOROP MODULATE / ARG1 DIFFUSE / ARG2 CURRENT, ALPHAOP
MODULATE / ARG1 DIFFUSE / ARG2 CURRENT (`0x00540975`..`0x005409af`). Stage-1 TEXCOORDINDEX is set to
0 at `0x00540c56` only when geometry flag `0x80` (TEXTURED2) is clear. Draw `0x00540d44`. Reset
`0x005411f3`..`0x0054127a`. Base texture absent (`0x005409be`..`0x00540a47`): TFACTOR `0xffffffff`,
dual on stage 0 with BLENDTEXTUREALPHA(TEXTURE, TFACTOR), ALPHAOP SELECTARG2 (TFACTOR), stage 1
MODULATE(DIFFUSE, CURRENT) colour and alpha. A stage-1 texture never changes blending:
`FUN_004d6ce0` skips the alpha rule for stage != 0 (`test ecx,ecx; jne` at `0x004d6d00`).
So per pixel: `C = Diffuse * lerp(T0, T1, T1.a)` on rgb and on alpha.

**C. Assets.** TRAINING `ROAD.DFF`: every geometry `0x2008b` (two UV sets), materials CliffEdge /
Dirt / RoadTarmac / Wall carry MatFX DUAL src 5 dst 6 with `newShadow` / `newShadow_Small`.
TRAINING `GRAPH.BSP` format `0x400200c9` (two UV sets), material 0 `CliffEdge_Small` DUAL
`newShadow_Small`. Arctic / City BSP: no MatFX. Vehicle DFFs: 18 geometries `0x200b3` with DUAL
`CarDirt` / `WheelDirt`, which are not in the vehicle TXD (`verify/rtex_20261016/rwmat_dump.py`).

**D. Car dirt (dual) textures.** `FUN_0040cea0` mounts `toastart/vehicles/dirt.piz`, reads
`FUN_00404e00(track)` (table `0x005ea0b8`: DirtBody / EarthBody / GrimeBody / SandBody, index
`DAT_00639cc8[track]`) and `FUN_00404e20(track, veh)` (table `0x005ea0c8`, index
`DAT_00636c08[track*0x4e+veh]`) through `FUN_004c5cb0`, stores them at `DAT_0063ba90/94`, and calls
`FUN_0041ecc0(car, body, wheel)` for cars 0..3. `FUN_0041ecc0`: for each part `0..0x7f` of the car
record (`DAT_0063d9e0 + car*0x2ac + part*4`), for each material with effect 4,
`FUN_005401d0` (set dual texture): parts 0..3 and 0x21..0x24 get the wheel texture, the rest the body
texture; when `DAT_00636ad4 != 0` it calls `FUN_0053fbb0(mat, 0)` (effects off) instead. dirt.piz
holds 128x128 32-bit PNGs.

**E. Car env (gloss).** `FUN_00420420` (per-car init): env texture = TXD lookup of the vehicle
table entry `+0x30` (`cargloss`, table at `0x005f37a8`, stride `0x84`), env frame
`DAT_0063d840[player]`, coefficient from `FUN_00420290`; for parts with `FUN_0041f360(part) != 0`
(4 5 8 10 11 14 15 0x25 0x26 0x29 0x2b 0x2c 0x2f 0x30, when `DAT_00636ad0 == 0`) it enables MatFX on
the atomic and sets every material to ENVMAP (`FUN_0053fbb0(mat, 2)`, `FUN_0053fd30`). Per frame
`FUN_00420870` -> `FUN_00420340` re-applies coefficient / frame / texture. Render `FUN_005412d0`:
coefficient byte `round(coef * 255.0)` (`[0x005cd04c]` = 255.0); single pass when the base is opaque
(stage 1 MULTIPLYADD CURRENT + TEXTURE * TFACTOR, or ADD at byte 255); otherwise the base draws
plain and a second pass draws the env texture on stage 0 with SRCBLEND 5 / DESTBLEND 2, ZWRITE 0,
LIGHTING 0, FOGCOLOR 0. Env texcoords `FUN_00541b50`: TCI CAMERASPACENORMAL, COUNT2, matrix
`(camera LTM x inverse(env frame LTM))`, right row negated, translation zeroed, times
`[0x005e4848]` = `{0.5,0,0,0, 0,-0.5,0,0, 0,0,1,0, 0.5,0.5,0,0}`.

**F. Part codes.** `FUN_004b5190(atomic)` reads the GEOMETRY (`atomic+0x18`) user-data array 0,
element 0. Every vehicle geometry carries `0.tv_part_id` (int) in its rpUSERDATA extension
(chunk 0x11F; `verify/rtex_20261016/geo_userdata.py`). U-9079 said the slot was not in the file
because frame / atomic extensions are empty; the data is on the geometry.

**G. Arctic WetRoad.** `FUN_0049a750` is a game render callback for one Arctic sector: per mesh,
textures named `W?t` / `Ic?` (`bl = 0x65` at `0x0049a8c8`) get a vertex shader `[0x00773974]`
and pixel shader `[0x0077397c]` (created at `0x0049aae0` from bytecode `0x005d0438` / `0x005d0518`),
stage-1 texture `[0x00773978]` (setter `0x0049a740`), TTFF PROJECTED, CLAMP.

## 2. Fixes (each with an env knob; default decided by section 4)

- **F-DUAL** (U-9692, TRAINING): parse MatFX DUAL and the second UV set in `Track/TrackWorld.cpp`
  and `Track/DffModel.cpp`; set `rw::MatFX` DUAL + texture + blend on the material, TEXTURED2 + UV
  set 1 on the geometry, MatFX on the atomic (`LibRw/RwSceneBuild.cpp`); implement section 1 B in
  librw's D3D9 MatFX callback (`deps/librw/src/d3d/d3d9matfx.cpp`, new VS/PS, MASHED LOCAL PATCH).
  Knob `MASHED_LIBRW_DUAL=0` disables.
- **F-DIRT** (U-9693 dual half): load dirt.piz per section 1 D, bind on car dual materials by part
  code, as an unnamed 128x128 A4R4G4B4 raster. Knob `MASHED_CAR_DIRT=0`.
- **F-ENV** (U-9693 env half): section 1 E (env on the listed parts, single pass / two pass).
  Knob `MASHED_CAR_ENV=0`.
- **F-WET** (U-9692 Arctic half): section 1 G. May be deferred if the shaders' inputs cannot be
  read in this session; then it is filed, not guessed.

## 3. Rules (per fix, on all 9 frames, default build vs the same build with the knob off)

1. **Targets.** F-DUAL: the TEX1 rows CliffEdge, CliffEdge_Small, Dirt, RoadTarmac, Wall
   (C16-TEX1-WORLD) disappear on the 3 TRAINING frames. F-DIRT: the TEX1 `Advantage ... A #128x128:26`
   rows disappear. F-ENV: the TEX1 `Advantage ... A CarGloss` rows and the MISSING `CarGloss` rows
   disappear. F-WET: the TEX1 `WetRoad` rows disappear.
2. **No new rows.** No (kind, tex, detail-class) row appears that the knob-off arm does not have,
   on any of the 9 frames (`rowdelta`), and every remaining row stays cited (0 RED).
3. **Matched draws do not drop.** Per frame, matched draws and matched triangles do not fall.
4. **No-op control.** Knob off reproduces the base build's rows on all 9 frames.
5. **G-CTRL** (`run_gctrl.py`, TRAINING default arm) gives the base trio before and after. Expected
   `c2e4d60d / b50bd39b / ba90ed88` (d1r1 rebased default).
6. **Pixels (report).** Mean abs RGB difference vs the original BMP per frame, both arms. A rise
   above 0.5 on any frame blocks default ON (recommendation instead).

A fix passing 1-5 ships default ON with its opt-out knob. Failing any keeps it default OFF.

## 4. Predictions

- Pr-1: F-DUAL leaves the TRAINING STATE rows' render-state tuples unchanged (stage-1 alpha does
  not touch blend, section 1 B).
- Pr-2: F-DUAL changes nothing on Arctic and City (no MatFX in their world or props; cars keep a
  null dual texture until F-DIRT).
- Pr-3: F-ENV's second pass adds one draw after each env-mapped draw with a blended base (Glass),
  with SRC 5 / DST 2 / ZWRITE 0, matching the MISSING row's state key.
- Pr-4: the car's env and dirt parts by part code reproduce the original's per-frame counts:
  28 dual-dirt + 12 env + 8 gloss-pass draws per frame per matched car set on TRAINING 884.

## 5. Amendments

Live values the formulas need but static reads cannot give (coefficient inputs `[0x0064435c]`,
`[0x00644360]`, car record `+0x284` / `+0x29c`, `DAT_00636ad0/ad4`, dirt indices) are read from
the original with an added `camera_probe.py --matfx` reader and recorded here as an amendment
before they are used.

### Amendment 1 (after F-DUAL was measured, before any F-DIRT / F-ENV run)

F-DUAL was measured first (rules 1-5 pass on 9/9 frames, `RESULT_rtex.md`). Then, with
`camera_probe.py --matfx` (every 100 trace rows; `log/rtex/orig_matfx2_{training,arctic,city}`,
summarised by `matfx_report.py` / `envframe_check.py`):

- `DAT_00636ad0` = `DAT_00636ad4` = 0 on all three tracks (env and dirt on).
- Dirt textures: TRAINING `DirtBody` / `DirtAdWheel`, Arctic and City `GrimeBody` /
  `GrimeAdWheel`; each 128x128, depth 16, cFormat 0x3 (4444, no mip bit), filterAddressing
  `0x1101`. The tables generated from `FUN_00404830`'s decompilation (`gen_dirt_tables.py`)
  predicted the Arctic / City names before the capture was read.
- Env coefficient range = COURSE.LUA `Vehicle_Shininess_Range` (TRAINING 0.7/1, Arctic
  0.6/1.0, City 0.5/1.0), read live as `[0x0064435c]` / `[0x00644360]`; binding `0x0047ac80`
  stores arg 1 / arg 2 at course `+0x204` / `+0x208`, defaults 0 / 1.0 at `0x0047a060` /
  `0x0047a066`.
- `FUN_00420290`'s formula with the live inputs reproduces the live coefficient to about 1e-4 on
  stationary rows; the `rec+0x29c` object's frame position equals the trace camera position on
  race rows; the env frame is a pure rotation of `rec+0x280` degrees with no parent.
- Ported with stated gaps ([UNCERTAIN], pixel-level only): the shade sample `FUN_00479100` on the
  collision soup (barycentrics replace `FUN_00559040`; flat soup instead of the BSP walk), the race
  frame counter `DAT_007f101c` (incremented once per rendered race frame; the tick site
  `0x004111c1`'s frequency is not read), the velocity units feeding the env-frame angle.

### Amendment 2 (scoring of draws that newly match, before any F-DIRT / F-ENV run)

F-ENV turns MISSING `CarGloss` draws into matched draws. librw's render-state defaults (cited
C19: ALPHAREF 10, ALPHAFUNC 7, CULL NONE) then show as a STATE row on them, as they already do on
every other matched car draw. Rule 2 therefore reads: a NEW STATE row whose differing fields are a
subset of {AREF, AF, CULL}, on a texture whose MISSING row disappeared in the same frame, is the
replacement of that MISSING row (cited C19), not a failure. Any other NEW row fails rule 2.
Arms: `carfx` (default), `carfx_nodirt` (`MASHED_CAR_DIRT=0`), `carfx_noenv`
(`MASHED_CAR_ENV=0`), `carfx_off` (both 0; rule 4 control: equals the F-DUAL arm's rows).
