// MATCH-S06: port body of 0x004299d0 = the byte-matched source re/match/s06/004299d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/TimeRecordWrite_ag.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00430790 -> GetDat0067f17c (Util/SmallLeaves_o6.cpp)
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004299d0 TimeRecordWriteTrackBest
#define FUN_00430790 GetDat0067f17c
#pragma comment(linker, "/EXPORT:TimeRecordWriteTrackBest=_TimeRecordWriteTrackBest")
extern "C" {
#include "../../../../re/match/s06/004299d0.c"
}
RH_ScopedInstall(TimeRecordWriteTrackBest, 0x004299d0);
