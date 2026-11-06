// MATCH-S16: port body of 0x00499730 = the byte-matched source re/match/s16/00499730.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/PromoLoop_round7.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00499730 BootPtr773818Get
#pragma comment(linker, "/EXPORT:BootPtr773818Get=_BootPtr773818Get")
extern "C" {
#include "../../../../re/match/s16/00499730.c"
}
RH_ScopedInstall(BootPtr773818Get, 0x00499730);
