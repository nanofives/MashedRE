// MATCH-S07: port body of 0x00431b70 = the byte-matched source re/match/s07/00431b70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_af5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00431b70 MenuFlagDat007f0f10Get
#pragma comment(linker, "/EXPORT:MenuFlagDat007f0f10Get=_MenuFlagDat007f0f10Get")
extern "C" {
#include "../../../../re/match/s07/00431b70.c"
}
RH_ScopedInstall(MenuFlagDat007f0f10Get, 0x00431b70);
