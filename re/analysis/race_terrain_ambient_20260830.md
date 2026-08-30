# RIGHT-TERRAIN band = ambient fill added to non-lit prelit ground (2026-08-30)

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
