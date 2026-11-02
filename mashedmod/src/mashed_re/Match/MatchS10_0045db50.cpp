// MATCH-S10: port body of 0x0045db50 = the byte-matched source re/match/s10/0045db50.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/FloatSliderStep.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045db50 FloatSliderStep
#pragma comment(linker, "/EXPORT:FloatSliderStep=_FloatSliderStep")
extern "C" {
#include "../../../../re/match/s10/0045db50.c"
}
RH_ScopedInstall(FloatSliderStep, 0x0045db50);
