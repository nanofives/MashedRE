# scr32 text-y: the six per-site fixes, applied and re-diffed (2026-10-13, claude-opus-5-5)

## Verdict

**GREEN on all six sites in y and cell size.** One build on top of the renderer fix (cfa6eb8d) carries all
six fixes. Against the original's captured glyph vertices, every fixed site reads |dy| 0.0000 and dh 0.0000
on every screen it appears on (worst 0.0001 px, screen 18 value row 1), across 33 screens.

- **x:** sites 1-4 also read dx 0.0000. Sites 5-6 (screen 18/24 labels and values) keep a constant x
  offset of +2.0000 / +3.5000. It comes from their x anchors, which the brief left out of scope (section 3).
- **Second original boot:** a fresh boot on screens 6, 15 and 18 reproduces every number to 4 decimals.
- **Draw lists:** widget RED stays 0 on all 12 scr19/30/32 diffs (anim4 / strict / 0.01 px).
- **Main menu:** across two boots the standalone's draw list changes only in the prompt-strip shadow glyphs,
  which move (+2, +2).

Three of the 14 measured standalone call sites still show nonzero dy. None of them was in this brief, and
none changed (section 3).

Two places where I went past the note's wording. Both follow the bytes:

1. **Sites 5-6: the cell changed, not only the pitch.** Pitch alone leaves a constant dy of −0.3367 on the
   labels and +2.7187 dh on the values, because the glyph top depends on the cell. The bytes give the scales
   (section 1e), so they went in too. Screen 24 shares these two lines, and its cells moved to the original's
   values as well (0.6 → 0.58).
2. **Screen 18's plates moved with the rows.** The plate, label, value and arrow all hang off one `cy`, and
   the original's own plates follow pitch 30 (backbuffer plate tops 80/110/140, against 79/109/139 now).
   So `y0`/`pitch` were changed for the whole row, not only for the text.

## 1. What the bytes say (listings in `verify/scr19_parity_20261012/scr32_texty_sites/bytes_sites.txt`)

**(a) Prompt-strip shadow, ret `0x0043d239`, `exe_main.cpp:4709`.** As the note says.
`0x0043d20f` `fadd [0x005cc31c]` (`0x40400000` = 3.0) on y and `0x0043d22a` the same on x, then
`call 0x00428140` at `0x0043d234`. Fix: `ob = 3.0f * kVScale`.

**(b, c) Screen 6/7 cup names, ret `0x00439aec`, `exe_main.cpp:6239`.** As the note says, with the
arithmetic read exactly:

- `0x0043975c` `cmp edi,7` / `0x0043975f` `fld [0x005cda68]` (118.0). When n < 7, `0x00439772..0x0043977d`
  computes `((7-n)*20)` and divides it by 2 with signed truncation (`cdq; sub eax,edx; sar eax,1`). Then
  `fild` and `fadd 118.0` at `0x00439787`.
- `0x00439793` `fild [0x0067f180]`, `0x004397a4` `fadd st(1)`, and `0x004397a6` stores the sum. So
  `base = float(scroll) + 118 + ((7-n)*20)/2`.
- Per row, `0x004399b0..0x004399bc`: `y = float(k) + base`, with `k += 0x16` at `0x00439d33`. So the row
  pitch is 22.
- `n` is counted by the loop at `0x00439730..0x0043975a`.
- `[0x0067f180]` is only moved by the clamp at `0x004397ae..0x004397c8`, which steps it by 3 when
  `22*sel + scroll` leaves 0..0x84. That cannot happen with n ≤ 7, so the port uses its BSS value 0
  (measured 0).
- x: `push 0x42800000` (64.0) at `0x00439ae1`.
- Scale: `0x3f19999a` (0.6) when `row == [0x0067f17c]` (`0x00439aa5`), otherwise `0x3f147ae1` (0.58)
  (`0x00439aaf`).

The bar and the row icons still use the old `row0 = 144` centre. Only the name call changed.

**(d) Screen 15 header tabs, rets `0x0043a9b5/0x0043a9d1/0x0043a9ed/0x0043aa0c`, `exe_main.cpp:5741`.**
As the note says: `push 0x431c0000` (156.0) and `push 0x3f400000` (0.75), align 2, at
`0x0043a989..0x0043aa07`. Applied to the ability path only. The team path (screen 16, `FUN_0043aa30`) keeps
its old anchor and cell, because its pushes are still unread.

**(e) Screen 18 rows and values, `exe_main.cpp:6532` / `6537`.** Pitch and base come from `FUN_004325c0`'s
layout:

- Every record gets `Q_ListBaseY(count, 0x1e) + 0x1e*row`: `0x004326bb` `mov ecx,0x1e`,
  `0x004326e1` `imul eax,eax,0x1e`, stored at `0x004326f2`.
- Table `0x005f7370` (screen 24) recomputes with `0x1c` and subtracts 58 (`0x004326f7..0x0043271c`,
  `[0x005cd900]` = 58.0).
- Table `0x005f72a0` (screen 18) keeps 0x1e and subtracts 58 at `0x0043272e`.
- `Q_ListBaseY` = `FUN_0042ac50`. With an odd count it returns `240 - ((n-1)>>1)*p` (`0x0042ac72..0x0042ac7d`).
  With an even count it returns `240 - p/2 - ((n-1)>>1)*p`.
- Result: screen 18 (7 rows) = 150 − 58 + 30r = **92 + 30r**. Screen 24 (8 rows) = 142 − 58 + 28r = 84 + 28r.
  This matches the original's pens exactly (s18 y 92..272, s24 84..280). The standalone had 98 + 28r on
  screen 18.

The two cells:

- **Values:** literal pushes. `0x3f051eb8` (0.52) on screen 18 at `0x0043b502` (ret `0x0043b529`), and
  `0x3f147ae1` (0.58) on screen 24 at `0x0043b304` (ret `0x0043b32b`).
- **Labels:** drawn by the record path `0x0043d145..0x0043d15f`, whose scale argument is the record's own
  `[esi-0x18]`. The settled frames on both screens pass `0x3f4ccccd` (0.8) on the cursor row and
  `0x3f147ae1` (0.58) on the others (call args in `orig_frame{18,24}.json`). The port had an ink fit of
  0.776 / 0.6. Its old `[UNCERTAIN] mechanism unknown` comment is replaced, because the mechanism is now
  read: per-record scale, not a second draw.

## 2. Per-site before / after

Before = the recheck's post build (cfa6eb8d). After = this build. Both are measured against the same
original vertices, as standalone − original in px at 640x480, using the median per call.

| # | site | original ret | screens | before dx / dy / dh | after dx / dy / dh |
|---|---|---|---|---|---|
| 1 | 4709 prompt shadow | `0x43d239` y 431 | all 33 (816 glyphs incl. y 51 title shadow) | −2.0000 / −2.0000 / 0.0000 | 0.0000 / 0.0000 / 0.0000 |
| 2 | 6239 cup name, selected | `0x439aec` s 0.6 | 6, 7 | +1.4750 / −4.2474 / +0.4996 | 0.0000 / 0.0000 / 0.0000 (dw 0.0000) |
| 3 | 6239 cup names, others | `0x439aec` s 0.58 | 6, 7 | +2.5429..+3.8532 / −4.5842 / +1.1792 | 0.0000 / 0.0000 / 0.0000 (dw 0.0000) |
| 4 | 5741 scr15 tabs | `0x43a9b5..0x43aa0c` s 0.75 | 15 | −0.0096..+0.0069 / −2.0336 / +0.0680 | 0.0000 / 0.0000 / 0.0000 |
| 5 | 6532 mode labels | `0x43d164`/`0x43d10c` s 0.8 / 0.58 | 18 | row 0 +0.55 / +6.4040 / −0.8156, rows 1..6 +3.02..+3.98 / +3.6633..−6.3367 / +0.6797 | +2.0000 / 0.0000 / 0.0000 every row |
| 5 | (same line) | same, s 0.58 / 0.8 | 24 | dy −0.3367 (0.58 rows), +0.4040 (row 0) | dy 0.0000, dh 0.0000 |
| 6 | 6537 mode values | `0x43b529` s 0.52 | 18 | −3.64..+1.74 / −7.3468..+2.6533 / +2.7187 | +3.5000 / ≤ 0.0001 / 0.0000 |
| 6 | (same line) | `0x43b32b` s 0.58 | 24 | +2.3466 / −0.3367 / +0.6797 | +3.5000 / 0.0000 / 0.0000 |

The fresh second original boot (`quad2_diff.txt`, screens 6/15/18) gives the same after column to 4 decimals.

**Why dx moved on site 6.** The values are right-anchored. Before, the glyphs were too wide (cell 0.6
against 0.52 / 0.58), so the left edges spread over −3.64..+2.35 depending on string length. With the cell
right, every glyph sits at the same +3.5000 offset. That is the anchor itself: the port's `valRight` is
312.5 and the original's computed x is 309.0. Site 5's +2.0000 is `labelX` 66 against the record's x 64.0
(`0x42800000`). Both are x-anchor items that the brief left out of scope.

## 3. Coverage and what remains

All 14 measured standalone call sites, after this build:

| call site | ret | dy after | status |
|---|---|---|---|
| 4598, 4600 watermark | `0x43c6c7`, `0x43c6f8` | 0.0000 (32 screens) | exact |
| 4709, 4711 prompt + shadow | `0x43d25a`, `0x43d239` | 0.0000 (33) | exact (4709 fixed here) |
| 4921, 4931 menu rows | `0x43d164`/`0x43d10c` s 0.8 | 0.0000 | exact |
| 5101, 5131 scr19/32 values | `0x431202`, `0x431426` | 0.0000 | exact |
| 5741 tabs, ability | `0x43a9b5..0x43aa0c` | 0.0000 | exact (fixed here) |
| 6239 cup names | `0x439aec` | 0.0000 | exact (fixed here) |
| 6532, 6537 mode rows / values | `0x43d164`/`0x43d10c`/`0x43b529`/`0x43b32b` | 0.0000 (max 0.0001) | dy/dh exact, dx +2.0 / +3.5 |
| **5364** scr4 digit | `0x433efc` | **+18.1918** | unchanged. The recheck flagged this pairing [UNCERTAIN] (a repeated digit cell inside the 40 px window). Cell 0.55 vs 0.6, dx +4.85 |
| **5741** tabs, team path | `0x43ab7a`/`0x43abb4` (9, 16) | **+0.8081** | unchanged. Cell 0.752 vs the original's 0.8 (dh −1.6312). `FUN_0043aa30`'s pushes are unread |
| **5912** player number | `0x43a8f1` (15), `0x43ae75` (9, 16) | **+1.8418** | unchanged. Cell 0.55 vs 0.6 (dh −1.6992), dx +5.0 / +1.0 |

`quad_diff_sweep.txt` gives per-screen mean |dy|. It is 0.0000 on 30 of the 33 screens. The three
exceptions are screen 4 at 0.6421 (site 5364), screen 15 at 0.0396 (site 5912) and screen 16 at 0.1716
(sites 5741-team and 5912).

**Draw lists** (`drawlist_summary.txt`, `--exclude-tex 9`, so text is not in them):

- widget_red 0 on all 12 diffs (scr19a/19b/30/32 × anim4 / strict / 0.01 px).
- Every count matches the recheck's post build except scr19a anim4: matched 101 / checker 35 against
  96 / 40. That row is the animated checker, which varies by boot. The two `other` rows there are the
  preview quads `0x474bd3`/`0x474d0c`, as before.

**Standalone before vs after, record by record** (`ds_compare_all.txt`):

- **Main menu (screens 0 and 1):** 8 glyph records differ, all prompt-strip shadow glyphs at
  (+2.0000, +2.0000), with dw/dh 0. 42 tex-0 records move by < 0.11 px. The control (two boots of the
  recheck's pre and post builds, same non-text code) shows the same 42 tex-0 records differing. So those
  are boot animation, not this change.
- **Every screen:** the shadow moves (+2, +2).
- **Screens 6/7:** the cup names change. The tex-86 category sprite is the time-based pulse.
- **Screen 15:** the tabs change.
- **Screen 18:** the plates, label/value text and arrows (tex 0 / 46) move by −6 + 2r, from the pitch.
- **Screen 24:** only text cells change.

## 4. Follow-ups this pass did not take

- Screen 18/24 x anchors: label `labelX` 66 → 64.0 (record x `0x42800000`). Value `valRight` 312.5 → the
  original's computed x 309.0, from the expression at `0x0043b4f2..0x0043b520` (s18) and its twin ahead of
  `0x0043b304` (s24). Neither was fully read here.
- The value arrow (`exe_main.cpp`, `MeasureMashedString(val, cell)`) still measures at 0.6. The original's
  arrow call `FUN_004282a0` gets the same 0.52 / 0.58 scale as the value (`0x0043b529` / `0x0043b32b`).
  Moving it is an arrow-parity change with its own capture.
- Screen 18 plate top: `cy - 13` gives 79/109/.. against the original's 80/110/.. (1 px), with a different
  border thickness.
- Screen 16 team tabs (`FUN_0043aa30` pushes), and the cells of sites 5364 / 5912.

## Proposed CHANGELOG line

`2026-10-13 — Frontend text per-site geometry from the bytes (work/scr32texty, with the cfa6eb8d renderer fix):
prompt-strip shadow +3.0 (0x0043d20f/0x0043d22a), cup names x 64 / scale 0.6|0.58 / y 22r+118+((7-n)*20)/2
(0x00439ae1, 0x00439aa5/aaf, 0x0043975c..0x004397a6), scr15 tabs y 156 / 0.75 (0x0043a989..), scr18 rows
92+30r (FUN_004325c0 0x004326bb/0x0043272e) with label 0.8|0.58 and value 0.52|0.58 cells (0x0043b502/0x0043b304).
All six sites |dy| 0.0000 vs original vertices on 33 screens, widget RED 0 on scr19/30/32.
Evidence verify/scr19_parity_20261012/scr32_texty_sites/, note re/analysis/negctl/session_20261012b/scr32_texty_SITES.md.`

## Provenance

`verify/scr19_parity_20261012/scr32_texty_sites/PROVENANCE.txt`.
