// MATCH-S15: port body of 0x00493550 = the byte-matched source re/match/s15/00493550.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/LaunchHandshake.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define thunk_FUN_004938c0 thunk_EngineStopDispatch
#define FUN_004938c0 MatchS15_00493550_Call004938c0
// Callees of the matched source: the old Boot/LaunchHandshake.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" void MatchS15_00493550_Call004938c0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004938c0u)();
}
#pragma comment(linker, "/EXPORT:thunk_EngineStopDispatch=_thunk_EngineStopDispatch")
extern "C" {
#include "../../../../re/match/s15/00493550.c"
}
RH_ScopedInstall(thunk_EngineStopDispatch, 0x00493550);
