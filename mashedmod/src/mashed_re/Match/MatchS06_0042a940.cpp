// MATCH-S06: port body of 0x0042a940 = the byte-matched source re/match/s06/0042a940.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuMenusMixed.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042a940 MenuTableSearch
#define FUN_0040ce80 PtrTable5f2770Get
#pragma comment(linker, "/EXPORT:MenuTableSearch=_MenuTableSearch")
extern "C" {
#include "../../../../re/match/s06/0042a940.c"
}
RH_ScopedInstall(MenuTableSearch, 0x0042a940);
