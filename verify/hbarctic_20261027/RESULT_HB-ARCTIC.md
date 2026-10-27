# HB-ARCTIC RESULT (2026-10-27): no light reaches the sea, and the Arctic sea is no longer dark

**Verdict: GREEN (analysis leg). There is nothing to port.** In the original, course lights
never reach an atomic whose geometry lacks `rpGEOMETRYLIGHT`: the atomic lighting callback
`0x004ea4e0` turns `D3DRS_LIGHTING` off for it. Both water `SEA.DFF`s (Arctic, SuperG) lack
that flag, so the original draws them as texture x prelit. That is exactly what the port's
default (fold OFF) draws. The "Arctic sea too dark" defect (U-9064, original 28.0 against
unfolded 9.1) does not reproduce on today's build: over the same 69.63% region the default
now reads 32.1 against the original's 28.0 (d4.1). That region was road in the original all
along, and the 2026-09-30 sea-tile fix moved our sea off it.

PREREG: [`PREREG.md`](PREREG.md), committed alone as `75568b8af` before any arm ran. Not amended.
No source file changed. Exe `mashedmod/build/mashed_re.exe` built from `orch/integration`
`ebc7b0c36`, SHA-256 `12fd8abef981d4de8d455b6607a28d6916c1a8be5d3bf1d9550117d25c2126e1`.

## 1. The mechanism (static, RVA-cited)

| step | site | what the bytes do |
|---|---|---|
| course lights | `0x00479330`, `Lights_Filename` branch | for each light in the loaded clump: `*(char*)(light+2) = 1`, then `RpWorldAddLight` (`0x004e4810`). The no-file branch builds `RpLightCreate(2)` + `RpLightCreate(1)` with the same flag. Flag `1` only, never `2`. |
| atomic pipeline | installer `0x004ea970` | callbacks: instance `0x004ea9a0`, reinstance `0x004eb350`, lighting `0x004ea4e0`, render `0x004ec190` |
| lighting gate | `0x004ea4e9`..`0x004ea4f2` | `mov eax,[ebp+0x18]` (geometry), `mov ebx,[eax+8]` (flags), `test bl,0x20`, `je 0x004ea6b5` |
| no-world gate | `0x004ea4f8`..`0x004ea502` | `[RwGlobals+4]` NULL also jumps to `0x004ea6b5` |
| lit path | `0x004ea508`..`0x004ea5b4` | walks the world's global light list (`[world+0x3c]`), keeps lights with flag value 1 (`test byte ptr [eax+2],1` at `0x004ea54e`), sums non-directional colours (`[light+1] != 1`) into `0x006181a0..a8`, enables a directional (`call 0x004f4b70`) only if the geometry has `0x10` (normals, `0x004ea55a`), writes `D3DRS_AMBIENT` (`0x8b`) 0 when none were found (`0x004ea5ad`) |
| unlit path | `0x004ea6b5` | `call 0x004f4f10(0, 1)` |
| lights off | `0x004f4f10` -> `0x004f5007` | first argument 0 -> `mov [esp+8],0; mov [esp+4],0x89; jmp 0x004d5480`, i.e. `D3DRS_LIGHTING` = FALSE |
| world sectors | sector installer `0x004eb520`, lighting `0x004eb550` | counts only lights with flag bit 2 (`test byte ptr [eax+2],2` at `0x004eb58a`), so course lights do not light the BSP either |

Water geometry flags (`re/tools/dff_dump.py`): Arctic `SEA.DFF` `0x1000f`, SuperG `SEA.DFF`
`0x1000f`. No `0x20` (LIGHT), no `0x40` (MODULATEMATERIALCOLOR).

The port already matches: `BuildClump` sets `rw::Geometry::LIGHT` only when the DFF has it
(`LibRw/RwSceneBuild.cpp:608`), so librw's lighting callback never touches these atomics. The
two folds (`MASHED_LIBRW_AMBFOLD`, `MASHED_LIBRW_AMBFOLD_SEA`, gate `:622-626`, bake `:627-648`) have no counterpart
in the original. A default-OFF knob for "the original's light path" would be the default.

The arena sea pass `0x00449030` (Arctic hook slot 2) was also decoded, because
`SEA_LEVEL_2026-09-29.md` U1b left its state pairs open: it renders the 25 tiles through
`RpClumpRender` (`0x004e6680`) with no state change of its own. The eight `RwGlobals+0x20`
pairs all follow the tiles and bracket the spark pass `0x004921b0` (ZWRITE off, FOG off,
SRCBLEND SRCALPHA / DESTBLEND ONE, then restore). They do not apply to the sea.

## 2. Arms (PREREG section 2)

| rule | verdict |
|---|---|
| R0 control, arctic_s8 OFF vs OFF2 | **PASS**: frame mean abs dL 0.000, 0.000% pixels differ |
| R1 / R2 on arctic_s8, arctic_s14 | **not scorable as written**: water mask 0.00% on both poses. The SEA arm is bit-identical to OFF (same SHA-256), so no water pixel is in view |
| R1 on superg_s14 | dOFF 5.6 <= 8 **PASS** |

| pose | mask % | original | OFF | SEA (fold) | dOFF | dSEA |
|---|---|---|---|---|---|---|
| arctic_s8 | 0.00 | | | | | |
| arctic_s14 | 0.00 | | | | | |
| superg_s14 | 36.66 | 152.6 | 158.2 | 189.5 | 5.6 | 36.9 |
| forest_s8 (not scored) | 5.23 | 21.5 | 48.1 | 67.5 | 26.6 | 45.9 |
| forest_s14 | no original bitmap committed (`verify/water_refs/forest_s14/` has only the json) | | | | | |

Fog on every arm (`log/librw_race.txt`): `P7 fogcolor ... MATCH` on all three tracks (Arctic
`282C30`, SuperG `FFFFFF`, Forest `889B8D`), Arctic `fogconst start=0.100 end=70.000`. The
black librw fog colour recorded in `verify/water_refs/U9064_MECHANISM.md` (2026-08-31) is gone.

## 3. Post-hoc (NOT pre-registered): the region the old row was measured on

Why the Arctic mask is empty: at both committed poses the camera looks down at the start
grid. In the original frame that area is road. The pre-fix standalone drew the mis-placed sea
over it (side-by-side `D:/MashedWT/HB-ARCTIC_data/arctic_s8_cmp.png`: original | today |
pre-fix). The sea now sits at Y = -4.1, below the road, and is not in view.

So the fair re-test is the old mask itself, `|folded - unfolded| > 6` from the committed
pre-fix captures in `verify/arctic_ref/sea_search/<pose>/` (directory `geomOFF` = folded,
`geomON` = unfolded, which reproduces the published 69.63% / 28.0 / 9.1 / 29.8 row exactly),
read on today's OFF capture. Script `posthoc_oldmask.py`.

| pose | old mask % | original | pre-fix folded | pre-fix unfolded | **today, default** | **d** |
|---|---|---|---|---|---|---|
| arctic_s8 | 69.63 | 28.0 | 29.8 | 9.1 | **32.1** | **4.1** |
| arctic_s14 | 58.37 | 32.6 | 27.2 | 9.8 | **37.1** | **4.5** |

Whole-frame mean abs luma difference, original vs today: 8.0 (s8), 4.7 (s14).

Reading: the 2026-08-31 "Arctic needs the fold" verdict compared the original's road
against our mis-placed sea, as `verify/sea_fix_20260930/RESULT.md` already suspected. With
the sea placed, the default lands within 4.5 luma of the original on that region with no fold.
U-9064's "same class, opposite verdicts" contradiction has no Arctic half left.

What this does not show: an Arctic pose with open sea in view. Neither committed reference
frames it. The mechanism in section 1 says the sea draws as texture x prelit with fog, and
SuperG's sea (same flags) matches within 5.6, but no Arctic water pixel was compared here.

## 4. Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe (`12fd8abe`), root `D:/MashedWT/HB-ARCTIC_root`, through test-slot and `mashed_lock.py`:

| gate | this exe | `merge_train.json` ref | |
|---|---|---|---|
| G-CTRL (H_DEF step / gates / ptrace) | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | **identical** |
| B (K0 / K1 / K26) | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | **identical** |

Expected, since no source changed. All 15 game processes in this step self-exited (rc 0) and none was killed.

## 5. HB-5 (nav_coverage, text only, no ledger change)

`py -3.12 re/tools/nav_coverage.py` on this tree: 34 screen ids, 33 with tables, 11 reachable
from root 1. Table hole: screen 27 (as in the first run, `re/analysis/parity_tooling.md:138`).
Kind consistency: kind 2 handled [4, 6, 8] / not [2, 3], kind 4 handled [19, 32] / not
[31, 33]. Against the first run, screen 4 moved to handled and the `0xff1d0000` dead end is no
longer listed. No newly wired screen lacks coverage.

## 6. Follow-ups (ledger rows added)

- **HB-ARCTIC-FOLDRM**: delete `MASHED_LIBRW_AMBFOLD` / `MASHED_LIBRW_AMBFOLD_SEA` and the
  water-scope helpers (`RwSceneBuild.cpp:339-445`, `:613-648`), and rewrite the header comment
  that still says the Arctic sea ships too dark. Default-neutral (both knobs are default OFF),
  gates must stay unchanged.
- **HB-ARCTIC-OPENSEA**: capture one original Arctic pose with open sea in view
  (`race_draw_burst.py --challenge 0`, sea-dominant settle) and score it against the default.
  Closes the gap in section 3.
- **HB-FOREST-WATER**: Forest water reads 48.1 against the original's 21.5 with the fold OFF.
  Forest `WATER.DFF` carries `0x1004f` (MODULATEMATERIALCOLOR) and material alpha 209
  (`U9064_MECHANISM.md`), which Arctic and SuperG lack. Separate defect, not lighting.
- U-9064 should be rewritten or closed through `re-classify` (tracker rows are not edited here).

## 7. Raw data (not committed), SHA-256

Root `D:/MashedWT/HB-ARCTIC_data/`.

```
c02b3ba24a380c029e72798240fd250c93caad28c60afc12dfe3c55d17450f35 *render/arctic_s14_OFF/race1/01_grid.bmp
c02b3ba24a380c029e72798240fd250c93caad28c60afc12dfe3c55d17450f35 *render/arctic_s14_SEA/race1/01_grid.bmp
9973e3de7b8e63fb060aeb0e2305782566a46d79f34d2c8c81bbfa40f0fa2c75 *render/arctic_s8_OFF/race1/01_grid.bmp
9973e3de7b8e63fb060aeb0e2305782566a46d79f34d2c8c81bbfa40f0fa2c75 *render/arctic_s8_OFF2/race1/01_grid.bmp
9973e3de7b8e63fb060aeb0e2305782566a46d79f34d2c8c81bbfa40f0fa2c75 *render/arctic_s8_SEA/race1/01_grid.bmp
e8c2185e352174e800d04889df7ce660b2cc76158404af45c6aadb3f07c6496b *render/forest_s14_OFF/race1/01_grid.bmp
d5f0aa69a70e9f891e8a5b820a8d85624300825596fc1a1cbc45300222e2c30e *render/forest_s14_SEA/race1/01_grid.bmp
a215595dbf3c22e2152e5a6f35c1f5c21c85ad5d87021b164cc2eb930769dac5 *render/forest_s8_OFF/race1/01_grid.bmp
9fb3d1f867cec25623e75614b3c912a2b1babc70296edc788b7d532967dce76a *render/forest_s8_SEA/race1/01_grid.bmp
b67bc45e59ca8335a2cd524e9937eb0cf2cda3da3ca84e68abc8986dc52cb718 *render/superg_s14_OFF/race1/01_grid.bmp
4d4cf8a482baf0b9cbfce3bad4e26a9d54443213b9e6dd5625dc69eb199d9c23 *render/superg_s14_SEA/race1/01_grid.bmp
9c47ef45833cbc91e28c3c79cf61031cb9263d964704f08a1749522307b53320 *measure.txt
35c7259ce1c007cccb1ba73138052d4be936d8f65b6d0dc96fc0111f225de4c5 *posthoc_oldmask.txt
fcfaea0efd814704cfc25b41abd9c3b94e26e2ee1e5a30a67c79cda4d22b5824 *nav_coverage.txt
a5ba3ad4a1003b0f752cdadc8741b9fe67fdd2d585358fe8d1bc35799aeaf5a3 *arctic_s8_cmp.png
19e72f498d204c33160a02679802d71b452a35617e7668c4c86046e36783fcac *gates.log
c9fda11af0c6a509b6e6070f84dbb5ced3d528dc0d3e2297bf29bde92e915598 *render.log
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 *arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 *arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 *arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f *arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 *arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d *arms/B_DEF_K26.step.csv
```
