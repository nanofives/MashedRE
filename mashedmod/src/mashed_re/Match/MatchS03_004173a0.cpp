// MATCH-T13-SEED: port body of 0x004173a0 = the byte-matched source re/match/s03/h1_004173a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// This RVA's only body in both targets (moved out of Util/PromoLoop_round35.cpp, asi-only). The exe calls it from
// the two Rule10Seed call sites in D3d9Render/TrackRenderer.cpp (0x004046a0's `fVar7 = FUN_004173a0()`), only
// under MASHED_RULE10_SEED_DIFF=1 (default OFF, verify/t13seed_20261102/PREREG_MATCH-T13-SEED.md).
// Compiled here as C++ with MATCH_PORT; DAT_0089a360 binds to its original address.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define DAT_0089a360 (*(float*)0x0089a360u)
#define FUN_004173a0 Float89a360Get
#pragma comment(linker, "/EXPORT:Float89a360Get=_Float89a360Get")
extern "C" {
// 0x004173a0  D9 05 60 A3 89 00 C3  (fld dword [0x0089a360]; ret)
#include "../../../../re/match/s03/h1_004173a0.c"
}
RH_ScopedInstall(Float89a360Get, 0x004173a0);  // promote-round-35 install, moved with the body
