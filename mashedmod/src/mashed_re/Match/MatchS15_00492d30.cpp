// MATCH-FIX-492D30: port body of 0x00492d30 = the byte-matched source re/match/s15/00492d30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilMid.cpp,
// which only the .asi compiles, so this wrapper is in asi_sources.rsp only).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The old body took other switch paths than the original (verify/matchs15_20261102/RESULT_MATCH-S15.md): cases
// 2/3/default called 0x0043dfd0 + 0x0043d7c0 where the original goes straight to the tail, cases 5/6 skipped both where
// the original calls both, cases 4/7 skipped 0x0043d7c0. The matched source is the original's form.
// Install stays mass-disabled as it was in the old TU:
// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(GameTickStateMachine7, 0x00492d30);

#define MATCH_PORT 1
#define FUN_00492d30 GameTickStateMachine7
#define FUN_004030d0 MatchS15_00492d30_Call004030d0
#define FUN_004111c0 MatchS15_00492d30_Call004111c0
#define FUN_0042b930 MatchS15_00492d30_Call0042b930
#define FUN_0043d7c0 MatchS15_00492d30_Call0043d7c0
#define FUN_0043dfd0 MatchS15_00492d30_Call0043dfd0
#define FUN_0045b350 MatchS15_00492d30_Call0045b350
#define FUN_004671a0 MatchS15_00492d30_Call004671a0
#define FUN_004950b0 MatchS15_00492d30_Call004950b0
#define FUN_00496920 MatchS15_00492d30_Call00496920
#define FUN_00498860 MatchS15_00492d30_Call00498860
#define FUN_005c9d00 MatchS15_00492d30_Call005c9d00
// Callees of the matched source: the old Util/UtilMid.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" void MatchS15_00492d30_Call004030d0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004030d0u)();
}
extern "C" void MatchS15_00492d30_Call004111c0(unsigned int p0) {
    reinterpret_cast<void (__cdecl*)(unsigned int p0)>(0x004111c0u)(p0);
}
extern "C" unsigned int MatchS15_00492d30_Call0042b930(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x0042b930u)();
}
extern "C" void MatchS15_00492d30_Call0043d7c0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0043d7c0u)();
}
extern "C" void MatchS15_00492d30_Call0043dfd0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0043dfd0u)();
}
extern "C" void MatchS15_00492d30_Call0045b350(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0045b350u)();
}
extern "C" unsigned int MatchS15_00492d30_Call004671a0(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int p0)>(0x004671a0u)(p0);
}
extern "C" unsigned int MatchS15_00492d30_Call004950b0(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x004950b0u)();
}
extern "C" unsigned int MatchS15_00492d30_Call00496920(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int p0)>(0x00496920u)(p0);
}
extern "C" void MatchS15_00492d30_Call00498860(unsigned int p0) {
    reinterpret_cast<void (__cdecl*)(unsigned int p0)>(0x00498860u)(p0);
}
extern "C" unsigned int MatchS15_00492d30_Call005c9d00(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x005c9d00u)();
}
#pragma comment(linker, "/EXPORT:GameTickStateMachine7=_GameTickStateMachine7")
extern "C" {
#include "../../../../re/match/s15/00492d30.c"
}
