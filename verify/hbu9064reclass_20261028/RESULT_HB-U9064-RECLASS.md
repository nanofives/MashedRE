# HB-U9064-RECLASS RESULT (2026-10-28): U-9064 resolved, both halves

**Verdict: GREEN.** Tracker-only step through `re-classify`. No source, no build, no game run, no gate run
(text-only by lead's instruction). Branch `orch/HB-U9064-RECLASS` from `orch/integration` `a7b3157c7`.

## What changed

| file | change |
|---|---|
| `UNCERTAINTIES.md` | U-9064 row struck through and marked `RESOLVED 2026-10-28`, resolution prepended, 2026-08-31 filing kept as history (same style as U-9062 / U-9063) |
| `re/analysis/CHANGELOG.md` | one entry inserted below `<!-- ENTRIES -->` |
| `hooks.csv`, `STUBS.md`, `DEFERRED.md` | no U-9064 rows exist, nothing to change |

No new U-NNNN or S-NNNN rows were created.

## Basis

- Arctic half: `verify/hbarcticfoldrm_20261027/RESULT_HB-ARCTIC-FOLDRM.md` (GREEN, folds deleted
  `954d3477e`, 144 deletions, gates equal `merge_train.json`) and `verify/hbarctic_20261027/RESULT_HB-ARCTIC.md`
  (cited through the FOLDRM RESULT, not re-read here). The original lights no atomic lacking
  `rpGEOMETRYLIGHT` (callback `0x004ea4e0`, gate `0x004ea4e9..0x004ea4f2`).
- Forest half: `verify/hbforestwater_20261027/RESULT_HB-FOREST-WATER.md` (GREEN, `71d4d4dcc`, forest_s8
  water luma 48.1 -> 24.5 vs original 21.5, R0..R6 pass). Hook table `0x005f33f8`, Course_Id 34, slot 0
  `0x0044dea0`, slot 1 `0x0044ded0`, ring `water00..water31`.
- L-44 (`log/orch/DECISIONS.md`): accept the `MASHED_TEXANIM` default-ON flip via HB-TEXANIM-REFS
  `da0572742`, first item of run 17.

## Caveat the row states

At this branch's base, HB-TEXANIM-REFS is not merged (its RESULT is not in the tree). `MASHED_TEXANIM`
is therefore still default OFF here. The row says the Forest half is resolved by the mechanism plus the
L-44 acceptance, and that the default flip lands with `da0572742`. If run 17 rejects or reorders that
merge, the row text about the default needs a correcting CHANGELOG entry.

## Carried forward, not in this row

- HB-WARZONE-RIVER (Warzone river 10-16 luma off, with TEXANIM on or off), per L-44.
- The D3D9 path has no TEXANIM equivalent.

## Not run

Gates (G-CTRL, B): not run, no code changed.
