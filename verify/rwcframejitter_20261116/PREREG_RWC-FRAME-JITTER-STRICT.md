# PREREG RWC-FRAME-JITTER-STRICT (2026-11-16, run 51)

Row: 42 of 82 menu draws per frame (A `@0x474464,0x42e89c`, B `RwCore_00473ee0+0x70c`) match the original only with
`drawlist_diff.py --tol-anim 8`. Goal: strict compare at an aligned jitter counter, target 80 / 82 per frame.

## The counter (cited)
- The checker jitter of 0x00473ee0 reads the float DAT_007f1010: original `fld dword ptr [0x7f1010]` at 0x00474167,
  `fadd st(0), st(0)` at 0x0047416d, the doubled value stored at 0x0047416f and added to each corner index before
  `fsin` (0x00474182, 0x0047419a, 0x004741d2, 0x004741f2, 0x00474208, 0x00474220, 0x00474234). Source form
  re/match/s11/00473ee0.c (`w2 = DAT_007f1010 + DAT_007f1010`).
- Writer on the menu: 0x00493390 (re/match/s15/00493390.c, `DAT_007f1010 = (float)now * 3.3333333e-07f`, now from
  0x004950b0), run from 0x00493480 outside 0x00492e90. Other writers in re/match: 0x00428d30 (+0.016666668f) and
  0x004111c0, not on the title frame. [UNCERTAIN] from code reading that no other writer runs inside the captured span;
  tested by the per-frame close log below.

## Change (default OFF, dump knob only)
- `re/frida/menu_draw_burst.py --peek-draw 0x007f1010`: each captured original draw record also stores the dword read
  at the draw (`"pk"`). Without the option the output is unchanged.
- `MASHED_RWC_DRAWDUMP_TPIN=<hex,...>` (RwCore/RwCore_Knob.cpp, read only when MASHED_RWC_DRAWDUMP is set): at the
  delimiter 0x00492e90 that opens dumped frame k, DAT_007f1010 := word k; logs the value before the pin and at the
  frame's close. Default path and knob-off paths do not read it.

## Arms
1. Original: `menu_draw_burst.py --frames 4 --delim 0x00492e90 --peek-draw 0x007f1010 --peek 0x0067eca4,0x0067ecb0,0x00771968`.
2. Core vs_dump and ddk_dump as RWC-FRAME-CLUMP run 49 (`MASHED_RWC_FRAME_N=590 MASHED_OT_TIMER=1 SKIP=200 FRAMES=4`)
   plus `MASHED_RWC_DRAWDUMP_TPIN` = the 4 original words.
3. `drawlist_diff.py --scale-b 1` strict (`--tol-anim 0`) and `--tol-anim 8`, orig_f0..3 vs core_f0..3.

## Scoring
- GREEN: strict 80 / 82 per frame on the object build (missing 2 = RWC-FRAME-MOVIE-BACKDROP, RWC-FRAME-TITLE-LOGO).
- The VS2022 build is reported apart: a VS2022 compile of a matched source need not compute the original's values (L-125).
- RED if strict stays below 80 with the clock pinned: report the residual rows and their coordinate deltas.
- Close log must show DAT_007f1010 unchanged from the pinned word; otherwise the pin point is wrong (RED, reason).
