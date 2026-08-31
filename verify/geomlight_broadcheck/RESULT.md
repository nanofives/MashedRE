# Class-scoped fold — broad-track check across all 13 kAreas (2026-08-31, race/arctic-cap)

**Result: the prototype scope key `numTexCoordSets<=1` is a good first cut but NOT precise
enough to ship unconditionally.** It correctly fixes the Arctic sea and is a no-op on 9 of
13 tracks, but it mis-folds a few non-water prelit props on CITY and DUMP (and ROUNDABOUT
in some vantages), over-brightening them. The scope needs a tighter water discriminator
before `race/geomlight` merges with the fold defaulted on.

## Method

Two-stage, to avoid unlocking/reaching every track on the original side:
1. Standalone-only sweep (no original needed): for each `MASHED_TRACK_SEL` 0-12, run geomON
   (fold off, current default) vs geomSEA (`MASHED_LIBRW_AMBFOLD_SEA=1`, scoped) and diff.
   Where they differ, the scoped fold FIRES (the track has water-class geometry in view).
2. For the tracks it fires on, capture a pose-matched original (via the wider save unlock —
   col1=1 + col3=2 on all 13 rows extends the challenge-select list to all tracks) and
   check whether the fold moves TOWARD the original (correct, like Arctic) or AWAY
   (over-bright). Luma measured over the fold mask (`|geomON-geomSEA|>6`).

Full challenge-index -> area map (all rows unlocked, `DAT_0067f17c` 0-12):
0 TRAINING, 1 EGYPT, 2 NEUSTEIN, 3 ARCTIC, 4 HIGHWAY, 5 SANDS, 6 SUPERG, 7 ROUNDABOUT,
8 STORM, 9 FOREST, 10 DUMP, 11 WARZONE, 12 CITY. (Save row == challenge index == launch
"track" index; this is NOT the kAreas[] order.)

## Per-track result

| kAreas sel | track | scoped fold fires? | vs original (fold mask luma) | verdict |
|---|---|---|---|---|
| 0 | Arctic | YES (56-69%) | orig 28-33, geomON ~9, geomSEA ~28-30 | **fold CORRECT** (fixes sea) |
| 1 | Egypt | no (0.00%) | — | safe (no-op) |
| 3 | Forest | no (0.00%) | — | safe (no-op) |
| 4 | Highway | no (0.00%) | — | safe (no-op) |
| 5 | Neustein | no (0.00%) | — | safe (no-op) |
| 6 | Storm | no (0.00%) | — | safe (no-op) |
| 7 | SuperG | no (0.00%) | — | safe (no-op) |
| 8 | Warzone | no (0.00%) | — | safe (no-op) |
| 10 | Sands | no (0.01%) | — | safe (no-op) |
| 12 | Training | no (road=0x2008b) | trON 15.45 = trSEA 15.45 | safe (win kept) |
| 2 | **City** | YES (1.9%) | orig 73.9, geomON 115.1 (Δ41), geomSEA 148.4 (Δ75) | **fold OVER-BRIGHTENS** |
| 11 | **Dump** | YES (0.2%) | orig 53.5, geomON 71.9 (Δ18), geomSEA 92.5 (Δ39) | **fold OVER-BRIGHTENS** |
| 9 | **Roundabout** | fires 2.0% (natural cam), 0% (ref vantage) | not in ref frame | **suspect** (not judged) |

## What the fold mis-catches (eyeballed)

`city/city_3way_orig_geomON_geomSEAmask.png`, `dump/dump_3way_orig_geomON_geomSEAmask.png`
(orig | geomON | geomSEA with the fold mask in RED):
- **City**: the mask lands on a green awning and a lamppost — NOT water. The fold adds
  ambient to them, pushing them 41→75 above the original.
- **Dump**: the mask is a small sky-edge sliver. Same additive over-bright.

So `numTexCoordSets<=1 && non-lit && prelit` is satisfied by a few non-water props (awnings,
lampposts, sky elements), not just the LAKE/WATER `0x1000f` surfaces it was meant for.

## Two SEPARATE pre-existing bugs (NOT caused by the fold)

Visible in both geomON and geomSEA, independent of `MASHED_LIBRW_AMBFOLD_SEA`:
- **City**: the road/ground renders BLACK (crushed) vs the original's dark-grey night road.
  The fold does not touch it (mask is on the awning), so this is a separate standalone
  render defect on City.
- **Dump**: the sky/background is BLOWN OUT WHITE. Also independent of the fold.
These are out of scope here but should be filed — they will dominate any City/Dump parity
number regardless of the geomlight decision.

## Verdict / recommendation

- The class-scoped fold does what it was designed for on the tracks that matter for the
  geomlight gate: fixes the Arctic sea, keeps the TRAINING road win, no-op on 9/13 tracks.
- But the scope KEY is too loose: it over-brightens a few non-water prelit props on City
  and Dump (and Roundabout fires too). Before defaulting the fold on, the parent needs a
  tighter water discriminator — flags alone can't separate water from awning/lamppost/sky
  (all are `numTexCoordSets<=1` non-lit prelit). Options: key on the DFF asset name
  (LAKE/WATER0x — needs the name plumbed to BuildClump), or on a material/texture signature
  of the water surface. That is a follow-on RE task, not a flag tweak.
- Interim: shipping geomlight with the fold as `MASHED_LIBRW_AMBFOLD_SEA` OFF by default
  (i.e. current geomlight behaviour) is safe on all tracks EXCEPT it leaves the Arctic sea
  too dark. So the true state is: no single global setting is correct for all tracks with
  this scope key. The precise fix is required.
