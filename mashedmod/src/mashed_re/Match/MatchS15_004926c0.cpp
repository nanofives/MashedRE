// MATCH-FIX-4926C0: port body of 0x004926c0 = the byte-matched source re/match/s15/004926c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/FrameDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The old body inverted the button-4 branch and divided the 60-frame accumulator unsigned
// (verify/matchs15_20261102/RESULT_MATCH-S15.md); the matched source is the original's form.
// Install stays mass-disabled as it was in the old TU:
// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(AudioTickAndAvg, 0x004926c0);

#define MATCH_PORT 1
#define FUN_004926c0 AudioTickAndAvg
// [RO-BIND-TIMER] exe: 0x004950b0 and 0x00496930 bind by name to their matched exe bodies. 0x0045d3f0, 0x00466b50 and
// 0x0045d430 (the audio branch, taken only while DAT_006147b8 != 0) have no exe body whose callees all bind: each binds
// to a named stop (Match/PortTop_00492370.cpp) that logs and ends the process. Reached only under MASHED_ORIG_TOP with
// MASHED_OT_TIMER=1. The .asi keeps the raw-address calls.
#ifdef MASHED_STANDALONE
#define FUN_004950b0 QpcTimeScaledTo3Mhz          // matched, Match/MatchS16_004950b0.cpp
#define FUN_00496930 Table773030Get               // matched, Match/MatchS16_00496930.cpp
#define FUN_0045d3f0 PortTop_Stop_0045d3f0
#define FUN_00466b50 PortTop_Stop_00466b50
#define FUN_0045d430 PortTop_Stop_0045d430
#else
#define FUN_004950b0 MatchS15_004926c0_Call004950b0
#define FUN_00496930 MatchS15_004926c0_Call00496930
#define FUN_0045d3f0 MatchS15_004926c0_Call0045d3f0
#define FUN_00466b50 MatchS15_004926c0_Call00466b50
#define FUN_0045d430 MatchS15_004926c0_Call0045d430
// Callees of the matched source: the old Boot/FrameDispatch.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" int MatchS15_004926c0_Call004950b0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x004950b0u)();
}
extern "C" int MatchS15_004926c0_Call00496930(int p0) {
    return reinterpret_cast<int (__cdecl*)(int p0)>(0x00496930u)(p0);
}
extern "C" unsigned int MatchS15_004926c0_Call0045d3f0(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int p0)>(0x0045d3f0u)(p0);
}
extern "C" void MatchS15_004926c0_Call00466b50(int p0) {
    reinterpret_cast<void (__cdecl*)(int p0)>(0x00466b50u)(p0);
}
extern "C" unsigned int MatchS15_004926c0_Call0045d430(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x0045d430u)();
}
#endif
#pragma comment(linker, "/EXPORT:AudioTickAndAvg=_AudioTickAndAvg")
extern "C" {
#include "../../../../re/match/s15/004926c0.c"
}
