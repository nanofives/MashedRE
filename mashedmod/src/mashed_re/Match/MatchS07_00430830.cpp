// PORT-STATE-01 (2026-11-08): port body of 0x00430830 = the byte-matched source re/match/s07/00430830.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, MATCH-S07).
// The same source text is this RVA's only body in both targets (the hand body in Frontend/MenuScoreSort.cpp is gone).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL), DAT_0067e9fc
// included. Exe callers pass their value through ModeWordScope (Frontend/PortStateFrontend.h).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00430830 SplitScreenTrackAssignment
#pragma comment(linker, "/EXPORT:SplitScreenTrackAssignment=_SplitScreenTrackAssignment")
extern "C" {
#include "../../../../re/match/s07/00430830.c"
}
RH_ScopedInstall(SplitScreenTrackAssignment, 0x00430830);
