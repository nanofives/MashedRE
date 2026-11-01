// MATCH-S07: port body of 0x0042d300 = the byte-matched source re/match/s07/0042d300.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuTime.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042d300 TimeDiffDecompose
#pragma comment(linker, "/EXPORT:TimeDiffDecompose=_TimeDiffDecompose")
extern "C" {
#include "../../../../re/match/s07/0042d300.c"
}
RH_ScopedInstall(TimeDiffDecompose, 0x0042d300);
