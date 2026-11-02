// Mashed RE — Frontend near-leaf reimplementations (c3-batch-s session 6).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00426cb0  SlotIndexToPtr       — 12B pure leaf; returns &DAT_00663664 + param_1*0x4c
//   0x00426d90  HandleArrayRelease   — void(void) bulk-release; callee 0x004e6680 C1 (DEFERRED)
//   0x0042a640  PathBuilderLoad      — char*(char*) vtable+file dispatch; no suitable arg_type (DEFERRED)
//   0x0042b920  ConstantGetter22     — 5B constant getter; returns 0x16 unconditionally
//   (0x0042bde0 HudRectEmitter was removed 2026-10-16, dupinstall: its one-body-per-RVA copy
//    is HudRectEmitter5 in Frontend/BatchAA_s1.cpp, the body hooks.csv's evidence cites)
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s4/FUN_00426cb0.md
//   re/analysis/frontend_c1_to_c2_s5/FUN_00426d90.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042a640.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042b920.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// SlotIndexToPtr  --  0x00426cb0
//
// Original: FUN_00426cb0 (12 bytes, 0x00426cb0..0x00426cbc)
// Signature: undefined* FUN_00426cb0(int param_1)
//   param_1: slot index
// Returns: pointer to slot N = &DAT_00663664 + param_1 * 0x4c
//
// Body (pure leaf, no branches):
//   return &DAT_00663664 + param_1 * 0x4c;  [0x00426cb0]
//
// Constants (cited from 0x00426cb0 body):
//   0x00663664 — base address of the 0x4c-stride slot table   [0x00426cb0]
//   0x4c (76)  — stride per slot (bytes)                      [0x00426cb0]
//
// Callees: none (pure leaf).
//
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426cb0.md
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x00426cb0 (SlotIndexToPtr) moved to Match/MatchS06_00426cb0.cpp,
// which compiles the byte-matched source re/match/s06/00426cb0.c.

// ---------------------------------------------------------------------------
// HandleArrayRelease  --  0x00426d90
//
// DEFERRED — callee 0x004e6680 (RpClumpRender) is C1; callee gate fails.
// Implementation authored as placeholder; NOT registered for hook install.
//
// Original: FUN_00426d90 (void(void))
// Body: iterates DAT_00656f48..DAT_00656f54 (4 int handles);
//       calls FUN_004e6680 on each non-null handle.
//
// Constants:
//   0x00656f48 — first handle slot   [0x00426d90]
//   0x00656f58 — exclusive end addr  [0x00426d90]
//
// ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426d90.md
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x00426d90 (HandleArrayRelease) moved to Match/MatchS06_00426d90.cpp,
// which compiles the byte-matched source re/match/s06/00426d90.c.

// ---------------------------------------------------------------------------
// PathBuilderLoad  --  0x0042a640
//
// DEFERRED — no suitable Frida arg_type for char* filename + vtable dispatch.
// Implementation authored as placeholder; not promoted this session.
//
// Original: FUN_0042a640 (undefined4 FUN_0042a640(char* param_1))
// Body:
//   1. vtable[0xcc](param_1) — copy filename into DAT_0067e3a8.
//   2. if (!_strchr(param_1, '.')) vtable[0xd4](&ext) — append extension.
//   3. if (FUN_0042a530(&DAT_0067dba8, &DAT_0067e3a8) != 0)
//        return FUN_004b3c60();
//   4. return 0;
//
// Constants:
//   0x007d3ff8 — vtable-like dispatch table base   [0x0042a640 body]
//   0xcc       — copy-string offset                [0x0042a640 body]
//   0xd4       — append-extension offset           [0x0042a640 body]
//   0x005f65b4 — pointer-to-default-extension str  [0x0042a640 body]
//   0x0067dba8 — start of path/name buffer         [0x0042a640 body]
//   0x0067e3a8 — destination string buffer         [0x0042a640 body]
//   0x2e ('.')  — extension separator              [0x0042a640 body]
//
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042a640.md
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x0042a640 (PathBuilderLoad) moved to Match/MatchS06_0042a640.cpp,
// which compiles the byte-matched source re/match/s06/0042a640.c.

// ---------------------------------------------------------------------------
// ConstantGetter22  --  0x0042b920
//
// Original: FUN_0042b920 (5 bytes, 0x0042b920..0x0042b924)
// Signature: undefined4 FUN_0042b920(void)
//   Returns: 0x16 (22) unconditionally, no side-effects.
//
// Body (pure constant getter):
//   MOV EAX, 0x16   [0x0042b920]
//   RET              [0x0042b924]
//
// Callees: none (pure leaf).
//
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042b920.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042b920 (ConstantGetter22) moved to Match/MatchS07_0042b920.cpp,
// which compiles the byte-matched source re/match/s07/0042b920.c.

// ---------------------------------------------------------------------------
// FUN_0042bde0 (5-call HUD rect emitter via ChromeBaseDraw 0x00472c60) was ported here
// first as HudRectEmitter; its one-body-per-RVA copy is HudRectEmitter5 in
// Frontend/BatchAA_s1.cpp, the body hooks.csv's evidence cites (draw_quad_observe,
// diff_hud_rect_emitter5.csv). The duplicate that sat here was removed 2026-10-16
// (dupinstall).
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md
// ---------------------------------------------------------------------------
