// MATCH-S06: port body of 0x004260c0 = the byte-matched source re/match/s06/004260c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TrackLoaderMicros_p3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004260c0 TrackPhysWorld3Get
#pragma comment(linker, "/EXPORT:TrackPhysWorld3Get=_TrackPhysWorld3Get")
extern "C" {
#include "../../../../re/match/s06/004260c0.c"
}
RH_ScopedInstall(TrackPhysWorld3Get, 0x004260c0);
