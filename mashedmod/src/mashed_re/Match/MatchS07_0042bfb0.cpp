// MATCH-S07: port body of 0x0042bfb0 = the byte-matched source re/match/s07/0042bfb0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuStateWriters_u1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042bfb0 MenuStateParamStore
#define FUN_0042bf30 Post0042bf30
// Callee of the matched source: 0x0042bf30's port body is Post0042bf30 (Match/MatchS07_0042bf30.cpp, the matched
// source re/match/s07/0042bf30.c). The raw-address forwarder that stood here until it had one is gone.
#pragma comment(linker, "/EXPORT:MenuStateParamStore=_MenuStateParamStore")
extern "C" {
#include "../../../../re/match/s07/0042bfb0.c"
}
RH_ScopedInstall(MenuStateParamStore, 0x0042bfb0);
