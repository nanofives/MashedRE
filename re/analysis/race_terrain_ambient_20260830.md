# RIGHT-TERRAIN band = ambient fill added to non-lit prelit ground (2026-08-30)

> **CORRECTED 2026-08-30 (branch `race/geomlight`).** The *measurements* in this
> note stand (the band is the ambient, not the sun; zeroing ambient moves it toward
> the original). The *site attribution below is WRONG.* This note blames
> `LightAtomicVertex` / `TrackRenderer.cpp:228`. That function is the D3D9 bake and
> the race terrain never reaches it on the default path: **librw is the default
> renderer** (`RwRaceSubmit.cpp` flag inverted 2026-08-18), and scoping the :228
> fill off (`MASHED_TERRAIN_NOLIGHT=4`) is **bit-identical to base** (commit
> `3551ba83`). The real site is the **manual prelit fold in `BuildClump`
> (`RwSceneBuild.cpp:479`)**, which adds `amb_world_` into the vertex colours of
> **non-lit prelit** DFF batches. It is NOT the librw ambient light
> (`RwRaceSubmit.cpp:555`): that light only reaches `rw::Geometry::LIGHT`-flagged
> geometry (cars/props), and the terrain ground carries no such flag — see below.
> The fix (default now: skip the fold, honour the asset flag) and the three-track
> measurements are in the CORRECTION section at the end of this file.

Branch `race/terrain`. Scene: TRAINING race first frame, 640x480, transplanted
pose, `01_grid.bmp` (pre-start, cars parked). Reference:
`verify/parity_race_20260830/orig_race.bmp`. Metric: `re/tools/imgdiff.py --grid 8x6`.

## What the band IS (measured, not inferred)

The RIGHT-TERRAIN residual is a **uniform, channel-uniform brightness offset** on
the terrain surfaces (road + right dirt embankment), NOT a texture-content, UV, or
tiling error. Per-region crop means (standalone `01_grid.bmp` vs `orig_race.bmp`):

| region (px)              | orig mean      | standalone mean | ratio |
|--------------------------|----------------|-----------------|-------|
| dirt LR   (480-560,320-400) | (81,73,48)   | (140,119,80)    | 1.73/1.63/1.67 |
| dirt corner (560-640,400-480) | (106,92,54)| (151,124,77)    | 1.42/1.35/1.43 |
| road      (320-400,240-320) | (108,94,57)  | (170,143,90)    | 1.57/1.52/1.58 |
| band aggregate            | (92,80,50)    | (147,123,79)    | 1.60/1.54/1.58 |

For the dirt-LR cell the per-pixel diff (50.4) is almost entirely the DC mean
difference (~45.7); only ~4.7 is high-frequency residual. The ratios are equal
across R,G,B -> a scalar brightness factor, i.e. a shade offset. The two hottest
grid cells (48.8, 47.2) are additionally inflated by the race-start countdown
digit "2" (`exe_main.cpp:2971`, `DrawMashedString(...,400,250,120,...)`) that the
original reference frame does not show there; that is a HUD-overlay/phase artifact,
separate from the terrain.

## Ruled out (each an A/B capture, band aggregate unchanged unless noted)

- **UV/tiling/texture content**: crop means differ by a DC offset, not phase noise.
- **Dynamic lighting model** `MASHED_RPLIGHT=0`: 18.51 (base 18.47), band identical.
- **librw instances** `MASHED_LIBRW_INST=0`: band identical (146.7 vs 146.8).
- **Fog** `MASHED_NO_FOG=1`: whole frame WORSE (20.66) but band bit-identical ->
  fog is not the band mechanism and is correctly on.
- **World BSP / prelit-only / MATID**: `MASHED_WORLD_MATID=1` and
  `MASHED_WORLD_PRELITONLY=1` flatten the buildings/structures but leave the
  road+terrain textured. The terrain is NOT the D3D9 world BSP; it is a prop-DFF
  drawn via `TrackRenderer::BuildBatchesFromModel` (TrackRenderer.cpp:457) baked at
  load through `LightAtomicVertex` (TrackRenderer.cpp:194).

## Mechanism (nailed, file:line + measured)

The terrain ground is a **non-lit prelit atom** (has baked prelit, no vertex
normals). In `LightAtomicVertex` it takes the prelit-non-lit `else` branch and
executes **`TrackRenderer.cpp:228`**:

```
r += lt.amb[0]; g += lt.amb[1]; b += lt.amb[2];   // add track ambient
```

TRAINING's ambient is **(0.5,0.5,0.5)** (parsed from its LIGHTS.DFF via
`ParseLightsDffFaithful`, TrackRenderer.cpp:1132; logged
`amb=(0.500,0.500,0.500)` in `log/librw_race.txt`). This ambient is added on top of
the baked prelit, over-brightening the ground. Per RenderWare -- and per the code's
own comment at **TrackRenderer.cpp:220-227** -- "strictly per RW a non-lit prelit
atomic gets NO runtime ambient". The fill was kept only because zeroing it darkened
the Arctic sea (WS-E s4, 2026-08-02).

Proven with a diagnostic (`MASHED_TERRAIN_NOLIGHT`, TrackRenderer.cpp:1158,
env-gated, inert by default; =1 zero amb+sun, =2 amb only, =3 sun only):

| case                    | frame mean | over-thresh | band aggregate |
|-------------------------|-----------|-------------|----------------|
| base                    | 18.47     | 42.35%      | (147,123,79)   |
| ambient zeroed (=2)     | **15.52** | **33.48%**  | (128,110,72)   |
| sun zeroed (=3)         | 18.50     | 42.4%       | (147,123,79)   |
| both zeroed (=1)        | 15.56     | 33.5%       | (128,110,72)   |
| ORIGINAL                | --        | --          | (92,80,50)     |

Zeroing the **sun does nothing** (ground has no normals -> the `lit && n` sun
branch at :206-216 never runs); the **entire effect is the ambient**. Removing it
takes the whole frame 18.47 -> 15.52 (-2.95) and the band toward the reference.

## Residual (still open)

After the ambient is removed the band is still (128,110,72) vs original (92,80,50) --
~1.39x, still channel-uniform. The baked prelit and/or the decoded texture is
intrinsically brighter than the original renders it. Candidates not yet
distinguished: prelit decode/scale, or a global exposure/modulate the original
applies that we do not. Needs a Frida readback of the original's terrain vertex
colours (or texture-vs-render decode compare) to split prelit from texture. Not
done here.

## Why not shipped as a code change

`LightAtomicVertex` (:194) and its ambient fill (:228) are SHARED by cars, props,
and the sea across all tracks. The `MASHED_TERRAIN_NOLIGHT=1` capture shows the
**cars go near-black** without the fill, and the comment at :220-227 documents that
zeroing it darkens the Arctic sea. A global removal regresses those. The correct
fix is scoped -- ambient off for track scenery/ground, kept-or-separately-fixed for
cars/sea -- which is a shared-lighting decision that couples to the active
race-camera / car-lighting work. Flagging for the parent rather than changing
shared lighting unilaterally.

## Recommended follow-ups (for re-classify / trackers -- NOT filed here)

1. Scope the :228 ambient fill so non-lit prelit **track scenery/ground** gets no
   runtime ambient (RW-correct), without regressing cars/sea. Verify across TRAINING
   + Arctic (sea) + a car-heavy chase shot before adopting.
2. Investigate the ~1.39x residual on the ground after ambient removal (prelit
   decode vs texture vs original exposure).
3. The car near-black under NOLIGHT suggests the cars are treated as non-lit prelit
   where the original lights them dynamically -- likely the same shared-lighting knot.

Artifacts: `verify/terrain_{base,nolight,ambonly,sunonly,prelitonly,worldonly,matid,nofog}/`.

---

# CORRECTION — the real site is the librw prelit fold, and the fix (2026-08-30, branch `race/geomlight`)

## The asset flags (read from the shipped TRAINING piz)

`re/tools/dff_dump.py` + per-geometry flag enumeration of
`original/TOASTART/TRACKS/training.piz`:

| DFF                | GEOM flags | rpGEOMETRYLIGHT (0x20) | NORMALS (0x10) | PRELIT (0x08) | role |
|--------------------|-----------|------------------------|----------------|---------------|------|
| ROAD.DFF (all 21 geos) | `0x2008b` | **no** | no | yes | the ground/terrain band |
| LAKE / WATER02 / WATER03.DFF | `0x1000f` | **no** | no | yes | water (== Arctic sea's `0x1000f`) |
| TEST.DFF (shrub)   | `0x1000f` | no  | no  | yes | scenery |
| SIGN02 / CRATE01.DFF | `0x10037` | **yes** | yes | no  | lit props |

So the TRAINING ground (`ROAD.DFF`, loaded as a `Clump_Filename` prop at
`TrackRenderer.cpp:1607-1614`) is **non-lit prelit with no rpGEOMETRYLIGHT** —
exactly like `sea.dff` (`0x1000f`). Per RenderWare an atomic whose geometry lacks
rpGEOMETRYLIGHT receives **no runtime lighting**; its prelit is the final colour.

## Does BuildClump propagate the flag? YES — it was never the bug

`BuildClump` (`RwSceneBuild.cpp:411`) sets the librw geometry flags **per batch**,
not blanket: `RwSceneBuild.cpp:468` `if (b.lit) flags |= rw::Geometry::LIGHT;`,
where `b.lit = geo.lit && have_n` and `geo.lit = (flags & 0x20)`
(`DffModel.cpp:186`/`:346`). ROAD.DFF has neither the flag nor normals, so `b.lit`
is false, so librw's geometry never gets `LIGHT`, so `lightingCB_Shader` never runs
for it and the `g_amb` light (`RwRaceSubmit.cpp:555`) **cannot touch it**. The
librw pipeline already honours the asset. The over-brightness came from OUR OWN
code.

## The real site: the manual prelit fold (`RwSceneBuild.cpp:479`)

The `[D-S3-6]` fold at `RwSceneBuild.cpp:473-500` runs `if (ambient &&
!b.prelit.empty() && !b.lit)` and adds `amb_world_` straight into the prelit vertex
colours of non-lit prelit batches — the librw analogue of the D3D9
`LightAtomicVertex:252` fill. Both were the same defect: injecting a runtime
ambient into geometry the asset says gets none.

**Decisive discriminator (measured):** the fix below skips the fold but leaves the
`g_amb` light at `:555` fully active. TRAINING still drops to the ambient-starve
ceiling. Therefore the light contributes **nothing** to the terrain band — the fold
was the entire effect. This rules out `:555` as the site.

## The fix

`RwSceneBuild.cpp` — gate the fold. Default now **skips** it (honour the asset
flag); `MASHED_LIBRW_AMBFOLD=1` restores the old fold for A/B. No ambient light
change, no per-name special-casing.

## Three-track measurement (one binary; env toggles the fold)

TRAINING (`MASHED_TRACK_SEL=12`), parity pose, `01_grid.bmp` vs
`verify/parity_race_20260830/orig_race.bmp`:

| build                          | frame mean | over-thresh 16 | hottest terrain cell |
|--------------------------------|-----------|----------------|----------------------|
| old fold (`MASHED_LIBRW_AMBFOLD=1`) | 18.47 | 42.35% | 50.4 |
| **fix (fold off, default)**    | **15.45** | **33.48%** | 37.9 |
| (ambient-starve ceiling, prior) | 15.52 | 33.48% | — |

The fix reaches the ambient-starve ceiling **without any global ambient change**,
and is mechanism-driven (remove the non-RW fold), not a fitted constant.

**Cars — body untouched (verified visually).** Car bodies are lit
(`LIGHT`-flagged) and never enter the fold in either build. A 4x-amplified
whole-frame diff of TRAINING `01_grid` (fix vs old) shows the change is the
road/dirt terrain surface and the countdown "2" digit; the parked cars are
near-zero (dark in the amplified diff). Any faint residual on a car is the car's
own non-lit prelit parts (glass), which correctly lose the fold too — the same
RW-correct behaviour as the terrain. The lit body is unchanged.

**Arctic (`MASHED_TRACK_SEL=0`) — sea darkens; NOT yet confirmed vs original.**
Fix vs old, `01_inrace_track` (sea-heavy): whole-frame mean-abs 5.93, concentrated
in the lower/sea rows, B-channel most (Arctic ambient is teal). Absolute sea region
(bottom-40%) brightness: old lum ~24 → fix lum ~11 — the foreground sea goes from
dark-teal-textured to near-black. This is the RW-correct result for a `0x1000f`
non-lit prelit surface, and the one original Arctic frame on hand
(`verify/carpos_probe/burst_arctic/`) is a dark, dim scene with no over-bright
terrain — consistent with the fix. **But** there is no pose-matched original Arctic
reference for the harbour/sea section, so the darker sea cannot be *proven* an
improvement. The old "sea goes near-black / cars go near-black" objections were
measured against a GLOBAL `amb_f_` zero (which also starves the lit path); this
scoped fix leaves cars alone, but the sea darkening is real and unconfirmed.

## Shippability verdict

Confirmed correct on the one track with a real reference (TRAINING) and per RW
semantics; provably harmless to cars. **Merge to `main` is gated on a pose-matched
original Arctic reference for the sea section** — until then the near-black sea is
an unconfirmed risk, exactly the case this note is meant to flag rather than paper
over. Kept on branch `race/geomlight`, default = honour-the-flag, with
`MASHED_LIBRW_AMBFOLD=1` as the restore switch.

Artifacts (branch `race/geomlight`):
`verify/geomlight_train_{fix,old}/`, `verify/geomlight_arctic_{fix,old}/`.
