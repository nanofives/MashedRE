// MATCH-S02: port body of 0x0040b6c0 = the byte-matched source re/match/s02/0040b6c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/FrontendAccessors.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b6c0 FrontendArrayGet
#pragma comment(linker, "/EXPORT:FrontendArrayGet=_FrontendArrayGet")
extern "C" {
#include "../../../../re/match/s02/0040b6c0.c"
}
RH_ScopedInstall(FrontendArrayGet, 0x0040b6c0);
