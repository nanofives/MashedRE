// Mashed RE — Frontend per-player score accessors (s2 cluster).
// Five score-getter functions from the Frontend subsystem.
// All read from per-player data arrays at base 0x00899a40, stride 0x4e dwords.
//
// Array indexing note (confirmed against s3 analysis plate FUN_00423ee0.md):
//   - Per-player score arrays are typed as dword[] in Ghidra.
//     Ghidra shows (&DAT_base)[param_1 * 0x4e], meaning the C++ access is
//     reinterpret_cast<uint32_t*>(base)[param_1 * 0x4e]
//     i.e. byte offset = param_1 * 0x4e * 4 = param_1 * 0x138.
//   - Team id array at 0x007f1a18: stride 0x10 bytes per player (4 dword slots).
//     Ghidra shows (&DAT_007f1a18)[param_1 * 4], meaning
//     reinterpret_cast<int32_t*>(0x007f1a18)[param_1 * 4]
//     i.e. byte offset = param_1 * 4 * 4 = param_1 * 0x10.
//
// Address verification (player stride 0x4e dwords = 0x138 bytes):
//   player 0 @ 0x00899a9c + 0*0x138 = 0x00899a9c  (DAT_00899a9c confirmed)
//   player 1 @ 0x00899a9c + 1*0x138 = 0x00899bd4  (DAT_00899bd4 confirmed)
//   player 2 @ 0x00899a9c + 2*0x138 = 0x00899d0c  (DAT_00899d0c confirmed)
//   player 3 @ 0x00899a9c + 3*0x138 = 0x00899e44  (DAT_00899e44 confirmed)
//
// Callees:
//   GetRaceSubMode  (0x0042f6a0, C3) — returns DAT_0067e9fc; race sub-mode int
//   GetDat0067ea64  (0x0042f500, C4) — returns DAT_0067ea64; team mode flag
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Session: c3-batch-s-s2  2026-05-26
// Analysis: re/analysis/frontend_c1_to_c2_s2/0x00423b40.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423b60.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423ba0.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423bc0.md
//           re/analysis/frontend_c1_to_c2_s2/0x00423c40.md

#include "../Core/HookSystem.h"

#include <cstdint>

// Forward declarations of callees (originals are still live; we call them by RVA).
// GetRaceSubMode  @ 0x0042f6a0  C3 — returns race sub-mode int (DAT_0067e9fc)
// GetDat0067ea64  @ 0x0042f500  C4 — returns team mode flag (DAT_0067ea64)

extern "C" std::int32_t __cdecl GetRaceSubMode();   // 0x0042f6a0
extern "C" std::int32_t __cdecl GetDat0067ea64();   // 0x0042f500

// ---------------------------------------------------------------------------
// 0x00423b40  PlayerScoreAccA
// undefined4(int param_1)
// Per-player score accessor #1.
// Guard: if GetRaceSubMode() == 4, return 0.
// Returns (&DAT_00899a94)[param_1 * 0x4e] — byte offset param_1 * 0x138.
// Array base: 0x00899a94 (field +0x54 of per-player block from 0x00899a40).
// Source addresses: 0x00423b46 (call), 0x00423b4c (guard), 0x00423b55 (read).
// ---------------------------------------------------------------------------
// 0x00423b40
extern "C" __declspec(dllexport) std::uint32_t __cdecl PlayerScoreAccA(std::int32_t param_1) {
    std::int32_t mode = GetRaceSubMode();          // @ 0x00423b46
    if (mode == 4) return 0u;                      // @ 0x00423b4c
    // @ 0x00423b55: (&DAT_00899a94)[param_1 * 0x4e] — dword array, stride 0x4e dwords
    return reinterpret_cast<std::uint32_t*>(0x00899a94u)[param_1 * 0x4e];
}
RH_ScopedInstall(PlayerScoreAccA, 0x00423b40);

// ---------------------------------------------------------------------------
// 0x00423b60  PlayerScoreAccB
// undefined4(int param_1)
// Per-player score accessor #2. Twin of PlayerScoreAccA with base 0x00899a98.
// Array base: 0x00899a98 (= 0x00899a94 + 4, field +0x58 of per-player block).
// Source addresses: 0x00423b66 (call), 0x00423b6c (guard), 0x00423b75 (read).
// ---------------------------------------------------------------------------
// 0x00423b60
extern "C" __declspec(dllexport) std::uint32_t __cdecl PlayerScoreAccB(std::int32_t param_1) {
    std::int32_t mode = GetRaceSubMode();          // @ 0x00423b66
    if (mode == 4) return 0u;                      // @ 0x00423b6c
    // @ 0x00423b75: (&DAT_00899a98)[param_1 * 0x4e] — dword array, stride 0x4e dwords
    return reinterpret_cast<std::uint32_t*>(0x00899a98u)[param_1 * 0x4e];
}
RH_ScopedInstall(PlayerScoreAccB, 0x00423b60);

// ---------------------------------------------------------------------------
// 0x00423ba0  PlayerScoreAccD
// undefined4(int param_1)
// Per-player score accessor #4. Cumulative counterpart of PlayerScoreAccB.
// Array base: 0x00899f78 (= 0x00899f74 + 4, field offset in second block).
// Source addresses: 0x00423ba6 (call), 0x00423bac (guard), 0x00423bb5 (read).
// ---------------------------------------------------------------------------
// 0x00423ba0
extern "C" __declspec(dllexport) std::uint32_t __cdecl PlayerScoreAccD(std::int32_t param_1) {
    std::int32_t mode = GetRaceSubMode();          // @ 0x00423ba6
    if (mode == 4) return 0u;                      // @ 0x00423bac
    // @ 0x00423bb5: (&DAT_00899f78)[param_1 * 0x4e] — dword array, stride 0x4e dwords
    return reinterpret_cast<std::uint32_t*>(0x00899f78u)[param_1 * 0x4e];
}
RH_ScopedInstall(PlayerScoreAccD, 0x00423ba0);

// ---------------------------------------------------------------------------
// 0x00423bc0  PlayerScoreTeamAccC
// int(int param_1)
// Team-aware per-player field accessor. Field at base 0x00899a9c, stride 0x4e dwords.
// If GetDat0067ea64() == 0 (no teams): return (&DAT_00899a9c)[param_1 * 0x4e].
// If team mode: return sum of field values for all players on same team as param_1
//   (excluding param_1 itself), plus param_1's own value.
//
// Team id array at 0x007f1a18, stride 0x10 bytes (4 dwords per player slot):
//   player 0 team id: DAT_007f1a18 (= team_base[0 * 4])
//   player 1 team id: DAT_007f1a28 (= team_base[1 * 4])
//   player 2 team id: DAT_007f1a38 (= team_base[2 * 4])
//   player 3 team id: DAT_007f1a48 (= team_base[3 * 4])
//
// Per-player field values (dword array, stride 0x4e dwords = 0x138 bytes):
//   player 0: DAT_00899a9c (= score_base[0 * 0x4e])
//   player 1: DAT_00899bd4 (= score_base[1 * 0x4e])  0x00899a9c + 0x138 = 0x00899bd4
//   player 2: DAT_00899d0c (= score_base[2 * 0x4e])  0x00899a9c + 0x270 = 0x00899d0c
//   player 3: DAT_00899e44 (= score_base[3 * 0x4e])  0x00899a9c + 0x3a8 = 0x00899e44
//
// Source addresses: 0x00423bc6 (call), 0x00423bcb (team==0 guard),
//   0x00423bd7 (score array), 0x00423bd1 (team id array).
// ---------------------------------------------------------------------------
// 0x00423bc0
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerScoreTeamAccC(std::int32_t param_1) {
    std::int32_t iVar2 = GetDat0067ea64();         // @ 0x00423bc6

    // Per-player field base: 0x00899a9c, stride 0x4e dwords.
    std::int32_t* const score_base = reinterpret_cast<std::int32_t*>(0x00899a9cu);  // @ 0x00423bd7
    // Team id array: 0x007f1a18, stride 4 dwords (= 0x10 bytes) per player.
    std::int32_t* const team_base  = reinterpret_cast<std::int32_t*>(0x007f1a18u);  // @ 0x00423bd1

    if (iVar2 == 0) {                              // @ 0x00423bcb — no teams
        iVar2 = score_base[param_1 * 0x4e];
        return iVar2;
    }

    // Team mode: get param_1's team id (stride 4 dwords = 0x10 bytes per player).
    std::int32_t iVar1 = team_base[param_1 * 4];  // get param_1's team id
    iVar2 = score_base[param_1 * 0x4e];           // own value

    // Accumulate teammates (same team id, excluding self).
    // player 0: team_base[0*4] == DAT_007f1a18; score_base[0*0x4e] == DAT_00899a9c
    if (team_base[0 * 4] == iVar1 && param_1 != 0)
        iVar2 += score_base[0 * 0x4e];
    // player 1: team_base[1*4] == DAT_007f1a28; score_base[1*0x4e] == DAT_00899bd4
    if (team_base[1 * 4] == iVar1 && param_1 != 1)
        iVar2 += score_base[1 * 0x4e];
    // player 2: team_base[2*4] == DAT_007f1a38; score_base[2*0x4e] == DAT_00899d0c
    if (team_base[2 * 4] == iVar1 && param_1 != 2)
        iVar2 += score_base[2 * 0x4e];
    // player 3: team_base[3*4] == DAT_007f1a48; score_base[3*0x4e] == DAT_00899e44
    if (team_base[3 * 4] == iVar1 && param_1 != 3)
        return iVar2 + score_base[3 * 0x4e];

    return iVar2;
}
RH_ScopedInstall(PlayerScoreTeamAccC, 0x00423bc0);

// ---------------------------------------------------------------------------
// 0x00423c40  PlayerScoreTeamAccBase
// int(int param_1)
// Team-aware accessor for field at base 0x00899a40 (offset +0x000 of player struct).
// Same accumulation logic as PlayerScoreTeamAccC but no race-sub-mode guard.
// Per-player field values (dword array, stride 0x4e dwords = 0x138 bytes):
//   player 0: DAT_00899a40 (= score_base[0 * 0x4e])
//   player 1: DAT_00899b78 (= score_base[1 * 0x4e])  0x00899a40 + 0x138 = 0x00899b78
//   player 2: DAT_00899cb0 (= score_base[2 * 0x4e])  0x00899a40 + 0x270 = 0x00899cb0
//   player 3: DAT_00899de8 (= score_base[3 * 0x4e])  0x00899a40 + 0x3a8 = 0x00899de8
//
// Source addresses: 0x00423c46 (call), 0x00423c57 (score array base 0x00899a40 stride 0x4e),
//   0x00423c51 (team id array base 0x007f1a18 stride 4).
// ---------------------------------------------------------------------------
// 0x00423c40
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerScoreTeamAccBase(std::int32_t param_1) {
    std::int32_t iVar2 = GetDat0067ea64();         // @ 0x00423c46

    // Per-player field base: 0x00899a40, stride 0x4e dwords.
    std::int32_t* const score_base = reinterpret_cast<std::int32_t*>(0x00899a40u);  // @ 0x00423c57
    // Team id array: 0x007f1a18, stride 4 dwords (= 0x10 bytes) per player.
    std::int32_t* const team_base  = reinterpret_cast<std::int32_t*>(0x007f1a18u);  // @ 0x00423c51

    if (iVar2 == 0) {                              // no teams
        iVar2 = score_base[param_1 * 0x4e];
        return iVar2;
    }

    // Team mode: get param_1's team id.
    std::int32_t iVar1 = team_base[param_1 * 4];
    iVar2 = score_base[param_1 * 0x4e];           // own value

    // Accumulate teammates.
    // player 0: DAT_007f1a18, DAT_00899a40
    if (team_base[0 * 4] == iVar1 && param_1 != 0)
        iVar2 += score_base[0 * 0x4e];
    // player 1: DAT_007f1a28, DAT_00899b78
    if (team_base[1 * 4] == iVar1 && param_1 != 1)
        iVar2 += score_base[1 * 0x4e];
    // player 2: DAT_007f1a38, DAT_00899cb0
    if (team_base[2 * 4] == iVar1 && param_1 != 2)
        iVar2 += score_base[2 * 0x4e];
    // player 3: DAT_007f1a48, DAT_00899de8
    if (team_base[3 * 4] == iVar1 && param_1 != 3)
        return iVar2 + score_base[3 * 0x4e];

    return iVar2;
}
RH_ScopedInstall(PlayerScoreTeamAccBase, 0x00423c40);
