// PORT-BATCH-GAMEPLAY (D4, L-107 (a)): port body of 0x00407b70 = the byte-matched source re/match/s01/00407b70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, C4 by byte match).
// In-place swap: this replaces the hand body RangeTableMemberTest that sat in Gameplay/RangeTable_ah1.cpp.
// Both targets, like the hand body. The exe callers (Race/PathProgStep_naked.inc, Race/PathProgStep.cpp) keep
// calling RangeTableMemberTest by name; the prototype is the hand one (cdecl, int args, result in eax).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00407b70 RangeTableMemberTest
// Callee FUN_00426090, per target, exactly as the hand body reached it: the exe binds the byte-matched
// GlobalDat0066ce58Get (Match/MatchS06_00426090.cpp); the .asi calls the original address.
#ifdef MASHED_STANDALONE
#define FUN_00426090 GlobalDat0066ce58Get
#else
#define FUN_00426090 PortBatchGameplay_00407b70_Call00426090
extern "C" int* PortBatchGameplay_00407b70_Call00426090(void) {
    return reinterpret_cast<int* (__cdecl*)(void)>(0x00426090u)();
}
#endif
#pragma comment(linker, "/EXPORT:RangeTableMemberTest=_RangeTableMemberTest")
extern "C" {
#include "../../../../re/match/s01/00407b70.c"
}
RH_ScopedInstall(RangeTableMemberTest, 0x00407b70);  // c3_batch_ah s1 install, moved with the body
