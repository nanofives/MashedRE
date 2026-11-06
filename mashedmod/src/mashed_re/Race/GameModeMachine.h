// Mashed RE — MATCH-PORTSTATE-GAMEFLOW-FU: the original game-mode machine 0x004929d0 in the exe, racing phases only.
// Exe only. PREREG: verify/matchgameflowfu_20261102/PREREG_MATCH-PORTSTATE-GAMEFLOW-FU.md.
#pragma once

namespace mashed_re {
namespace Race {

// MASHED_PORTSTATE_GAMEFLOW_FU (default OFF, read once). With the knob OFF this returns at once.
// Called once per main-loop frame after GameFlow_Update. With the knob ON:
//   phase word DAT_00771968 in {3, 7}: runs the machine 0x004929d0 (re/match/gffu/004929d0.c), which calls
//     0x004927c0 and then case 3 or case 7;
//   any other phase: runs only 0x004927c0, the call the machine makes on entry in every phase.
// Cases 1, 2, 4, 5, 6 stay with GameFlow / the dev-race route, which publish those phases themselves
// (GameModeState_SetPhase, MASHED_PORTSTATE_GAMEFLOW). Case 2 calls 0x0040d440 -> 0x0040d270, the original course
// loader, which has no exe body; it cannot be reached from here.
void GameModeMachine_Tick();

} // namespace Race
} // namespace mashed_re
