// ViewportScaledRectDraw.cpp -- 0x00428610, re-ported from the listing as an x87 TU (bnormalise 2026-10-12).
// Was Frontend/Cluster_v3.cpp (c3-batch-v session 3).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Original: FUN_00428610 (body 0x00428610..0x00428753, `add esp,0x48` at 0x00428750 = 0x2c locals + 7 pushed args).
// Signature: void __cdecl (uint32* tex, float x, float y, float w, float h, int coordMode, int blendMode).
// Read with `py -3.12 re/tools/disasm_fn.py 0x00428610 0x00428754` on original\MASHED.exe.unpatched.
// Callees by Ghidra function boundary: 0x0042b8b0, 0x0042b8c0, 0x00450b10.
//
// DEFECTS IN THE OLD BODY (each corrected from the listing):
//  1. coordMode not 0/1/2: the original leaves x and y at the 0.0 stored at 0x00428615 / 0x0042861d (`jne 0x4286f3`
//     at 0x004286ab skips both stores). The old body fell through to the coordMode-0 formula.
//  2. coordMode 2: 0x004286ad `fld [esp]; fmul x` keeps sx*x in ST and subtracts the STORED w ([esp+0x14], rounded at
//     0x00428695) times [0x5cc32c], rounding once at 0x004286c1 `fstp`. The old SSE2 body rounded sx*x to float first.
//     This TU is in mashedmod/x87_tus.txt: every value the listing keeps in ST is a `double`, rounded to `float` only
//     where the bytes `fstp dword`, so the body follows the caller's precision control as the original does.
// Both were invisible to the old draw_quad_observe entry: its vectors used coordMode 0..2 only, and it read the
// 0x00898a20 vertex block at the harness CW alone. The texture argument never reached that block at all
// (0x00450c15 passes it to the render-state slot, not the vertex buffer).

// MATCH-S06: the body of 0x00428610 (ViewportScaledRectDraw) moved to Match/MatchS06_00428610.cpp,
// which compiles the byte-matched source re/match/s06/00428610.c.
