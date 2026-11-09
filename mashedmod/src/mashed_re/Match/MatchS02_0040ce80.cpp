// PORT-STATE-02 (2026-11-08, L-101 seed default ON): port body of 0x0040ce80 = the byte-matched source re/match/s02/0040ce80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in both targets (moved out of Util/PromoLoop_round49.cpp). Compiled here as C++
// with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). Its file-backed .data reads come from
// the original in the .asi and from the port-state seed (Compat/PortStateSeed.cpp) in the exe.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040ce80 PtrTable5f2770Get
#pragma comment(linker, "/EXPORT:PtrTable5f2770Get=_PtrTable5f2770Get")
extern "C" {
#include "../../../../re/match/s02/0040ce80.c"
}
RH_ScopedInstall(PtrTable5f2770Get, 0x0040ce80);
