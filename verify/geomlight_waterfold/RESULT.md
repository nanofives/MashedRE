# Water-scoped ambient fold — refined discriminator, verified (2026-08-31, race/geomlight)

**VERDICT: the refined key works. Arctic sea matches the original (28.0 vs 29.8, Δ1.8);
City, Dump and TRAINING are exact no-ops (fold mask 0.00%).** This closes the blocker that
`verify/geomlight_broadcheck/RESULT.md` raised against the flags-only prototype.

## What changed, and why the flags-only key could not work

The prototype scoped the fold on `numTexCoordSets<=1 && !lit && prelit`. That is a
GEOMETRY-CLASS key, not a water key: a green awning and a lamppost on City and a skydome
sliver on Dump satisfy it too, so the fold added ambient to them and pushed them further
from the original than no fold at all (City orig 73.9 → 148.4; Dump 53.5 → 92.5).

No combination of RW flags separates those cases — they are genuinely the same class. The
discriminator has to come from the ASSET. Water surfaces live in single-purpose DFFs on
every track that has water (`piz_extract.py list`, all 13 archives, 2026-08-31):

| track | water DFFs |
|---|---|
| Arctic | `SEA.DFF` |
| City | `WATER.DFF` |
| Forest | `WATER.DFF`, `WATERFALL.DFF` |
| sands | `WATER.DFF` |
| Storm | `WATER.DFF` |
| SuperG | `SEA.DFF` |
| training | `LAKE.DFF`, `WATER02.DFF`, `WATER03.DFF` |
| Warzone | `RIVER.DFF` |
| Egypt, Highway, Neustein, rouabout, dump | none |

`MIST.DFF` (Arctic, Storm) is deliberately NOT matched: it is a fog billboard, not a water
surface, and folding ambient into it is the same additive over-bright defect.

### The plumbing

`DffModel::Parse` receives only a blob, so the name had to be carried in:

- `Track/DffModel.h` — new `DffModel::source_name[64]`, filled by the caller.
- `D3d9Render/TrackRenderer.cpp:1383` — `load_prop` is the one place that knows the name;
  it copies `dff_name` into `m.source_name` right after `Parse`.
- `LibRw/RwSceneBuild.cpp` — `ModelIsWaterAsset()` (case-insensitive PREFIX match on
  `SEA`/`WATER`/`LAKE`/`RIVER`; prefix not substring, so nothing merely containing the word
  is dragged in), evaluated once per model. ANDed with the existing `BatchIsWaterClass()`.

Because the two halves are **ANDed**, the new scope is strictly NARROWER than the
prototype: every track the prototype was already a no-op on stays a no-op by construction.

The world path and the car model never set `source_name`, so they read as "not water" and
are unaffected.

## Measurement

Protocol identical to `verify/geomlight_broadcheck/RESULT.md`: an ORIGINAL 12-float camera
basis transplanted via `MASHED_CAM_POSE` so both arms and the original share a vantage;
mean luma over the fold mask `|geomON − geomWATER| > 6`, which IS the folded surface
because the toggle changes nothing else. Driver: `run_arms.ps1` (10 captures, sequential,
each waited to process EXIT — a shot polled for early is a menu frame). Shot measured:
`<arm>/race1/01_grid.bmp`.

The captured BMPs are NOT committed (88 MB). The three-panel PNGs preserve the visual
evidence, the original references are already committed on `race/arctic-cap`
(`verify/arctic_ref/sea_search/s{8,14}/a.bmp`,
`verify/geomlight_broadcheck/{city,dump}/orig.bmp`,
`verify/parity_race_20260830/orig_race.bmp`), and `run_arms.ps1` regenerates the arms.

Arms: `geomON` = current `race/geomlight` default (fold entirely off);
`geomWATER` = `MASHED_LIBRW_AMBFOLD_SEA=1` with the refined key.

**Harness validated before use**: replaying the measurement over the broadcheck's own City
captures reproduces its published row (mask 1.91% vs 1.9%, orig 74.0 vs 73.9, geomON 115.0
vs 115.1, geomSEA 148.3 vs 148.4) — within 0.1 luma, so the numbers below are comparable
to the earlier table rather than a differently-defined measure.

| track | SEL | fold mask | original | geomON | geomWATER | verdict |
|---|---|---|---|---|---|---|
| Arctic `s8` | 0 | **69.63%** | 28.0 | 9.1 (Δ18.9) | **29.8 (Δ1.8)** | sea FIXED |
| Arctic `s14` | 0 | **58.37%** | 32.6 | 9.8 (Δ22.9) | **27.2 (Δ5.4)** | sea FIXED |
| City | 2 | **0.00%** | — | — | — | exact no-op |
| Dump | 11 | **0.00%** | — | — | — | exact no-op |
| TRAINING | 12 | **0.00%** | — | — | — | exact no-op |

TRAINING whole-frame `imgdiff --grid 8x6` against `verify/parity_race_20260830/orig_race.bmp`:
**15.45 on BOTH arms**, byte-identical (`R=17.40 G=15.33 B=13.63`, 33.48% over threshold 16,
same 48 grid cells). The geomlight road win is untouched.

The City and Dump over-brightening from the broadcheck (Δ75 and Δ39 on the fold mask) is
gone because the fold no longer fires there at all.

## The fold is attributed by NAME in the log, not inferred from a mask

`BuildClump` now logs `dff=` and `water_asset=` per clump, so the asset can be named
directly. Over the whole 10-run set (`log/librw_scene.txt`), exactly one asset matched:

```
clump[4]: dff='sea.dff' water_asset=1 mats=1 named=1 resolved=1 batches=1
```

All 32 other distinct DFFs logged `water_asset=0`, including the exact props the prototype
mis-caught: `objects01.dff` / `objects02.dff` (City awning), `Lamposts.dff`,
`skydome.dff` / `skydome01.dff` / `sky.dff` (Dump sliver), and `Road.dff`. Note the
on-disk names are mixed case (`sea.dff`), which the match normalises.

Full per-asset list: `fold_attribution.txt`.

Visual: `arctic_s8/arctic_s8_3way.png`, `arctic_s14/…`, `city/city_3way.png`,
`dump/dump_3way.png` (original | geomON | geomWATER with the mask tinted RED). On Arctic
the red mask covers the sea and dock and nothing else; the geomON panel is near-black
there while the original and geomWATER both show the lit wet dock. (Those PNGs are
untracked — `.gitignore:141` excludes `verify/**/*.png` project-wide, same as the
broadcheck's three-panel images. `run_arms.ps1` regenerates the arms they were built from.)

## Two SEPARATE standalone bugs, re-confirmed as fold-independent

Both are visible identically in geomON and geomWATER (fold mask 0.00% on these tracks, so
the fold provably does not touch them). Carried over from the broadcheck, filed separately:

- **City: the road/ground renders BLACK** vs the original's dark-grey night road.
- **Dump: the sky/background is BLOWN OUT WHITE.**

They dominate any City/Dump whole-frame parity number regardless of the geomlight decision.

## Status

The scope key is no longer the blocker. What remains is a defaulting decision: the refined
fold is still behind `MASHED_LIBRW_AMBFOLD_SEA=1`, so the shipping default still leaves the
Arctic sea at luma ~9 against the original's ~28. Making it the default is what turns this
measurement into a shipped fix.
