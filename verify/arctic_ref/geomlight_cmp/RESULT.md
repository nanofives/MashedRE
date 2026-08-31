# geomlight vs the Arctic reference — result (2026-08-31, branch race/arctic-cap)

**Question (T-ARCTIC gate):** `race/geomlight` removes a non-RW manual prelit ambient
fold (RwSceneBuild.cpp:479-500). It helps TRAINING (18.47→15.45) but darkens the Arctic
sea (prelit class 0x1000f). With no pose-matched Arctic reference the ship decision was
blocked. Now there is one (`../orig_arctic.bmp`). This is that diff.

## Setup

Standalone = `.worktrees/race-geomlight/mashedmod/build/mashed_re.exe` (branch
`race/geomlight` @ 984bca34, build 2026-08-30, clean). Driven into the Arctic race demo
with the ORIGINAL's transplanted 12-float camera basis:

```
MASHED_ROOT=<main repo>  MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DETERMINISTIC=1
MASHED_TRACK_SEL=0   # 0 = Arctic in kAreas[]
MASHED_CAM_POSE=-25.52725,4.55718,39.87078,-0.87753,0.47719,-0.04718,0.47952,0.87327,-0.08634,-0.00000,-0.09839,-0.99515
```

- **geomON** (candidate, fold removed = default): `geomON/race1/01_grid.bmp`
- **geomOFF** (baseline, fold restored via `MASHED_LIBRW_AMBFOLD=1`): `geomOFF/race1/01_grid.bmp`

Compared against `../orig_arctic.bmp` with `imgdiff.py --grid 8x6`.

## Whole-frame diff vs the Arctic reference

| build | mean abs | over threshold 16 |
|---|---|---|
| geomON (fold removed) | 15.19 | 34.64% |
| geomOFF (fold restored) | 15.11 | 34.86% |

Near-tie (Δ0.08 mean), both ~parity with TRAINING's 15.45. The AMBFOLD toggle changes
only a localized lower-center patch (geomON-vs-geomOFF = 0.81 mean / 2.51% of pixels, max
cell 9.3) — the prelit sea/road surface. The whole-frame number is **confounded**: HUD
player-squares, the countdown digit "2", and the foreground player car all render in the
standalone but not in the original reference, and there is sub-frame roll drift. Those
pollute exactly the cells the fold touches, so the 15.19-vs-15.11 tie is NOT a clean sea
measurement.

## Isolated prelit sea/road patch — mean luma (the clean signal)

Patch = box (240,240)-(500,480), the region the AMBFOLD toggle actually changes:

| | mean luma | Δ from original |
|---|---|---|
| **original** | **30.5** | — |
| geomON (fold removed) | 33.5 | **3.0** |
| geomOFF (fold restored) | 37.3 | 6.8 |

The original's sea/road surface is **darker** than both standalone builds, and removing
the fold moves the standalone **toward** the original (33.5 vs 37.3). Confirmed by eye
(`sea_patch_cmp.png`, orig | geomON | geomOFF): on the exposed road strips geomON is
darker, matching the original; geomOFF is greyer/brighter.

## Verdict

**The feared regression does not occur.** On the pose-matched Arctic reference:
- Whole-frame: geomlight is **neutral** (within 0.08 mean-abs of the baseline).
- On the prelit sea/road surface it targets, geomlight is an **improvement** — the
  darker sea matches the original (Δ3.0 vs Δ6.8).

By the handoff's own decision rule ("if the darker sea matches → merge geomlight; if not
→ track-scope the fold fix"), the darker sea **matches**. The Arctic reference therefore
**supports shipping `race/geomlight`**, not blocking it. Recommend the parent merge
`race/geomlight` (do not merge from this branch).

## [UNCERTAIN] / caveats

- One frame only, and the sea is a small fraction of this rolled grid view; a
  sea-dominant pose would strengthen the luma signal.
- Sub-frame roll drift between the reference basis and BMP (memory
  `race-camera-rolls-30deg-sine`) adds per-pixel noise to the whole-frame mean.
- Standalone HUD/countdown/foreground-car artifacts inflate both absolute numbers
  equally (they cancel in the geomON-vs-geomOFF comparison, not in vs-original).
