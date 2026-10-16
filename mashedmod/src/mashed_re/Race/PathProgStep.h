// Mashed RE — D-11072 leg P2: the original's per-frame path progress (FUN_00408610).
// See PathProgStep.cpp. PREREG: verify/d3_modes37_20261009/PREREG_P2.md.
#pragma once

namespace mashed_re {
namespace Race {

// FUN_00407a60: zero the four 0x30c-byte per-car blocks at 0x008a9620 and set +0x28 = -1.
// The original runs it from FUN_004111c0 at race reset. No-op unless PathProg_Enabled().
void PathProg_RaceInit();

// FUN_0040da50's FUN_00408610 loop, once per frame, before the AI tick. No-op unless enabled.
void PathProg_FrameTick();

// [U-9685] Arm R (MASHED_RACEPCT_FUN408610, see PathNodes.h): once per race frame at the end of UpdateCar,
// the verbatim step runs on a private shadow row per car and only race_pct (+0xcc) is written into the
// real row 0x008a9620 + v*0x30c. No-op unless RacePctOrig_Enabled() and not PathProg_Enabled().
void PathProg_RacePctTick();

// [PATHPROG split] MASHED_PP_PUBLISH=<mask> (default 0): 0x1 lap counter, 0x2 node progress, 0x4 the rest of the row are
// also published from the shadow row; 0x8 runs the tick in UpdateRace (before the AI tick) instead of at the end of UpdateCar.
unsigned PathProg_PublishMask();
// [d3b] car v's node progress +0xc8 as FUN_00408a50 would read it (shadow row under arm R, else the real row).
float PathProg_NodeProgress(int v);

// G-P2-REPLAY: MASHED_PATHPROG_REPLAY=<pathprog.csv> replays the original's recorded calls
// through the port step and writes a report to MASHED_PATHPROG_REPLAY_OUT. Called once after
// the course block is built.
void PathProg_MaybeReplay();

}  // namespace Race
}  // namespace mashed_re
