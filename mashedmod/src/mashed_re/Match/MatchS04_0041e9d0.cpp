// MATCH-S04: port body of 0x0041e9d0 = the byte-matched source re/match/s04/0041e9d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TrackNodeLeaves_o1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041e9d0 TrackNodeFnPtrGet14
#pragma comment(linker, "/EXPORT:TrackNodeFnPtrGet14=_TrackNodeFnPtrGet14")
extern "C" {
#include "../../../../re/match/s04/0041e9d0.c"
}
RH_ScopedInstall(TrackNodeFnPtrGet14, 0x0041e9d0);
