// MATCH-S06: port body of 0x0042a980 = the byte-matched source re/match/s06/0042a980.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/BatchAA_s5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042a980 MenuTableSearchAlt
#define FUN_0040ce80 PtrTable5f2770Get
#pragma comment(linker, "/EXPORT:MenuTableSearchAlt=_MenuTableSearchAlt")
extern "C" {
#include "../../../../re/match/s06/0042a980.c"
}
RH_ScopedInstall(MenuTableSearchAlt, 0x0042a980);
