// MATCH-T13-SEED-WRITER: port body of 0x00414060 = the byte-matched source re/match/s03/h3_00414060.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// This RVA's only body in both targets. Before this step the exe held a hand-written step-6-only tail in
// Race/AiDifficultyReset.cpp (GATEFIRE idx364); that file now calls this body (see its header).
//
// What the body does (h3_00414060.c, disassembly 0x00414060..0x00414119):
//   f = (float)DAT_0089a384[FUN_00413fa0()] + DAT_0089a37c * DAT_005cc32c      0x00414066..0x0041407b
//   FUN_0042f6a0() == 6  -> f = (float)FUN_00431d80() * DAT_005cd088            0x00414084
//   FUN_0042f6a0() == 10 -> f = (float)FUN_0042fe80() * DAT_005cd088            0x004140a5
//   clamp to [DAT_005d757c, DAT_005cc55c]                                         0x004140c3 / 0x004140da
//   DAT_0089a360 = f (sole store in the image, 0x004140f4)
//   DAT_0089a870..87c = 0xbf800000, DAT_0089a364 = -1
//
// Globals bind to their original addresses. The four .rdata constants do NOT in the exe: the exe's image pad
// is zero-filled over the original's .rdata (Ai/AiState.h header, Ai/AiBankSwitch.cpp:25), so the exe binds
// them to named constants holding the image's bytes, read from MASHED.exe.unpatched on 2026-11-02:
//   0x005cc32c = 00 00 00 3f = 0.5     0x005cd088 = 00 00 20 40 = 2.5
//   0x005d757c = 00 00 00 00 = 0.0     0x005cc55c = 00 00 20 41 = 10.0
// The asi binds them to the live image addresses.
//
// Callers. Original: 0x0040ffc7 in FUN_004111c0 (race init, after FUN_0040b180 at 0x0040ffb8). Exe:
//   - RaceAiDifficultyResetTail (Race/AiDifficultyReset.cpp), every race load, which restores DAT_0089a360
//     afterwards unless MASHED_DIFF360_LIVE=1, so the default keeps the 2.5 seed and only the step-6 stores
//     land (the same five stores the old hand tail made).
//   - RaceDiff360Live_RaceInit, from TrackRenderer::StartMatch after SeedMatchScores (FUN_0040b180), only under
//     MASHED_DIFF360_LIVE=1 (default OFF, verify/matcht13seedwriter_20261102/PREREG_MATCH-T13-SEED-WRITER.md).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00414060 Race89a360Compute
#define FUN_00413fa0 Calc413fa0
#define FUN_0042f6a0 GetRaceSubMode
#define FUN_0042fe80 GetRaceEndFlag
#define FUN_00431d80 TiebreakFlagGet
#define DAT_0089a37c (*(float*)0x0089a37cu)
#define DAT_0089a360 (*(float*)0x0089a360u)
#define DAT_0089a364 (*(int*)0x0089a364u)
#define DAT_0089a870 (*(int*)0x0089a870u)
#define DAT_0089a874 (*(int*)0x0089a874u)
#define DAT_0089a878 (*(int*)0x0089a878u)
#define DAT_0089a87c (*(int*)0x0089a87cu)
#define DAT_0089a384 ((int*)0x0089a384u)
#ifdef MASHED_STANDALONE
static const float s_c_005cc32c = 0.5f;    // image 0x005cc32c = 0x3f000000
static const float s_c_005cd088 = 2.5f;    // image 0x005cd088 = 0x40200000
static const float s_c_005d757c = 0.0f;    // image 0x005d757c = 0x00000000
static const float s_c_005cc55c = 10.0f;   // image 0x005cc55c = 0x41200000
#define DAT_005cc32c s_c_005cc32c
#define DAT_005cd088 s_c_005cd088
#define DAT_005d757c s_c_005d757c
#define DAT_005cc55c s_c_005cc55c
#else
#define DAT_005cc32c (*(const float*)0x005cc32cu)
#define DAT_005cd088 (*(const float*)0x005cd088u)
#define DAT_005d757c (*(const float*)0x005d757cu)
#define DAT_005cc55c (*(const float*)0x005cc55cu)
#endif
#pragma comment(linker, "/EXPORT:Race89a360Compute=_Race89a360Compute")
extern "C" {
int Calc413fa0(void);
int GetRaceSubMode(void);
int GetRaceEndFlag(void);
int TiebreakFlagGet(void);
// 0x00414060  (186 B)
#include "../../../../re/match/s03/h3_00414060.c"
}
RH_ScopedInstall(Race89a360Compute, 0x00414060);
