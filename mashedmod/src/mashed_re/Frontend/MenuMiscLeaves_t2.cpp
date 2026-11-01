// Mashed RE — Frontend menu-miscellaneous leaves (c3-batch-t session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00423cc0  PlayerScoreTeamAccGatedB  — team-aware getter, block-1 +0x04, RaceSubMode gate
//   0x00423d50  PlayerScoreTeamAccBaseB   — team-aware getter, block-1 +0x50, no gate
//   0x00423dd0  PlayerScoreTeamAccGatedC  — team-aware getter, block-1 +0x08, RaceSubMode gate
//   0x00423e60  PlayerScoreTeamAccCumB    — team-aware getter, block-2 +0x5c, no gate
//   0x00424920  EndOfRoundAccumulator     — pure-leaf 32-add accumulator block-1→block-2
//
// REFUSED in this batch (and reasons):
//   0x00422aa0  SlotFieldSet            — caller 0x0041f880 still C1 (caller-gate)
//   0x00425b90  ClumpForAllAtomicsWrap  — both callees C1 (RpClumpForAllAtomics, FUN_004516d0)
//   0x00426cb0  SlotPtrLookup4c         — both callers (0x00407e20, 0x00408b00) C1
//   0x00424270  RankSortDispatcher      — no harness arg_type fits void(int*, int, int)
//   0x0042bfb0  StateBlockWrite6        — no harness arg_type fits 6-arg conditional writer
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s2/0x00423cc0.md
//   re/analysis/frontend_c1_to_c2_s2/0x00423d50.md
//   re/analysis/frontend_c1_to_c2_s2/0x00423dd0.md
//   re/analysis/frontend_c1_to_c2_s2/0x00423e60.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00424920.md

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S05: the callee declarations and shared constants that served the five bodies below were
// removed with them (no other user in this TU).

// ---------------------------------------------------------------------------
// PlayerScoreTeamAccGatedB  --  0x00423cc0
//
// Original: FUN_00423cc0 (~120 bytes, 0x00423cc0..0x00423d4f)
// Signature: int __cdecl FUN_00423cc0(int param_1)
//   param_1: player index 0..3
// Returns: team-aggregated value at base 0x00899a44 (block-1 +0x04),
//          or 0 if GetRaceSubMode() == 4.
//
// Body (mechanical, cited from analysis):
//   iVar2 = GetRaceSubMode()                    // @ 0x00423cc6
//   if (iVar2 == 4) return 0                    // @ 0x00423ccc
//   iVar2 = GetTeamMode()                       // @ 0x00423cd2
//   score_base = 0x00899a44 ; team_base = 0x007f1a18
//   if (iVar2 == 0): return score_base[param_1 * 0x4e]
//   iVar1 = team_base[param_1 * 4]              // team id
//   iVar2 = score_base[param_1 * 0x4e]          // own value
//   for k in {0,1,2,3}: if team_base[k*4]==iVar1 && param_1!=k: iVar2 += score_base[k*0x4e]
//
// Constants cited:
//   0x00899a44  — score array base (block-1 +0x04 from 0x00899a40)  [0x00423ce3]
//   0x4e        — dword stride per player                              [0x00423ce3]
//   0x007f1a18  — team-ID array root                                   [0x00423cdd]
//   4           — race-sub-mode guard value                            [0x00423ccc]
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00423cc0.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423cc0 (PlayerScoreTeamAccGatedB) moved to Match/MatchS05_00423cc0.cpp,
// which compiles the byte-matched source re/match/s05/00423cc0.c.

// ---------------------------------------------------------------------------
// PlayerScoreTeamAccBaseB  --  0x00423d50
//
// Original: FUN_00423d50 (~120 bytes, 0x00423d50..0x00423dcf)
// Signature: int __cdecl FUN_00423d50(int param_1)
//   param_1: player index 0..3
// Returns: team-aggregated value at base 0x00899a90 (block-1 +0x50).
//          No race-sub-mode guard.
//
// Body (mechanical):
//   iVar2 = GetTeamMode()                       // @ 0x00423d56
//   score_base = 0x00899a90 ; team_base = 0x007f1a18
//   if (iVar2 == 0): return score_base[param_1 * 0x4e]
//   iVar1 = team_base[param_1 * 4]
//   iVar2 = score_base[param_1 * 0x4e]
//   for k in {0,1,2,3}: if team_base[k*4]==iVar1 && param_1!=k: iVar2 += score_base[k*0x4e]
//
// Constants cited:
//   0x00899a90  — score array base (block-1 +0x50)  [0x00423d67]
//   0x4e        — dword stride per player           [0x00423d67]
//   0x007f1a18  — team-ID array root                [0x00423d61]
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00423d50.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423d50 (PlayerScoreTeamAccBaseB) moved to Match/MatchS05_00423d50.cpp,
// which compiles the byte-matched source re/match/s05/00423d50.c.

// ---------------------------------------------------------------------------
// PlayerScoreTeamAccGatedC  --  0x00423dd0
//
// Original: FUN_00423dd0 (~120 bytes, 0x00423dd0..0x00423e5f)
// Signature: int __cdecl FUN_00423dd0(int param_1)
//   param_1: player index 0..3
// Returns: team-aggregated value at base 0x00899a48 (block-1 +0x08),
//          or 0 if GetRaceSubMode() == 4.
//
// Body (mechanical):
//   iVar2 = GetRaceSubMode()                    // @ 0x00423dd6
//   if (iVar2 == 4) return 0                    // @ 0x00423ddc
//   iVar2 = GetTeamMode()                       // @ 0x00423de2
//   score_base = 0x00899a48 ; team_base = 0x007f1a18
//   if (iVar2 == 0): return score_base[param_1 * 0x4e]
//   ...team-accumulate as above...
//
// Constants cited:
//   0x00899a48  — score array base (block-1 +0x08)  [0x00423df3]
//   0x4e        — dword stride per player           [0x00423df3]
//   0x007f1a18  — team-ID array root                [0x00423ded]
//   4           — race-sub-mode guard value         [0x00423ddc]
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00423dd0.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423dd0 (PlayerScoreTeamAccGatedC) moved to Match/MatchS05_00423dd0.cpp,
// which compiles the byte-matched source re/match/s05/00423dd0.c.

// ---------------------------------------------------------------------------
// PlayerScoreTeamAccCumB  --  0x00423e60
//
// Original: FUN_00423e60 (~120 bytes, 0x00423e60..0x00423edf)
// Signature: int __cdecl FUN_00423e60(int param_1)
//   param_1: player index 0..3
// Returns: team-aggregated value at base 0x00899f7c (block-2 +0x5c from
//          DAT_00899f20, i.e. block-2 +0x5c). No race-sub-mode guard.
//
// Body (mechanical):
//   iVar2 = GetTeamMode()                       // @ 0x00423e66
//   score_base = 0x00899f7c ; team_base = 0x007f1a18
//   if (iVar2 == 0): return score_base[param_1 * 0x4e]
//   ...team-accumulate as above...
//
// Constants cited:
//   0x00899f7c  — score array base (block-2 +0x5c)  [0x00423e77]
//   0x4e        — dword stride per player           [0x00423e77]
//   0x007f1a18  — team-ID array root                [0x00423e71]
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00423e60.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423e60 (PlayerScoreTeamAccCumB) moved to Match/MatchS05_00423e60.cpp,
// which compiles the byte-matched source re/match/s05/00423e60.c.

// ---------------------------------------------------------------------------
// EndOfRoundAccumulator  --  0x00424920
//
// Original: FUN_00424920 (0x00424920..0x00424b7f)
// Signature: void __cdecl FUN_00424920(void)
// Returns: void
//
// Body: linear 32-add accumulator. Adds 32 stat fields from the "current
// match" block (block-1, rooted near DAT_00899a40) into the "cumulative
// tournament" block (block-2, rooted near DAT_00899f20). 32 pairs cover 8
// fields × 4 players. No branching, no loops, no calls. Pure leaf.
//
// Mechanical transcript: each line is `block2_field += block1_field`.
// All 32 pairs listed below; see analysis note for full address breakdown.
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00424920.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00424920 (EndOfRoundAccumulator) moved to Match/MatchS05_00424920.cpp,
// which compiles the byte-matched source re/match/s05/00424920.c.
