// MATCH-S08: port body of 0x00436810 = the byte-matched source re/match/s08/00436810.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuHelpers.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00436810 LocalPlayerSlotCheck
#define FUN_00430760 IsMultiplayerMode
#pragma comment(linker, "/EXPORT:LocalPlayerSlotCheck=_LocalPlayerSlotCheck")
extern "C" {
#include "../../../../re/match/s08/00436810.c"
}
RH_ScopedInstall(LocalPlayerSlotCheck, 0x00436810);
