# U-9062 root cause: `Clump_Exclude_From_World` is read as "do not draw" (2026-08-31)

**VERDICT: U-9062 and U-9063 are ONE bug, and it is not a lighting bug. The road surface
is MISSING, not dark.** Loading the excluded clumps takes City from 25.43 to 11.23 mean
abs diff and Dump from 84.78 to 9.62, with no regression on Arctic or TRAINING.

## The bug

`TrackRenderer.cpp:1621` treats `Clump_Exclude_From_World(N)` as "do not load clump N at
all". City's `COURSE.LUA` declares 14 clumps and then excludes 6-13:

```
Clump_Filename(6..9,  "Build01..04.dff")     <- excluded, absent from our frame
Clump_Filename(10,    "Standard.dff")        <- excluded, absent
Clump_Filename(11,    "road.dff")            <- excluded, absent  == the "black road"
Clump_Filename(12,    "Trunk.dff")           <- excluded, absent
Clump_Filename(13,    "water.dff")           <- excluded, absent
```

The name says "exclude from the **world**", i.e. do not merge into the static BSP. That is
not the same as "do not draw", and reading it as the latter drops the road, four buildings,
and the water.

**Dump is the same bug.** Its clump 9 is `Road.dff`, also excluded. The "blown out white
sky" of U-9063 was never a sky defect: it is the white background showing through the hole
where the road should be. With the road loaded the white area is gone
(`city_dump_fix_grid.png`, top row).

## What was ruled out first

- **Not a librw-vs-D3D9 asymmetry.** Both renderers agree on City: crushed-black pixels
  57.88% (librw) vs 58.25% (D3D9) against the original's 11.32%. `MASHED_RENDER_LIBRW=0`
  changes nothing, so the defect is upstream of both.
- **Not the ambient fold.** Confirmed independent when U-9062/U-9063 were filed (mask 0.00%,
  identical across both fold arms).
- **Not texture resolution.** The two problem world materials are `mat[38]` (no texname,
  rgba 102,102,102 mid-grey) and `mat[61]` (`JetRanger`, unresolved, white) — neither is a
  black road.
- **Not the world path.** The road is not world geometry at all; it is a declared clump.

## Measurement

`run_excl_sweep.ps1`, every track that has a pose-matched original reference, both arms,
basis transplanted via `MASHED_CAM_POSE`. `imgdiff --grid 8x6` against the original.

| track | skip (current) | load (probe) | delta | verdict |
|---|---|---|---|---|
| Arctic s8 | 18.85 / 59.18% | 18.85 / 59.18% | +0.00 | unchanged |
| **City** | 25.43 / 72.48% | **11.23 / 22.52%** | **-14.20** | BETTER |
| **Dump** | 84.78 / 48.71% | **9.62 / 17.26%** | **-75.16** | BETTER |
| TRAINING | 15.45 / 33.48% | 15.40 / 33.41% | -0.05 | unchanged (no regression) |

City crushed-black pixels: original 11.32%, skip 57.88%, **load 12.36%** — within ~1 point
of the original.

Arctic is unchanged because its exclusions are clumps 8 and 9, neither in view at this
vantage. TRAINING moves by 0.05, i.e. the headline 15.45 survives.

## It also answers the water-fold open question

`verify/geomlight_waterfold/RESULT.md` left open why no water clump was built on the other
water tracks, and floated "maybe their water is world/BSP geometry". **That was wrong.** It
is this same skip. Every water clump in the game except Arctic's is excluded:

| track | water clump | excluded? |
|---|---|---|
| Forest | `Water.dff` (0) | yes |
| Warzone | `River.dff` (0) | yes |
| SuperG | `sea.dff` (0) | yes |
| Storm | `Water.dff` (0) | yes |
| sands | `Water.dff` (1) | yes |
| training | `Water02` (3), `Water03` (4), `Lake.dff` (9) | yes |
| **Arctic** | **`sea.dff` (2)** | **NO** |

So Arctic's sea being the one surface the water fold could act on was not a coincidence of
camera vantage — it is the only non-excluded water clump in the game. Once this is fixed,
the water fold will start firing on six more tracks, and those tracks have no original
reference. That is a NEW verification debt this fix creates, and it should be booked
before defaulting both on together.

## Status

Probe is env-gated and **default OFF**: `MASHED_TRACK_LOAD_EXCLUDED=1` enables it.

**What is NOT established:** the actual semantics of `Clump_Exclude_From_World` in the
original. The evidence here is behavioural parity against pose-matched original frames,
which is this project's acceptance standard for standalone visual work, and it is strong
(-14 and -75 mean abs diff, crushed fraction landing within 1 point). But no RVA has been
read, so "excluded means excluded from the BSP merge, still drawn" is an inference from the
command name plus the measurement, not a decoded fact. The clean confirmation is the Lua
command binding in `MASHED.exe`. Until then this is a measured improvement with an
un-decoded mechanism, and it should not be described as a verbatim port.

Whether "still drawn" also implies "not collidable" / "drawn in a different pass" is
likewise unknown, and a purely visual measurement cannot see it.
