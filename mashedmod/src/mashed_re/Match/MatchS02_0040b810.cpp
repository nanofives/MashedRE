// MATCH-S02: port body of 0x0040b810 = the byte-matched source re/match/s02/0040b810.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/TimerReset.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b810 TimerGlobalsReset
#pragma comment(linker, "/EXPORT:TimerGlobalsReset=_TimerGlobalsReset")
extern "C" {
#include "../../../../re/match/s02/0040b810.c"
}
RH_ScopedInstall(TimerGlobalsReset, 0x0040b810);
