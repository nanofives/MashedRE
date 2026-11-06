// MATCH-S11: port body of 0x00474e60 = the byte-matched source re/match/s11/00474e60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/Cluster_v1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00474e60 DegToRad
#pragma comment(linker, "/EXPORT:DegToRad=_DegToRad")
extern "C" {
#include "../../../../re/match/s11/00474e60.c"
}
RH_ScopedInstall(DegToRad, 0x00474e60);
