// MATCH-S02: port body of 0x0040b250 = the byte-matched source re/match/s02/0040b250.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilLeaves_ac.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b250 UtilReset8a9500
#pragma comment(linker, "/EXPORT:UtilReset8a9500=_UtilReset8a9500")
extern "C" {
#include "../../../../re/match/s02/0040b250.c"
}
RH_ScopedInstall(UtilReset8a9500, 0x0040b250);
