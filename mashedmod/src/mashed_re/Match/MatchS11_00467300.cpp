// MATCH-S11: port body of 0x00467300 = the byte-matched source re/match/s11/00467300.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/Damage.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00467300 VehicleCollisionWinTrigger
#define FUN_00413c70 MatchS11_00467300_Call00413c70
// Callee of the matched source: the old Vehicle/Damage.cpp body called 0x00413c70 through its original address
// (a __cdecl function-pointer cast); its port body Util/PromoLoop_sessionB.cpp Idx2RecordCondSet413c70 is .asi-only
// and is reached there through the installed JMP at that address. This forwarder makes that same call.
extern "C" void MatchS11_00467300_Call00413c70(int p0, int p1, unsigned int p2) {
    reinterpret_cast<void (__cdecl*)(int, int, unsigned int)>(0x00413c70u)(p0, p1, p2);
}
#pragma comment(linker, "/EXPORT:VehicleCollisionWinTrigger=_VehicleCollisionWinTrigger")
extern "C" {
#include "../../../../re/match/s11/00467300.c"
}
RH_ScopedInstall(VehicleCollisionWinTrigger, 0x00467300);
