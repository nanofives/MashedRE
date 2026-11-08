// PORT-STATE-02 (2026-11-08, L-101 seed default ON): port body of 0x00476b30 = the byte-matched source re/match/s12/00476b30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in both targets (moved out of Render/RfxEmitter.cpp). Compiled here as C++
// with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). Its file-backed .data reads come from
// the original in the .asi and from the port-state seed (Compat/PortStateSeed.cpp) in the exe.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00476b30 Set476b30
#pragma comment(linker, "/EXPORT:Set476b30=_Set476b30")
extern "C" {
#include "../../../../re/match/s12/00476b30.c"
}
RH_ScopedInstall(Set476b30, 0x00476b30);
