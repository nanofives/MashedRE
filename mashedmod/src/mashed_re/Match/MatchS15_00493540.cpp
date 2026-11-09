// MATCH-S15: port body of 0x00493540 = the byte-matched source re/match/s15/00493540.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/LaunchHandshake.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define thunk_FUN_00495150 thunk_LaunchLangGate
// [PORT-TOP-04A] exe: 0x00495150 binds by name to its matched exe body (Match/PortTop_00495150.cpp). The .asi keeps
// the raw-address call.
#ifdef MASHED_STANDALONE
#define FUN_00495150 HwInitGate_00495150
#else
#define FUN_00495150 MatchS15_00493540_Call00495150
// Callees of the matched source: the old Boot/LaunchHandshake.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" bool MatchS15_00493540_Call00495150(void) {
    return reinterpret_cast<bool (__cdecl*)()>(0x00495150u)();
}
#endif
#pragma comment(linker, "/EXPORT:thunk_LaunchLangGate=_thunk_LaunchLangGate")
extern "C" {
#include "../../../../re/match/s15/00493540.c"
}
RH_ScopedInstall(thunk_LaunchLangGate, 0x00493540);
