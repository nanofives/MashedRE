// MATCH-S06: port body of 0x00429b70 = the byte-matched source re/match/s06/00429b70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// The old naked body called 0x0042f6a0 / 0x0042b8c0 at their original addresses; both have port bodies, bound here.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00429b70 Init429b70
#define FUN_0042b8c0 ScreenHeightGet
#define FUN_0042f6a0 GetRaceSubMode
#pragma comment(linker, "/EXPORT:Init429b70=_Init429b70")
extern "C" {
#include "../../../../re/match/s06/00429b70.c"
}
RH_ScopedInstall(Init429b70, 0x00429b70);
