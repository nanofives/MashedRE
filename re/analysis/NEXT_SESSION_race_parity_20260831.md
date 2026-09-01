# Kickoff for the next race-parity session (written 2026-08-31)

Follows `NEXT_SESSION_race_parity_20260830.md`, whose one blocker was **T-ARCTIC**: no
pose-matched original Arctic reference existed, so `race/geomlight` could not be judged.
That blocker is gone. Branch: **`race/first-frame-parity`**.

Paste the block at the bottom into a fresh session. Everything above is context for a human.

## The one-line summary

Two real defects were found and fixed, one shipped fix was **reverted the same day** when
better evidence arrived, and the thing that started it all — the Arctic sea — turned out to
be a **fog bug**, diagnosed but deliberately not yet fixed. That fix is the next slice.

## Parity movement this session (imgdiff 8x6 vs pose-matched originals)

| track | start | now | why |
|---|---|---|---|
| City | 25.43 | **11.23** | `Clump_Exclude_From_World` fix |
| Dump | 84.78 | **9.62** | same fix |
| TRAINING | 15.45 | **15.40** | headline held throughout |
| Arctic s8 | 18.85 | 20.24 | *worse* — accepted cost of reverting the fold |
| Forest s8 | — | 15.37 | new reference |
| SuperG s14 | — | 39.19 | new reference |

## Landed, with evidence

1. **Water fold scoped by DFF asset name** (`race/geomlight` merged, `89569084`). RW flags
   cannot tell water from an awning: `numTexCoordSets<=1 && !lit && prelit` also matches
   City's awning/lamppost and Dump's skydome. Keyed on `SEA/WATER/LAKE/RIVER` prefix ANDed
   with the flag class, plumbed via `DffModel::source_name`.
   **Default OFF** — see Corrections. `verify/geomlight_waterfold/RESULT.md`.

2. **`Clump_Exclude_From_World` DECODED and fixed** (`7735c7d4`). It never meant "do not
   load". Handler `0x0047aa20` only sets `desc[0xd4 + idx*4] = 1`; loader `0x00479330` loads
   every named clump; the flag gates exactly one call, `0x004e4450(world, clump)` at
   `0x00479db3`, which registers the clump into the world. `0x00474fd0` (get-first-atomic)
   runs either way and its handle lands at `+0x120`. Misreading it deleted City's road + 4
   buildings + Standard + Trunk + water, and Dump's road. **Default ON**,
   `MASHED_TRACK_SKIP_EXCLUDED=1` reverts. `verify/city_blackroad/RESULT.md`.

3. **`race/arctic-cap` merged** (`695525dc`) — the pose-matched references and
   `race_draw_burst.py --challenge`.

4. **U-9062 / U-9063 closed** as one decoded root cause; **U-9064 filed** for the Arctic sea.

## THE NEXT SLICE — fix the librw fog colour

Full analysis: **`verify/water_refs/U9064_MECHANISM.md`**. Short version:

- Arctic `SEA.DFF` prelit mean luma **28.6**; original renders **28.2**; we render **6.3**.
  We DARKEN it ~4.5x. Texture modulation explains that (`sea` texture mean luma 66.9,
  factor 0.263; 28.6 x 0.263 = 7.5).
- The original's brightness is **FOG**: `Setup_Fog(0.1,70,40,44,48)`, colour luma 43.3.
- Our fog contributes **+0.0** on the sea, and `MASHED_NO_FOG=1` changes **0.00% of pixels
  whole-frame**. Fog is inert everywhere in the librw path.
- Already instrumented, never chased —
  `P7 fogcolor: d3d9=282C30 librw_ps=(0.0000,0.0000,0.0000) *** MISMATCH ***`
  (`RwRaceSubmit.cpp:886`, log kept at `verify/water_refs/librw_race_arctic_fogprobe.txt`).

**Prime suspect, `[UNCERTAIN]` not asserted:** `SetRenderState(rw::FOGCOLOR, ...)` is issued
at `RwRaceSubmit.cpp:524`, **before** `g_cam->beginUpdate()`. The fog RANGE had exactly this
bug — `beginUpdate` welds `fogData.end` to the far plane, and the `[I4-fog CLOSED]` fix
(`RwRaceSubmit.cpp:614-627`) re-issues the range **after** it, commented *"Must follow
beginUpdate, not precede it."* **The colour was never moved.** One edit, one run, and the
`P7 fogcolor` line reads the verdict directly.

Expected consequences, to be measured not assumed: Arctic's sea rises toward ~28 without any
fold; the water fold may become unnecessary everywhere; **every fogged track changes**, so
re-run all six references. TRAINING's 15.45 is the guard — it declares `Setup_Fog(20,360)`.

Also open in the same area: shader `start=5.000` against the requested `0.1`.

## Corrections — four claims of mine that later evidence overturned

Recording these because each was stated confidently before it was checked.

1. **"The water fold is safe, default it ON."** Wrong. It is a regression on Forest and
   SuperG. SuperG unfolded is within **4-5 luma** of the original; folded it is **37-40**
   out, on a 27-42% water frame. Reverted (`3676040b`). The fold was fitted to Arctic and
   matched its *average* missing fog lift on one frame.
2. **"The other tracks' water may be world/BSP geometry."** Wrong. It was the
   `Clump_Exclude_From_World` skip. Every water clump in the game except Arctic's `sea.dff`
   is excluded, which is why Arctic was the fold's only possible target — structural, not a
   camera coincidence.
3. **"U-9062 is a black road, U-9063 is a white sky."** Both wrong: one bug, and the
   surfaces were MISSING, not mis-lit. Dump's "sky" was the background through the road hole.
4. **"Non-lit prelit water needs ambient folded in."** Wrong; it needs fog.

## Traps that cost real time — do not rediscover

- **`verify/**/*.bmp` is gitignored.** A blanket `find ... -name "*.bmp" -delete` destroyed
  freshly captured ORIGINAL references. Re-capture is **not** bit-reproducible (`--settle`
  is wall-clock; the SuperG eye moved ~1.0u), so `orig.bmp` + `orig_cambasis.txt` are a
  MATCHED PAIR and every arm measured against the old one must be re-run. **`git add -f` any
  original reference immediately after capture; prune arm shots only, never `orig*`.**
- **The demo hangs at challenge-select roughly 1 run in 5.** Hit 4+ times on unrelated arms.
  All capture drivers now retry 3x. Never read a hang as a result.
- **A 0.00% diff mask has two causes** — "fired and was inert" vs "never ran". Instrument
  the gate (`BuildClump` now logs `dff= water_asset=`) before reporting a null as good news.
- **Judge water on a WATER-DOMINANT pose.** A 2.5%-water frame produced a confidently wrong
  Arctic call once. Capture 3 settle times and pick by mask fraction.
- **Render through BOTH renderers early.** `MASHED_RENDER_LIBRW=0` agreed within 0.4 points
  on City, which killed the entire lighting-asymmetry family in one run.
- **Don't name a scratch dir `verify/u9062_*`** — `遢` is parsed as a Unicode escape and
  mangles the path.
- **Ghidra MCP was not wired in.** `analyzeHeadless` read-only on a pool slot is Ghidra on
  the same binary and satisfies the no-documentation-fallback rule. Recipe in memory
  `ghidra-mcp-down-use-analyzeheadless`.
- **PowerShell ordered dicts with integer keys** index positionally, not by key. Silently
  collapsed 12 runs into 3 colliding dirs.

## Housekeeping state

- `original/gamesave.bin` pristine, sha `bd18788182b2`. The unlock only ever touched a copy.
- Ghidra pool slot 14 released; `.pool_slot` removed.
- Working tree clean. Six pose-matched references now exist: Arctic (s8/s14), City, Dump,
  TRAINING, Forest (s8/s14/s20), SuperG (s8/s14/s20).

---

## PASTE BLOCK

```
Fix the librw fog colour, which is the real mechanism behind U-9064 (Arctic sea too dark).
Branch race/first-frame-parity, repo C:\Users\maria\Desktop\Proyectos\Mashed.

Read verify/water_refs/U9064_MECHANISM.md first. Established there by measurement:
Arctic SEA.DFF prelit mean luma 28.6, the ORIGINAL renders 28.2, we render 6.3; texture
modulation explains the 6.3 (sea texture mean luma 66.9); the original's brightness is FOG
(Setup_Fog(0.1,70,40,44,48), colour luma 43.3); and our fog contributes +0.0 -- MASHED_NO_FOG=1
changes 0.00% of pixels whole-frame, so fog is inert everywhere in the librw path.
log/librw_race.txt already reports it:
  P7 fogcolor: d3d9=282C30 librw_ps=(0.0000,0.0000,0.0000) *** MISMATCH ***

Prime suspect, marked [UNCERTAIN] and NOT yet confirmed: SetRenderState(rw::FOGCOLOR,...) at
RwRaceSubmit.cpp:524 is issued BEFORE g_cam->beginUpdate(). The fog RANGE had exactly this bug
and its [I4-fog CLOSED] fix (RwRaceSubmit.cpp:614-627) re-issues the range AFTER beginUpdate,
commented "Must follow beginUpdate, not precede it." The colour was never moved.

Step 1: move/re-issue the FOGCOLOR write after beginUpdate, rebuild (mashedmod\build.bat),
run Arctic, and read the P7 fogcolor line. Mismatch clearing is the verdict -- do not infer it
from pixels. If it does NOT clear, the suspect is wrong; find what actually writes
rw::d3d::d3dShaderState.fogColor and say so rather than patching around it.

Step 2: re-measure ALL SIX references, because every fogged track changes. Drivers exist:
verify/water_refs/run_final.ps1 and verify/city_blackroad/run_excl_sweep.ps1 (both retry 3x;
the demo hangs at challenge-select ~1 run in 5 -- never read a hang as a result). Guard rails:
TRAINING must hold at 15.45 (it declares Setup_Fog(20,360)); City 11.23 and Dump 9.62 must not
regress. Target: Arctic s8 sea rises from 6.3 toward the original's 28.2 with
MASHED_LIBRW_AMBFOLD_SEA left OFF.

Step 3: if fog alone fixes Arctic, the water ambient fold is dead code -- say so and remove it
rather than leaving a flag that is never right. Then close U-9064 via re-classify.

Rules that cost time this session:
- verify/**/*.bmp is GITIGNORED. git add -f any ORIGINAL reference right after capture, and
  prune arm shots only, never orig*. Re-capture is not bit-reproducible (--settle is wall-clock),
  so orig.bmp + orig_cambasis.txt are a matched pair.
- Judge water on a WATER-DOMINANT pose; a 2.5%-water frame gave a confidently wrong verdict once.
- Ghidra MCP is not wired in; use analyzeHeadless read-only on a pool slot (memory
  ghidra-mcp-down-use-analyzeheadless). Run scripts/ghidra_assert.sh preflight first.
- NEVER touch original/gamesave.bin (sha bd18788182b2). Original-side captures use
  re/tools/gamesave_edit.py on a COPY wrapped in run_with_unlocked_save.py.
Stop and ask before flipping any default: a default was flipped and reverted the same day this
session because it was decided on one track's evidence.
```
