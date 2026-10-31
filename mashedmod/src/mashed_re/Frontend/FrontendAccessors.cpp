// Mashed RE - Frontend accessor reimplementations.
// Analysis note: re/analysis/hud_frontend_d3/
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FrontendGlobalGet  --  0x0040ad20
//
// Original: FUN_0040ad20 (6 bytes, 0x0040ad20..0x0040ad26)
// Returns: DAT_008a95ac (uint32_t global, 4 bytes).
// No parameters, no branches, no side-effects.
// ref: re/analysis/hud_frontend_d3/0x0040ad20.md
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040ad20 (FrontendGlobalGet) moved to Match/MatchS02_0040ad20.cpp,
// which compiles the byte-matched source re/match/s02/0040ad20.c.

// ---------------------------------------------------------------------------
// FrontendArrayGet  --  0x0040b6c0
//
// Original: FUN_0040b6c0 (11 bytes, 0x0040b6c0..0x0040b6cb)
// Signature: uint32_t(int param_1)
// Returns: DAT_008a94f0[param_1] — 4-byte indexed read from array base.
// No branches, no side-effects.
// Array base address cited at: 0x0040b6c3 -> 0x008a94f0
// Note: no bounds check in original — cast param_1 directly as index.
// ref: re/analysis/hud_frontend_d3/0x0040b6c0.md
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040b6c0 (FrontendArrayGet) moved to Match/MatchS02_0040b6c0.cpp,
// which compiles the byte-matched source re/match/s02/0040b6c0.c.
