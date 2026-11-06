// MATCH-S14: port body of 0x00484c70 = the byte-matched source re/match/s14/00484c70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/WorldObjectsBase.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00484c70 WorldObjectsBaseGet
#pragma comment(linker, "/EXPORT:WorldObjectsBaseGet=_WorldObjectsBaseGet")
extern "C" {
#include "../../../../re/match/s14/00484c70.c"
}
RH_ScopedInstall(WorldObjectsBaseGet, 0x00484c70);
