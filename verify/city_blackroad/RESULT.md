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

## Status — SUPERSEDED by the DECODED section below

Everything above was written before the binary was read, when the fix was still env-gated
at default OFF and the mechanism was an inference from the command name plus the parity
measurement. It is kept as the investigation record.

**The inference was correct and is now decoded** — see "DECODED 2026-08-31" below for the
handler, the gated call and their RVAs. The fix is now ON by default, and the toggle was
inverted to `MASHED_TRACK_SKIP_EXCLUDED=1` (the old `MASHED_TRACK_LOAD_EXCLUDED` no longer
exists). One caveat from this section does survive the decode: what world registration
changes beyond that single call is still unknown.

---

# DECODED 2026-08-31 — and defaulted ON

Ghidra MCP was not wired into the session; used `analyzeHeadless` read-only against pool
slot `Mashed_pool14` (same binary, so this satisfies the ghidra-pool "no documentation
fallback" rule rather than working around it). Preflight asserts passed; slot released.

## What the binary says

The Lua registration table at `0x00440bc0..0x00440d40` binds the command string at
`0x005cde94` to handler `0x0047aa20` via registrar `0x0047b980`.

**Handler `0x0047aa20` sets a flag and does nothing else:** `desc[0xd4 + idx*4] = 1`. It
never unloads, frees, or clears the filename slot. The descriptor is 0x21c bytes, zeroed
by `0x0047a020` (`DAT_006bf1cc` = its base), owned by track loader `0x00426e10`.

**The loader ignores the flag when loading.** `Course::CreateFromDescription`
(`0x00479330`) loads every clump whose filename slot is non-empty — the loop gates on the
name only (`"LOADING Clump [%s] with index of [%d]"`), never on the flag.

**The flag gates exactly one call.** `add edi,0xd4` at `0x00479d93` walks the flag array
against the 64 clump slots:

```
0x00479da6   cmp dword ptr [edi],0          <- the exclude flag
0x00479da9   jnz  (skip the add)
0x00479dab   mov edx,[ebx+0x105d4]          <- the RpWorld
0x00479db3   call 0x004e4450(world, clump)  <- the ONLY gated call
0x00479dbe   call 0x00474fd0(clump)         <- ALWAYS runs, excluded or not
0x00479dc6   mov [esi+0x120],eax            <- atomic handle kept either way
```

`0x004e4450` links the clump's frame and runs `RpClumpForAllAtomics` /
`RpClumpForAllLights` to register it INTO the world. That the first argument is the
RpWorld is not assumed: `[course+0x105d4]` is the same field `RpWorldAddLight` is called
on at `0x00479330`. `0x00474fd0` is get-first-atomic and runs unconditionally.

**Conclusion:** an excluded clump IS loaded and DOES keep a live atomic handle. It is only
kept out of the world's frame/atomic/light registration. "Exclude from world" means
exactly what it says, and reading it as "skip loading" is the defect.

We render props explicitly rather than through a world pass, so loading them as ordinary
props is the faithful analogue. Default flipped;
`MASHED_TRACK_SKIP_EXCLUDED=1` restores the old behaviour for A/B.

## Final numbers on the shipping default (no env vars set)

Re-measured rather than inherited: the earlier sweep reached this code path via an opt-in
env var on a build where it was opt-in, and "logically identical" is not a measurement.

| track | before this session | now | delta |
|---|---|---|---|
| Arctic s8 | 18.85 | 18.85 | +0.00 |
| **City** | 25.43 | **11.23** | **-14.20** |
| **Dump** | 84.78 | **9.62** | **-75.16** |
| TRAINING | 15.45 | **15.40** | -0.05 |

TRAINING's headline survives, marginally improved, even though it now loads and folds
three water clumps (`Lake.dff`, `Water02`, `Water03`).

## New verification debt, booked not buried

Water clumps now load on seven tracks that previously built none, so the water fold fires
on them for the first time. Magnitude of the fold at a natural-camera vantage:

| track | water clump now loaded | fold mask |
|---|---|---|
| Forest | `Water.dff` | 1.84% |
| SuperG | `sea.dff` | 0.86% |
| training | `Lake`, `Water02`, `Water03` | 0.12% |
| sands | `Water.dff` (17 batches) | 0.00% |
| Storm | `Water.dff` | 0.00% |
| Warzone | `River.dff` | 0.00% |
| City | `water.dff` | 0.00% |
| rouabout | none declared | 0.00% |

Forest and SuperG are the two that now get materially folded water with **no pose-matched
original reference to judge it against**. TRAINING has one but the effect there is 0.12%
and its number improved. This is unverified, not wrong — and unlike before, the zero rows
are now genuine "fired and was inert" zeros, because the clump provably loads.

## Still not established

What world registration changes in the original beyond this one call — PVS participation,
culling, draw order. A pixel measurement cannot see any of it and none is claimed.
