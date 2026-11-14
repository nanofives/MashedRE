// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x00489c60 = the matched source re/match/s14/00489c60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: p[3] = 0, RwGlobals (0x007d3ff8) slot +0x10c on p[0], p[1], p[2], 0x004898d0(p[10]), p[10] = 0, slot +0x10c on p.
// Callers in the exe: the matched 0x004555d0 (this row).
// Callees bind by name, no raw-address call:
//   0x004898d0 = Orig_004898d0 (Match/PortTop_004898d0.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00489c60 Orig_00489c60
#define FUN_004898d0 PortTop_00489c60_Call004898d0
extern "C" {
void Orig_004898d0(void* p0);   // matched, Match/PortTop_004898d0.cpp
void PortTop_00489c60_Call004898d0(int p0) { Orig_004898d0(reinterpret_cast<void*>(static_cast<unsigned long>(p0))); }
#include "../../../../re/match/s14/00489c60.c"
}
#endif  // MASHED_STANDALONE
