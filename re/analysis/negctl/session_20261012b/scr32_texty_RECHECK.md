# scr32 / scr19 text-y fix (cfa6eb8d): independent recheck on claude-opus-5-5 (2026-10-13)

## Verdict

**CONFIRMED.** Removing the `+0.055 * cell_h` term from `DrawMashedString` (and the `- 1.0f * S`
compensator at the scr19 insults value text) is correct. I re-derived it from the bytes, captured the
original's glyph vertices fresh with my own probe, and measured it with a second witness that does not go
through the camera projection. Every number the previous child reported that I re-ran reproduces, to
4 decimals on the vertex diff.

Two corrections to the previous note, neither of which changes the verdict:

1. **Coverage was smaller than it read, and the regression list was incomplete.** The previous sweep
   measured 7 screens. This one measures **33** (every tabled screen id 0..33 except 27) and **14 of 47**
   `DrawMashedString` call sites. Note: there are 47, not 48. Six call sites now show a growing |dy|,
   not three. The three new ones are partial: a screen-18 row/value pitch error (lines 6481, 6486) and
   the scr15 header tabs (line 5727). Each one was already wrong in the original geometry before the fix.
2. **"Identical ink boxes" on scr32 depends on the boot.** Its best-overlap shift goes from −1 to 0 in
   both sessions. But my standalone boot puts the value-text ink top row at 234 against the original's 233,
   while the previous child's boot hits 233. The noise floor is measured below.

Do not merge on my word alone. The three call sites the brief named have exact per-site fixes from the
bytes (section 4), and the prompt-strip shadow fix is a one-constant change. Applying it would turn the
largest every-screen regression (816 glyphs on 33 screens) into 0.0000.

## 1. The y law, from the bytes (no inference)

Disassembly of `original\MASHED.exe.unpatched` (anchor SHA-256 `BDCAE093...`), `bytes_text_y.txt`, plus
the decomp `decomp_text_pipeline.txt`.

- `FUN_00427680` (args `x`, `y`, `align`, `scale` at `[esp+0xc..0x18]` after `sub esp,8`; out pointer in
  ESI): `0x0042774b` `fmul [0x005cc560]` (`0x3b088889`, 0.00208333344), `0x00427751` `fmul y`,
  `0x00427755` `fdiv H`, `0x00427759` `fmul [0x0067d834]`, `0x0042775f` `fsubr [0x005cc320]` (1.0),
  `0x00427769` `fmul [0x005cc9a4]` (`0x3ccccccd`, 0.025) by scale, `0x0042776f` `fsubp`,
  `0x00427771` `fstp [esi+4]`. So `pen.y = 1.0 - ((H*0.00208333344*y)/H)*[0x0067d834] - scale*0.025`.
- `FUN_00427e00` passes `scale*[0x005cd5fc]` (`0x3d90ff97`, 0.0708) to the text thunk (`0x00427eae`).
  This is `s'`. `FUN_00556ca0` dispatches through font+0x138, and the glyph submits come from
  `FUN_00554940` (the 0x004cd070 return addresses in the capture).
- `FUN_00554940`: `FUN_00552df0(pen.x, pen.y)`, then `FUN_00552da0(s', s')`, then `FUN_00552df0(0, -[font+8])`,
  then `FUN_00552e40()`. `FUN_00552df0` = `RwMatrixTranslate` `0x004c51a0` and `FUN_00552da0` =
  `RwMatrixScale` `0x004c5010`. Both are pushed with combine `1` (pre-concat) at `0x00552e13` / `0x00552dc3`.
  The glyph-local y written is `0` (`pfVar8[1]`) and `1.0` (`pfVar8[10]`). A local point therefore lands at
  `n = pen + s'*(local - (0, c8))`.
- The four text entry points that call `FUN_00427680` (E8 scan of `.text`): `0x00427e00`, `0x00427f00`,
  `0x00427ff0`, `0x00428140` (call sites `0x00427e9c`, `0x00427f90`, `0x0042809c`, `0x004281b1`).

Closed form, with pixel = `(1 - n.y) * 480`: `top_px = y*[0x0067d834] + cell_h*(0.025/0.0708 + c8 - 1)`,
`cell_h = scale*0.0708*480`. **No extra term.** This is the post-fix standalone expression exactly.

## 2. Fresh witnesses

**Probe** `recheck_frame_probe.py` (mine). It records one frame between two entries of `0x0042e3a0` and
hooks all four text entries plus `FUN_00427680` entry and leave. From that hook it gets the pen written at
`[esi]` / `[esi+4]`, `[0x0067d828..834]` and `[[0x0067d838]+8]`. It also hooks Im3D `0x004cd070` filtered to
`0x00554940..0x005551ca`. The screen is pushed with `FUN_0043d2a0(screen, 0)` and `[0x0067ecb0]`, with no
key input. Each run killed only the PID it spawned. All 33 original runs succeeded on the first attempt, so
no piz-collision re-runs were needed.

**Projection self-check** `orig_law_check.py`. Two predictions are made for every submitted vertex on all
33 screens: (a) the camera projection, `p = local*ltm`, `p' = p*view`, `(x/z*640, y/z*480)`, and (b) the
camera-free byte law from section 1. They agree within **2.9e-5 px** (worst screen 18). The closed form
matches both to 1e-4.

- `[0x0067d830]` and `[0x0067d834]` read `0x3f7fffff`, and `c8` reads `0x3e1b26ca` (0.151515), on every
  text call of all 33 screens.
- Camera: proj 1, raster 640x480, view diag −0.625 / −0.625 / 1.
- scr32 id `0x59`, y 240.0, s 0.7 (ret `0x431426`): quad top **228.2156** from all three, pen
  `(0.587500, 0.482500)`.
- s 0.8 menu row (`0x43d164`, y 240): top 226.5321.
- s 0.6 strip (`0x43d25a`, y 428): top 417.8990.

**Second witness: ink** (`ink_check.py`, `value_text_ink_box.py`). This compares the original's backbuffer
ink against the standalone's on the same FGDC20 atlas cells. It does not compare ink against a law, which is
the biased use. The integer-shift IoU and the ink-centroid delta are independent of the projection.

The noise floor, measured on scr32 value text:

| comparison | IoU | centroid |
|---|---|---|
| original vs original across boots | 0.949 | +0.08 px |
| two standalone post boots | differ by 9 ink px | top row 233 vs 234 |

## 3. Agreement with the previous child's numbers

Vertex-level dy (standalone − original, px at 640x480). The previous child's capture versus my fresh
original, measured on a fresh pre build (parent `5c144c99`) and a fresh post build (HEAD).

| item | previous | recheck | agree |
|---|---|---|---|
| watermark `0x43c6c7/0x43c6f8` s 0.8 pre→post | +1.4953 → 0.0000 | +1.4953 → 0.0000 (32 screens) | yes |
| menu rows `0x43d164/0x43d10c` s 0.8 | +1.4953 → 0.0000 | +1.4953 → 0.0000 (20 screens, excl. scr18/24 row 0x150) | yes |
| title / prompt `0x43d25a` s 0.6 | +1.1215 → 0.0000 | +1.1215 → 0.0000 (33 screens) | yes |
| title shadow `0x43d239` y 51 | (in +1.1215 family) | +1.1215 → 0.0000 | yes |
| scr32 value `0x431426` s 0.7 | +1.3084 → −0.0000 | +1.3084 → −0.0000 | yes |
| scr19 value `0x431202` s 0.7 | +0.3084 → +0.0000 | +0.3084 → +0.0000 | yes |
| prompt shadow `0x43d239` y 431 | −0.8785 → −2.0000, dx −2 | −0.8785 → −2.0000, dx −2.0000 (33 screens) | yes |
| scr6 `0x439aec` id 0x49 | −3.0985 → −4.2474 | −3.0985 → −4.2474 (scr 6 and 7) | yes |
| scr6 `0x439aec` ids 0x4a..0x4c | −3.4352 → −4.5842 | −3.4352 → −4.5842 | yes |
| scr15 `0x43a9b5..0x43aa0c` | −0.6281 → −2.0336 | −0.6281 → −2.0336 | yes |
| per-screen mean abs(dy) 1/6/8/15/19/30/32 | 1.3310→0.1322, 1.9112→1.8421, 1.3002→0.2913, 1.0567→0.8214, 1.2360→0.3107, 1.2466→0.1667, 1.1799→0.4507 | identical, all 14 values | yes |
| `[0x0067d830]`/`[0x0067d834]` | 0x3f7fffff (scr19 x2, scr32 x2) | 0x3f7fffff on 33 screens | yes |
| scr32 value ink, best shift | (0,−1) 0.865 → (0,0) 0.910 | (0,−1) 0.833 → (0,0) 0.872. Centroid +1.596 → +0.247 | shift yes |
| scr32 value ink, boxes after | "identical" y 233..246 | standalone y **234**..246 against original 233..246. Crossing sessions: the previous standalone image gives 233 against my original, and my standalone gives 234 against theirs | **boot-dependent** |
| scr19 value ink | not run | (0,0) 0.796, centroid +0.327 → (0,0) 0.840, centroid −0.054 | n/a |
| draw lists scr19/30/32, anim4 / strict / 0.01 px | widget RED 0 (post only) | widget RED 0 on all 24 (pre and post), against fresh original captures (19 x2, 30, 32). Pre and post counts identical in every diff | yes |
| draw lists `other` rows | scr30 anim4 other 2 (preview quads) | scr30 anim4 other 0. The preview quads `0x474bd3`/`0x474d0c` appear in my scr19 boot `orig19a` instead | boot-dependent, not text |

Ink witness on the clean regions (`ink_check_all.txt`, standalone − original):

| region | pre: best shift / centroid dy | post: best shift / centroid dy | IoU pre → post |
|---|---|---|---|
| menu rows `0x43d164` | (0,+2) / +1.61..+1.82 | (0,0) / −0.07..+0.17 | 0.80..0.87 → 0.93..0.98 |
| white prompt `0x43d25a` | (0,0) / +0.62..+0.77 | (0,0) / −0.01..+0.08 | 0.56..0.62 → 0.98..0.996 |
| white watermark `0x43c6f8` | (0,+2) | (0,0) | 0.55..0.60 → 0.89..0.99 |
| scr15 tabs | (0,−1) / −0.94..−1.20 | (0,−2) / −2.02..−2.07 | worse, same as vertex |

**Defect in the previous probe.** It hooked only `0x00427e00` and `0x00428140`, so glyphs from
`0x00427f00` / `0x00427ff0` calls were tagged to the preceding call. Two cases: scr15's player numbers at
ret `0x43a8f1`, and scr9/16's `0x43ae75`. This did not touch any shared-law number above.

## 4. The three regressed call sites: what the standalone gets wrong, and the fix (NOT applied)

**(a) Prompt-strip shadow, ret `0x0043d239` (every screen). Standalone `exe_main.cpp:4704-4706`.**
The original adds 3.0 to both coordinates: `0x0043d209` `fld [esi-0x10]` (y) then `0x0043d20f`
`fadd [0x005cc31c]` (`0x40400000` = 3.0), and `0x0043d227` `fld [esi-0x14]` (x) then `0x0043d22a`
`fadd [0x005cc31c]`. The result feeds `call 0x00428140` at `0x0043d234`. The same path draws the title
shadow, and the standalone already has that one right: line 4917, `sh = 3.0f`, dy 0.0000.

What is wrong: `ob = 1.0f * kVScale`. Measured dx −2.0000 and dy −2.0000, with x, scale and cell all
exact (dh 0.0000).

**Fix:** `const float ob = 3.0f * kVScale;`, citing `0x0043d20f` / `0x0043d22a`. Predicted dx and dy go to
0.0000, because the pen is base + (3, 3) on all 33 screens (pens `(67, 431)` vs `(64, 428)`). The comment
"#4 CORRECTED 2026-06-14 ... SINGLE thin drop-shadow" was an ink judgement, and the bytes say 3 px.

**(b) Screen 6/7 cup track names, ret `0x00439aec`. Standalone `exe_main.cpp:6199` (and its `cy`, `lcell`).**
The original call is at `0x00439ae7`: `FUN_00427e00(row + 0x49, 0x42800000, y, colour, scale, 0)`.

- **x:** `0x42800000` = 64.0, pushed at `0x00439ae1`. The standalone uses 64.375 and gets dx +1.475..+3.853
  (the cell also differs).
- **scale:** `0x3f19999a` (0.6) when `row == [0x0067f17c]` (`0x00439a9b..0x00439aa5`), otherwise
  `0x3f147ae1` (0.58) (`0x00439aaf`). The standalone uses `lcell` = 0.6147 on every row, giving dh +0.4996
  (selected) and +1.1792 (others).
- **y:** stored at `0x004399bc` and read as the arg at `0x00439ac8`. It is
  `22*row + [0x0067f180] + 118.0` (`0x00439787` `fadd [0x005cda68]` = `0x42ec0000`), plus
  `((7-n)*20)/2` when `n < 7` (`0x0043975c` `cmp n,7`, `0x00439772..0x0043977d`). Here `n` counts the
  available entries and the row step is `+0x16`. Measured pens: y 148/170/192/214, i.e. n = 4 and
  `[0x0067f180]` = 0. The standalone uses `row0 = 144` with `rowdy 22`, an anchor 4.0 px high.

**Fix:** x `64.0f`, cell `(i == [0x0067f17c] ? 0.6f : 0.58f) * 0.0708f * 480.f`, and
`y = 22*i + scroll + 118.0f + (n < 7 ? ((7-n)*20)/2 : 0)` (integer division as at `0x0043977d`).
Predicted post dy 0.0000 for all four rows. The pre-fix `row0 = 144` and `lcell = 0.6147` pair was fitted
to ink, and that is why it read close while +0.055 was present.

**(c) Screen 15 header tabs, rets `0x0043a9b5 / 0x0043a9d1 / 0x0043a9ed / 0x0043aa0c`. Standalone
`exe_main.cpp:5706` and `5727`.**
The original `FUN_0043a610` at `0x0043a989..0x0043aa07` makes four calls
`FUN_00427e00(id, x, 0x431c0000, colour, 0x3f400000, 2)` for ids `0xe3`/`0xd2`/`0xd3`/`0xd4`, with x
`0x42fa0000`/`0x437f0000`/`0x43c08000`/`0x4400c000` = 125/255/385/515, y 156.0, scale 0.75 and align 2.

What is wrong: the standalone anchors at `hy + hh*0.5` = 140 + 14 = 154 (−2.0 px) with `hcell` 0.752,
giving dh +0.0680. Its x already lands on 125/255/385/515 (dx within ±0.01).

**Fix:** `DrawMashedString(lbl, hx + hw*0.5f, 156.0f*kVScale, 0.75f*0.0708f*480.f*kVScale, ...)` for
the ability path, citing the pushes at `0x0043a98b`/`0x0043a9a1`. Predicted dy 0.0000. The team path
(`FUN_0043aa30`, screen 16, rets `0x43ab7a`/`0x43abb4`, scale 0.8 measured) shares line 5727. There the
fix improves dy (+2.2137 → +0.8081) and its cell is 0.8, not 0.752 (dh −1.6312). It needs its own
y/scale from `FUN_0043aa30` [UNCERTAIN: its pushes were not disassembled in this session].

## 5. Coverage

The original's frame was captured on **33 screens** (0..33 except 27, which has no table:
`PTR_005f7638[27] = 0`). The standalone ran with `MASHED_GOTO=<id>` and `MASHED_SETTINGS_DEFAULTS=1` on
the same 33. `sites_table.md` lists every original call site.

**Standalone call sites measured: 14 of 47** (survey of `exe_main.cpp`, every non-definition call):
4598, 4600, 4705, 4707, 4917, 4927, 5097, 5127, 5360, 5727, 5898, 6199, 6481, 6486.

Two of those carry caveats. Line 5360 (scr4, ret `0x433efc`): 3 of 4 glyphs matched, at dy +19.2198 →
+18.1918, dx +4.85, cell 0.55 vs the original's 0.6. A 19 px offset is inside the matcher's 40 px window
for a repeated digit cell, so treat that pairing as [UNCERTAIN]. It improves either way. Line 5898
(ret `0x43a8f1` on 15, `0x43ae75` on 9/16): dy +2.8698 → +1.8418, which improves. The cell is 0.55 vs 0.6
(dh −1.6992) and dx is +5.0 / +1.0.

The other 33 are not reachable from a menu screen without input or race state:

| unreachable call sites | what they draw |
|---|---|
| 2955, 2980 | helpers |
| 3800..4065 (16 sites) | race, results, pause |
| 4458..4489 (4) | splash |
| 4544, 4546 | title |
| 5008, 5170 | arrow fallbacks, inactive when the arrow sprite is ready |
| 5986 | team verdict, port-only |
| 6218, 6261, 6265 | screen 6/7 detail panel. The standalone draws 5 lines at y0 270..356, the original's settled frame has no text call there |
| 6741..6761 (4) | modal |

Call sites whose |dy| grows on at least one screen (pre → post, px):

| site | original ret | screens | pre → post | already wrong in |
|---|---|---|---|---|
| 4705 prompt shadow | `0x43d239` (y 431) | all 33 | −0.8785 → −2.0000 | x: dx −2 (section 4a) |
| 6199 cup names | `0x439aec` | 6, 7 | −3.0985/−3.4352 → −4.2474/−4.5842 | x, scale, anchor (section 4b) |
| 5727 header tabs | `0x43a9b5..0x43aa0c` | 15 | −0.6281 → −2.0336 | anchor 154 vs 156, cell 0.752 vs 0.75 (section 4c). Improves on 16 |
| 6481 mode rows | `0x43d164`/`0x43d10c` s 0.58 | 18 | rows 4..6: −1.2152/−3.2152/−5.2152 → −2.3367/−4.3367/−6.3367 (rows 1..3 improve) | pitch: the original's rows are y 92+30r on screen 18, the standalone's pitch is 28. Cell 0.6 vs 0.58 (dh +0.6797). Screen 24 (original pitch 28) improves +0.7848 → −0.3367 |
| 6486 mode values | `0x43b529` s 0.52 align 1 | 18 | −0.2253..−6.2253 → −1.3468..−7.3468 on rows 3..6 | same pitch error. Cell 0.6 vs the original's 0.52 (dh +2.7187). Screen 24 (`0x43b32b`, s 0.58) improves +0.7848 → −0.3367 |

For screen 18, the original's per-row y (92, 122 .. 272) and scales (0.8 selected, 0.58 rows, 0.52
values, x 64 / 309) are the measured `FUN_00427680` inputs from `orig_frame18.json`.
[UNCERTAIN: the instructions producing the pitch 30 in `FUN_0043af10`/`FUN_004325c0` were not read in
this session.]

Original call sites the standalone does not draw as `tex 9` glyphs (no dy measurable):

- `0x42e351`/`0x42e37f` (scr1, ids 0x15c/0x15d, s 0.6 align 2)
- `0x403020`/`0x403040` (scr33, s 1.2)
- `0x42f2e8` (10), `0x42fd85` (17), `0x430039` (28), `0x430224` (29), `0x43165c` (31, 33)
- `0x434989`/`0x434d63`/`0x435247`/`0x4366b0`/`0x4366e0` (5), `0x43d13d` (26)
- scr28's four item rows, and 2 of 4 scr15 `0x43a8f1` player numbers

**Every site that stays on the shared law reads 0.0000 post-fix on every screen it appears on.** Every site
whose |dy| grows already disagrees with the original in x, scale, or a per-call y input.

## Provenance

`verify/scr19_parity_20261012/scr32_texty_recheck/PROVENANCE.txt`.
