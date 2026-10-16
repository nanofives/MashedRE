# RESULT rres: render residuals (C10 car-shadow render-to-texture, SuperG, leaf sprites)

Branch `work/rres`, rebased on `race/first-frame-parity` 7f5f2ed30 (contains txdscope and d3b). Model: claude-sonnet-5-5.
Instrument: the 13-track x 3-frame matched-pose draw-list replay (`verify/rsky_20261016/run13.py`, drawlist3d, 3D channel), private game root,
`MASHED_NO_DEPLOY=1` builds. PREREG: `PREREG_rres.md` (amendments 1 and 2). Evidence: `evidence/final_7f5f2ed30/` (the verdict arms) and
`evidence/prerebase_2cc15b2c/` (the same lanes measured before the rebase, on the 2cc15b2c base).

## Verdict

| Gate | Result |
|---|---|
| A1: 0 RED on 39 frames | **PASS**, 39 of 39 CITED in every arm (default, HUD replay, opt-out, tip baselines) |
| A2: G-CTRL unchanged | **PASS**, `d3b90ee0 / 80f01f35 / 56d62ae7` on the rres exe (`evidence/final_7f5f2ed30/gctrl_fin2.txt`), also on the pre-rebase exe |
| A3: no new row identities | **2 new identities, both cited** (see below), net rows -180 |
| A4: opt-out arm == baseline | **PASS**: per-track and per-frame row and matched counts equal on 13 of 13 tracks, row identities equal on 12 of 13 |
| A5: counts per track vs baseline | table below |

Rows, default arm (no HUD replay): tip baseline **1047 -> 867** (-180). HUD-replay arm (C08 removed by the harness): **813 -> 633** (-180).
Both lanes default ON, each with an opt-out. No Ai/, Vehicle/, Collision/ file touched.

| track | t0 tip | g0 rres | t1 tip +HUD | g1 rres +HUD |
|---|---:|---:|---:|---:|
| 01 Egypt | 67 | 60 | 49 | 45 |
| 02 Neustein | 64 | 58 | 46 | 40 |
| 03 Arctic | 103 | 103 | 85 | 85 |
| 04 Highway | 58 | 54 | 40 | 36 |
| 05 Sands | 70 | 66 | 52 | 48 |
| 06 SuperG | 210 | **73** | 192 | **55** |
| 07 Roundabout | 88 | 84 | 70 | 63 |
| 08 Storm | 69 | 69 | 51 | 51 |
| 09 Forest | 57 | 53 | 39 | 35 |
| 10 Dump | 60 | 56 | 42 | 38 |
| 11 Warzone | 79 | 72 | 61 | 54 |
| 12 City | 59 | 59 | 41 | 41 |
| 13 Training | 63 | 60 | 45 | 42 |
| total | 1047 | **867** | 813 | **633** |

By cite (t0 -> g0): C10 38 -> 0, C19 132 -> 50, C21 91 -> 40, C03 39 -> 36, C17 16 -> 15, C01 10 -> 8, C02 4 -> 1, C25 2 -> 0, C29 1 -> 0, C31 0 -> 3 (new, below). Everything else unchanged
(C08 234 is the harness, 0 under `--hud`).

## What changed in the default build

1. **Car shadow render-to-texture (C10), `MASHED_CAR_SHADOW_RT=0` opts out.** `LibRw/RwRaceFx.cpp` `RaceFx_ShadowPass` / `RaceFx_ShadowQuads`, called from `RwRaceSubmit.cpp` per car like
   `FUN_00420050`: a parallel RT camera (window 0.75, `carpos + lightAt * -4.0`), a 256x256 or 64x64 A8R8G8B8 camera texture (`rec+0x298 == 2` picks 64), the hull part 0x3b drawn black
   with Z test and write off, up to 0x40 ground triangles collected in a 1.5 sphere (`FUN_00422570`: normal.y >= 0.75, plane distance 1.0 in front / 0.1 behind, y += 0.01, cache re-query past
   sqrt(0.125) at `0x005cd318`, alpha `trunc(153.0 [0x005cd320] * rec+0x288 * rec+0x284)`), then `FUN_004228f0`'s Im3D quads with the RT raster and UV `0.5 - d.axis / 1.5`. Gate: course not 0 / 0x1a / 0x24.
   The hull is baked into the body frame (hull frames hang off the body, not the clump root: 2e-4 vs 4e-3). The record LOD state uses the previous frame's eye (`MASHED_CAR_LOD_EYE=last` restores this frame's).
   Result: C10 38 -> 0 on all 13 tracks, the C03 hull rows go with it, the car-LOD rows C02 / C25 drop (the boundary Neustein 1169 car). Pixel witness: `evidence/shadow_zoom_700.png`
   (original, shadow-on, shadow-off, amplified difference: the original's dark band under the rear and the right arch appears only with the pass on).
2. **SuperG, `MASHED_LIBRW_SUPERG_VA=0` and `MASHED_SUPERG_REFLECT_FIRST=0` opt out.** SuperG's 210 rows were C19 96, C21 51, C08 18, C16 9, C05 8, C06 6, C07 5 and 15 small ones. Two causes ported:
   - **Sticky vertex alpha** (F3 extended to Course_Id 25): SuperG shares Arctic's custom world-sector callback (`SetSectorCallback` `FUN_00496d00`, callers `0x00448951` and `0x00450f35`),
     so its sectors inherit the alpha-blend state the frame's screen rectangle `FUN_00450b10` (`0x004271e8`) leaves. C19 -28.
   - **Reflections.dff drawn first, with CULL NONE.** The 81 draws that open the recorded frame (before the screen rectangle at draw 81) are clump 1 `Reflections.dff`, which slot 4 `FUN_00451090`
     renders between its reflection camera's update calls (`FUN_004c1a00` / `FUN_004c19f0`, `RpClumpRender(*(course + 0x10114))`) under the cull state the previous frame's HUD pass left. The standalone drew
     it after the world with CULL BACK. New instance kind `kInstPreWorld` draws it ahead of the world walk with the hook entry's cull 1. C21-ORDER 51 -> 0, C19 CULL 54 -> 0 (SuperG: 210 -> 178 after the shadow and sticky-alpha lanes, 178 -> 73 with this one).
   SuperG 210 -> 73 (-137, -65%), the whole lane 0 RED, instrument gates for SuperG PASS.
   This lane was not in the PREREG amendment 2 predictions (found while classifying the rows after the finals were first measured); its numbers are measurements, not predictions.
3. Not ported: the reflection render target itself (`FUN_00484000(0,0x100,0x100,0x600)`) and its consumers (the Wet / Ice two-stage world materials, C16 9 rows), the shadow's switch-table colour exclusion, the leaf-sprite RpPTank pass.

## New row identities (A3) and gate changes, stated plainly

- `C31-SPLIT-PAIR` (Egypt, default arm only, 3 frames): once the hulls match, the original-side untextured residual is three 2-triangle draws (a 512x512 screen quad and two start-display pieces, C08) and the
  standalone residual is the 6-triangle C29 ground patch. They sum to the same 6 triangles with the same texture name, so drawlist3d pairs them as one SPLIT row. The HUD-replay arm removes the original side and the
  row; cite added in `cites_rres.json` with the draws listed in `evidence/egypt_split_pairing.txt`. Not a defect.
- SuperG frame 818, `STATE Camstand` (C19, 1 row, both arms): the original's sticky alpha-blend state is cleared mid-run by the `PowerUpModelsFinal` draw (original draw 120, tied into the world), the standalone's
  pickups are D3D9 draws elsewhere (C06), so 14 tail rows on that frame carry the opposite polarity. Net SuperG is -137. Tied to U-9737.
- **Instrument gate I-NAME (>= 95% of textured draws named) now fails on the standalone side at Neustein row 1169 (55/59) and, default arm only, Highway row 928 (73/77 = 94.8%).** The four extra unnamed draws are the
  shadow quads, whose render textures carry the original's own `#256x256:21` / `#64x64:21` unnamed-raster names (the original side counts the same four, and already failed I-NAME at Neustein 1169 before this lane).
  Highway 928 fails by one draw because the standalone frame holds 82 draws against the original's 131. The gate was not relaxed. User decision below.

## Methods notes worth keeping

- The instrument's per-texture labelling of STATE rows is **not deterministic across runs of the same exe** (`06_SuperG`, base exe twice: totals 72 / 73 / 65 equal, `Box 8 -> 19` style label shifts). Compare row and matched
  counts, not row identities, on SuperG.
- Hull frames, LOD state and the shadow UV were each fixed against a measured defect (4e-3 hull AABB offset, one boundary car, stale `viewMatrix` at collect time), recorded in PREREG amendment 2.
- Disk reached 0 B once during the lane: runs that wrote during that window were re-run, private roots moved off the Temp scratchpad (its `original\` contents vanished twice).

## Follow-ups (U-rows in UNCERTAINTIES.md)

U-9732 LOD-state camera (previous eye measured, mechanism not decoded). U-9733 SuperG reflection render target and Ice / Wet consumers. U-9734 shadow collect: switch-table colour exclusion not modelled. U-9735 shadow cache
anchor under replay (reset at replay row 0). U-9736 instrument I-NAME counts unnamed rasters. U-9737 SuperG sticky-alpha reset by in-world pickups (C06 coupling). U-9708 (SuperG part) and U-9701 notes updated.
