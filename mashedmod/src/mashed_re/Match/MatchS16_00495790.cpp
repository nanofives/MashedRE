// MATCH-S16: port body of 0x00495790 = the byte-matched source re/match/s16/00495790.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round34.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00495790 Global772facGet
#pragma comment(linker, "/EXPORT:Global772facGet=_Global772facGet")
extern "C" {
#include "../../../../re/match/s16/00495790.c"
}
RH_ScopedInstall(Global772facGet, 0x00495790);
