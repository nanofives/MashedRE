# HB-FOREST-WATER RESULT (2026-10-27): Forest water is a texture flip, not a lighting or blend defect

**Verdict: GREEN.** The original never draws Forest's water with the texture `WATER.DFF` ships.
Forest's hook record replaces it every frame with an animated ring `water00`..`water31` from the
course TXD. The port drew `wet_watercol00` (luma 132.6) where the original draws `waterNN`
(luma about 20.6). Ported behind `MASHED_TEXANIM=1` (default OFF): on the forest_s8 water mask the
knob takes the port from 48.1 to 24.5 against the original's 21.5 (|d| 26.6 to 2.9). All PREREG
rules R0..R6 hold. Default gates unchanged.

- PREREG: [`PREREG.md`](PREREG.md), committed alone as `4b4954dd9` before any arm. Not amended.
- Code: `71d4d4dcc` on `orch/HB-FOREST-WATER` (base `orch/integration` `6368c086a`, which contains
  HB-ARCTIC-FOLDRM `954d3477e`). This RESULT commit is the branch head.
- Exe `mashedmod/build/mashed_re.exe` SHA-256 `689a201f0052af9dc7f38a4cf1673d22c512314c77b9b68ef3e9e43ddf9dc26d`.

## 1. Mechanism (static, from the bytes)

Hook table `0x005f33f8`, record "forest", Course_Id 34: slot 0 `0x0044dea0`, slot 1 `0x0044ded0`,
slot 2 `0x0044df00`, slot 5 `0x0044df30`.

| step | site | what the bytes do |
|---|---|---|
| ring build | slot 0, `0x0044deb1`..`0x0044dec5` | `push [esi+0x105d0]` (course TXD), `push 0x5ce2d0` ("water"), `call 0x475090`, `mov [0x895948],eax` |
| name list | `FUN_00475090` | `"%s%.2d"` (`0x005cead8`) for index 0, 1, ... looked up with `FUN_004c5c00` (case-insensitive name walk of the dictionary) until the first miss. Ring is circular (last node -> first). Forest `TEXTURES.TXD` holds `Water00`, `Water01`, `water02`..`water31`: 32 |
| tick | slot 1 -> `FUN_004751d0(DAT_00895948, units)` | `add edx,ecx` (`0x004751db`), `cmp ecx,0x12c` / `jl` (`0x004751df`): at 300 or more, advance one node and zero the accumulator (`0x004751f4`) |
| apply | slot 1 -> `FUN_00475060(*(course+0x10110), tex)` | `FUN_00474fb0` -> `RpClumpForAllAtomics(.., 0x00474f90)` -> `RpGeometryForAllMaterials` (`0x004e89a0`, on atomic+0x18) -> `0x00475040`: non-NULL texture -> `RpMaterialSetTexture` (`0x004e8090`) |
| units | `FUN_00426ab0` | its `param_2` goes to slot 1 (`0x0041e8c0`) and to `FUN_00426700` alike, the value the port's `CopterTick(units)` already receives |
| draw | slot 2 `0x0044df00` | `RpClumpRender(*(course+0x10110))` (`0x004e6680`), then `FUN_0044dce0` (object world) |

Same mechanism on three more records (ring store / clump / tick multiplier):

| track | Course_Id | store | clump | tick |
|---|---|---|---|---|
| Forest | 34 | `0x0044dec5` -> `DAT_00895948` | `+0x10110` (0) | `units` (`0x0044ded0`) |
| Sands | 11 | `0x00450cb9` -> `DAT_0088ffd8` | `+0x10114` (1) | `units * 2` (`0x004507b0`) |
| Training | 30 | `0x004512ad` -> `DAT_0088ff90` | `+0x10134` (9) | `units` (`0x004513b0`) |
| Warzone | 33 | `0x00452a9e` -> `DAT_00684c50` | `+0x10110` (0) | `units * 2` (`0x00452b20`) |

Warzone's store sits inside slot 0 `0x004527f0`: linear disassembly from `0x004527f0` reaches
`0x00452a8a` and `0x00452a9e` before its first `ret` at `0x00452ac9` (Ghidra mis-disassembled
this body as `FUN_00452a8b`).

Asset numbers (mean RGB, luma):

| item | RGB | luma |
|---|---|---|
| `wet_watercol00` (WATER.DFF material, alpha 2..246) | 126.1 / 135.1 / 137.1 | 132.6 |
| `water00`..`water31` (PAL4, alpha 255) | about 16 / 23 / 19 | 19.9 .. 21.4 |
| WATER.DFF prelit | 91.6 / 88.0 / 83.8 | 88.6 |
| WATER.DFF material colour | 255 / 255 / 255 / 209 | |

The old lead ("MODULATEMATERIALCOLOR + alpha 209") is not the cause: the material colour is white,
and both sides keep the same alpha 209. The texture is what differs.

## 2. Change

- `LibRw/RwRaceSubmit.{h,cpp}`: `RaceSubmit_TexAnimCreate` / `_TexAnimTick` / `_TexAnimApply`, one
  ring per course (the original keeps one global per track), cleared on track load and shutdown.
- `LibRw/RwSceneBuild.{h,cpp}`: `ResolveTextureByName` (the existing case-insensitive lookup).
- `D3d9Render/TrackRenderer.{h,cpp}`: table `kTexAnim` (the four rows above), ring built after the
  clump props load (on every prop with that `clump_idx`), tick + apply in `CopterTick` next to
  Warzone's `Pontoon_Tick`. Knob `MASHED_TEXANIM=1`, default OFF. librw path only.

## 3. Arms and score (PREREG sections 3-5)

Pose forest_s8 (the only committed original Forest bitmap), `run_render.py`, private root
`D:/MashedWT/HB-FOREST-WATER_root`. W1 = HB-ARCTIC's mask (5.23%), W2 = this knob's footprint.
Full table: [`measure_table.txt`](measure_table.txt).

| mask | % frame | original L | OFF L | ANIM L | dOFF | dANIM | original RGB | OFF RGB | ANIM RGB | grad orig / OFF / ANIM |
|---|---|---|---|---|---|---|---|---|---|---|
| W1 | 5.23 | 21.5 | 48.1 | 24.5 | 26.6 | **2.9** | 17.5/24.3/17.7 | 46.1/49.8/45.1 | 22.1/26.1/22.4 | 2.22 / 3.89 / 2.05 |
| W2 | 5.17 | 21.6 | 48.4 | 24.5 | 26.9 | 2.9 | 17.6/24.3/17.8 | 46.4/50.1/45.4 | 22.1/26.1/22.4 | 2.23 / 3.91 / 2.04 |

| rule | measured | verdict |
|---|---|---|
| R0 OFF vs OFF2 bit-identical | 0.000% differ (same SHA-256). This OFF is also bit-identical to HB-ARCTIC's forest_s8_OFF | PASS |
| R1 W1 dANIM <= 8 and < dOFF | 2.9 vs 26.6 | PASS |
| R2 W1 per-channel within 8 | 4.6 / 1.8 / 4.7 | PASS |
| R3 W1 gradient ratio in [0.5, 2] | 0.92 (OFF was 1.75) | PASS |
| R4 ANIM != OFF outside W1 dilated 3 px | 0.000% of frame | PASS |
| R5 imgdiff global mean, orig vs ANIM <= orig vs OFF | 9.59 vs 10.80 (pixels over 16: 13.31% vs 18.05%) | PASS |
| R6 default gates | identical, section 4 | PASS |

imgdiff 8x6 grid: the six water cells (row 3 col 6-8, row 4 col 7-8, row 5 col 8) go from
6.8 / 18.3 / 8.3 / 9.8 / 29.4 / 6.6 to 3.5 / 3.8 / 3.2 / 3.3 / 4.2 / 3.2. Every other cell is unchanged. The
remaining large cells (top row 36.6 / 47.2, rows 4 and 6 at 20-38) are the cars and the start
lights, which this pose does not pin, and are not this step.

Smoke (not scored): `sands_ANIM`, `training_ANIM`, `warzone_ANIM` each logged
`TEXANIM ... textures=32 models=1` (mult 2 / 1 / 2) and self-exited rc 0 with a grid capture.
No original water reference exists for those three, so their water is wired, not verified.

Residual 2.9 luma (ANIM brighter by 4.6 R / 4.7 B): not chased. The capture pose does not pin
which ring texture is current (ring phase depends on the units clock before the grid frame, and
slot 1's gate `FUN_0041e9e0` at `0x00426ac3`..`0x00426ae3` is not ported), and the 32 textures
differ by up to 1.5 luma among themselves.

## 4. Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on exe `689a201f`,
knob OFF, through test-slot and `mashed_lock.py`:

| gate | this exe | `merge_train.json` ref | |
|---|---|---|---|
| G-CTRL (step / gates / ptrace) | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | identical |
| B (K0 / K1 / K26) | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | identical |

The first gate attempt stopped on a missing output folder when copying its H_DEF results (its exit
status was not printed); the rerun above is the scored one. The other 10 game processes in this
step (6 render, 4 gate) self-exited rc 0. This step killed no process.

## 5. Ledger-ready summary

HB-FOREST-WATER GREEN: Forest water is a hook-table texture flip (slot 0 `0x0044dea0` builds
`water00..31` via `FUN_00475090`, slot 1 `0x0044ded0` ticks `FUN_004751d0` every 300 units and sets
it on clump 0 via `FUN_00475060`), not lighting or MODULATE. Ported behind `MASHED_TEXANIM=1`
(default OFF) for Forest, Sands, Training, Warzone. forest_s8 water 48.1 -> 24.5 vs original 21.5,
R0..R6 pass, gates unchanged. Follow-ups: decision row for the default flip, original water
references for Sands / Training / Warzone, the D3D9 path has no equivalent.

## 6. Follow-ups

- **Decision:** flip `MASHED_TEXANIM` default ON (recommend yes: Forest scored GREEN, the
  mechanism is one decoded function pair shared by the four tracks, and gates are render-blind).
- **HB-TEXANIM-REFS:** capture original Sands / Training / Warzone poses with water in view and
  score them like section 3.
- U-9064's Forest half (the "Forest water over-bright even unfolded" note in
  `verify/water_refs/U9064_MECHANISM.md`) is answered here; close it through `re-classify`.

## 7. Raw data (not committed), SHA-256

Root `D:/MashedWT/HB-FOREST-WATER_data/`.

```
22a09eef6389b1bea547c5b78f9131077e1feb5b18f305239f8ac005c7202bc5 *render/forest_s8_ANIM/race1/01_grid.bmp
a215595dbf3c22e2152e5a6f35c1f5c21c85ad5d87021b164cc2eb930769dac5 *render/forest_s8_OFF/race1/01_grid.bmp
a215595dbf3c22e2152e5a6f35c1f5c21c85ad5d87021b164cc2eb930769dac5 *render/forest_s8_OFF2/race1/01_grid.bmp
592a31525c117e53a5bd561e305c8912bfd8d7643acf55fdb093083b7cc45420 *render/sands_ANIM/race1/01_grid.bmp
0d3ab868a86fc7b8bc84881e4a06b4c3f9a8b44e3c97966dbbb004772302781b *render/training_ANIM/race1/01_grid.bmp
c186eb17308e10f49027616382941748fd3f905709388605d5b0caec5f56bc35 *render/warzone_ANIM/race1/01_grid.bmp
18480ada87f1da822f0181a0ea84cf80df59309788fa0b044087ab3e87616f0e *forest_s8_cmp.png
f0d7a68b8487096b76c83595b699bc47ae245374d4059e3a8753e93d3b6d3c7e *render.log
c0ef5ffe3955c8ab91799703bbe61988eb110c409f2141efc2cbbe3b4fcc6515 *gates.log
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 *arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 *arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 *arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f *arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 *arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d *arms/B_DEF_K26.step.csv
```

`forest_s8_cmp.png` = original | OFF | ANIM crop of the water (rows 159-363, cols 430-639).
