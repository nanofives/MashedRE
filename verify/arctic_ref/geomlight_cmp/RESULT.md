# geomlight vs the Arctic reference — result (2026-08-31, branch race/arctic-cap)

**VERDICT: geomlight over-darkens the Arctic sea. Do NOT merge it unscoped.**
The feared regression is real. On a sea-dominant pose-matched Arctic frame, removing the
prelit ambient fold crushes the sea/dock surface (prelit class `0x1000f`) to luma ~9-10,
while the original renders it bright (~28-33); the fold-RESTORED baseline matches the
original within ~2-5. The fold fix must be scoped (keep the fold for `0x1000f`, drop it
for the road `0x2008b`) before `race/geomlight` can ship.

> Supersedes the first cut of this file (commit ef4a21e0), which read "supports
> shipping" off the START-GRID frame. That was WRONG: on the grid frame the sea is only
> 2.5% of the view and is occluded by the standalone's foreground player car, so the
> luma there (orig 30.5 / geomON 33.5 / geomOFF 37.3) accidentally favoured geomON. The
> sea-dominant frames below (56-69% sea) are authoritative and reverse it.

## Setup

Standalone = `.worktrees/race-geomlight/mashedmod/build/mashed_re.exe` (`race/geomlight`
@ 984bca34). Driven into the Arctic race demo with an ORIGINAL 12-float camera basis
transplanted via `MASHED_CAM_POSE` (so the standalone renders the same Arctic world from
the same vantage; the sea is static geometry, so basis-match = same sea in view):

```
MASHED_ROOT=<main repo> MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DETERMINISTIC=1
MASHED_TRACK_SEL=0   # 0 = Arctic in kAreas[]
MASHED_CAM_POSE=<basis from the matching original capture>
# geomON  = default (fold removed);  geomOFF = MASHED_LIBRW_AMBFOLD=1 (fold restored)
```

Three original Arctic vantages were captured (`race_draw_burst.py --challenge 3 --settle
{4,8,14}`, each wrapped in `run_with_unlocked_save.py`): the start grid (`orig_arctic.bmp`)
and two mid-race dock views (`sea_search/s8`, `sea_search/s14`).

## The sea signal — luma over the fold-affected mask (authoritative)

The fold toggle changes ONLY the `0x1000f` prelit surface, so `mask = |geomON-geomOFF|>6`
IS the sea/dock. Mean luma over that mask, all three builds:

| frame | sea mask | original | geomON (fold off) | geomOFF (fold on) |
|---|---|---|---|---|
| grid (`orig_arctic.bmp`) | 2.5% | 30.5 | 33.5 (Δ3.0) | 37.3 (Δ6.8) |
| **s8** | **68.7%** | **27.9** | **9.1 (Δ18.8)** | **30.1 (Δ2.2)** |
| **s14** | **56.5%** | **32.5** | **9.8 (Δ22.7)** | **27.7 (Δ4.8)** |

On the two sea-dominant frames the original sea is bright (28-33) and:
- **geomON (the geomlight candidate) is 18-23 luma too dark** — it crushes the sea to ~9.
- **geomOFF (fold restored) matches the original** within 2-5 luma.

Confirmed by eye: `sea_search/s8_3way_orig_geomON_geomOFF.png` — orig = wet grey-brown
dock with reflections; geomON = near-black; geomOFF = wet blue-grey dock, matches orig.

## Whole-frame 8x6 imgdiff vs the grid reference (for completeness)

| build | mean abs | over threshold 16 |
|---|---|---|
| geomON | 15.19 | 34.64% |
| geomOFF | 15.11 | 34.86% |

Near-tie on the grid frame ONLY because its sea is tiny and the frame is confounded by
HUD player-squares, the countdown digit, the foreground player car, and sub-frame roll
drift. Not a sea measurement — see the luma table above.

## Recommendation

Do NOT merge `race/geomlight` as an unconditional default. The blanket removal of the
manual prelit ambient fold (RwSceneBuild.cpp:479-500) is right for the road (`0x2008b`,
which TRAINING's 18.47->15.45 win came from) but WRONG for the Arctic sea (`0x1000f`),
which the original keeps bright. The original is therefore NOT doing pure-RW no-ambient
lighting on `0x1000f`; the manual fold approximates whatever it does. Scope the fold fix
by geometry class/flags (drop for `0x2008b`, keep for `0x1000f`) and re-run this diff.

## [UNCERTAIN] / caveats

- Car positions differ between the original and the deterministic standalone demo, so the
  per-pixel whole-frame diff is noisy; the class-masked luma is the clean metric and is
  robust to the car mismatch (mask is 56-69% dock).
- Sub-frame roll drift between the transplanted basis and the BMP adds noise but cannot
  explain an 18-23 luma gap.
- The exact scoped-fix mechanism (per-class fold) is a recommendation, not yet verified;
  the measured fact is only that fold-removed != original and fold-restored ~= original
  on `0x1000f`.
