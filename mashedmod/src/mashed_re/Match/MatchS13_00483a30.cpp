// MATCH-S13: port body of 0x00483a30 = the byte-matched source re/match/s13/00483a30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/ReplayRewind_ag.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00483a30 ReplayRewind
#pragma comment(linker, "/EXPORT:ReplayRewind=_ReplayRewind")
extern "C" {
#include "../../../../re/match/s13/00483a30.c"
}
RH_ScopedInstall(ReplayRewind, 0x00483a30);
