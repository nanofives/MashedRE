// MATCH-S06: port body of 0x00426bb0 = the byte-matched source re/match/s06/00426bb0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round31.cpp and the naked copies PP_426bb0 / AL_426bb0 / TG_426bb0).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426bb0 Global66d6d8Get
#pragma comment(linker, "/EXPORT:Global66d6d8Get=_Global66d6d8Get")
extern "C" {
#include "../../../../re/match/s06/00426bb0.c"
}
RH_ScopedInstall(Global66d6d8Get, 0x00426bb0);
