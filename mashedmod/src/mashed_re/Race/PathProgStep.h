// Mashed RE — D-11072 leg P2: the original's per-frame path progress (FUN_00408610).
// See PathProgStep.cpp. PREREG: verify/d3_modes37_20261009/PREREG_P2.md.
#pragma once

namespace mashed_re {
namespace Race {

// FUN_00407a60: zero the four 0x30c-byte per-car blocks at 0x008a9620 and set +0x28 = -1.
// The original runs it from FUN_004111c0 at race reset.
void PathProg_RaceInit();

// FUN_0040da50's FUN_00408610 loop, once per frame. No-op when the track has no course block.
void PathProg_FrameTick();
// [cev] The caller runs it after every car's physics (the end of UpdateCar, where arm R's tick runs), not in
// UpdateRace, which runs before the opponents' physics. The MASHED_PATHPROG_LATE knob (seeded ON) and PathProg_Late()
// were deleted by D5-KNOBS-D-SEEDED6.

// [U-9685] Arm R's shadow-row tick PathProg_RacePctTick and MASHED_PP_PUBLISH were deleted by D5-KNOBS-E4-MODE-REG
// with MASHED_PATHPROG: the step above writes the real rows.
// [d3b] car v's node progress +0xc8 as FUN_00408a50 reads it (the real row).
float PathProg_NodeProgress(int v);

// G-P2-REPLAY: MASHED_PATHPROG_REPLAY=<pathprog.csv> replays the original's recorded calls
// through the port step and writes a report to MASHED_PATHPROG_REPLAY_OUT. Called once after
// the course block is built.
void PathProg_MaybeReplay();

}  // namespace Race
}  // namespace mashed_re
