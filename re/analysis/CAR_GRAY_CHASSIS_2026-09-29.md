# Car chassis renders as grey blocks in the standalone — 2026-09-29

Investigation session. User report: in `mashed_re.exe` the car **chassis renders as grey
blocks**. This note records what was measured, what was refuted, and what is still open.

## Run anchors

| item | value |
|---|---|
| git HEAD | `83a7b6ea529f9a349da11eacdd30d11a4838f0fb` (branch `race/first-frame-parity`) |
| standalone binary | `verify/car_gray_20260929/bin/mashed_re.exe` (copied from `mashedmod/build/`, 1 939 968 B) |
| its SHA-256 | `773A3A492E7FE7F18A3A6BA5C89002AAF8889005EDBEF862B766EC6BC545A9C8` |
| original | `original/MASHED.exe` SHA-256 `B9977DAB70607720CF58BC7832937C203B1E0B33FF5896DA586BAF56AA79FE87` (the ten boot patches are applied; the `.unpatched` backup carries the version anchor) |
| `original/TOASTART/VEHICLES/Advantag.piz` | SHA-256 `6271501C820C4B2D3E0FD26EF3310A739CA39F7E292E8110EB9961D732B88001`, **verified unchanged at end of session** |

Every standalone run in this note used the pinned copy above (`SA_EXE` override on a copy of
`re/tools/sa_capture.py`), `MASHED_MUTE=1`, `MASHED_TITLE="gray-chassis investigation"`.
`mashedmod\build.bat` was NOT run and nothing under `mashedmod/src` was edited — another
session holds the build.

---

# MEASURED

## M1 — The symptom is (b) real mesh, wrong geometry set drawn over it. NOT (a) placeholder boxes.

Original reference (in-race, Training, 4 cars, backbuffer dump through the d3d9 shim
`MASHED_ORIG_BBDUMP_REQ`): `verify/car_gray_20260929/orig_bb_a.png`, crop
`crop_orig_bb_a_car.png`. The car is a smooth red sports car with grey glass.

Standalone, same track and pose: `verify/car_gray_20260929/sahi_train_t16.png`, crop
`crop_sahi_t16_car.png`. Side by side: `sbs_orig_vs_sa.png`. The standalone's car has the
**same silhouette and the same parts** as the original (roof, windscreen, rear wing, tail
lights all land in the same places), so the real DFF clump is loaded and instanced. What is
wrong is that a **coarse, faceted, untextured grey shell plus a ring of dark spikes is drawn
on top of the body**, with the red body z-fighting through it.

No placeholder/box/proxy car code exists anywhere under `mashedmod/src/mashed_re/`
(greps for `placeholder|proxy|box|cube|stand-in|dummy|fallback mesh|DrawBox|D3DFILL_WIREFRAME`
return only menu quads, physics "proxy body", mip box filters and `bbox` variables).

## M2 — The grey shell is four untextured atomics inside the vehicle DFF itself.

`original/TOASTART/VEHICLES/Advantag.piz` → `ADVANTAGE0.DFF` parsed independently in Python
(not through our C++ loader). 71 geometries, 71 materials, 71 atomics:

| materials | count | texture | rgba |
|---|---|---|---|
| `Advantage` | 36 | yes | (255,255,255,255) |
| `AdvantageSmall` | 4 | yes | (255,255,255,255) |
| `Glass` | 4 | yes | (255,255,255,255) |
| **(none)** | **27** | **no** (`textured=0`, no TEXTURE chunk) | **(102,102,102,255)** |

The 27 untextured ones are atomics **7..33**, all with geometry flags **`0x73`**
(`rpGEOMETRYTRISTRIP|POSITIONS|NORMALS|LIGHT|MODULATEMATERIALCOLOR`, no `TEXTURED` 0x04 and no
`TEXTURED2` 0x80). Split by size, after baking each atomic's frame LTM to model space:

* **atomics 13,14,15,16** — 56 / 44 / 60 / 40 triangles, each spanning the whole car
  (`ext ≈ 0.40 × 0.28 × 0.84..0.96` against a car bbox of `0.459 × 0.368 × 1.004`). These are
  the grey shell.
* **atomics 7..12 and 17..33** — 23 atomics of **one triangle each**, 0.1×0.1 quads parked at
  the corners, the roof, the wheel arches and the bumper ends. These are the spikes.

## M3 — Neutralising exactly those atomics in the asset reproduces the original's car.

Control method (build-free, `original/` never touched): the whole `original/` tree was copied
to the scratchpad and the standalone re-pointed at it with `MASHED_ROOT`. Baseline against the
copy is bit-equivalent to the baseline against the real tree (mean abs pixel diff 0.0106 over
1024×768, i.e. run-to-run jitter only): `base_root_t16.png` vs `sahi_train_t16.png`.

In the **copy**, each target atomic's geometry index (a single dword in the atomic STRUCT) was
repointed at geometry 61, a 1-triangle degenerate at the origin. Same-size edit, patched in
place in the `.piz`, no repack.

| run | edit | dominant car body colour | evidence |
|---|---|---|---|
| base | none | **(51,51,51)** grey | `crop_sahi_t16_car.png` |
| A | atomics 13–16 → sink | **(118,26,30)** red, spikes remain | `crop_nohull_t16_car.png` |
| B | A + atomics 7–12, 17–33 → sink | **(118,26,30)** red, **spikes gone** | `crop_nodummy_t16_car.png` |

Run B's car matches the original's crop part for part. The grey and the spikes are fully
accounted for by those 27 atomics.

Colour arithmetic ties it shut: the grey (51,51,51) is exactly the untextured material's
(102,102,102) × 0.5, and the red (118,26,30) is exactly the `Advantage` atlas paint swatch
(236,52,60) × 0.5 (see M5). On the legacy D3D9 path (`MASHED_RENDER_LIBRW=0`,
`d3d9_train_t16.png`) the same two halvings appear, with the yellow livery swatch
(252,212,4) → (126,106,2).

## M4 — It affects every car, and it is track-independent.

All 13 vehicle pizzes parsed for `<NAME>0.DFF`:

```
piz            dff                  atoms notex  1tri hulls texAt
Advantag.piz   ADVANTAGE0.DFF          71    27    23     4    44
Atmos.piz      ATMOS0.DFF              67    28    24     4    39
BamBam.piz     BAMBAM0.DFF             69    23    19     4    46
Bullet.piz     BULLET0.DFF             69    20    16     4    49
Cooler.piz     COOLER0.DFF             66    25    21     4    41
Creeper.piz    CREEPER0.DFF            66    25    21     4    41
Crusader.piz   CRUSADER0.DFF           69    26    22     4    43
Formula.piz    FORMULA0.DFF            70    25    21     4    45
Kustom.piz     KUSTOM0.DFF             69    20    16     4    49
Shorty.piz     SHORTY0.DFF             69    26    22     4    43
Shuriken.piz   SHURIKEN0.DFF           67    28    24     4    39
Sputter.piz    SPUTTER0.DFF            69    26    22     4    43
Stallion.piz   STALLION0.DFF           67    22    18     4    45
```

**Every** vehicle has exactly 4 untextured multi-triangle hulls plus 16–24 one-triangle
locators. Track-independence confirmed on Arctic (`arctic_t22.png`): same grey blocky cars.

## M5 — Separate, second defect: the car renders at exactly half brightness.

Original in-race body pixels (`orig_bb_a.png`, 640×480 R5G6B5 backbuffer): dominant
**(232,52,56)**; the `Advantage` TXD paint swatch decodes to **(236,52,60)**. Within R5G6B5
quantization the original draws the body at **1.0 × texel**.

Standalone with the shell removed (run B): dominant **(118,26,30)** = texel × **0.5**.

`MASHED_DBG_CARLIGHT=1` on Training reports:

```
CARLIGHT rp_on=1 car_relight=1 has_sun=1 amb=(0.500,0.500,0.500) sun=(1.000,1.000,1.000)
L=(-0.734,0.030,-0.678) body_batches: tot=67 lit=64 hasN=64 relit=64 prelit=3
```

`LightAtomicVertex` (`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:232-308` at HEAD)
gives a lit vertex `amb + sun·max(0, N·L)`. Training's light direction has `L.y = 0.030`, so
up-facing body panels receive ≈0.03 of the directional term and land at ≈0.5 — which is the
measured factor. The ambient value and the near-horizontal light are read from the track's
`LIGHTS.DFF` by `ParseLightsDffFaithful` (`TrackRenderer.cpp:1202`), not invented.

Also measured: `MASHED_RPLIGHT=0` (the legacy load-time bake) renders the car **solid black**
`(0,0,0)` — `nodummy_rplight0_t16.png`. That revert path is broken independently.

## M6 — The vehicle DFF also ships a duplicate low-detail set that the standalone draws too.

Pairwise-identical model-space bboxes, same parts, different triangle counts:

| low | tris | high | tris |
|---|---|---|---|
| atomics 34..46 | 830, 6, 10, 522, 380, 28, 84, 86, 190, 685, 288, 18, 98 | atomics 48..60 | 1518, 8, 18, 1028, 746, 52, 166, 144, 374, 1486, 664, 42, 214 |
| atomics 62..65 (wheels) | 96 each | atomics 66..69 (wheels) | 352 each |

Plus atomics 3,4,5,6: four whole-car single-atomic meshes (624/618/651/636 tris) sharing one
bbox. The standalone renders all of them. Visual impact was not isolated in this session, but
it means the wheel heuristic at `TrackRenderer.cpp:2234-2281`, which requires **exactly four**
disc-shaped candidates, is choosing among **eight** coincident wheel atomics.

## M7 — What the original does instead (cited).

The original never renders the vehicle clump wholesale. Chain, read from a read-only Ghidra
pool slot via `re/tools/decomp_pc.py`:

* `FUN_0040d110` **`0x0040d110`** — post-load per-player track/vehicle assignment. Builds the
  piz path via `FUN_00420230` (`0x00420230`, `"toastart/vehicles/" + name + suffix`), mounts it,
  then calls `FUN_00420420(slot, vehicleIndex)` per player.
* `FUN_00420420` **`0x00420420`** — per-player vehicle init. Loads the clump
  (`FUN_0042a5d0` `0x0042a5d0`), then:

  ```c
  uVar3 = FUN_004b3fc0(uVar3, aiStack_204);          // 0x004b3fc0: enumerate atomics -> count
  ...
  iVar2 = aiStack_204[iStack_25c];                   // atomic
  iVar1 = *(int *)(iVar2 + 0x18);                    // its geometry/frame ref
  iVar5 = FUN_004b5190(iVar2, 0, 0);                 // 0x004b5190: atomic -> PART CODE
  *(int *)(puVar8 + iVar5 * 4) = iVar2;              // scatter into the player block by part code
  ```

  `puVar8 = &DAT_0063d9e0 + player*0x2AC`. The per-player 0x2AC block **is** an atomic-pointer
  table indexed by part code; parts the game does not look up are simply never drawn.
* `FUN_004b3fc0` **`0x004b3fc0`** wraps `RpClumpForAllAtomics` **`0x004e66d0`**.
* Per-part special-casing in the same loop, all keyed on the part code, not on DFF order:
  * codes `0x3b..0x3e`: `FUN_004b52c0(geom,0x20,0)` clears `rpGEOMETRYLIGHT`,
    `FUN_004b52c0(geom,0x40,1)` sets `MODULATEMATERIALCOLOR`, and
    `*(*(geom+0x20)+4) = 0xff000000` forces material 0 to opaque black;
  * codes `0x11`, `0x15`, `0x18`: same two flag writes with `0xffffffff` (white);
  * code `0x12`: `FUN_005449f0` / `FUN_00544a70` / `FUN_00544ad0`;
  * codes `0x3b..0x3e` and `0x40`: `FUN_0053d090` **`0x0053d090`** / `FUN_0053d400`
    **`0x0053d400`** — the **BVH build** path, i.e. those parts are collision geometry.
* `FUN_0041fe10` **`0x0041fe10`** (called from the same function) clones the clump, keeps only
  part codes `{4,6,8,9,0xa,0xc}`, destroys the rest, and derives the 6-float AABB at
  `+0x230..+0x244` — a collision-side filter, not the render set.

---

# REFUTED

* **R1 — "the car is a placeholder box"**. Refuted by M1: the real clump is loaded, instanced
  and recognisable; and no box/proxy car code exists in the tree.
* **R2 — "a default-OFF scaffold flag is drawing boxes"**. Refuted: the symptom is identical
  with `MASHED_RENDER_LIBRW=0` (`d3d9_train_t16.png`) and with the librw default
  (`sahi_train_t16.png`), and it survives every combination tried. It comes from the shared
  DFF→batch stage, not from a renderer-selection flag.
* **R3 — "the vehicle TXD fails to decode / the textures are not found"**. Refuted: the
  `Advantage` 512×512 PAL8 atlas decodes correctly (`txd_Advantage.png`, swatch grid
  `crop_txd_swatches.png`), and `log/librw_scene.txt` shows the car clump with
  `mats=71 named=44 resolved=44` — every named texture resolves.
* **R4 — "our DFF parser loses 27 texture names"**. Refuted by the independent Python parse in
  M2: those 27 materials genuinely carry `textured=0` and no TEXTURE chunk in the file. The
  in-source comment at `TrackRenderer.cpp:538-541` ("27 colour-only, by design") is correct
  about the data; what is wrong is that we draw them.
* **R5 — "the grey is the wrong UV set on the two-UV-set (`0x200b3`) body atomics"**.
  Refuted: `crop_nohull_t16_car.png` shows those very atomics rendering the correct red paint
  once the shell is removed.
* **R6 — "the livery/colour selection is broken"**. Refuted: `ADVANTAGE0..5.DFF` differ only in
  **UV coordinates** (float pairs), pointing at different cells of the atlas paint-swatch grid;
  the D3D9 A/B shows four AI cars picking up four distinct swatch colours (yellow, blue, green,
  red) correctly.
* **R7 — "this is D-S3-5, the known librw car-relight gap"**. Not this. D-S3-5 is a shading
  delta on the librw path only; M2/M3 is geometry, present on both paths.

---

# ROOT CAUSE

**The standalone renders every atomic of the vehicle DFF. The original renders only the
atomics whose part code it looks up in the per-player table at `DAT_0063d9e0 + player*0x2AC`
(`FUN_00420420` @ `0x00420420`).** Twenty-seven of the ~67–71 atomics in every vehicle DFF are
not body geometry: four are coarse untextured car-sized hulls (material colour
`(102,102,102)`, geometry flags `0x73`) and 16–24 are one-triangle locators. Drawing them
covers the painted body with a grey faceted shell and rings it with dark spikes — the reported
"grey blocks".

Entry point in our code: `TrackRenderer::LoadCar`
(`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2205`) hands the **whole** parsed
`Track::DffModel` to `BuildDffBatches` (`:2286`) and to
`LibRw::RaceSubmit_RegisterModel` (`:2289`) with no atomic filter; `LoadCarLiveries`
(`:2573`, parse at `:2620`, build at `:2622`) does the same for the AI liveries. The only
atomic-level selection anywhere on the car path is the 4-wheel bbox heuristic at `:2234-2281`.

---

# READY-TO-APPLY FIX

**File**: `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`
**Functions**: `TrackRenderer::LoadCar` (`:2205`) and `TrackRenderer::LoadCarLiveries` (`:2573`)
**Change**: immediately after the `model.Parse(...)` call in each — `:2231` in `LoadCar`,
`:2620` in `LoadCarLiveries`, i.e. **before** the wheel heuristic, before `BuildDffBatches`
and before `RaceSubmit_RegisterModel` — drop every batch whose geometry declares no texture
coordinates:

```cpp
    // The vehicle DFF ships non-render atomics alongside the body: four coarse
    // untextured car-sized hulls (collision; the original builds a BVH for them at
    // FUN_00420420 -> FUN_0053d400 @ 0x0053d400) and 16-24 one-triangle locators.
    // Every one of them has geometry flags 0x73 -- no rpGEOMETRYTEXTURED (0x04) and
    // no rpGEOMETRYTEXTURED2 (0x80), material rgba (102,102,102). The original never
    // renders them: FUN_00420420 @ 0x00420420 scatters each atomic into the
    // per-player table at DAT_0063d9e0 + player*0x2AC by the part code from
    // FUN_004b5190 @ 0x004b5190, and only looked-up parts are drawn.
    // Measured 2026-09-29 on all 13 vehicle DFFs: this predicate selects exactly
    // that set and nothing else.  re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md
    model.batches.erase(
        std::remove_if(model.batches.begin(), model.batches.end(),
                       [](const Track::DffBatch& b) {
                           return (b.geo_flags & 0x84u) == 0;   // TEXTURED | TEXTURED2
                       }),
        model.batches.end());
```

`DffBatch::geo_flags` already carries the raw RW format dword
(`mashedmod/src/mashed_re/Track/DffModel.h:51`, set at `Track/DffModel.cpp:189` and copied to
the batch at `Track/DffModel.cpp:350`), so no loader change is needed. `<algorithm>` is
already included (`TrackRenderer.cpp:5`). An equivalent predicate is
`((b.geo_flags >> 16) & 0xFF) != 0` (numTexCoordSets), which matches on the same 13/13 assets.

Erase from `model.batches` only — **do not** recompute `model.bbox`. The locator atomics
extend the model bbox (atomics 21–24 reach `y = -0.038` and `|x| = 0.228` against the body's
`y = 0.0`, `|x| = 0.219`), and `model.bbox` feeds `car_ground_off_`, `car_len_` and
`car_height_` at `TrackRenderer.cpp:2415-2422`, i.e. the chase-camera rig and the ground
offset. Leaving it untouched keeps this change purely visual.

**Scope it to the car path only.** Do NOT put this in `BuildDffBatches` or in `BuildWorld` —
the track BSP legitimately uses untextured colour-only materials for its collision/surface
materials (`re/analysis/formats/track_world_bsp.md:92`), and props were not measured here.

**Expected effect**: the car body renders in its livery paint instead of flat grey; the corner
spikes disappear; the wheel heuristic at `:2234` sees a cleaner candidate set. Verified
equivalently at the asset level (M3, runs A and B) — `crop_nodummy_t16_car.png` is what the
fix should produce, modulo the separate half-brightness of M5.

**NOT verified in-binary.** This session could not build (another session holds the build).
Acceptance for this fix is a rebuild plus a parity capture against
`verify/car_gray_20260929/orig_bb_a.png`.

---

# OPEN

* **O1 [UNCERTAIN] — the original's part code is not derivable from the DFF.**
  `FUN_004b5190` @ `0x004b5190` reads `*(atomic+0x18)` and walks a plugin block at
  `frame + DAT_007dc634`. Every frame- and atomic-extension chunk in `ADVANTAGE0.DFF` is
  **zero length** (measured: 83 frame extensions, all size 0; 71 atomic extensions, 18 of
  which carry only a 4-byte `0x120` = `rwID_MATERIALEFFECTSPLUGIN` payload of `1`). So the
  part code is produced at runtime by an unidentified mechanism — this is the same hole as
  the OPEN row **U-9079** in `UNCERTAINTIES.md:77`. Consequence: a *verbatim* port of the
  original's atomic selection is blocked on U-9079; the fix above is a measured data-driven
  equivalent, not a verbatim port, and should be recorded as such.
  *Next command*: `py -3.12 re/tools/decomp_pc.py 0x00543d40 0x00543d70 0x0055deb0 0x00543df0 --datarefs`
  (the four-call chain inside `FUN_004b5190`) and resolve `DAT_007dc634`.
* **O2 [UNCERTAIN] — why the car is exactly half bright (M5).**
  The arithmetic `amb 0.5 + sun·N·L` with Training's near-horizontal `L.y = 0.030` reproduces
  the 0.5 exactly, but it is not established that the original lights the body at all: its
  in-race body pixels equal the raw texel. It is possible the original clears
  `rpGEOMETRYLIGHT` on the body parts the way `FUN_00420420` demonstrably does for part codes
  `0x3b..0x3e` / `0x11` / `0x15` / `0x18`.
  *Next command*: `py -3.12 re/tools/decomp_pc.py 0x004b5190 0x0041f360 --callers` then decode
  `LAB_004b5300` / `LAB_004b5560` (the two vehicle per-atomic callbacks, undecoded per the
  tracker sweep) to see which part codes get their `LIGHT` bit cleared.
* **O3 — the duplicate low/high detail sets (M6) are still both drawn.** Not isolated
  visually. Whether the original selects by distance or simply never looks up the low set's
  part codes is unknown.
* **O4 — `MASHED_RPLIGHT=0` renders the car solid black.** Measured
  (`nodummy_rplight0_t16.png`), not investigated. The legacy A/B revert is unusable for cars.
* **O5 — props were not checked** for the same over-draw. The track clumps in
  `log/librw_scene.txt` are single-material and did not show the pattern, but this was not
  swept.

---

# Evidence index (`verify/car_gray_20260929/`)

Committed crops:

| file | what |
|---|---|
| `crop_orig_bb_a_car.png` | ORIGINAL, in-race Training, player car |
| `crop_sahi_t16_car.png` | STANDALONE, same track/pose — the reported symptom |
| `sbs_orig_vs_sa.png` | the two side by side |
| `crop_nohull_t16_car.png` | run A — atomics 13–16 neutralised |
| `crop_nodummy_t16_car.png` | run B — atomics 7–33 neutralised |
| `crop_txd_swatches.png` | the paint-swatch grid in `ADVANTAGE.TXD` |

Full frames kept locally but not committed (1 MB each): `sahi_train_t*.png`,
`d3d9_train_t*.png`, `nohull_t16.png`, `nodummy_t16.png`, `nodummy_rplight0_t16.png`,
`base_root_t16.png`, `arctic_t*.png`, `orig_bb_{a,b,c,d}.png`.

Reproduction of the original-side reference:

```
$env:__COMPAT_LAYER="RunAsInvoker WIN98RTM HighDpiAware EmulateHeap"
$env:MASHED_ORIG_BBDUMP_REQ="<abs path to a request file>"
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --hold 45
# then write an ABSOLUTE .bmp path into the request file while phase==3
```

`PrintWindow` on the original returns all-white on this machine (`orig_race_t32.png` is the
white frame that proves it) — the backbuffer dump is the only original-side pixel channel.
