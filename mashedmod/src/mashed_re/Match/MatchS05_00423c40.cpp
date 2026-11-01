// MATCH-S05: port body of 0x00423c40 = the byte-matched source re/match/s05/00423c40.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SlotZeroers_s2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00423c40 PlayerScoreTeamAccBase
#define FUN_0042f500 GetDat0067ea64
#pragma comment(linker, "/EXPORT:PlayerScoreTeamAccBase=_PlayerScoreTeamAccBase")
extern "C" {
#include "../../../../re/match/s05/00423c40.c"
}
RH_ScopedInstall(PlayerScoreTeamAccBase, 0x00423c40);
