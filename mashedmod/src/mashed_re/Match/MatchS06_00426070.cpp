// MATCH-S06: port body of 0x00426070 = the byte-matched source re/match/s06/00426070.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TrackLoaderMicros_p3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426070 TrackPhysWorld2Get
#pragma comment(linker, "/EXPORT:TrackPhysWorld2Get=_TrackPhysWorld2Get")
extern "C" {
#include "../../../../re/match/s06/00426070.c"
}
RH_ScopedInstall(TrackPhysWorld2Get, 0x00426070);
