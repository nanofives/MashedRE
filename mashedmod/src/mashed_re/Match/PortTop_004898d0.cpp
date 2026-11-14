// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x004898d0 = the matched source re/match/s14/004898d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: pool +0x54 = 0, +0x58 = 1, 0x004768c0(pool + 0x10), RwGlobals (0x007d3ff8) slot +0x10c on pool +0x00.
// Callers in the exe: the matched 0x0045a790, 0x004530b0 and 0x00489c60 (this row).
// Callees bind by name, no raw-address call:
//   0x004768c0 = Orig_004768c0 (Match/PortTop_004768c0.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004898d0 Orig_004898d0
#define FUN_004768c0 PortTop_004898d0_Call004768c0
extern "C" {
void Orig_004768c0(int p0);   // matched, Match/PortTop_004768c0.cpp
void PortTop_004898d0_Call004768c0(void* p0) { Orig_004768c0(static_cast<int>(reinterpret_cast<unsigned long>(p0))); }
#include "../../../../re/match/s14/004898d0.c"
}
#endif  // MASHED_STANDALONE
