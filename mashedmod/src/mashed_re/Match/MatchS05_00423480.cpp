// MATCH-S05: port body of 0x00423480 = the byte-matched source re/match/s05/00423480.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/ScenarioLeaves_sa2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00423480 AiFilenameBuild
#define FUN_00426c00 MatchS05_00423480_Call00426c00
#define FUN_004a2b60 MatchS05_00423480_Call004a2b60
// Callees of the matched source: the old HUD/ScenarioLeaves_sa2.cpp body called 0x00426c00 and 0x004a2b60 through their original addresses
// (__cdecl function-pointer casts, in both targets), so each forwarder makes that same call. 0x004a2b60 is variadic:
// its forwarder is a naked tail jump, so the caller's stack arguments pass through unchanged.
extern "C" int MatchS05_00423480_Call00426c00(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00426c00u)();
}
extern "C" __declspec(naked) int MatchS05_00423480_Call004a2b60(char *dst, const char *fmt, ...) {
    __asm {
        mov eax, 004A2B60h
        jmp eax
    }
}
#pragma comment(linker, "/EXPORT:AiFilenameBuild=_AiFilenameBuild")
extern "C" {
#include "../../../../re/match/s05/00423480.c"
}
RH_ScopedInstall(AiFilenameBuild, 0x00423480);
