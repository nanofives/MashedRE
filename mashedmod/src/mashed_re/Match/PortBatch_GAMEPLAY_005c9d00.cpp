// PORT-BATCH-GAMEPLAY (D4, L-107 (a)): port body of 0x005c9d00 = the byte-matched source re/match/gffu/005c9d00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, C4 by byte match).
// In-place swap: this replaces the hand body GetRaceEndTrigger that sat in Util/GameStateGetters.cpp. Both targets,
// like the hand body. Callers (Race/GameModeMachine.cpp, Match/PortTop_00492370.cpp, Match/PortTop_00493710.cpp)
// bind FUN_005c9d00 to GetRaceEndTrigger by name.
// No RH_ScopedInstall, as before: the original is 3 bytes (33 c0 c3) and a 5-byte JMP would overwrite past it
// (hooks.csv C3->C2 demotion 2026-05-22).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_005c9d00 GetRaceEndTrigger
#pragma comment(linker, "/EXPORT:GetRaceEndTrigger=_GetRaceEndTrigger")
extern "C" {
#include "../../../../re/match/gffu/005c9d00.c"
}
