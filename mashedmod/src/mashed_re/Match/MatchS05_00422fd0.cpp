// MATCH-S05: port body of 0x00422fd0 = the byte-matched source re/match/s05/00422fd0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuMixed.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00422fd0 FrontendRaceResultsDispatch
#define FUN_0046c5c0 MatchS05_00422fd0_Call0046c5c0
#define FUN_0046c790 MatchS05_00422fd0_Call0046c790
#define FUN_004215c0 Accum4215c0
#define FUN_0045ba00 MatchS05_00422fd0_Call0045ba00
#define FUN_0040e470 MatchS05_00422fd0_Call0040e470
#define thunk_FUN_00419760 MatchS05_00422fd0_Call004189f0
// Callees of the matched source: the old Frontend/MenuMixed.cpp body called these five through their original addresses (__cdecl
// function-pointer casts; thunk_FUN_00419760 = the thunk at 0x004189f0), so each forwarder makes that same call.
// FUN_004215c0 binds to Accum4215c0 (Match/MatchS05_00420de0.cpp); the 50.0f argument stays the raw dword 0x42480000.
extern "C" unsigned int MatchS05_00422fd0_Call0046c5c0(unsigned int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(unsigned int)>(0x0046c5c0u)(p0);
}
extern "C" unsigned int MatchS05_00422fd0_Call0046c790(unsigned int p0, unsigned int p1) {
    return reinterpret_cast<unsigned int (__cdecl*)(unsigned int, unsigned int)>(0x0046c790u)(p0, p1);
}
extern "C" void MatchS05_00422fd0_Call0045ba00(int p0, unsigned int p1) {
    reinterpret_cast<void (__cdecl*)(int, unsigned int)>(0x0045ba00u)(p0, p1);
}
extern "C" unsigned int MatchS05_00422fd0_Call0040e470(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int)>(0x0040e470u)(p0);
}
extern "C" void MatchS05_00422fd0_Call004189f0(int p0, int p1) {
    reinterpret_cast<void (__cdecl*)(int, int)>(0x004189f0u)(p0, p1);
}
#pragma comment(linker, "/EXPORT:FrontendRaceResultsDispatch=_FrontendRaceResultsDispatch")
extern "C" {
#include "../../../../re/match/s05/00422fd0.c"
}
RH_ScopedInstall(FrontendRaceResultsDispatch, 0x00422fd0);
