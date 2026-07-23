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

// ---------------------------------------------------------------------------
// Forward declarations for callees
// ---------------------------------------------------------------------------

// 0x0042f500  GetDat0067ea64  — C4 (GameState/StateAccessors.cpp); call the ported
// export directly instead of trampolining to the raw original RVA (account2 rewire).
// Returns DAT_0067ea64 — team-mode flag (0 = no teams).
extern "C" std::uint32_t __cdecl GetDat0067ea64(void);

// 0x0042f6a0  GetRaceSubMode  — C3 (Util/GameStateGetters.cpp)
// std::int32_t __cdecl GetRaceSubMode(void)
// Returns DAT_0067e9fc — RaceSubMode register.
static auto* const s_GetRaceSubMode =
    reinterpret_cast<std::int32_t(__cdecl*)(void)>(0x0042f6a0);

// ---------------------------------------------------------------------------
// Shared constants for team-aware getters
// ---------------------------------------------------------------------------

// Team-ID array rooted at DAT_007f1a18, stride 4 dwords (= 0x10 bytes) per player.
static const unsigned k_TeamIdRoot   = 0x007f1a18u;
static const unsigned k_TeamIdStride = 4u;    // dword stride per player

static const unsigned k_PlayerStride = 0x4eu; // dword stride per player slot

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

// 0x00423cc0
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerScoreTeamAccGatedB(std::int32_t param_1) {
    std::int32_t mode = s_GetRaceSubMode();          // @ 0x00423cc6
    if (mode == 4) return 0;                         // @ 0x00423ccc

    std::int32_t iVar2 = static_cast<std::int32_t>(GetDat0067ea64());            // @ 0x00423cd2

    std::int32_t* const score_base = reinterpret_cast<std::int32_t*>(0x00899a44u);
    std::int32_t* const team_base  = reinterpret_cast<std::int32_t*>(k_TeamIdRoot);

    if (iVar2 == 0) {
        iVar2 = score_base[param_1 * k_PlayerStride];
        return iVar2;
    }

    std::int32_t iVar1 = team_base[param_1 * k_TeamIdStride];
    iVar2 = score_base[param_1 * k_PlayerStride];   // own value

    if (team_base[0 * k_TeamIdStride] == iVar1 && param_1 != 0)
        iVar2 += score_base[0 * k_PlayerStride];
    if (team_base[1 * k_TeamIdStride] == iVar1 && param_1 != 1)
        iVar2 += score_base[1 * k_PlayerStride];
    if (team_base[2 * k_TeamIdStride] == iVar1 && param_1 != 2)
        iVar2 += score_base[2 * k_PlayerStride];
    if (team_base[3 * k_TeamIdStride] == iVar1 && param_1 != 3)
        return iVar2 + score_base[3 * k_PlayerStride];

    return iVar2;
}
RH_ScopedInstall(PlayerScoreTeamAccGatedB, 0x00423cc0);

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

// 0x00423d50
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerScoreTeamAccBaseB(std::int32_t param_1) {
    std::int32_t iVar2 = static_cast<std::int32_t>(GetDat0067ea64());           // @ 0x00423d56

    std::int32_t* const score_base = reinterpret_cast<std::int32_t*>(0x00899a90u);
    std::int32_t* const team_base  = reinterpret_cast<std::int32_t*>(k_TeamIdRoot);

    if (iVar2 == 0) {
        iVar2 = score_base[param_1 * k_PlayerStride];
        return iVar2;
    }

    std::int32_t iVar1 = team_base[param_1 * k_TeamIdStride];
    iVar2 = score_base[param_1 * k_PlayerStride];

    if (team_base[0 * k_TeamIdStride] == iVar1 && param_1 != 0)
        iVar2 += score_base[0 * k_PlayerStride];
    if (team_base[1 * k_TeamIdStride] == iVar1 && param_1 != 1)
        iVar2 += score_base[1 * k_PlayerStride];
    if (team_base[2 * k_TeamIdStride] == iVar1 && param_1 != 2)
        iVar2 += score_base[2 * k_PlayerStride];
    if (team_base[3 * k_TeamIdStride] == iVar1 && param_1 != 3)
        return iVar2 + score_base[3 * k_PlayerStride];

    return iVar2;
}
RH_ScopedInstall(PlayerScoreTeamAccBaseB, 0x00423d50);

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

// 0x00423dd0
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerScoreTeamAccGatedC(std::int32_t param_1) {
    std::int32_t mode = s_GetRaceSubMode();          // @ 0x00423dd6
    if (mode == 4) return 0;                         // @ 0x00423ddc

    std::int32_t iVar2 = static_cast<std::int32_t>(GetDat0067ea64());            // @ 0x00423de2

    std::int32_t* const score_base = reinterpret_cast<std::int32_t*>(0x00899a48u);
    std::int32_t* const team_base  = reinterpret_cast<std::int32_t*>(k_TeamIdRoot);

    if (iVar2 == 0) {
        iVar2 = score_base[param_1 * k_PlayerStride];
        return iVar2;
    }

    std::int32_t iVar1 = team_base[param_1 * k_TeamIdStride];
    iVar2 = score_base[param_1 * k_PlayerStride];

    if (team_base[0 * k_TeamIdStride] == iVar1 && param_1 != 0)
        iVar2 += score_base[0 * k_PlayerStride];
    if (team_base[1 * k_TeamIdStride] == iVar1 && param_1 != 1)
        iVar2 += score_base[1 * k_PlayerStride];
    if (team_base[2 * k_TeamIdStride] == iVar1 && param_1 != 2)
        iVar2 += score_base[2 * k_PlayerStride];
    if (team_base[3 * k_TeamIdStride] == iVar1 && param_1 != 3)
        return iVar2 + score_base[3 * k_PlayerStride];

    return iVar2;
}
RH_ScopedInstall(PlayerScoreTeamAccGatedC, 0x00423dd0);

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

// 0x00423e60
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerScoreTeamAccCumB(std::int32_t param_1) {
    std::int32_t iVar2 = static_cast<std::int32_t>(GetDat0067ea64());           // @ 0x00423e66

    std::int32_t* const score_base = reinterpret_cast<std::int32_t*>(0x00899f7cu);
    std::int32_t* const team_base  = reinterpret_cast<std::int32_t*>(k_TeamIdRoot);

    if (iVar2 == 0) {
        iVar2 = score_base[param_1 * k_PlayerStride];
        return iVar2;
    }

    std::int32_t iVar1 = team_base[param_1 * k_TeamIdStride];
    iVar2 = score_base[param_1 * k_PlayerStride];

    if (team_base[0 * k_TeamIdStride] == iVar1 && param_1 != 0)
        iVar2 += score_base[0 * k_PlayerStride];
    if (team_base[1 * k_TeamIdStride] == iVar1 && param_1 != 1)
        iVar2 += score_base[1 * k_PlayerStride];
    if (team_base[2 * k_TeamIdStride] == iVar1 && param_1 != 2)
        iVar2 += score_base[2 * k_PlayerStride];
    if (team_base[3 * k_TeamIdStride] == iVar1 && param_1 != 3)
        return iVar2 + score_base[3 * k_PlayerStride];

    return iVar2;
}
RH_ScopedInstall(PlayerScoreTeamAccCumB, 0x00423e60);

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

// 0x00424920
extern "C" __declspec(dllexport) void __cdecl EndOfRoundAccumulator() {
    // Player 0 — block-1 root 0x00899a40 + various offsets → block-2 root 0x00899f20.
    *reinterpret_cast<std::int32_t*>(0x00899f20u) += *reinterpret_cast<std::int32_t*>(0x00899a40u);
    *reinterpret_cast<std::int32_t*>(0x00899f74u) += *reinterpret_cast<std::int32_t*>(0x00899a94u);
    *reinterpret_cast<std::int32_t*>(0x00899f24u) += *reinterpret_cast<std::int32_t*>(0x00899a44u);
    *reinterpret_cast<std::int32_t*>(0x00899f78u) += *reinterpret_cast<std::int32_t*>(0x00899a98u);
    *reinterpret_cast<std::int32_t*>(0x00899f28u) += *reinterpret_cast<std::int32_t*>(0x00899a48u);
    *reinterpret_cast<std::int32_t*>(0x00899f70u) += *reinterpret_cast<std::int32_t*>(0x00899a90u);
    *reinterpret_cast<std::int32_t*>(0x00899f7cu) += *reinterpret_cast<std::int32_t*>(0x00899a9cu);
    *reinterpret_cast<std::int32_t*>(0x00899e80u) += *reinterpret_cast<std::int32_t*>(0x008999a0u);

    // Player 1 — block-1 0x00899b78 area → block-2 0x0089a058 area.
    *reinterpret_cast<std::int32_t*>(0x0089a058u) += *reinterpret_cast<std::int32_t*>(0x00899b78u);
    *reinterpret_cast<std::int32_t*>(0x0089a0acu) += *reinterpret_cast<std::int32_t*>(0x00899bccu);
    *reinterpret_cast<std::int32_t*>(0x0089a05cu) += *reinterpret_cast<std::int32_t*>(0x00899b7cu);
    *reinterpret_cast<std::int32_t*>(0x0089a0b0u) += *reinterpret_cast<std::int32_t*>(0x00899bd0u);
    *reinterpret_cast<std::int32_t*>(0x0089a060u) += *reinterpret_cast<std::int32_t*>(0x00899b80u);
    *reinterpret_cast<std::int32_t*>(0x0089a0a8u) += *reinterpret_cast<std::int32_t*>(0x00899bc8u);
    *reinterpret_cast<std::int32_t*>(0x0089a0b4u) += *reinterpret_cast<std::int32_t*>(0x00899bd4u);
    *reinterpret_cast<std::int32_t*>(0x00899fb8u) += *reinterpret_cast<std::int32_t*>(0x00899ad8u);

    // Player 2 — block-1 0x00899cb0 area → block-2 0x0089a190 area.
    *reinterpret_cast<std::int32_t*>(0x0089a190u) += *reinterpret_cast<std::int32_t*>(0x00899cb0u);
    *reinterpret_cast<std::int32_t*>(0x0089a1e4u) += *reinterpret_cast<std::int32_t*>(0x00899d04u);
    *reinterpret_cast<std::int32_t*>(0x0089a194u) += *reinterpret_cast<std::int32_t*>(0x00899cb4u);
    *reinterpret_cast<std::int32_t*>(0x0089a1e8u) += *reinterpret_cast<std::int32_t*>(0x00899d08u);
    *reinterpret_cast<std::int32_t*>(0x0089a198u) += *reinterpret_cast<std::int32_t*>(0x00899cb8u);
    *reinterpret_cast<std::int32_t*>(0x0089a1e0u) += *reinterpret_cast<std::int32_t*>(0x00899d00u);
    *reinterpret_cast<std::int32_t*>(0x0089a1ecu) += *reinterpret_cast<std::int32_t*>(0x00899d0cu);
    *reinterpret_cast<std::int32_t*>(0x0089a0f0u) += *reinterpret_cast<std::int32_t*>(0x00899c10u);

    // Player 3 — block-1 0x00899de8 area → block-2 0x0089a2c8 area.
    *reinterpret_cast<std::int32_t*>(0x0089a2c8u) += *reinterpret_cast<std::int32_t*>(0x00899de8u);
    *reinterpret_cast<std::int32_t*>(0x0089a31cu) += *reinterpret_cast<std::int32_t*>(0x00899e3cu);
    *reinterpret_cast<std::int32_t*>(0x0089a2ccu) += *reinterpret_cast<std::int32_t*>(0x00899decu);
    *reinterpret_cast<std::int32_t*>(0x0089a320u) += *reinterpret_cast<std::int32_t*>(0x00899e40u);
    *reinterpret_cast<std::int32_t*>(0x0089a2d0u) += *reinterpret_cast<std::int32_t*>(0x00899df0u);
    *reinterpret_cast<std::int32_t*>(0x0089a318u) += *reinterpret_cast<std::int32_t*>(0x00899e38u);
    *reinterpret_cast<std::int32_t*>(0x0089a324u) += *reinterpret_cast<std::int32_t*>(0x00899e44u);
    *reinterpret_cast<std::int32_t*>(0x0089a228u) += *reinterpret_cast<std::int32_t*>(0x00899d48u);
}
RH_ScopedInstall(EndOfRoundAccumulator, 0x00424920);
