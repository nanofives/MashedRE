// MATCH-S05: port body of 0x00426060 = the byte-matched source re/match/s05/00426060.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TrackLoaderMicros_p3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426060 TrackPhysWorld1Get
#pragma comment(linker, "/EXPORT:TrackPhysWorld1Get=_TrackPhysWorld1Get")
extern "C" {
#include "../../../../re/match/s05/00426060.c"
}
RH_ScopedInstall(TrackPhysWorld1Get, 0x00426060);
