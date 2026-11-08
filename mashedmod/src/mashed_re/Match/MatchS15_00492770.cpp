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
#define FUN_00495110 MatchS15_00492770_Call00495110
#define FUN_004c57a0 MatchS15_00492770_Call004c57a0
// Callees of the matched source: the old Boot/FrameDispatch.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" void MatchS15_00492770_Call00495110(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00495110u)();
}
extern "C" unsigned int MatchS15_00492770_Call004c57a0(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x004c57a0u)();
}
#pragma comment(linker, "/EXPORT:MainLoopInit=_MainLoopInit")
extern "C" {
#include "../../../../re/match/s15/00492770.c"
}
RH_ScopedInstall(MainLoopInit, 0x00492770);
