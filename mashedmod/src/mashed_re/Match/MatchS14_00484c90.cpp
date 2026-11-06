// MATCH-S14: port body of 0x00484c90 = the byte-matched source re/match/s14/00484c90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Race/WorldObjectLinks.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00484c90 Zero484c90
#pragma comment(linker, "/EXPORT:Zero484c90=_Zero484c90")
extern "C" {
#include "../../../../re/match/s14/00484c90.c"
}
RH_ScopedInstall(Zero484c90, 0x00484c90);
