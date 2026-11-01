// Mashed RE — Boot/GameStateCluster.cpp
// Skeleton game-state cluster: 6 pure-leaf getters/setters/zeroing ops.
//
// Covers:
//   0x00426c00  GameStateFlagGet      — returns DAT_00644158 (pure DWORD getter)
//   0x0042b8e0  StatePhaseIsTwo       — returns (DAT_0067eca4 == 2) as bool
//   0x0042b910  RaceEndConstGet       — returns literal constant 5
//   0x0042b940  StatePhaseSubSet      — writes param_1 to DAT_0067ecb0 (pure DWORD setter)
//   0x0042c1c0  RaceInterruptFlagGet  — returns DAT_0067eab0 (pure DWORD getter)
//   0x0042c1d0  RaceStateArrayZero    — zeroes 12 DWORDs at DAT_0067ea10
//
// All 6 are pure leaf functions (callees_depth1: []).
// Leaf-function exemption applies for C2->C3 structural anti-island rule.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/skeleton_prep_game_state/00426c00.md
//   re/analysis/skeleton_prep_game_state/0042b8e0.md
//   re/analysis/skeleton_prep_game_state/0042b910.md
//   re/analysis/skeleton_prep_game_state/0042b940.md
//   re/analysis/skeleton_prep_game_state/0042c1c0.md
//   re/analysis/skeleton_prep_game_state/0042c1d0.md

#include "../Core/HookSystem.h"
#include <cstdint>


// ---------------------------------------------------------------------------
// GameStateFlagGet  --  0x00426c00
//
// Original: FUN_00426c00 (5 bytes, 0x00426c00–0x00426c04).
// Decompiled body (verbatim from analysis note 00426c00.md):
//   MOV EAX, [0x00644158]
//   RET
//
// Pure global getter: returns DAT_00644158 without modification.
// No branches, no writes, no callees.
// Callers compare result against 0x21 (33 dec).
//
// Constants cited (from analysis table):
//   0x00426c01  0x00644158  — DWORD global read and returned
//
// Uncertainties:
//   [UNCERTAIN U-0421] Semantic meaning of DAT_00644158; callers compare to 0x21.
//                       Blocks=none (leaf exemption applies).
//
// Caller: 0x00402a40.
// Callees: none (leaf exemption applies).
// ---------------------------------------------------------------------------

// 0x00426c00
extern "C" __declspec(dllexport) std::uint32_t __cdecl GameStateFlagGet() {
    // 0x00426c01: read and return DAT_00644158.
    return *reinterpret_cast<const std::uint32_t*>(0x00644158u);
}

RH_ScopedInstall(GameStateFlagGet, 0x00426c00);  // re-enabled 2026-05-24 phase-a2 GREEN read_global


// MATCH-S07: the body of 0x0042b8e0 (StatePhaseIsTwo) moved to Match/MatchS07_0042b8e0.cpp,
// which compiles the byte-matched source re/match/s07/0042b8e0.c.


// MATCH-S07: the body of 0x0042b910 (RaceEndConstGet) moved to Match/MatchS07_0042b910.cpp,
// which compiles the byte-matched source re/match/s07/0042b910.c.


// MATCH-S07: the body of 0x0042b940 (StatePhaseSubSet) moved to Match/MatchS07_0042b940.cpp,
// which compiles the byte-matched source re/match/s07/0042b940.c.


// MATCH-S07: the body of 0x0042c1c0 (RaceInterruptFlagGet) moved to Match/MatchS07_0042c1c0.cpp,
// which compiles the byte-matched source re/match/s07/0042c1c0.c.


// MATCH-S07: the body of 0x0042c1d0 (RaceStateArrayZero) moved to Match/MatchS07_0042c1d0.cpp,
// which compiles the byte-matched source re/match/s07/0042c1d0.c.
