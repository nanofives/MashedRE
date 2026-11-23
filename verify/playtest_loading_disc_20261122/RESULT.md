# Playtest fix: loading-screen spinning disc showed no texture (2026-11-22)

Verdict: FIXED on the default path (hand path, MASHED_ORIG_TOP off). Two defects in `mashedmod/src/mashed_re/exe_main.cpp`, both on the boot loading splash (`g_frontend_phase == 0`), the only place the default exe draws the disc (`kHandleLoadIcon`, one draw site).

## Reproduction

- The shipped LAN build (`mashedmod/build/lan_release/mashed_re.exe`, run in the staged layout with no `MASHED.exe`) embeds `RWTEXDICTIONARY/404` (33932 bytes, lang 0x809) and logs `loadicon: 256x128 decode+upload OK`. The asset is present and loads, so this is not a missing-MASHED.exe layout problem: it fails in every layout.
- The disc draws as a flat white ellipse: `before_lan_splash_f40.png`, `before_splash_001.png`. Disc-region saturation 0.0 on every splash frame (`pick.py`).

## Original at the same moment

`MASHED_ORIG_BBDUMP_REQ` timed dumps (`run_orig.ps1`): `orig_t069_viewportinit_disc.png` (ViewportInit 0x00428590 frame, phase 1.5707964, front face), `orig_t084_splash.png` (boot splash 0x00428a30, back face). The front is a rainbow CD, the back is the MASHED logo in a ring, read left to right.

## Cause

1. Texture decode. `LoadLoadIconResource` wrote RGB = 255 and kept only palette alpha. The TXD palette is full-colour RGBA (224 of the 255 used entries are not grey, `loadicon_rgba_on_black.png`). The original reads the TXD with RW's own reader (0x004921d0, 100% match: `FUN_004997b0(0x194, "RWTEXDICTIONARY")` -> `FUN_004b3eb0` -> `FUN_004c5c00(dict, "LoadIcon")` into `DAT_00771960`), and 0x00428450 draws it with colour 0xff x4, so the palette colours reach the screen unchanged.
2. Back-face UVs. 0x00428450 (re/match/s06/00428450.c) uses uv {1.0, 0, 0.5, 1} for sin < 0 together with a negative width `r.w = s*80*w`. 0x00450b10 (re/match/s08/00450b10.c) puts u0 at x and u1 at x + w, so the right edge gets u = 1.0 and the left edge u = 0.5: the right half, not mirrored. The port draws a positive width from the left edge with the same UVs, which mirrored the logo (hidden while the disc was white).

## Fix

- Expand the palette through R,G,B,A into BGRA (same order as `QuadRenderer.cpp:239-246`).
- Back-face UVs `{0.5, 0, 1.0, 1}` left to right.

## Evidence (disc region x 496..592, y 36..124, `re/tools/imgdiff.py --threshold 32`, closest-phase pairs)

| Pair | Before: mean abs diff / px over 32 | After |
|---|---|---|
| back (orig_t084 vs before_001 / after_006) | 107.88 / 50.02% | 5.10 / 7.55% |
| front (orig_t069 vs before_002 / after_000) | 66.99 / 47.01% | 4.55 / 2.44% |

The remaining difference is 1 to 2 px of spin-phase width (the captures are not phase-synced) and the original's R5G6B5 backbuffer. Montage: `disc_orig_before_after.png` (rows back, front; columns original, before, after). Heatmaps: `heat_*.png`.

Draw-list diff not run: `menu_draw_burst.py` reaches screens by nav push and the boot splash has no screen id. Quad geometry already agreed before the fix (disc rows 44..117 and centre x 544 on both sides). The change is texture content and UV orientation, which the pixel compare covers.

## Gates (fixed exe b949ac89, `verify/savelink_20261023/run_arms.py`, root D:/MashedWT/mtrain_root)

G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3: equal to the `re/orchestrator/merge_train.json` refs.
