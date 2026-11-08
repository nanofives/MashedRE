// Mashed RE — Frontend score-sort and player-color helpers.
// Analysis notes: re/analysis/frontend_promote_menus_b/
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// NOT INCLUDED here (callee gate failures):
//   0x0040b460  SlotSortByScoreWithModeOverride — refused: callee 0x00417740 is C0 (STUB S-0491)
//   0x00429a30  LapTimeStoreToPlayerArrays      — blocked: callee 0x00430790 is C1 with open U-3470

#include "../Core/HookSystem.h"
#include "RowMovers.h"

#include <cstdint>

// SplitScreenTrackAssignment (FUN_00430830): the byte-matched source re/match/s07/00430830.c is its one body,
// Match/MatchS07_00430830.cpp (PORT-STATE-01, 2026-11-08). The hand port that stood here took the mode word as a
// parameter. The exe callers now pass it through the DAT_0067e9fc bridge (Frontend/PortStateFrontend.h).

// ---------------------------------------------------------------------------
// PlayerColorTableGet  --  0x0040e3a0
//
// Original: FUN_0040e3a0 (141 bytes, 0x0040e3a0..0x0040e42d)
// Signature: void FUN_0040e3a0(undefined4 param_1, undefined1 *param_2)
//   param_1: player color index 0..5
//   param_2: RGBA byte[4] output pointer
//
// 6-way switch writing 4 RGBA bytes to param_2.
// For the default case, calls FUN_004a332b(0xfffffff2) — non-returning abort.
//
// Color table (all bytes cited from body 0x0040e3a0..0x0040e42d):
//   case 0: (0x98, 0x3a, 0x3d, 0xff)  — red-ish
//   case 1: (0x4e, 0x89, 0xae, 0xff)  — blue-ish
//   case 2: (0x61, 0x76, 0x56, 0xff)  — green-ish
//   case 3: (0xdb, 0xc3, 0x62, 0xff)  — yellow-ish
//   case 4: (0xeb, 0xa7, 0xa7, 0xff)  — pink-ish
//   case 5: (0x00, 0x00, 0x00, 0xff)  — black
//   default: FUN_004a332b(0xfffffff2) — abort (0xfffffff2 = -14 signed)
//
// FUN_004a332b is an abort/assert — non-returning cold path; C-level irrelevant.
// Declared extern as an abort helper to match original behavior exactly.
//
// ref: re/analysis/frontend_promote_menus_b/0040e3a0.md
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040e3a0 (PlayerColorTableGet) moved to Match/MatchS02_0040e3a0.cpp,
// which compiles the byte-matched source re/match/s02/0040e3a0.c.
