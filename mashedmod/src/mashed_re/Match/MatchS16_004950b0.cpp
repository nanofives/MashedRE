// MATCH-FIX-4950B0: port body of 0x004950b0 = the byte-matched source re/match/s16/004950b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilMid.cpp,
// which only the .asi compiles, so this wrapper is in asi_sources.rsp only).
// Compiled here as C++ with MATCH_PORT; the global binds to its original address (preamble MGLOBAL).
// The old body (verify/matchs16_20261102/RESULT_MATCH-S16.md) did QPC * 3000000 / freq in UNSIGNED 64-bit math and
// returned 0 when freq (0x00771e70) was 0. The original does it in SIGNED __int64 (__allmul / __alldiv at 0x004950b0)
// with no zero guard, so the two differ once the product passes 2^63, and on freq == 0 the original raises #DE.
// The matched source is the original's form: signed math, no guard.
// No windows.h here: the matched source declares QueryPerformanceCounter itself (same import, LargeInt argument).
// Install stays mass-disabled as it was in the old TU:
// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(QpcTimeScaledTo3Mhz, 0x004950b0);

#define MATCH_PORT 1
#define FUN_004950b0 QpcTimeScaledTo3Mhz
#pragma comment(linker, "/EXPORT:QpcTimeScaledTo3Mhz=_QpcTimeScaledTo3Mhz")
extern "C" {
#include "../../../../re/match/s16/004950b0.c"
}
