// MATCH-S11: port body of 0x0046b4f0 = the byte-matched source re/match/s11/0046b4f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilLeaves_ab6.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046b4f0 UtilTableRead46b4f0
#pragma comment(linker, "/EXPORT:UtilTableRead46b4f0=_UtilTableRead46b4f0")
extern "C" {
#include "../../../../re/match/s11/0046b4f0.c"
}
RH_ScopedInstall(UtilTableRead46b4f0, 0x0046b4f0);
