// MATCH-S05: port body of 0x00423dd0 = the byte-matched source re/match/s05/00423dd0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuMiscLeaves_t2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00423dd0 PlayerScoreTeamAccGatedC
#define FUN_0042f500 GetDat0067ea64
#define FUN_0042f6a0 MatchS05_00423dd0_Call0042f6a0
// Callee of the matched source: the old Frontend/MenuMiscLeaves_t2.cpp body called FUN_0042f6a0 through its original address
// (a __cdecl function-pointer cast to the raw address), so this forwarder makes that same call.
extern "C" int MatchS05_00423dd0_Call0042f6a0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0042f6a0u)();
}
#pragma comment(linker, "/EXPORT:PlayerScoreTeamAccGatedC=_PlayerScoreTeamAccGatedC")
extern "C" {
#include "../../../../re/match/s05/00423dd0.c"
}
RH_ScopedInstall(PlayerScoreTeamAccGatedC, 0x00423dd0);
