# RESULT fe_residuals (2026-10-16, model claude-sonnet-5-5)

Base: 017fc6984c4d0240cf4346c6b78d0ab707aadc36 (= race/first-frame-parity). Branch work/fe_residuals_b, worktree .worktrees/fe_residuals.
The prepared worktree .claude/worktrees/fe_residuals (branch work/fe_residuals) was locked by live claude pid 13016 (EnterWorktree refused), so a fresh worktree was made.

## STATUS: PARTIAL. Item 2 narrowed (one small fix committed). Items 1 and 3 NOT STARTED.
No standalone build, no original-side capture, no G-CTRL run was done. Default build behaviour is unchanged (no .cpp touched).

## Item 2 / U-9680: static derivation (Ghidra slot Mashed_pool1, read-only), not yet a replacement for 0x3f7fffff
- FUN_004275d0 (0x004275d0): FUN_00552a60(p), FUN_00552d10, FUN_00552750, u=FUN_00553ef0, FUN_00552920(u), FUN_00553cf0(u,&0x0067d828), ...
- FUN_00553cf0 (0x00553cf0) fills float[4] at 0x0067d828: [0]=min x, [1]=min y, [2]=max x - min x (final 0x00553dd?), [3]=max y - min y. So 0x0067d830 = bbox WIDTH and 0x0067d834 = bbox HEIGHT of the path built by FUN_00552920 (bbox per path: FUN_00553c50, points at stride 6 floats).
- FUN_00552920 (0x00552920): rectangle corners at +-0.5 ([0x005cd50c]=0xbf000000, [0x005cc32c]=0x3f000000) times DAT_00912b08/0c (width/height) times s, s = 1.0 if [[0x00912c0c]+0x14]==2 else DAT_00912be0 (only write 0x00552c48 = 0x3f800000). The 4 points then go through the RW function at *(DAT_007d3ffc+0xc+DAT_007d3ff8) (FUN_004c3d90) with matrix DAT_00912b58 built by FUN_00552e40.
- DAT_00912b08/0c = 2 * camera view window x/y (0x00552a99; FUN_00552a60 reads [cam+0x68]/[cam+0x6c]). Frontend view window is 0.8 (exe_main.cpp comment), so the untransformed rectangle is 1.6 wide; 0x3f7fffff must come from the matrix/transform (FUN_00552e40 builds it from FUN_004c4600/4c5010/4c51a0/4c4dc0 and the camera matrix via FUN_004c0ed0). I did NOT decode those, so the 1-ulp-below-1.0 value is not derived.
- Conclusion: the value is a function of the frontend camera view window (not a font constant); it is the same for x and y at view window 0.8x0.8. The measured 0x3f7fffff stays in exe_main.cpp:2898. Proposed U-9680 narrowing (tracker NOT edited): "divisors are bbox width/height of FUN_00552920's rectangle through FUN_00552e40's matrix; owed: decode FUN_004c4600/4c5010/4c51a0/4c4dc0 + FUN_004c0ed0 and the RW transform to reproduce 0x3f7fffff bit-exactly; other screens use the same camera so no per-screen variation expected [UNCERTAIN, unprobed]".

## Item 2 residue: sumdiff.py (done, commit below)
verify/scr19_parity_20261012/after_fix/sumdiff.py: the fixed 0xb8202 return-address filter is now keyed on the map symbol LogoOverlayDraw when run with `--map <mashed_re.map>` (reuses re/tools/drawlist_diff.load_msvc_map); the old dead `'LogoOverlayDraw' in json.dumps(r)` test (JSON has no symbols) is gone. Without --map it falls back to 0xb8202. NOT verified with a map (no .map on disk, no build run); verified only that the no-map path gives the same output as before on diff_orig1_vs_sa_on.json (matched=135, other=2).
Not done: scr32 value text 1 px low, 0x00430b90 port text y 284 vs 285 (both need original captures + standalone runs).

## Item 1 / U-9672 (3 sub-items) and Item 3 / collision-FX re-measure: not started. Needs original captures / race runs.
