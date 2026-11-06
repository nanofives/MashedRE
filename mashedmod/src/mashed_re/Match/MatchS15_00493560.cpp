// MATCH-S15: port body of 0x00493560 = the byte-matched source re/match/s15/00493560.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/LaunchHandshake.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define thunk_FUN_004954f0 thunk_HwExitDispatch
#define FUN_004954f0 MatchS15_00493560_Call004954f0
// Callees of the matched source: the old Boot/LaunchHandshake.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" unsigned int MatchS15_00493560_Call004954f0(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x004954f0u)();
}
#pragma comment(linker, "/EXPORT:thunk_HwExitDispatch=_thunk_HwExitDispatch")
extern "C" {
#include "../../../../re/match/s15/00493560.c"
}
RH_ScopedInstall(thunk_HwExitDispatch, 0x00493560);
