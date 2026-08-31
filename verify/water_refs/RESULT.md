# Forest + SuperG water references — the fold is a REGRESSION on both (2026-08-31)

**VERDICT: the water ambient fold, which I defaulted ON earlier today, is WRONG on
Forest and SuperG. On SuperG the unfolded standalone matches the original within 4-5 luma
and the fold pushes it 37-40 away.** Arctic still needs the fold. So the fold is not a
universal truth about water; it is compensating for something Arctic-specific.

This was the debt booked in `verify/city_blackroad/RESULT.md`. It is now measured, and it
did not come out the way the Arctic result predicted.

## Captures

Original side: `race_draw_burst.py --challenge N --settle M`, wrapped in
`run_with_unlocked_save.py` over a COPY of the save with all 13 rows unlocked
(`gamesave_edit.py --rows 0..12 --set c1=1,c3=2`). `original/gamesave.bin` never touched,
sha `bd18788182b2` verified before and after. Challenge index 9 = FOREST, 6 = SUPERG, both
confirmed by the tool's own `ORIGINAL TRACK =` readout rather than assumed.

Standalone: basis transplanted via `MASHED_CAM_POSE`, `MASHED_TRACK_SEL` 3 (Forest) / 7
(SuperG), arms differing only in `MASHED_LIBRW_AMBFOLD_SEA`. Three settle times per track
because the verdict has to be read on a WATER-DOMINANT pose — reading Arctic on a 2.5%-water
frame produced a confidently wrong call once already.

## Result — luma over the fold mask (`|foldOFF - foldON| > 6`, i.e. the water)

| vantage | water mask | original | foldOFF | foldON (shipping) | verdict |
|---|---|---|---|---|---|
| forest_s8 | 4.32% | 21.6 | 44.1 (Δ22.5) | 63.3 (Δ41.7) | fold WORSE |
| forest_s14 | 4.45% | 19.8 | 39.0 (Δ19.2) | 58.4 (Δ38.6) | fold WORSE |
| forest_s20 | 0.00% | — | — | — | no water in view |
| **superg_s8** | **26.89%** | **148.0** | **151.9 (Δ3.9)** | **187.8 (Δ39.8)** | **fold WORSE** |
| **superg_s14** | **41.78%** | **154.1** | **159.4 (Δ5.3)** | **190.8 (Δ36.7)** | **fold WORSE** |
| superg_s20 | 0.00% | — | — | — | no water in view |

SuperG is the authoritative row by the same standard applied to Arctic: 27% and 42% of the
frame is water. Unfolded, the standalone is within 4-5 luma of the original. Folded, it is
37-40 out. Visual confirmation `superg_s14_3way.png` — the mask lands exactly on the icy
sea around the dock, and the fold-OFF panel matches the original's pale ice while fold-ON
blows it out.

## The contradiction, stated plainly

| track | water mask | original | unfolded | folded |
|---|---|---|---|---|
| Arctic s8 | 69.63% | 28.0 | **9.1 (Δ18.9)** | 29.8 (Δ1.8) |
| SuperG s14 | 41.78% | 154.1 | 159.4 (Δ5.3) | **190.8 (Δ36.7)** |

Arctic's water is far too DARK unfolded and the fold fixes it. SuperG's is already right
unfolded and the fold breaks it. Same code, same class of surface, opposite conclusions —
so "non-lit prelit water needs ambient folded in" is NOT the rule. Something else explains
Arctic, and the fold was fitted to that one case.

**Lead worth following, not yet evidence:** no track declares `Ambient_RGB` in its
`COURSE.LUA`. All four checked (Arctic, Forest, SuperG, training) declare
`Lights_Filename("Lights.dff")`, and the decompiled loader `0x00479330` loads that light
clump and calls `RpWorldAddLight` for each of its lights (the `param_2[0x2640] != '\0'`
branch). So the original lights these surfaces with REAL RenderWare lights in the world.
Our manual prelit fold is a substitute for that mechanism, which is why it fits one track
and not others. Confirming this means reading how those lights reach a non-`rpGEOMETRYLIGHT`
atomic in the original, which has not been done.

## Secondary finding: Forest water is over-bright even unfolded

Forest foldOFF is 39-44 against an original 20-22, i.e. roughly double, before the fold adds
anything. That is a separate defect from the fold decision and is not explained here. SuperG
does not share it (151.9 vs 148.0), so it is not a general water problem.

## Consequence

~~The shipping default is a net regression on 2 of the 3 water tracks that now have
pose-matched references. This needs a decision before anything else lands.~~ RESOLVED below: fold defaulted OFF.

---

# Resolution 2026-08-31: fold DEFAULTED OFF

`MASHED_LIBRW_AMBFOLD_SEA` now defaults **OFF** (`=1` enables). Chosen because off is
correct on 2 of the 3 referenced water tracks, and the cost is stated rather than hidden:
**the Arctic sea ships too dark** (luma ~9 against the original's ~28). Filed as U-9064.

## Branch state — shipping default, no env vars, every referenced track

| track | session start | shipping | over threshold |
|---|---|---|---|
| Arctic s8 | 18.85 | **20.24** | 56.83% |
| City | 25.43 | **11.23** | 22.52% |
| Dump | 84.78 | **9.62** | 17.26% |
| TRAINING | 15.45 | **15.40** | 33.40% |
| Forest s8 | (new reference) | 15.37 | 24.70% |
| SuperG s14 | (new reference) | 39.19 | 40.95% |

Arctic's whole-frame number gets *worse* (18.85 -> 20.24). That is the Arctic sea going
back to unfolded, and it is the deliberate trade, not a surprise.

SuperG's whole-frame 39.19 is dominated by things outside this decision (HUD, car
positions); its WATER is within ~5 luma of the original unfolded, which is the number the
fold decision turns on.

## Process note — a self-inflicted redo worth recording

The first pass pruned captures with `find verify/water_refs -name "*.bmp" -delete`, which
deleted the **original reference** BMPs along with the standalone arm shots. The Forest and
SuperG references had to be re-captured. Re-capture is NOT bit-reproducible: `--settle` is
wall-clock, so the car is at a slightly different point and the camera basis differs
(Forest eye moved ~0.1u, SuperG ~1.0u). The new `orig.bmp` and the new
`orig_cambasis.txt` are therefore a MATCHED PAIR and must be used together; the standalone
arms were re-run against the new bases.

The fold-vs-original numbers in the table above this section are unaffected — each was
measured against its own contemporaneous reference and basis.

**Rule for next time: prune arm shots only, never `orig*`.** An original-side capture costs
a full unlocked-save game run; a standalone arm costs 90 seconds.
