// RaceModes — standalone port of Mashed's game-mode → race-rule pipeline
// (WS-G1, 2026-06-16). Replaces the MASHED_RACE_MODE/MASHED_LAPS env scaffold:
// derives the original's race-rule (DAT_007f0fd0, 0..10) from the real game
// mode (DAT_0067e9fc) + track, and collapses it onto the standalone race
// layer's two objectives (elimination vs non-elimination race).
//
// Full RE map + RVA citations: re/analysis/game_mode_rules_REmap_20260616.md.
// Binary anchor: MASHED.exe SHA-256 (unpatched)
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Pure logic (no global reads): the standalone cannot read MASHED's .rdata at
// runtime (image-pad zeroes 0x005f65c8), so the cup event-type table is baked
// from bytes read in Ghidra. Game mode + track come in as parameters.
#pragma once

namespace mashed_re {
namespace Race {
namespace RaceModes {

// 0x0042f6b0 FUN_0042f6b0 (MenuModeSync): menu selection index -> game-mode id.
// Returns the game-mode id (2..10), or -1 if `sel` has no mapping (4,6,7,>11).
int SelectionToGameMode(int sel);

// Event-type code for a championship (mode 3/4/5) race on `track`, read from
// the cup event-type table DAT_005f65c8 (high word of entry [modeOff+track*3]).
// Returns the event-type (0..0xc), or -1 for non-championship modes / OOB track.
int EventType(int gameMode, int track);

// 0x0043dfd0 race-launch action 0xff240000: event-type -> race rule
// (DAT_007f0fd0). See the RE map step 2.
int RaceRuleFromEvent(int eventType);

// Top-level championship-path rule for a game mode + track. Returns the real
// DAT_007f0fd0 race-rule value (0..10). Modes 2/6/7/8/9/10 -> 0 on this path;
// modes 3/4/5 -> RaceRuleFromEvent(EventType(...)).
int RaceRule(int gameMode, int track);

// MP/quick-race path (action 0xff420000): game length (DAT_0067ea88) -> rule.
// 0->0, 1->1, 2->2 (else 0). Not on the standalone's current race-launch path;
// provided for when MP/quick-race is wired (WS-G2).
int RaceRuleFromGameLength(int gameLength);

// Proven objective split (FUN_00410d10 control flow): proximity-elimination
// runs for every rule EXCEPT {5,7}. true => elimination active.
bool IsEliminationRule(int rule);

// Convenience: race-layer objective for a rule. 0 = elimination, 1 = laps /
// non-elimination race (TrackRenderer::SetRaceMode race_mode_).
int RaceModeForObjective(int rule);

}  // namespace RaceModes
}  // namespace Race
}  // namespace mashed_re
