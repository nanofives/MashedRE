// MATCH-S02: port body of 0x0040e3a0 = the byte-matched source re/match/s02/0040e3a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuScoreSort.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040e3a0 PlayerColorTableGet
#define FUN_004a332b MatchS02_0040e3a0_Call004a332b
// Callee of the matched source (default case, exit(-14) shape): the old Frontend/MenuScoreSort.cpp body called it
// through its original address (s_AbortFn), so this forwarder makes that same call.
extern "C" __declspec(noreturn) void MatchS02_0040e3a0_Call004a332b(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x004a332bu)(p0);
}
#pragma comment(linker, "/EXPORT:PlayerColorTableGet=_PlayerColorTableGet")
extern "C" {
#include "../../../../re/match/s02/0040e3a0.c"
}
RH_ScopedInstall(PlayerColorTableGet, 0x0040e3a0);
