// Mashed RE — promote-round round 10 (L3 curated, race-lane + live-global set).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   [MOVED 2026-10-09, D-11073 leg 1a] EntityVelocityCounterGet (FUN_0046c750) and
//   EntityDamageStateGet (FUN_0046c730) -> Race/SubStateCallees.cpp
//   0x0040b410  RaceScoreTimerGet        — util; unbounded indexed getter
//   0x0040e360  RaceModeSet              — util; 9B setter on the LIVE
//                                          race-phase global (vectors restricted)
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); cites in the per-function headers.
// NOTE the SIGNED bound compare in the entity getters (jl) — int param.
//
// Analysis:
//   re/analysis/bucket_ai_00452eb0_004c3df0/0046c750.md
//   re/analysis/bucket_ai_00452eb0_004c3df0/0046c730.md
//   re/analysis/game_state_d5_cont2/0x0040b410.md
//   re/analysis/leaderboard_d2/0x0040e360.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RaceScoreTimerGet  --  0x0040b410   (subsystem: util)
//
// Original: FUN_0040b410 (12 bytes, 0x0040b410..0x0040b41b)
// Bytes: 8B 44 24 04 / 8B 04 85 20 95 8A 00 / C3
//   (mov eax,[esp+4]; mov eax,[eax*4 + 0x008a9520]; ret)
//   Scaled-index addressing: element stride 4 BYTES (dword array).
// Signature: undefined4 FUN_0040b410(int param_1)
//
// NO bounds check (caller-responsible per the note) — vectors stay small
// non-negative so the deref lands inside the BSS array.
//
// Constants (cited from function body, base at 0x0040b416):
//   0x008a9520 — score/timer dword array base (group B; FUN_0040b250 resets
//                entries to -1000 / 0xfffffc18)
//
// Caller: FUN_0040e590 (C2).
// ---------------------------------------------------------------------------

// 0x0040b410
extern "C" __declspec(dllexport) std::uint32_t __cdecl RaceScoreTimerGet(int param_1) {
    // 0x008a9520 base + *4 scaled index cited at 0x0040b410 body.
    return *reinterpret_cast<const std::uint32_t*>(
        0x008a9520u + static_cast<std::uint32_t>(param_1) * 4u);
}

RH_ScopedInstall(RaceScoreTimerGet, 0x0040b410);

// ---------------------------------------------------------------------------
// RaceModeSet  --  0x0040e360   (subsystem: util; hooks.csv RaceMode::Set)
//
// Original: FUN_0040e360 (10 bytes, 0x0040e360..0x0040e369)
// Bytes: 8B 44 24 04 / A3 8C BA 63 00 / C3
//   (mov eax,[esp+4]; mov [0x0063ba8c],eax; ret)
// Signature: void FUN_0040e360(undefined4 param_1)
//
// CAUTION: 0x0063ba8c is the LIVE game-mode/race-phase global (read by
// GetRenderSubMode 0x0040e350 C3 and the AI dispatcher). The diff harness
// saves/restores it per vector, but the game thread could sample mid-bracket
// — so test vectors are RESTRICTED to values observed in the original
// (compares 4/5/8/9 per the 0x0040e350 notes; writes 7 at 0x0040e57e and 6
// at 0x00410504 per the hooks.csv row). No garbage values.
//
// Constants (cited from function body at 0x0040e360):
//   0x0063ba8c — game-mode/race-phase global
//
// Callers: containing fns of the cited call sites — FUN_0040e560 (C2) and
// Race::EvaluateResult 0x00410510 (C2).
// ---------------------------------------------------------------------------

// 0x0040e360
extern "C" __declspec(dllexport) void __cdecl RaceModeSet(std::uint32_t param_1) {
    // 0x0063ba8c cited at 0x0040e360 body.
    *reinterpret_cast<std::uint32_t*>(0x0063ba8cu) = param_1;
}

RH_ScopedInstall(RaceModeSet, 0x0040e360);
