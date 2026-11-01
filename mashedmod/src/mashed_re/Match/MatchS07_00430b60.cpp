// MATCH-S07: port body of 0x00430b60 = the byte-matched source re/match/s07/00430b60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/FrontendState.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00430b60 MenuSlotCount
#pragma comment(linker, "/EXPORT:MenuSlotCount=_MenuSlotCount")
extern "C" {
#include "../../../../re/match/s07/00430b60.c"
}
RH_ScopedInstall(MenuSlotCount, 0x00430b60);
