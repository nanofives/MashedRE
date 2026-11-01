// MATCH-S07: port body of 0x0042c220 = the byte-matched source re/match/s07/0042c220.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// New body (no earlier port body, no install). Callees bind to the port's bodies through extern "C" forwarders
// (the port symbols return std::uint32_t, the matched source declares int): FUN_0040e470 -> CarSlotStateGet
// (Frontend/RaceResults.cpp), FUN_00496900 -> SlotActiveThunk (Util/PromoLoop_round24.cpp). Both are .asi-only.

#define MATCH_PORT 1
#define FUN_0040e470 MatchS07_0042c220_Fwd0040e470
#define FUN_00496900 MatchS07_0042c220_Fwd00496900
extern "C" unsigned int __cdecl CarSlotStateGet(int param_1);
extern "C" unsigned int __cdecl SlotActiveThunk(int player_idx);
extern "C" int MatchS07_0042c220_Fwd0040e470(int p0) {
    return static_cast<int>(CarSlotStateGet(p0));
}
extern "C" int MatchS07_0042c220_Fwd00496900(int p0) {
    return static_cast<int>(SlotActiveThunk(p0));
}
#pragma comment(linker, "/EXPORT:FUN_0042c220=_FUN_0042c220")
extern "C" {
#include "../../../../re/match/s07/0042c220.c"
}
