// MATCH-S11: port body of 0x00467210 = the byte-matched source re/match/s11/00467210.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Camera/FollowTargetFieldPtr.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


#define MATCH_PORT 1
#define FUN_00467210 FollowTargetField10_impl
#define FUN_0042b930 MatchS11_00467210_Call0042b930
#define FUN_0042f510 MatchS11_00467210_Call0042f510
// Callees: the old Camera/FollowTargetFieldPtr.cpp body (shared helper FollowTargetField)
// reached 0x0042b930 / 0x0042f510 through their original addresses, so each forwarder makes that same call.
extern "C" int MatchS11_00467210_Call0042b930(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0042b930u)();
}
extern "C" int MatchS11_00467210_Call0042f510(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0042f510u)();
}
#pragma comment(linker, "/EXPORT:FollowTargetField10_impl=_FollowTargetField10_impl")
extern "C" {
#include "../../../../re/match/s11/00467210.c"
}
