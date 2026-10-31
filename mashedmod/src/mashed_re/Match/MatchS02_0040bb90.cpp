// MATCH-S02: port body of 0x0040bb90 = the byte-matched source re/match/s02/0040bb90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SpriteGate.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040bb90 SpriteLookupTableB
#define FUN_004c5c00 MatchS02_0040bb90_Call004c5c00
// Callee of the matched source: the port's body of that callee (Util/PromoLoop_sessionB.cpp Search4c5c00) is .asi-only and
// naked with no C signature, so this forwarder calls the callee through its original address, the same call the
// old Frontend/SpriteGate.cpp body made (in the .asi that address carries the installed JMP into Search4c5c00).
extern "C" unsigned int* MatchS02_0040bb90_Call004c5c00(int p0, char* p1) {
    return reinterpret_cast<unsigned int* (__cdecl*)(int, char*)>(0x004c5c00u)(p0, p1);
}
#pragma comment(linker, "/EXPORT:SpriteLookupTableB=_SpriteLookupTableB")
extern "C" {
#include "../../../../re/match/s02/0040bb90.c"
}
RH_ScopedInstall(SpriteLookupTableB, 0x0040bb90);
