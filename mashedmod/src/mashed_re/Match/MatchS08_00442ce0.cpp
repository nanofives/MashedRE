// MATCH-S08-FU: port body of 0x00442ce0 = the byte-matched source re/match/s08/00442ce0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// The same source text is this RVA's only body in both targets (moved out of Vehicle/ForceIntegratorLeaves.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). The .rdata
// floats 1.0 / 0.0 / 0.6 / 0.5 / 0.25 are literals in the source (the exe's copy of the original .rdata reads 0).
// The .asi install stays on RubberBandGripEntry (Vehicle/ForceIntegratorLeaves.cpp, MASHED_LEAF_SELFTEST dispatcher),
// which calls the C++ name below.
#include <cstdint>
#include <cstring>
#include "../Core/HookSystem.h"
#include "../Vehicle/ForceIntegrator.h"

#define MATCH_PORT 1
#define FUN_00442ce0 RubberBandGripMatched
// Callees of the matched source, both with one port body in both targets: 0x0040e340 GetLiveCarCount
// (Match/MatchS02_0040e340.cpp, a getter; the original discards its result at 0x00442d06) and 0x0046dbe0
// VehicleRacePositionGet (Vehicle/VehicleState.cpp).
#define FUN_0040e340 GetLiveCarCount
#define FUN_0046dbe0 VehicleRacePositionGet
extern "C" {
#include "../../../../re/match/s08/00442ce0.c"
}
namespace mashed_re {
namespace Vehicle {
// C++ name the callers use (ForceIntegrator.cpp A5RubberGrip, the .asi LeafGripDispatch): forwards to the matched body.
// The original never reads arg 2; it is passed through bit for bit.
float RubberBandGrip(int car, float current, float band) {
    int cur_bits;
    std::memcpy(&cur_bits, &current, 4);
    return RubberBandGripMatched(car, cur_bits, band);
}
}  // namespace Vehicle
}  // namespace mashed_re
