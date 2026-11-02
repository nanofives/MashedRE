// MATCH-S08: port body of 0x00443090 = the byte-matched source re/match/s08/00443090.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round40.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00443090 Ret897ff0
#pragma comment(linker, "/EXPORT:Ret897ff0=_Ret897ff0")
extern "C" {
#include "../../../../re/match/s08/00443090.c"
}
RH_ScopedInstall(Ret897ff0, 0x00443090);
