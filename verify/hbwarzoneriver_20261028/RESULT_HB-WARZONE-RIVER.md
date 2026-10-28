# HB-WARZONE-RIVER RESULT (2026-10-28): the Warzone river gap is the reference frame's letterbox, not the port

**Verdict: GREEN, no code change.** The 10 to 16 luma Warzone gap in HB-TEXANIM-REFS (warzone_p3
16.5 / 16.7, warzone_p2 10.1 / 10.3) comes from the two original references, not from the port's
river. Both Warzone round-2 originals are letterboxed "Current Standings" frames: 114 rows have mean
luma below 1 (viewport rows 81..415). 25.7% (p3) and 20.4% (p2) of the PREREG2 water mask fall on
those black rows, where the original reads 0.0 luma. Outside the letterbox the port's river is
+2.3 (p3) and -2.1 (p2) luma from the original. That is inside the Training (3.3 / 3.6) and Sands
(3.9 / 3.9) residuals that HB-TEXANIM-REFS accepted with the knob ON. The Training and Sands
references have 0 black rows, so their scores are not affected.

- No source change, so no knob, no PREREG and no build. Gates and 13-track not run: the exe is
  `orch/integration` `49a5beb5a`'s, unchanged.
- No game process was started in this step (original or standalone). Everything below reads files
  already on disk under `D:/MashedWT/HB-TEXANIM-REFS_data/` and `verify/capfix_20261016/`.
- Script: [`measure.py`](measure.py). Output: [`measure_table.txt`](measure_table.txt).

## 1. The Warzone references are letterboxed

| case | rows with mean luma < 1 | 3D viewport rows |
|---|---|---|
| training_p3 / p4 | 0 / 0 | full frame |
| sands_p11 / p8 | 0 / 0 | full frame |
| warzone_p3 / p2 | 114 / 114 | 81..415 (white rule on rows 80 and 416) |

Both Warzone frames show the "MASHED / Current Standings" header and two standings panels with
`+1` / `-1` badges (rows 81..184, cols 0..269). The port's frames for the same poses show the start
lights, full frame. The round-1 Warzone grid references are split the same way: `warzone_s8` has 0
black rows, `warzone_s14` and `warzone_s20` have 99. Round 1 also has `sands_s20` at 114 black
rows. Round 1 scored none of those frames (mask under 0.5%), so no published number uses them.

What puts the original on that screen is not established here. The capture log's CARPROJ projects
all four cars off-screen on p3 and on-screen on p2 (`orig2/warzone_p{2,3}/capture.log`), and both
frames are letterboxed, so on-screen versus off-screen cars does not decide it. [UNCERTAIN] Missing:
the original's game phase at capture time.

## 2. The PREREG2 mask split at the letterbox (geometry unchanged)

| case | region | % of mask | original L | OFF L | ANIM L | OFF - orig |
|---|---|---|---|---|---|---|
| warzone_p3 | all (the PREREG2 number) | 100.0 | 54.9 | 71.4 | 71.6 | +16.5 |
| warzone_p3 | letterbox rows | 25.7 | 0.0 | 57.6 | 57.9 | +57.6 |
| warzone_p3 | outside letterbox | 74.3 | 73.9 | 76.2 | 76.4 | **+2.3** |
| warzone_p2 | all (the PREREG2 number) | 100.0 | 62.4 | 72.5 | 72.7 | +10.1 |
| warzone_p2 | letterbox rows | 20.4 | 0.0 | 57.9 | 58.2 | +57.9 |
| warzone_p2 | outside letterbox | 79.6 | 78.3 | 76.2 | 76.4 | **-2.1** |

Arithmetic check, p3: 0.257 x 0.0 + 0.743 x 73.9 = 54.9, the PREREG2 original value.

## 3. The 3D view inside the letterbox is a different projection

The original's letterboxed 3D view does not overlay the port's frame pixel for pixel. A grid search
of `orig(x, y) <- port((x - ox) / sx, (y - oy) / sy)`, scored by gradient correlation over original
rows 190..411:

| case | sx | sy | ox | oy | r | used |
|---|---|---|---|---|---|---|
| warzone_p2 | 0.810 | 0.790 | 24 | 13 | 0.818 | scored |
| warzone_p3 | 0.650 | 0.670 | 88 | 82 | 0.327 | not scored (r < 0.5) |

The two poses give different scales, and `orig_lens.json` records the view window
(1.0004, 0.7503) for a 640x480 raster on both, which maps at scale 1.0. So the recorded lens does
not describe the letterboxed view. [UNCERTAIN] Missing: which camera and raster draw the
letterboxed view.

warzone_p2, port resampled onto the original (mask eroded 3 px, standings panels and letterbox
removed, 22.81% of the frame): original 73.6, OFF 70.7, ANIM 70.9 (OFF - orig -2.8). By original
row band:

| original rows | n | original L | OFF L | OFF - orig |
|---|---|---|---|---|
| 190-214 | 2119 | 104.0 | 96.9 | -7.1 |
| 215-239 | 7656 | 103.0 | 89.4 | -13.6 |
| 240-264 | 8746 | 90.3 | 80.5 | -9.8 |
| 265-289 | 11182 | 77.9 | 73.9 | -4.0 |
| 290-314 | 12229 | 69.2 | 68.1 | -1.1 |
| 315-339 | 11698 | 62.3 | 63.5 | +1.2 |
| 340-364 | 10181 | 57.2 | 59.7 | +2.5 |
| 365-389 | 6275 | 52.6 | 56.5 | +3.9 |

The far rows are brighter on the original and the near rows on the port. This band pattern is not
a finding: the projection differs (above), so a band row does not sample the same distances on
both sides. It is listed so a later matched capture can check it.

## 4. Draw state of the river (13-track row 624, both sides already on disk)

`verify/capfix_20261016/11_Warzone/orig_row_00624.draws.json` against
`D:/MashedWT/HB-TEXANIM-REFS_data/tracks_flip/11_Warzone/stand/d1r2/stand_row_00624.draws.json`
(the L-42 flipped default build). The two river draws (84 and 16 triangles, same bounding boxes):

| field | original | port |
|---|---|---|
| texture | `water15`, then `water16` | `water16` |
| FVF | 322 (XYZ, DIFFUSE, TEX1) | 322 |
| ZENABLE, ZWRITE, ZFUNC | 1, 1, 4 | 1, 1, 4 |
| ALPHATEST, REF, FUNC | 0, 0, 5 | 0, 0, 5 |
| ALPHABLEND, SRC, DEST | 0, 5, 6 | 0, 5, 6 |
| CULLMODE | 2 | 2 |
| FOGENABLE, LIGHTING | 1, 0 | 0, 1 |
| stage 0 COLOROP / ALPHAOP, stage 1 COLOROP | 4 / 4, 1 | 4 / 2, 1 |
| vertex shader | 0 (fixed function) | 1 (librw shader) |

Texture, vertex format, depth and blend state match. The FOGENABLE, LIGHTING and stage-0 ALPHAOP
slots differ, but the port draws through librw's vertex and pixel shaders (`vs` = 1), and those
fixed-function states do not feed a shader draw. So the record does not show a fog or lighting
difference on the port. It does not rule one out either. That needs the shader constants, which
`DrawRec.cpp` does not record.

## 5. What this changes

- HB-TEXANIM-REFS section 6's follow-up ("Warzone river residual, OFF and ON both 10 to 16 luma
  brighter, unmeasured cause") is answered: it is the reference's letterbox. With the letterbox
  rows taken out, Warzone sits at +2.3 / -2.1, next to Training and Sands.
- The L-42 flip needs no revisit for Warzone.
- Any later Warzone water score must check the original for letterbox rows first. The `measure.py`
  section 1 check covers it.

## 6. Follow-ups (optional, not blocking)

- A finer Warzone river score (the distance bands in section 3) needs an original capture that is
  not letterboxed with the river in view: `re/frida/race_draw_burst.py --pose=<warzone_p2 basis
  from poses_r2.json>`, taken while the original still shows the race view (no "Current
  Standings" overlay). The basis is in `verify/hbtexanimrefs_20261027/poses_r2.json`. That is an
  original-side capture, so it is not done in this step.
- `DrawRec.cpp` could record librw's fog and ambient shader constants, so a draw-list diff can show
  shader-side fog and lighting.

## 7. Raw data (not committed), SHA-256

Inputs (from HB-TEXANIM-REFS, unchanged):

```
dbe3b7c1d97665ae99d671c607a8c867f9ed468c1406ef1a0f712044f13f6e53 *D:/MashedWT/HB-TEXANIM-REFS_data/orig2/warzone_p2/orig.bmp
44329ba43e7774b17d3dd972c2562184475f7cc9670f4e15c09ca38d5a0fa15e *D:/MashedWT/HB-TEXANIM-REFS_data/render2/warzone_p2_OFF/race1/01_grid.bmp
f3e9b7ddd66bbd9df9a854c64a5992555bc76f1e408de6bfe8a00f0a13ca1b88 *D:/MashedWT/HB-TEXANIM-REFS_data/render2/warzone_p2_ANIM/race1/01_grid.bmp
a7bbfd2d0e9285e2766ce6b40369ef0554e4ce7ebde5bca1c2ee0e08744c7d96 *D:/MashedWT/HB-TEXANIM-REFS_data/render2/warzone_p2_MASK/race1/01_grid.bmp
7b50ecd6e037b4da20713736f1e5523ae162694beccd2f4f40d602ef59f5a0b1 *D:/MashedWT/HB-TEXANIM-REFS_data/orig2/warzone_p2/capture.log
ea65c7abe658459501d4ff6d71100408342cf5de72d6e936624f28d54c853b34 *D:/MashedWT/HB-TEXANIM-REFS_data/orig2/warzone_p3/orig.bmp
02180a03469b0d54e7c61aef632bf52280cc3bacbbdf07116296d45720d95569 *D:/MashedWT/HB-TEXANIM-REFS_data/render2/warzone_p3_OFF/race1/01_grid.bmp
247a9c5bdc180e13af7792270b06839171cc4fbf39e769bcf2835419562bcbb9 *D:/MashedWT/HB-TEXANIM-REFS_data/render2/warzone_p3_ANIM/race1/01_grid.bmp
e098d7c33eca8b83cf03ddbf76fe3f1fedd5ef35d1f4e02f444a3839392c3dc5 *D:/MashedWT/HB-TEXANIM-REFS_data/render2/warzone_p3_MASK/race1/01_grid.bmp
c2a87aa8c5b793eab3fc553fb7ca836590cb14962c05aa8f29ff90188cf37773 *D:/MashedWT/HB-TEXANIM-REFS_data/orig2/warzone_p3/capture.log
52e015dd8a49d84ac2c5d6a1587f8b1d80a7bc4a40035d770d3473b460efda0e *verify/capfix_20261016/11_Warzone/orig_row_00624.draws.json
4a9086f78f6ca3d38f907ad8b40dfdd896216b1d02f93bce1813f2f7bada7058 *D:/MashedWT/HB-TEXANIM-REFS_data/tracks_flip/11_Warzone/stand/d1r2/stand_row_00624.draws.json
```

Made in this step, `D:/MashedWT/HB-WARZONE-RIVER_data/`:

```
fbb857f807e3c86a70a4cada4d1e35ba6b1dfbf9ea5d135ada6ce650e1345468 *warzone_p2_aligned.png
3d43fe9bb6ccb29d41b9890bc3c9b24af0abb7920cfd39ea6e3152b209436fb5 *warzone_p3_full.png
7b14215dc15de2dcb43089345c8df48f9a877805e9f4f32524040f3c2a1ce227 *warzone_p2_full.png
```

`warzone_p2_aligned.png` = original | port OFF resampled onto the original | original under the
scored mask. `warzone_p{2,3}_full.png` = original | port OFF, unmodified.
