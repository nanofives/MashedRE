// MATCH-S14: port body of 0x00484170 = the byte-matched source re/match/s14/00484170.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_standins_r1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00484170 Rec18Create
#pragma comment(linker, "/EXPORT:Rec18Create=_Rec18Create")
extern "C" {
#include "../../../../re/match/s14/00484170.c"
}
RH_ScopedInstall(Rec18Create, 0x00484170);
