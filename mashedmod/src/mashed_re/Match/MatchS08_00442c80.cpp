// MATCH-S08-FU: port body of 0x00442c80 = the byte-matched source re/match/s08/00442c80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// The same source text is this RVA's only body in both targets (moved out of Vehicle/ForceIntegratorLeaves.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). The .rdata
// threshold 7.0 is a literal in the source (the exe's copy of the original .rdata reads 0).
// The .asi install stays on RubberBandGateEntry (Vehicle/ForceIntegratorLeaves.cpp, MASHED_LEAF_SELFTEST dispatcher),
// which calls the C++ name below.
#include "../Core/HookSystem.h"
#include "../Vehicle/ForceIntegrator.h"

#define MATCH_PORT 1
#define FUN_00442c80 RubberBandGateMatched
#define FUN_0040e350 MatchS08_00442c80_Call0040e350
// Callee of the matched source. Exe: the stand-in Fi_GameMode() (Vehicle/ForceIntegratorStubs.cpp, U-9784), as the old
// exe body called it. Asi: the original function through its address, as the old .asi body called it.
extern "C" int MatchS08_00442c80_Call0040e350(void) {
#ifdef MASHED_STANDALONE
    return mashed_re::Vehicle::Fi_GameMode();
#else
    return reinterpret_cast<int (__cdecl*)()>(0x0040e350u)();
#endif
}
extern "C" {
#include "../../../../re/match/s08/00442c80.c"
}
namespace mashed_re {
namespace Vehicle {
// C++ name the callers use (ForceIntegrator.cpp A5RubberGate, the .asi LeafGateDispatch): forwards to the matched body.
int RubberBandGate(int car) { return RubberBandGateMatched(car); }
}  // namespace Vehicle
}  // namespace mashed_re
