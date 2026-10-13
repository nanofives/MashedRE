# scr32 / scr19 value-text y: the shared renderer carried a fitted +0.055 cell (2026-10-13)

## Verdict

**Cause found and removed.** The 1 px was not per-call: `DrawMashedString` added
`+0.055 * cell_h` to every glyph in the standalone, a term fitted on 2026-06-12 against
backbuffer **ink centroids**. Measured against the original's **own submitted glyph vertices**,
every text call on screens 1 / 6 / 8 / 15 / 19 / 30 / 32 was low by exactly `0.055 * cell_h`
(1.4953 px at s 0.8, 1.3084 at 0.7, 1.1215 at 0.6). The scr19 insults row carried a second,
compensating `- 1.0f * S` at its call site.

Both rows the prompt named share the one cause, so the fix is one place (plus dropping the scr19
compensator). After the fix the vertex-level dy is **0.0000** on all nine call sites that use the
shared law, on all seven screens. The scr32 ink check moves from best overlap at dy -1 to **dy 0**
with identical ink boxes. Draw-list diffs scr19 / scr30 / scr32 stay **widget RED 0** at
`--tol-anim 4`, strict and 0.01 px.

**Trade-off to read before merging:** two call sites that do *not* follow the shared law get worse
in dy, because the global bias was masking their own error. Both already disagree with the original
in `dx` and in cell height, so neither was a faithful port before. Numbers in §4.

Branch `work/scr32texty`. Evidence `verify/scr19_parity_20261012/scr32_texty/`.

## 1. The cause, from the bytes

`FUN_00427e00(id, x, y, colour, scale, align)` reaches `FUN_00427680`
(plate `re/analysis/hud_promote_c2_b/0x00427680.md`), which writes

```
screen_y = [0x005cc320] - ((H * [0x005cc560] * y) / (float)H) * [0x0067d834]
                        - scale * [0x005cc9a4]
```

`FUN_00554940` then builds the glyph cell in unit local space (`y` 0..1, `z` 1; vertex stride 0x24,
uv at +0x1c/+0x20) and submits it through Im3D `0x004cd070` with the frontend camera at
`[0x00912c0c]`.

Live capture at screen 32, one `FUN_00427e00` call filtered on return address `0x00431426`
(the call at `0x00431421`), PID 33136, `orig_texty32.json`:

| what | measured |
|---|---|
| args | id `0x59`, x `376.0` (`43bc0000`), y `240.0` (`43700000`), colour `ff000000`, scale `0.7` (`3f333333`), align 0 |
| `[0x0067d828..0x0067d834]` | `0`, `0`, `3f7fffff`, `3f7fffff` (0.99999994 for both scale factors) |
| LTM | diag `(-0.07929600, +0.07929600, 1)`, translate `(-0.13999987, -0.04001457, 0)` |
| camera | raster 640x480, view window 0.8 x 0.8, view offset 0, view matrix diag `-0.625 / -0.625 / 1`, offset row `(0.5, 0.5)` |

`0.079296 = 0.7 * 0.0708 * 1.6`, and `1.6 = 2 / 0.8` is the view-window factor, so the LTM is the
cell scaled by the print scale `scale * 0.0708` (`_DAT_005cd5fc`) in both axes.

Projecting the submitted vertices (`p = local * ltm`, `p' = p * view`,
`pixel = (p'.x / p'.z * 640, p'.y / p'.z * 480)`):

```
local y = 1  ->  pixel y 228.2156     (quad top)
local y = 0  ->  pixel y 252.0044     (quad bottom)
height            23.7888 = 0.7 * 0.0708 * 480 = cell_h
top = 240.0 - 0.49537 * cell_h
```

which is exactly `anchor_y + cell_h * (0.025/0.0708 + c8 - 1)` with `0.025` from `[0x005cc9a4]` and
`c8 = 0.151515` (charset+0x08, FGDC20.RWF) — i.e. the law already written in `exe_main.cpp`, with
**no** additional term. Cross-check on the same numbers: the normalized pen row from
`FUN_00427680` is `1 - 240/480*0.99999994 - 0.7*0.025 = 0.48250003`, pixel `248.4`, and
`248.4 - cell_h * (1 - c8) = 228.218` — the measured quad top.

The standalone was computing

```
top_y = anchor_y + cell_h * (0.025/0.0708 + c8 - 1 + 0.055)
```

`0.055 * 23.7888 = 1.3084`, so the scr32 value text sat 1.3084 px low. The `+0.055` was introduced
2026-06-12 by fitting ink centroids in `verify/orig_backbuffer_f300.bmp`; an ink centroid includes
the glyph's empty top and bottom rows and the raster's edge coverage, so it is a biased estimator of
the quad. The vertex capture above measures the quad itself.

## 2. The witness

`text_frame_probe.py` (this folder) takes ONE frame of the original between two entries of the frame
shell `0x0042e3a0`: entry hooks on `0x00427e00` / `0x00428140` record every text call (id, x, y,
colour, scale, align, return address), and an entry hook on `0x004cd070` kept only when the return
address lies inside `FUN_00554940` (`0x00554940..0x005551ca`) records every glyph vertex plus the
LTM and the camera. Read-only, no key input, kills only its own PID. `text_quad_diff.py` matches
each original glyph to a standalone `tex 9` quad by atlas cell (u,v within 1e-3) and nearest
top-left within 40 px, and reports the median `dx` / `dy` / cell-height delta per original text call
(standalone minus original).

The projection is self-checking: on every shared-law call site it reports `dx 0.0000` and
`dh -0.0000`, so the only residual was in y.

## 3. Before / after

Per-call `dy` (standalone minus original, px at 640x480), the families that use the shared law:

| call site | scale | before | after |
|---|---|---|---|
| `0x43c6c7` / `0x43c6f8` watermark (every screen) | 0.8 | +1.4953 | **+0.0000** |
| `0x43d164` / `0x43d10c` menu rows | 0.8 | +1.4953 | **+0.0000** |
| `0x43d25a` title + prompt strip | 0.6 | +1.1215 | **+0.0000** |
| `0x431426` scr32 autosave value "Off" | 0.7 | +1.3084 | **-0.0000** |
| `0x431202` scr19 insults value "Manual" | 0.7 | +0.3084 | **+0.0000** |

Per-screen `mean|dy|` over all matched glyphs:

| screen | glyphs | before | after |
|---|---|---|---|
| 1 (main menu) | 121 | 1.3310 | **0.1322** |
| 6 | 114 | 1.9112 | **1.8421** |
| 8 | 103 | 1.3002 | **0.2913** |
| 15 | 93 | 1.0567 | **0.8214** |
| 19 | 103 | 1.2360 | **0.3107** |
| 30 | 72 | 1.2466 | **0.1667** |
| 32 | 71 | 1.1799 | **0.4507** |

Every screen improves in aggregate, including 6 and 15.

scr32 value-text ink (`value_text_ink.py`, ink = `max(R,G,B) < 48` in x 374..410, y 225..255):

| | orig box | standalone box | best IoU |
|---|---|---|---|
| before (`../scr32_fix/value_text_ink.txt`) | x 377..405 y 233..246 | x 377..405 y **235..247** | 0.865 at (0, **-1**) |
| after (`value_text_ink_post.txt`) | x 377..405 y 233..246 | x 377..405 y **233..246** | 0.910 at (0, **0**) |

(The two runs use different original boots, so the original's ink pixel count differs, 197 vs 208;
its bounding box is the same.)

Draw-list diffs, all `--scale-b 1 --exclude-tex 9 --map mashedmod/build/mashed_re.map
--rotate-a 0x42e65a`, summarized with `../scr32_fix/sumdiff32.py`:

| diff | `--tol-anim 4` | strict | 0.01 px |
|---|---|---|---|
| `after_fix/orig_scr32.json` vs `sa32_post.json` | widget 0, other 0 | widget 0, other 0 | widget 0, other 2 |
| `orig1_settled.json` vs `sa19_post.json` | widget 0, other 0 | widget 0, other 0 | widget 0, other 2 |
| `orig2_settled.json` vs `sa19_post.json` | widget 0, other 0 | widget 0, other 0 | widget 0, other 2 |
| `after_fix/orig_scr30.json` vs `sa30_post.json` | widget 0, other 2 | widget 0, other 2 | widget 0, other 4 |

The `other` rows are the same pre-existing set as in `scr32_arrow_RESULT.md`: the half-pixel
512x512 background pair (`0x473ebd` vs the standalone's -0.5) and, on screen 30, the two preview
quads at `0x474bd3` / `0x474d0c`. These diffs pass `--exclude-tex 9`, so they are blind to text by
construction and could not have regressed; they are reported to show nothing else moved.

## 4. The trade-off (two call sites get worse)

Removing a global bias exposes per-call-site error that the bias was partly cancelling:

| screen | call site | ids | dx | dh | dy before | dy after |
|---|---|---|---|---|---|---|
| 1/6/8/15/19/30/32 | `0x43d239` prompt-strip shadow | n/a | **-2.0000** | 0.0000 | -0.8785 | **-2.0000** |
| 6 | `0x439aec` | 0x49 | +1.4750 | **+0.4996** | -3.0985 | **-4.2474** |
| 6 | `0x439aec` | 0x4a/0x4b/0x4c | +2.54..+3.85 | **+1.1792** | -3.4352 | **-4.5842** |
| 15 | `0x43a9b5`..`0x43aa0c` | 0xe3/0xd2/0xd3/0xd4 | ~0.01 | **+0.0680** | -0.6281 | **-2.0336** |

- `0x43d239` is the prompt strip's shadow copy. Its dy now equals its dx: the original draws the
  shadow at base + (3, 3) (`67, 431` against `64, 428`), the standalone at base + (1, 1). That is a
  single per-call-site offset, now readable; before the fix the y half of it was hidden. Its
  magnitude grew from 0.8785 to 2.0000 px.
- `0x439aec` (screen 6) and the `0x43a9b5` family (screen 15) disagree with the original in `dx`
  (up to 3.85 px) and in cell height (`dh` up to +1.18 px), so those rows do not run the original's
  scale and were never faithful. Their dy grew by `0.055 * cell_h` of the standalone's own cell
  (1.1489 / 1.1490 on screen 6, 1.4055 on screen 15).

None of the four screens the prompt gated on (19, 30, 32, main menu) regresses at any call site.
Screens 6 and 15 regress at the sites listed above and improve at every other site. **Per the
prompt's rule this counts as a mixed result, so the merge decision is left open** — the change is
committed on `work/scr32texty` and not merged.

## 5. Coverage limit

`exe_main.cpp` has 48 `DrawMashedString` call sites. This sweep measured seven screens (1, 6, 8,
15, 19, 30, 32). Any unmeasured screen whose y was fitted while the `+0.055` was in place will move
up by `0.055 * cell_h` (1.1 px at s 0.6, 1.5 px at s 0.8). Nothing in the source names such a fit
(`0.055` appears nowhere else, and the only `- 1.0f * S` on a text y was the scr19 one removed
here), but that is a grep, not a measurement.

## 6. What changed

`mashedmod/src/mashed_re/exe_main.cpp`, two edits plus comments:

1. `DrawMashedString`: `top_y = anchor_y + cell_h * (0.025/0.0708 + baseline_c8 - 1)`, dropping
   `+ 0.055f`. Comment rewritten to cite `FUN_00427680`'s formula, the measured
   `[0x0067d834] = 0x3f7fffff`, the measured LTM/camera and the projected quad rows.
2. scr19 insults value text (`kTriText`): y is `rec.y * S`, dropping `- 1.0f * S`.

The scr32 toggle value text already passed `rec.y * S` and is unchanged. No other behavior touched.
No tracker, `NEXT_SESSION` or `original\` file touched.

## 7. Proposed CHANGELOG line (not applied)

`2026-10-13 standalone text vertical anchor (exe_main.cpp DrawMashedString): dropped the fitted +0.055*cell_h added 2026-06-12, so top_y = anchor_y + cell_h*(0.025/0.0708 + c8 - 1) verbatim per FUN_00427680 (screen_y = [0x005cc320] - (H*[0x005cc560]*y)/H*[0x0067d834] - scale*[0x005cc9a4]) and FUN_00554940's unit-cell quad. Measured on the original's own submitted glyph vertices via Im3D 0x004cd070 at screen 32 (id 0x59, y 240.0, s 0.7): [0x0067d830]=[0x0067d834]=0x3f7fffff, LTM diag -0.079296/+0.079296, projected quad y 228.2156..252.0044 = anchor_y - 0.49537*cell_h. Also dropped the compensating -1.0*S on the scr19 insults value text (0x431202). Vertex-level dy per text call 1.4953/1.3084/1.1215 px -> 0.0000 on screens 1/6/8/15/19/30/32; scr32 value-text ink best overlap dy -1 -> dy 0; scr19/scr30/scr32 draw lists stay widget RED 0 at 0.01 px. Known trade-off: the unported sites 0x439aec (scr6) and 0x43a9b5 (scr15) and the prompt-strip shadow 0x43d239 lose the masking bias, see the note. Evidence verify/scr19_parity_20261012/scr32_texty/, note re/analysis/negctl/session_20261012b/scr32_texty_RESULT.md.`

## 8. Follow-ups (not done)

- Prompt-strip shadow offset `0x43d239`: standalone base + (1, 1) against the original's
  base + (3, 3), on every screen. One call-site constant, now isolated at (-2, -2).
- `0x439aec` (scr6) and `0x43a9b5` family (scr15): cell height and x disagree with the original;
  those rows need their own port before their dy means anything.
- `[0x0067d834]`'s producer is still not ported (same open row as `[0x0067d830]`); it is read as
  the literal `0x3f7fffff` measured on three original boots now (scr19 x2, scr32 x2).

## Provenance

- Standalone: this worktree's `mashedmod\build\mashed_re.exe` built with
  `$env:MASHED_NO_DEPLOY='1'; cmd /c mashedmod\build.bat`, run through `run_sa.ps1` with
  `MASHED_SETTINGS_DEFAULTS=1`, `MASHED_DBG_DRAWSTREAM=200:203`, `MASHED_WIN_POS=primary-bl`.
  PIDs: 10880 (scr1), 12656 (scr6), 22972 (scr8), 34576 (scr15), 25228 (scr19), 28440 (scr30),
  25976 (scr32), 31648 (scr32 + backbuffer dump). Each run killed only its own PID. No
  `mashed_re_settings.bin` in this worktree; the repo-root file was not touched.
- Original: `orig_frame{1,6,8,15,19,30,32}.json` from `text_frame_probe.py` (previous session,
  reused), `orig_texty32.json` from `text_y_probe.py` PID 33136 (this session),
  `orig_scr32_bb.bmp` reused. Draw lists reused: `../after_fix/orig_scr32.json`,
  `../after_fix/orig_scr30.json`, `../orig1_settled.json`, `../orig2_settled.json`.
- Pre-fix standalone captures `sa*_pre.json` and `pre_quad_diff_*.txt` are from the previous child
  session on the same worktree and the same source state (the uncommitted edit it left was reviewed
  against the bytes above and kept).
