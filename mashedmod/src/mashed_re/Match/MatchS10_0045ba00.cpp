// MATCH-S10: port body of 0x0045ba00 = the byte-matched source re/match/s10/0045ba00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SmallLeaves_n2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045ba00 RaceResultIndexedStore
#pragma comment(linker, "/EXPORT:RaceResultIndexedStore=_RaceResultIndexedStore")
extern "C" {
#include "../../../../re/match/s10/0045ba00.c"
}
RH_ScopedInstall(RaceResultIndexedStore, 0x0045ba00);
