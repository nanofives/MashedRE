# scr19 parity: settings screen (page 0x13) draw-list diff, original vs standalone (2026-10-12)

## Verdict: RED

14 RED rows per settled frame across 4 frames (`orig1_f0..f3` vs `f200..f203`), all of them inside the
settings widget (`0x00430b90` on the original side, `exe_main.cpp` ~4880-5025 on the standalone side).
Nothing outside the widget is RED.

| # | Rows (A = original, B = standalone, f0/f200) | Original | Standalone | Attribution |
|---|---|---|---|---|
| 1 | A[111]/B[111] bar bg (`0x430d9c`), A[112]/B[112] top border (`0x430db6`), A[114]/B[114] left (`0x430e11`), A[115]/B[115] right (`0x430e31`), row 0 | y = 188.0000153, bottom 204 | y = 187.0, bottom 203 | **Defect, standalone geometry.** The bar box sits 1 px high |
| 2 | Same four quads on row 1: A/B[119] `0x430f85`, [120] `0x430f9f`, [122] `0x430ffa`, [123] `0x43101a` | y = 218 | y = 217 | Same defect |
| 3 | Same four quads on row 2: A/B[127] `0x4310c2`, [128] `0x4310dc`, [130] `0x431137`, [131] `0x431157` | y = 248 | y = 247 | Same defect |
| 4 | A[116] / B[116] row-0 fill (`0x430e8d`) | w = 70.0 | w = 100.0 | **Settings value, not code.** Music is 0.7 on the original and 10 on the standalone (section 3) |
| 5 | A[135] / B[135] row-3 right arrow (`0x430ef1`) | x = 457.0 | x = 417.65 | **Settings value, not code.** The original shows "Manual" (id `0x1b1`), the standalone shows "Off" (section 3) |

Rows 1-3 make 12 RED rows. The default `drawlist_diff` run hides them: `--tol-pos 1.0` uses `<=` and the measured
gap is 1.0000153 px. `--tol-anim 4` also pairs them as "matched". They show up as `MISMATCH(moved)` only in the strict run
(`diff_orig1_vs_sa_strict.txt`). They are not animation: both sides put the same values in all 4 frames, and the
original gives identical values on both runs.

Colours match on every widget row: fill `ffd88020`, bar bg `7f000000`, borders and arrows `ff000000`.
The bottom border (A/B[113] y=201, [121] y=231, [129] y=261), the row-1 and row-2 fills (w=70 on both sides), both
arrows on rows 0-2 and the left arrow on row 3 are GREEN.

Evidence: `verify/scr19_parity_20261012/` (commands in `PROVENANCE.txt`).

## Noise floor (how far each side differs from itself)

- **Original vs original** (`diff_orig1_vs_orig2.txt`): 59 matched, 77 mismatched per frame, all outside the widget.
  42 are `moved` checker cells (`0x474464` via `0x42e89c`, the fsin phase). 35 are `reordered` rows, which are the
  frame-split wrap: arc-wash strips `0x4733a2`, video quad `0x42e65a`, previews `0x42e80c`, caps `0x473532`.
  All 26 widget rows are identical between the two original runs, and so are both text traces.
- **Standalone vs standalone** (`diff_sa_vs_sa.txt`): 236/236 matched, GREEN.
- **Original vs standalone with `--tol-anim 4`** (`diff_orig1_vs_sa.txt`): 134/136 matched per frame. The only REDs
  are rows 4 and 5. The orig2 pairing shows 22 extra checker `moved` rows, all at `0x474464` (fsin phase beyond 4 px).

## (1) The three known code differences in the standalone (`exe_main.cpp` ~4916-4994) vs `0x00430b90`

Constants were read from `original\MASHED.exe.unpatched` .rdata: `[0x005cc9b8]` = 7.0, `[0x005cd8d8]` = 13.0,
`[0x005cc31c]` = 3.0, `[0x005cc320]` = 1.0, `[0x005cc568]` = 100.0, `[0x005cc9f4]` = 8.0, `[0x005cc72c]` = 30.0.

**a. Fixed alpha 0xff vs the screen-fade alpha `0x0067e7fc`.** Not observable on the reachable route. No row is RED
because of it.
- In settled frames the original's alpha is 0xff, so `col1/col2/col3` = `ffd88020` / `7f000000` / `ff000000`. These
  equal the standalone's constants exactly.
- Fade-in: the 12-frame burst at `--settle 0.05` (`orig_fade.json`) already shows alpha 0xff on all 26 widget rows.
  The per-frame read-only probe (`orig_alpha_probe.json`) explains why. Frame 1: mode `[0x0067e7f8]` = 0, alpha = 0,
  screen 33. Frame 31: mode 1, alpha 255, screen 19. It stays there to frame 210. The transition float `[0x0067e840]`
  reads 0.0 on every frame. Per `0x0043c303` (TimerSlotTickDispatcher), state 1 writes `ftol((1.0 - x) * 255.0)`, so
  with x = 0 the alpha steps 0 -> 255 in one frame. Before that step the original draws no widget at all
  (mode 0 and alpha < 0x60 takes the early-out at `0x00430bcf..0x00430bf8`).
- The difference is real in code, and it is wider than the fill. The standalone hard-codes **all** widget colours
  (bg `0x7f000000`, borders and arrows `0xff000000`, fill `0xff2080d8`). The original derives all three colour words
  from the per-row alpha (`0x00430c70..0x00430c7a`, re-read at `0x00430efb`), and the standalone has no early-out.
- [UNCERTAIN] Whether any natural route reaches screen 19 with x != 0 while `0x0067e7f8` is 1 or 2, which is where
  the fixed alpha would show. Missing evidence: a natural-navigation capture. The original ignores injected keys, so
  the only available route is the synthetic push. The exit fade (state 2, `ftol(x * 255.0)`) was not captured
  because the burst tool has no pop.

**b. Fill always drawn vs the `fill > 1.0` gate (`[0x005cc320]` = 1.0, fcomp at `0x00430e5a`).** Not exercised.
Fill = global × 100.0, and every slider in both captures is above the gate (70/70/70 original, 100/70/70 standalone).
Static prediction: a slider at 0 makes the standalone emit a w=0 fill quad where the original emits nothing, which
would be one EXTRA row per zero slider. It is not in today's evidence.

**c. Constant-rate slider animation (deliberate, per the user's request in the code comments).** Not exercised by a
settled capture. The standalone's fill is already at its target at f200 (w=100 = value 10, g_sliderDisp started at
7, 8 units/s). The original's fill is instantaneous from the global. This is reported, not classified as a defect.

## (2) Other RED rows

- **Bar box 1 px high**: rows 1-3 in the verdict table. Mechanism: the original draws the bar at `yy = rowY - 7.0`
  (`[0x005cc9b8]`), the bottom border at `yy + 13.0` (`[0x005cd8d8]`) and the fill at `yy + 3.0` (`[0x005cc31c]`).
  It draws the arrows at `(rowY - 8.0) * H * (1/480)` (`[0x005cc9f4]`, `[0x005cc560]`). The standalone uses the arrow
  y as its bar y (`by`, `exe_main.cpp:4891`). It then gets the bottom border and fill right only because the offsets
  were fitted as `by + 14` and `by + 4` (box `exe_main.cpp:4949-4953`, bottom `:4951`, fill `:4954`), whose comment cites "bar 247". The original's
  bar is at 248.
- **Row-3 value text, by static comparison only.** The original draw stream cannot see glyphs, so these come from the
  call arguments. The original calls `0x00427e00(0x1b1, x=376.0, y=284.0, ff000000, scale 0x3f333333 = 0.7, 0)`
  (`orig1/orig2_settled.text.json`, push site `0x004311ee..0x004311fd`). The standalone draws at x=378 with scale
  0.72 (glyph quads B tex 9 at x=378.0, y=273.2), a split already recorded as [UNCERTAIN] in the code. Its arrow
  formula `378 + Measure + 9` differs from the original's `W * (ftol(0x004282a0(id, 0.7)) + 0x17e) / 640` = 382 +
  width (`0x00431210..0x00430ea1`). The measured arrow gap (row 5) cannot separate formula from value, because the
  strings differ.
- [UNCERTAIN], side note for `Frontend/ProgressBarSetA.cpp` (the hooked port, not the standalone screen): the
  original's text y comes from `[esp+0x18]` (`0x004311e6`). The port passes `rowY`. The arrow arithmetic puts row-3
  rowY at 285 (arrow y 277 = rowY - 8), while the traced text y is 284.0. Which store feeds `[esp+0x18]` at
  `0x004311e6` has not been traced through the stack-depth changes.
- No geometry, colour, arrow or chrome RED outside the widget at `--tol-anim 4`.

## (3) Are the slider values the same? No

- Original: all three fills are w=70.0 at `0x430e8d`. Fill = global × 100.0, so `[0x007f0f04]` (music),
  `[0x007f0f00]` (sfx) and `[0x007f0f08]` (insults volume) are 0.7 each. The insult state `[0x007f0f10]` = 2,
  because the traced id is `0x1b1` (`0x004311d9..0x004311de`).
- Standalone: `mashed_re_settings.bin` at the repo root (gitignored, mtime 2026-09-07, read and not modified) holds
  music 10, sfx 7, insults 7, insults_on 0 ("Off": 3 glyphs, the last two with identical UVs), gamma 10, autosave 0.
  The compiled defaults (`exe_main.cpp:1804-1811`) are 7/7/7/Manual, which match the original. The file overrides
  them, and the exe always `chdir`s to the folder that holds `original\` (`exe_main.cpp:8204-8232`).
- Comparable rows: the row-1 and row-2 fills (both sides at 7) and all value-independent geometry. Not comparable:
  the row-0 fill and the row-3 text and right arrow. Making them comparable needs a settings edit or a harness knob,
  both outside this measurement's rules, so neither was done.

## Proposed fixes (not applied)

1. `exe_main.cpp:4891` ff.: add `bary = by + 1.0f * S` and use `bary`, `bary + 13*S` and `bary + 3*S` for
   box/bottom/fill (that is rowY-7 / +13 / +3 from `0x005cc9b8` / `0x005cd8d8` / `0x005cc31c`). Arrows stay at `by`.
   This closes 12 rows and moves the box bottom edge from 203 to 204.
2. Gate the fill on `disp * 10.0f > 1.0f` (fill width > `[0x005cc320]` = 1.0).
3. Derive the widget colours from a standalone mirror of the screen-fade alpha (`col3 = a<<24`, `col2 = (a>>1)<<24`,
   fill `(a<<24)|rgb`) and add the `mode 0 && a < 0x60` early-out. Low priority: no observed divergence on the
   reachable route.
4. Harness: add a parity knob that makes the standalone ignore `mashed_re_settings.bin`, or point it at a path such
   as a `MASHED_SETTINGS_PATH`, so value rows are comparable without touching the user's file. Also,
   `parity_tooling.md` still documents `--scale-b 0.8`, but the standalone now emits 640x480 virtual coordinates
   (`--scale-b 1` gives 134/136, while 0.8 gives 0/136).
