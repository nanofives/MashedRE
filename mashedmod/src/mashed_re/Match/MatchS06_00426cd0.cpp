// MATCH-S06: port body of 0x00426cd0 = the byte-matched source re/match/s06/00426cd0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TrackLoaderMicros_p3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426cd0 TrackSlotArrayReset
#pragma comment(linker, "/EXPORT:TrackSlotArrayReset=_TrackSlotArrayReset")
extern "C" {
#include "../../../../re/match/s06/00426cd0.c"
}
RH_ScopedInstall(TrackSlotArrayReset, 0x00426cd0);
