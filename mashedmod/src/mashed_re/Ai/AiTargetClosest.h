// Mashed RE — D-11072 leg P3: FUN_00414a70 (closest car ahead: chase / brake) + FUN_00414300.
// Exe-only TU (exe_sources.rsp). PREREG: verify/d3_modes37_20261009/PREREG_P3.md.
#pragma once

namespace Ai {

// MASHED_TGT3 (default OFF). The progress FUN_00414a70 reads comes from the path-progress step FUN_00408610
// (Race/PathProgStep.cpp), which always runs since D5-KNOBS-E4-MODE-REG deleted MASHED_PATHPROG.
bool TargetClosest_Enabled();

// 0x00414a70 FUN_00414a70(spline, &look, &cand, v), verbatim naked, run at x87 PC_24.
// Returns 0 / 1 (cand written by FUN_00414300) / 2. `look` and `cand` are {x, z}.
int TargetClosest(int v, const float look[2], float cand[2]);

// G-P3-REPLAY: MASHED_TGT3_REPLAY=<o_*.msd.tgt.csv> replays the original's recorded calls through
// the naked body; report to MASHED_TGT3_REPLAY_OUT. Called once after the course block is built.
void TargetClosest_MaybeReplay();

// [mode 7 M4] FUN_00414c30 (+ FUN_00414490). Always runs (MASHED_MODE7 and the M3 set's knobs were deleted by
// D5-KNOBS-E4-MODE-REG). Returns 0 / 1 (mode 3) / 2 (mode 7); cand = the target xz. Hosts: FUN_0040e350 and the
// held-powerup word.
void ModeSeven_SetHost(int (*sub_mode)(), int (*held)(int));
int  ModeSeven(unsigned int spline, int v, const float look[2], float cand[2]);
// G-M4-REPLAY: MASHED_C30_REPLAY=<a.c30.tsv>[;...], report to MASHED_C30_REPLAY_OUT.
void ModeSeven_MaybeReplay();

}  // namespace Ai
