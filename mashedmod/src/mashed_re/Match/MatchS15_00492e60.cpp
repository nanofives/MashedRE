// MATCH-FIX-492E60: port body of 0x00492e60 = the byte-matched source re/match/s15/00492e60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/SubsystemInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The old body called FUN_004671a0(-1, &v) then FUN_004c1c80(handle), so the 0.8f pair went to the wrong callee
// (verify/matchs15_20261102/RESULT_MATCH-S15.md); the matched source is the original's form:
// FUN_004c1c80(FUN_004671a0(-1), v).
// Install stays mass-disabled as it was in the old TU:
// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(SetDefaultViewWindow, 0x00492e60);

#define MATCH_PORT 1
#define FUN_00492e60 SetDefaultViewWindow
#define FUN_004671a0 MatchS15_00492e60_Call004671a0
#define FUN_004c1c80 MatchS15_00492e60_Call004c1c80
// Callees of the matched source: the old Boot/SubsystemInit.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" int MatchS15_00492e60_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int p0)>(0x004671a0u)(p0);
}
extern "C" int MatchS15_00492e60_Call004c1c80(int p0, float *p1) {
    return reinterpret_cast<int (__cdecl*)(int p0, float *p1)>(0x004c1c80u)(p0, p1);
}
#pragma comment(linker, "/EXPORT:SetDefaultViewWindow=_SetDefaultViewWindow")
extern "C" {
#include "../../../../re/match/s15/00492e60.c"
}
