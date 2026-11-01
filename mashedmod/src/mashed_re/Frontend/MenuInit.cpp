// Mashed RE - Frontend menu initialisation reimplementations.
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042fe80.md  (GetRaceEndFlag)
//   re/analysis/promote_c2_frontend_menus/0x0042f0b0.md  (GetFrameCounterPlus73)
//   re/analysis/promote_c2_frontend_menus/0x0042d3e0.md  (MenuEntryArrayInit)
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetRaceEndFlag  --  0x0042fe80
//
// Original: FUN_0042fe80 (5 bytes, 0x0042fe80..0x0042fe85)
// Body: MOV EAX, [0x0067ea90]; RET
// Returns: DAT_0067ea90 (race-end flag or slot gate).
// No callees, no branches, no side-effects.
// ref: re/analysis/promote_c2_frontend_menus/0x0042fe80.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042fe80 (GetRaceEndFlag) moved to Match/MatchS07_0042fe80.cpp,
// which compiles the byte-matched source re/match/s07/0042fe80.c.

// ---------------------------------------------------------------------------
// GetFrameCounterPlus73  --  0x0042f0b0
//
// Original: FUN_0042f0b0 (8 bytes, 0x0042f0b0..0x0042f0b8)
// Body: MOV EAX, [0x0067f17c]; ADD EAX, 0x49; RET
// Returns: DAT_0067f17c (animation frame counter) + 0x49 (73, cited at 0x0042f0b4).
// No callees, no branches, no side-effects.
// ref: re/analysis/promote_c2_frontend_menus/0x0042f0b0.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042f0b0 (GetFrameCounterPlus73) moved to Match/MatchS07_0042f0b0.cpp,
// which compiles the byte-matched source re/match/s07/0042f0b0.c.

// ---------------------------------------------------------------------------
// MenuEntryArrayInit  --  0x0042d3e0
//
// Original: FUN_0042d3e0 (59 bytes, 0x0042d3e0..0x0042d41a). Clears 14 stores per entry over 30 entries of a 52-byte struct array at 0x00898ac0..0x008990dc.
// The body is Frontend/MenuEntryZero.h:MenuEntryZeroBody, written from the disassembly (verify/dualcopy4_20261016/disasm_42d3e0.txt): dwords at
// entry +0, +4, +8, bytes +12..+15, dwords +16, +20, +24, +28, +36, +40, +44; entry +32 and entry 30 onwards are NOT written.
//
// dualcopy4 (2026-10-16) CORRECTION. The body that stood here (C3 at 9b0248128) did not match those bytes: it wrote the dword before the entry,
// bytes +9..+12 instead of +12..+15, +32 instead of +28 and +44 (host test verify/dualcopy4_20261016/maskcheck_42d3e0.txt). Its registry key
// observed only the first dword of entry 0, which both bodies clear, so it could not see that. The exe's Frontend/MenuNavSM.cpp:RecordsZero
// (memset of its own g_records) is now a call into the same body.
// ref: re/analysis/promote_c2_frontend_menus/0x0042d3e0.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042d3e0 (MenuEntryArrayInit) moved to Match/MatchS07_0042d3e0.cpp,
// which compiles the byte-matched source re/match/s07/0042d3e0.c.
// It writes the same 14 stores per entry as Frontend/MenuEntryZero.h:MenuEntryZeroBody, which stays only as the helper the
// exe's Frontend/MenuNavSM.cpp:RecordsZero runs on its own g_records (port memory, not this RVA's body).
