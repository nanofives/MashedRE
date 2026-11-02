// MATCH-S07: port body of 0x00430290 = the byte-matched source re/match/s07/00430290.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Save/CareerEvents.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00430290 ChampionshipComplete
#define FUN_0042a920 PostTrophyEvent
#define FUN_0040dd60 MatchS07_00430290_Call0040dd60
// Callee of the matched source: the old Save/CareerEvents.cpp body called FUN_0040dd60 through its original
// address (kGuardConcludedAndP1Won, a __cdecl function-pointer cast to 0x0040dd60), so the forwarder makes that same call.
extern "C" unsigned int MatchS07_00430290_Call0040dd60(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x0040dd60u)();
}
#define FUN_004099a0 MatchS07_00430290_Call004099a0
// Callee of the matched source: the old Save/CareerEvents.cpp body called FUN_004099a0 through its original
// address (kAutosaveTrigger, a __cdecl function-pointer cast to 0x004099a0), so the forwarder makes that same call.
extern "C" void MatchS07_00430290_Call004099a0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004099a0u)();
}
#define printf MatchS07_00430290_Call004a2cbd
// printf of the matched source: the old Save/CareerEvents.cpp body called the original's CRT printf through its address
// (kPrintf, a __cdecl varargs function-pointer cast to 0x004a2cbd), so the forwarder jumps there with the caller's
// stack untouched. The format strings are the source's literals, the same text as the original's 0x005cd8c8 /
// 0x005cd8b0 / 0x005cd8a8 (silver add / TOTALNUMTRACKS-3 / HERE!!, checked against the unpatched image).
extern "C" __declspec(naked) int MatchS07_00430290_Call004a2cbd(const char *fmt, ...) {
    __asm {
        mov   eax, 0x004a2cbd
        jmp   eax
    }
}
#pragma comment(linker, "/EXPORT:ChampionshipComplete=_ChampionshipComplete")
extern "C" {
#include "../../../../re/match/s07/00430290.c"
}
RH_ScopedInstall(ChampionshipComplete, 0x00430290);
