// Mashed RE — round-restart grid, Stage A (verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md).
// Exe-only TU. The original grids round 2+ at the surviving car's progress node:
//   FUN_00410860 (0x004108c5) DAT_0063ba80 = __ftol(progress[survivor])
//   FUN_0040e590 -> FUN_00409290(&DAT_0063ba80) (snap into Safe_Start_Lines) -> FUN_00408b00 anchor.
#pragma once

namespace mashed_re {
namespace Race {

// MASHED_ROUND_GRID (default OFF) AND MASHED_PATHPROG (progress + course block producer).
bool RoundGrid_Enabled();

// Race init: DAT_0063ba80 = 0 (FUN_004111c0 0x0040fef0).
void RoundGrid_RaceInit();

// Round end, next round follows (the exe's r == 0 branch). alive[i] = race_[i].alive; exeWinner = the exe's
// round_winner_ choice (first alive), logged next to the original's rule (last alive) for the gate.
void RoundGrid_RoundEnd(const bool alive[4], int exeWinner, const int codes[4]);
// codes = the round's per-car score deltas (the exe's score_delta_, = the original's DAT_008a9520 written by
// FUN_0040b290 0x0040b360), kept for Stage B until the next round start.

// Round start: snap DAT_0063ba80 in place with FUN_00409290 (SnapNodeToSafeStartLines, Race/RoundGridSnap.cpp,
// the body shared with the .asi), then read the node record:
//   A = centre (FUN_00426cb0: 0x00663664 + n*0x4c), dir = record +0 (FUN_00426cc0: 0x00663658 + n*0x4c),
//   lat = normalize(corner0 - corner3) (FUN_00426d00(n,0/3): 0x00663670 + n*0x4c + k*12; FUN_004c39b0 LUT).
// lat = x/z of the normalized 3-D corner axis; dir = x/z of the RAW record direction (FUN_00408b00 scales both as
// is; the yaw uses atan2 of dir, which ignores its length). Returns false (caller keeps the gate anchor) if the course block
// has no nodes.
bool RoundGrid_Anchor(float A[3], float lat[2], float dir[2], int* node);
// Same, at an explicit node and without the snap (G-RG-GRID check).
bool RoundGrid_AnchorAt(int node, float A[3], float lat[2], float dir[2]);

// Stage B (MASHED_ROUND_GRID_ORDER, default OFF, needs MASHED_ROUND_GRID): FUN_0040e590's per-car grid slot value
// (mode DAT_008a94d0 = 4, rule 0, DAT_0067ea64 = 0: sections C/D/E + `local_44 ? i + 4 : local_2c[i]`) from the
// kept codes; the codes then reset to -1000 (FUN_0040b250, before the placement loop). Formation slot = value & 3
// (FUN_00408b00 cases 0..3 and 4..7 are the same four offsets). Returns false when the knob is off.
bool RoundGrid_SlotOrder(int slotValue[4]);
// The assignment itself (pure), exposed for the replay.
void RoundGrid_AssignMode4(const int codes[4], int slotValue[4]);
bool RoundGrid_OrderEnabled();

// G-RG-SNAP: MASHED_ROUND_GRID_REPLAY=<rows.tsv> (re/tools/roundgrid_unicorn.py) replays FUN_00409290 rows;
// report to MASHED_ROUND_GRID_REPLAY_OUT. Called once after the course block is built.
void RoundGrid_MaybeReplay();

}  // namespace Race
}  // namespace mashed_re
