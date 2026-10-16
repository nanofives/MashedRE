# RESULT fe_residuals (2026-10-16, model claude-sonnet-5-5)

Base: 017fc6984c4d0240cf4346c6b78d0ab707aadc36 (= race/first-frame-parity). Branch work/fe_residuals_b, worktree .worktrees/fe_residuals.
The prepared worktree .claude/worktrees/fe_residuals (branch work/fe_residuals) was locked by live claude pid 13016 (EnterWorktree refused), so a fresh
worktree and branch were made. The prepared one was left untouched.

## G-CTRL (default build, run_h4_wt.ps1 F_NEW, private MASHED_ROOT gameroot)
Before any change (017fc698 code): 991a5fff / be0d47ea / bc75b405. After all three items (final tree): 991a5fff / be0d47ea / bc75b405.
Note: the exe chdirs into MASHED_ROOT, so a worktree run with MASHED_ROOT = the main checkout writes mashed_re.log / player_trace.log into the
shared main checkout (a first run did, before it was noticed). All later runs used a private copy of original\ (~480 MB) as MASHED_ROOT.

## Item 2 (U-9680 and residues): CLOSED, commits ccb5548a, 0b46d491 (and e3ef411b, the earlier partial)
- U-9680: [0x0067d830] / [0x0067d834] derived. Probe (verify/fe_residuals_20261016/u9680/rect_probe.py, original, read-only): view window 0x3f4ccccd,
  DAT_00912b08/0c 0x3fcccccd, DAT_00912b18 = diag(-1.6,1.6,1) pos(0.8,-0.8), DAT_00912b58 (cofactor inverse, flag word 0) diag(-0.62499994,0.62499994)
  pos(0.49999997). The ported RwMatrixInvert reproduces all 16 words at PC_24 (inv_test.cpp); the rectangle corners +-0.8 map to a 0x3f7fffff-wide box.
  exe_main.cpp `DeriveMenuMeasureDiv` computes it (0x3f7fffff at PC_24 and PC_64, derive_test.cpp). Standalone draw lists for screens 19/30/32 are
  byte-identical to the pre-change build except the animated logo-overlay rows (sa_ctrl_* vs sa_new_*). Row closed in UNCERTAINTIES.md, CHANGELOG line added.
- scr32 value text y 240.0 and scr19 value text y 285.0 (the 0x00430b90 row): fresh original captures (text/orig_frame{19,32}.json, text_frame_probe.py)
  vs the current build, text_quad_diff.py: 103 and 71 glyphs, dy 0.0000. The NEXT_SESSION notes predate the 2026-10-13 text-y anchor fix. No code change.
- sumdiff.py keys the LogoOverlayDraw checker on the map symbol with --map (not run against a map; no-map output unchanged).
- Whole-screen drawlist_diff vs the original is RED on this base for reasons present before the change (the logo overlay and the order of 14 draws);
  ctrl and new builds give identical diffs, so the change is neutral (u9680/diff_orig_vs_{ctrl,new}_*.txt, diff_cur_*.txt).

## Item 1 (U-9672): NARROWED, commit 9ff28b50
Done: screens 20/23 captured on the original; one real defect fixed (0xff4c0000 on screen 23 now pushes screen 1, slot kind = screen id); single-OK
refusal popups 0x24f (no opponents) and 0xd6..0xd9 (team composition) open; key repeat on hold for screens 6/7 matches the original timeline
(first repeat 0.4 s, then 0.2 s). Not done, each named with its bytes in u9672/RESULT.md section E and the U-9672 row: push-6 side writes
(consumers: screen-6 overlay state), the `DAT_00898ab0 == 0` input gate (writers not decoded), the refusal sound and the `_DAT_0067e840` overlay ramp
(sound source not located), rows 8..12 vs 8 cup tracks. mashedmod\run_tests.bat: navsm / ruleengine / racemodes GREEN; menustr / badges FAIL only for
lack of original\TOASTART in the worktree.

## Item 3 (collision FX re-measure): DONE, commit 695724f3
verify/fe_residuals_20261016/fx/RESULT_FX.md. Original-side: only the spark path has a counterpart (24 bursts / 115 particles in about 45 s of a 4-car
race, FUN_004864f0 / 00486370 / 00486610 entry counters); skids have no original counterpart. Default build, 240 s: skids/slot 1027 / 2519 / 12668 / 16959,
sparks 392 / 302 / 1498 / 1728, pool peak 1200 of 1200, median 188. Cause: EmitCarFx received the chain's internal-unit speed (about 389x the world
step; |dpos|/dt / sp = 0.00257 over 3449 frames). Fixed from the position step: skids 62 / 815 / 5139 / 5489, sparks 2 / 4 / 15 / 32, pool peak 598, median
110. Baseline compared against for skids/pool: the 2026-08-15 standalone numbers (772/358/241/378; peak 175, median 130), a different race length and
track. Residual: AI slots 2/3 still over-emit skids (14-22x that baseline); thresholds untouched.
