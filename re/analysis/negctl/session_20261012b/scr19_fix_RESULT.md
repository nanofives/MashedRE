# scr19 settings widget: fixes 1, 2 and 4 applied, parity re-run (2026-10-12)

## Verdict: widget GREEN except one row, the row-3 right arrow (out of scope, attributed below)

Of the 14 RED widget rows per frame in `scr19_parity_RESULT.md`, 13 are closed. With `MASHED_SETTINGS_DEFAULTS=1` the
standalone uses 7/7/7/Manual. The one remaining widget row is the row-3 right arrow: original x = 457.00,
standalone x = 465.10. Both sides now show "Manual", so it is a formula difference, not a settings difference.
None of the three approved fixes covers it. Nothing outside the widget is RED except the checker-cell fsin phase
noise already measured as original-vs-original noise.

| Run (orig1 vs standalone, per frame) | Before (`../../../verify/scr19_parity_20261012/`) | After (`after_fix/`, knob ON) |
|---|---|---|
| Default (`--tol-anim 4`) | 134/136 matched, widget RED **2** (row-0 fill w, row-3 arrow) | 135/136 matched, widget RED **1** (row-3 arrow) |
| Strict (no `--tol-anim`) | 83 matched, widget RED **14** (12 bar-box `moved` + fill + arrow), 39-40 checker `moved` | 93 matched, widget RED **1** (arrow), 42 checker `moved` |
| Sub-pixel (`--tol-pos 0.01 --tol-size 0.01`) | not run | widget RED **1** (arrow). The other 25 widget rows match within 0.01 px |
| orig2 vs standalone, default / strict | (orig2 default: rows 4, 5 + 22 checker) | widget RED **1** (arrow) in both. The rest is checker `0x474464` only |

Strict-run checker rows are all `0x474464` via `0x42e89c`, the fsin phase that already differs between the two
original runs (`diff_orig1_vs_orig2.txt`, 42 `moved` per frame). They are not widget rows.

Knob check (memory "verify the harness knob actually took"): the boot log line is
`settings: source=defaults(MASHED_SETTINGS_DEFAULTS) music=7 sfx=7 insults=7 insults_on=2 gamma=3 autosave=1`.
The worktree root held a byte copy of the user's `mashed_re_settings.bin` (10/7/7/Off) during that run, so the knob
ignored a file that was present.

## What changed (commit `a9a066f9`, `exe_main.cpp`)

Constants read from `original\MASHED.exe.unpatched` with capstone (`after_fix/listing_00430b90_00431240.txt`):

1. **Bar box 1 px lower.** `FUN_00430b90` row 0: `fsub [0x005cc9b8]` (7.0) at `0x00430d5c` gives `yy = rowY - 7`.
   The arrows use `rowY - [0x005cc9f4]` (8.0) at `0x00430cbd`, which equals the standalone's `by`. New
   `bary = by + 1*S`. bg/top/left/right are at `bary`. Bottom border at `bary + 13*S` (`fadd [0x005cd8d8]` = 13.0
   at `0x00430dba`). Fill at `bary + 3*S` (`fadd [0x005cc31c]` = 3.0 at `0x00430e35`). The old `by + 14` and `by + 4`
   gave the same absolute y for those two, so only the four box quads per row move. Rows 1 and 2 repeat the
   constants at `0x00430f45`/`0x00430fa3`/`0x0043101e` and `0x00431082`/`0x004310e0`/`0x0043115b`.
2. **Fill gate.** `fld [0x007f0f04]`, `fmul [0x005cc568]` (100.0) at `0x00430e50`, `fcomp [0x005cc320]` (1.0) at
   `0x00430e5a`, `test ah,0x41` / `jne` at `0x00430e62`/`0x00430e65`. Equal or below skips the quad. The standalone
   now draws the fill only when `value/10 * 100 > 1.0` on the **target** value, because the original has no
   animation. The drawn width keeps the constant-rate animation. Self-check (`after_fix/diff_sa_off_vs_music0.*`):
   with music = 0 the row-0 fill quad is absent and every other row is unchanged. Before this fix it was emitted as
   a w=0 quad. There is no original-side zero-slider capture, so this is a standalone-side check plus the listing,
   not a cross-side diff.
4. **`MASHED_SETTINGS_DEFAULTS=1`.** `LoadMenuSettings` returns before opening the file, and `SaveMenuSettings`
   returns too, so a run with the knob on never rewrites the user's file. Default OFF. The boot log now prints the
   settings values in use (`settings: source=... music=...`). It is documented in `re/analysis/parity_tooling.md`
   (tool table, recipe, and the known-asymmetries section). That section now says `--scale-b 1`, and that
   `--tol-pos 1.0` uses `<=` and hides a gap of exactly 1 px. `FLAG_INVENTORY_2026-08-15.md` is a generated dated
   snapshot, so it was not hand-edited.

Fix 3 (fade alpha / early-out) was not done. `ProgressBarSetA.cpp` and the 284-vs-285 text-y question were not touched.

## Remaining RED row: row-3 right arrow, A[135] / B[135]

| | Original | Standalone |
|---|---|---|
| x | 457.00 | 465.10 |
| y, size, colour | 277, 16x16, ff000000 | same |
| formula | `W * (ftol(FUN_004282a0(0x1b1, 0x3f333333 = 0.7)) + 0x17e) / 640`: measure at `0x0043121a`, ftol `0x00431222`, `add edx,0x17e` at `0x00430ea1`, /640 via `0x66666667` / `sar 8` at `0x00430eaa`-`0x00430eb5`. With W = 640 that is 382 + 75 | `378 + MeasureMashedString(val, 0.72 law) + 9` (`exe_main.cpp`, `dyn_rax` in the kTriText branch) = 378 + 78.10 + 9 |

Attribution: **defect, standalone arrow formula, outside the approved fixes.** Two terms differ. The base is 387
(378 + 9) where the original uses 382. The scale is 0.72 where the original measures at 0.7, which is the split already
recorded as [UNCERTAIN] in the code comment. The original also truncates the width with ftol.
[UNCERTAIN] Whether `MeasureMashedString` at 0.7 returns the same width as `FUN_004282a0(0x1b1, 0.7)`. Missing
evidence: a call-level comparison of the two measures. Without it, a formula change cannot be predicted to land on
457 exactly. The value text itself (x 376.0 `0x43bc0000` at `0x004311f7` vs the standalone's 378, scale 0.7 vs 0.72)
belongs to the same item and is invisible to the draw-list diff (tex 9).

## Knob OFF (user's settings path)

`after_fix/sa_knob_off.json`, log `music=10 sfx=7 insults=7 insults_on=0 gamma=10 autosave=0` (the user's values).
- Pre-fix vs post-fix standalone, same settings (`diff_sa_prefix_vs_knob_off_tight.txt`, `--tol-pos 0.5`): exactly
  **12** rows per frame differ, all `moved`. They are the bg/top/left/right box quads of rows 0-2, each +1 px in y
  (187->188, 217->218, 247->248). The other 124 non-glyph rows are identical. The 100 MISSING rows per frame are
  side-A tex-9 glyph quads, because `--exclude-tex` filters side B only (`drawlist_diff.py:336-337`).
- Vs the original (`diff_orig1_vs_sa_off.txt`): the same two settings-value rows as before the fix, unchanged
  (row-0 fill w 100 vs 70, row-3 arrow 417.65 vs 457 for "Off" vs "Manual"). So with the user's settings, the
  only visible change is the 1 px box move.
- The repo-root `mashed_re_settings.bin` was read, never written: SHA-256 `95FCEF16...93C89`, mtime 2026-09-07T09:06:13,
  unchanged after all runs.

## Shared-code-path screens

- **Screen 30 (gamma slider)**: GREEN for the widget. Original captured fresh (`orig_scr30.json`, same burst recipe,
  synthetic push, no key input). The original draws it from a different function, with callers `0x004318c1`..`0x00431a93`
  (arrow y 232, box 233, bottom 246, fill 236, w = 30). It uses the same constants: `0x004318d4` -7.0,
  `0x00431932` +13.0, `0x004319a7` +3.0, `0x004319c2` x100.0, gate `0x004319cc`/`0x004319d4`. All 8 widget rows
  match within 0.01 px with the knob on (gamma 3). Default and strict: the only non-checker rows are the two
  preview half-quads at `0x42e80c` (parity_tooling validation finding 2, not widget). At 0.01 px the video quad
  also shows its -0.5 px half-texel offset, also not widget. The fix therefore also closed screen 30's 1 px box
  offset, which the shared code had before.
- **Screen 32 (autosave toggle)**: no bar, so fixes 1 and 2 do not run there. Default and strict: 0 non-checker
  RED. At 0.01 px the right arrow is 411.00 (`0x00431489`) vs 410.07 (0.93 px). That path is the toggle
  `dyn_rax` formula, which this change does not touch, so it is not a regression.

## Proposed CHANGELOG line (not applied, trackers untouched)

`2026-10-12 scr19 settings widget (standalone exe_main.cpp, no hooks.csv row): bar box moved to rowY-7 ([0x005cc9b8] @0x00430d5c), fill gated on value*100 > 1.0 ([0x005cc568]/[0x005cc320] @0x00430e50/0x00430e5a), new MASHED_SETTINGS_DEFAULTS=1 parity knob. scr19 widget RED 14 -> 1 strict, 2 -> 1 default (remaining: row-3 right arrow 457 vs 465.10, arrow formula + 0.7/0.72 scale split, [UNCERTAIN] measure parity). scr30 widget GREEN at 0.01 px, scr32 unchanged. Evidence verify/scr19_parity_20261012/after_fix/, note re/analysis/negctl/session_20261012b/scr19_fix_RESULT.md.`
