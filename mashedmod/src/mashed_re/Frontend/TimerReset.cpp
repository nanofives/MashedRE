// Mashed RE - Timer reset / global-clear reimplementations.
// timer_d2_cont1 cluster — all pure leaves, no callees, no branches outside loops.
//
// RVA 0x00422b30  FUN_00422b30  "TimerArrayClear"
//   16 bytes. Loops 0x138 (312) times, writing 0 to successive dwords starting
//   at DAT_00899e80. Equivalent to memset(0x00899e80, 0, 312 * 4 = 1248 bytes).
//   No calls, no branches outside loop.
//
// RVA 0x0040b810  FUN_0040b810  "TimerGlobalsReset"
//   124 bytes. Writes 0 to 21 explicit global addresses across three address
//   ranges (0x008a94f0..0x008a94fc, 0x008a9530..0x008a953c, 0x008a9540..0x008a956c)
//   plus one isolated write at 0x0063b8ec. Decomp write order reproduced exactly.
//   No calls, no branches.

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// MATCH-S05: the body of 0x00422b30 (TimerArrayClear) moved to Match/MatchS05_00422b30.cpp,
// which compiles the byte-matched source re/match/s05/00422b30.c.


// MATCH-S02: the body of 0x0040b810 (TimerGlobalsReset) moved to Match/MatchS02_0040b810.cpp,
// which compiles the byte-matched source re/match/s02/0040b810.c.


// ---------------------------------------------------------------------------
// FUN_0042af50 (menu readiness check) was ported here first as MenuReadinessCheck; its
// one-body-per-RVA copy is MenuReadinessCheckC in Frontend/MenuStateMachine.cpp, the
// body hooks.csv's evidence cites (diff_menu_readiness_check_c.csv). Both bodies read
// identically on both paths (guard, 0x1000 dispatch, 0x007f1501/0x007f1041 char tables
// at stride 0x4c, 0x007f1a14 index array, 0x0040e470 callee). The duplicate that sat
// here was removed 2026-10-16 (dupinstall).
// ---------------------------------------------------------------------------
