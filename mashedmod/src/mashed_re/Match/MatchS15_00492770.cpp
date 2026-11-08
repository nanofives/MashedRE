// MATCH-S15: port body of 0x00492770 = the byte-matched source re/match/s15/00492770.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/FrameDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00492770 MainLoopInit
#define FUN_0042b920 ConstantGetter22  // Match/MatchS07_0042b920.cpp (both targets), PORT-TOP-01
#define FUN_0042b950 Set7f1a0c_1000  // Match/MatchS07_0042b950.cpp (exe since PORT-TOP-01)
#define FUN_00433240 RaceArmReset  // Match/MatchS08_00433240.cpp (exe since PORT-TOP-01)
#define FUN_00495110 Store7f1030_Qpc3Mhz  // Match/MatchS16_00495110.cpp (new body, PORT-TOP-01)
// FUN_004c57a0 (RwMatrixCreate shape, re/match/rwl01/004c57a0.c): the matched body reads the RW globals at 0x007d3ff8,
// which the exe maps zero-filled, so the exe binds librw's Matrix::create (LibRw/RwBridge.cpp, L-98). The .asi keeps
// the raw-address call (MASHED.exe's own RW core).
#ifdef MASHED_STANDALONE
extern "C" unsigned int __cdecl MashedRw_MatrixCreate_004c57a0(void);
#define FUN_004c57a0 MashedRw_MatrixCreate_004c57a0
#else
#define FUN_004c57a0 MatchS15_00492770_Call004c57a0
#endif
// .asi only: raw-address forwarder for 0x004c57a0, the call the old Boot/FrameDispatch.cpp body made (the other four
// callees were forwarders too until PORT-TOP-01 bound them by name; verify/porttop_20261108/BINDINGS.tsv).
#ifndef MASHED_STANDALONE
extern "C" unsigned int MatchS15_00492770_Call004c57a0(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x004c57a0u)();
}
#endif
#pragma comment(linker, "/EXPORT:MainLoopInit=_MainLoopInit")
extern "C" {
#include "../../../../re/match/s15/00492770.c"
}
RH_ScopedInstall(MainLoopInit, 0x00492770);
