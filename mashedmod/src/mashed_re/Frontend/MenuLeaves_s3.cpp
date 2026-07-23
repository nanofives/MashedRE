// Mashed RE — Frontend menu-leaf reimplementations (c3-batch-s session 3).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00423ee0  PlayerBlock2Field00Get    — team-aggregating getter, block-2+0x00, no gate
//   0x00423f60  PlayerBlock2Field04Get    — team-aggregating getter, block-2+0x04, RaceSubMode gate
//   0x00423ff0  PlayerBlock2Field50Get    — team-aggregating getter, block-2+0x50, no gate
//   0x00424070  PlayerBlock2Field08Get    — team-aggregating getter, block-2+0x08, RaceSubMode gate
//   0x004241b0  GetDat008994c0            — 5-byte leaf; returns DAT_008994c0
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s3/FUN_00423ee0.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00423f60.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00423ff0.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00424070.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_004241b0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees
// ---------------------------------------------------------------------------

// 0x0042f500  GetDat0067ea64  — C4 (GameState/StateAccessors.cpp); call the ported
// export directly instead of trampolining to the raw original RVA (account2 rewire).
// Returns DAT_0067ea64 — team-mode flag (0 = no teams).
extern "C" std::uint32_t __cdecl GetDat0067ea64(void);

// 0x0042f6a0  GetRaceSubMode  — C3 (GameState/StateAccessors.cpp)
// uint32_t __cdecl GetRaceSubMode(void)
// Returns DAT_0067e9fc — RaceSubMode register.
static auto* const s_GetRaceSubMode =
    reinterpret_cast<std::int32_t(__cdecl*)(void)>(0x0042f6a0);

// ---------------------------------------------------------------------------
// Constants shared across the four team-aggregation getters
// ---------------------------------------------------------------------------

// Block-2 per-player data array rooted at DAT_00899f20.
// stride: 0x4e dwords (0x138 bytes) per player slot.  [cited from 0x00423f04]
static const unsigned k_Block2Root   = 0x00899f20u;
static const unsigned k_Block2Stride = 0x4eu;          // dwords per player

// Team-ID array rooted at DAT_007f1a18.
// stride: 4 bytes per slot.  [cited from 0x00423f1a]
static const unsigned k_TeamIdRoot   = 0x007f1a18u;
static const unsigned k_TeamIdStride = 4u;             // bytes per player

static const int k_PlayerCount = 4;

// ---------------------------------------------------------------------------
// PlayerBlock2Field00Get  --  0x00423ee0
//
// Original: FUN_00423ee0 (0x00423ee0..0x00423f5e)
// Signature: int FUN_00423ee0(int param_1)
//   param_1: player index 0..3
// Returns: sum of block-2 field +0x00 for param_1's slot and all team-mates
//          when teams are enabled; raw field value otherwise.
//
// Algorithm (cited from 0x00423ee0 body):
//   1. Call GetDat0067ea64(); if 0 → return (&DAT_00899f20)[param_1*0x4e]. [0x00423ef8/04]
//   2. Load iVar1 = team-ID of param_1 from (&DAT_007f1a18)[param_1*4]. [0x00423f1a]
//   3. Seed iVar2 = (&DAT_00899f20)[param_1*0x4e]. [0x00423f28 area]
//   4. For k in {0,1,2,3}: if k==param_1 skip; else if team-ID[k]==iVar1:
//        iVar2 += (&DAT_00899f20)[k*0x4e]. [0x00423f28/32/3c/46]
//   5. Return iVar2.
//
// Constants cited:
//   0x00899f20 — block-2 array root                        [0x00423f04]
//   0x4e       — dword stride per slot                     [0x00423f04]
//   0x007f1a18 — team-ID array root                        [0x00423f1a]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423ee0.md
// ---------------------------------------------------------------------------

// 0x00423ee0
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerBlock2Field00Get(
    int param_1)
{
    // Field +0x00: dword offset 0 from block-2 root. [0x00423f04]
    const unsigned field_dword_offset = 0u;

    // Step 1: team-mode gate.
    if (GetDat0067ea64() == 0) {
        // No teams — return this player's field directly.
        return *reinterpret_cast<std::int32_t*>(
            k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);
    }

    // Step 2: read caller's team-ID.
    std::int32_t myTeam = *reinterpret_cast<std::int32_t*>(
        k_TeamIdRoot + static_cast<unsigned>(param_1) * k_TeamIdStride);

    // Step 3: seed with own value.
    std::int32_t iVar2 = *reinterpret_cast<std::int32_t*>(
        k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);

    // Step 4: accumulate matching team-mates.
    for (int k = 0; k < k_PlayerCount; ++k) {
        if (k == param_1) continue;
        std::int32_t kTeam = *reinterpret_cast<std::int32_t*>(
            k_TeamIdRoot + static_cast<unsigned>(k) * k_TeamIdStride);
        if (kTeam == myTeam) {
            iVar2 += *reinterpret_cast<std::int32_t*>(
                k_Block2Root + (k_Block2Stride * static_cast<unsigned>(k) + field_dword_offset) * 4u);
        }
    }
    return iVar2;
}

RH_ScopedInstall(PlayerBlock2Field00Get, 0x00423ee0);

// ---------------------------------------------------------------------------
// PlayerBlock2Field04Get  --  0x00423f60
//
// Original: FUN_00423f60 (0x00423f60..0x00423feb)
// Signature: int FUN_00423f60(int param_1)
//   param_1: player index 0..3
// Returns: aggregated block-2+0x04 field; 0 if RaceSubMode==4.
//
// Algorithm (cited from 0x00423f60 body):
//   1. Call GetRaceSubMode(); if == 4 → return 0. [0x00423f6c]
//   2. Call GetDat0067ea64(); if 0 → return (&DAT_00899f24)[param_1*0x4e]. [0x00423f78/85]
//   3. Team-aggregate as per PlayerBlock2Field00Get over field dword offset 1 (+0x04). [0x00423f85..]
//
// Constants cited:
//   4          — RaceSubMode early-out sentinel          [0x00423f6c]
//   0x00899f24 — block-2+0x04 root (DAT_00899f20+4)     [0x00423f85]
//   0x4e       — dword stride per slot                   [0x00423f85]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423f60.md
// ---------------------------------------------------------------------------

// 0x00423f60
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerBlock2Field04Get(
    int param_1)
{
    // Step 1: RaceSubMode==4 gate. [0x00423f6c]
    if (s_GetRaceSubMode() == 4) {
        return 0;
    }

    // Field +0x04: dword offset 1 from block-2 root. [0x00423f85]
    const unsigned field_dword_offset = 1u;

    // Step 2: team-mode gate.
    if (GetDat0067ea64() == 0) {
        return *reinterpret_cast<std::int32_t*>(
            k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);
    }

    // Step 3: team-ID of caller.
    std::int32_t myTeam = *reinterpret_cast<std::int32_t*>(
        k_TeamIdRoot + static_cast<unsigned>(param_1) * k_TeamIdStride);

    // Seed with own value.
    std::int32_t iVar2 = *reinterpret_cast<std::int32_t*>(
        k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);

    // Accumulate matching team-mates.
    for (int k = 0; k < k_PlayerCount; ++k) {
        if (k == param_1) continue;
        std::int32_t kTeam = *reinterpret_cast<std::int32_t*>(
            k_TeamIdRoot + static_cast<unsigned>(k) * k_TeamIdStride);
        if (kTeam == myTeam) {
            iVar2 += *reinterpret_cast<std::int32_t*>(
                k_Block2Root + (k_Block2Stride * static_cast<unsigned>(k) + field_dword_offset) * 4u);
        }
    }
    return iVar2;
}

RH_ScopedInstall(PlayerBlock2Field04Get, 0x00423f60);

// ---------------------------------------------------------------------------
// PlayerBlock2Field50Get  --  0x00423ff0
//
// Original: FUN_00423ff0 (0x00423ff0..0x0042406e)
// Signature: int FUN_00423ff0(int param_1)
//   param_1: player index 0..3
// Returns: aggregated block-2+0x50 field; no RaceSubMode gate.
//
// Algorithm (cited from 0x00423ff0 body):
//   1. Call GetDat0067ea64(); if 0 → return (&DAT_00899f70)[param_1*0x4e]. [0x00424002]
//   2. Team-aggregate over field dword offset 0x14 (+0x50 from block-2 root). [0x00424002..]
//
// Constants cited:
//   0x00899f70 — block-2+0x50 root (DAT_00899f20 + 0x50)  [0x00424002]
//     field dword offset = 0x50 / 4 = 0x14 = 20
//   0x4e       — dword stride per slot                     [0x00424002]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423ff0.md
// ---------------------------------------------------------------------------

// 0x00423ff0
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerBlock2Field50Get(
    int param_1)
{
    // Field +0x50: dword offset 0x14 (= 0x50/4 = 20) from block-2 root. [0x00424002]
    const unsigned field_dword_offset = 0x14u;

    // Step 1: team-mode gate.
    if (GetDat0067ea64() == 0) {
        return *reinterpret_cast<std::int32_t*>(
            k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);
    }

    // Step 2: team-ID of caller.
    std::int32_t myTeam = *reinterpret_cast<std::int32_t*>(
        k_TeamIdRoot + static_cast<unsigned>(param_1) * k_TeamIdStride);

    // Seed with own value.
    std::int32_t iVar2 = *reinterpret_cast<std::int32_t*>(
        k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);

    // Accumulate matching team-mates.
    for (int k = 0; k < k_PlayerCount; ++k) {
        if (k == param_1) continue;
        std::int32_t kTeam = *reinterpret_cast<std::int32_t*>(
            k_TeamIdRoot + static_cast<unsigned>(k) * k_TeamIdStride);
        if (kTeam == myTeam) {
            iVar2 += *reinterpret_cast<std::int32_t*>(
                k_Block2Root + (k_Block2Stride * static_cast<unsigned>(k) + field_dword_offset) * 4u);
        }
    }
    return iVar2;
}

RH_ScopedInstall(PlayerBlock2Field50Get, 0x00423ff0);

// ---------------------------------------------------------------------------
// PlayerBlock2Field08Get  --  0x00424070
//
// Original: FUN_00424070 (0x00424070..0x004240fb)
// Signature: int FUN_00424070(int param_1)
//   param_1: player index 0..3
// Returns: aggregated block-2+0x08 field; 0 if RaceSubMode==4.
//
// Algorithm (cited from 0x00424070 body):
//   1. Call GetRaceSubMode(); if == 4 → return 0. [gate at 0x00424070 area]
//   2. Call GetDat0067ea64(); if 0 → return (&DAT_00899f28)[param_1*0x4e].
//   3. Team-aggregate over field dword offset 2 (+0x08 from block-2 root).
//
// Constants cited:
//   4          — RaceSubMode early-out sentinel
//   0x00899f28 — block-2+0x08 root (DAT_00899f20 + 0x08)
//     field dword offset = 0x08 / 4 = 2
//   0x4e       — dword stride per slot
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00424070.md
// ---------------------------------------------------------------------------

// 0x00424070
extern "C" __declspec(dllexport) std::int32_t __cdecl PlayerBlock2Field08Get(
    int param_1)
{
    // Step 1: RaceSubMode==4 gate (same pattern as PlayerBlock2Field04Get).
    if (s_GetRaceSubMode() == 4) {
        return 0;
    }

    // Field +0x08: dword offset 2 from block-2 root. [DAT_00899f28 = 0x00899f20+8]
    const unsigned field_dword_offset = 2u;

    // Step 2: team-mode gate.
    if (GetDat0067ea64() == 0) {
        return *reinterpret_cast<std::int32_t*>(
            k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);
    }

    // Step 3: team-ID of caller.
    std::int32_t myTeam = *reinterpret_cast<std::int32_t*>(
        k_TeamIdRoot + static_cast<unsigned>(param_1) * k_TeamIdStride);

    // Seed with own value.
    std::int32_t iVar2 = *reinterpret_cast<std::int32_t*>(
        k_Block2Root + (k_Block2Stride * static_cast<unsigned>(param_1) + field_dword_offset) * 4u);

    // Accumulate matching team-mates.
    for (int k = 0; k < k_PlayerCount; ++k) {
        if (k == param_1) continue;
        std::int32_t kTeam = *reinterpret_cast<std::int32_t*>(
            k_TeamIdRoot + static_cast<unsigned>(k) * k_TeamIdStride);
        if (kTeam == myTeam) {
            iVar2 += *reinterpret_cast<std::int32_t*>(
                k_Block2Root + (k_Block2Stride * static_cast<unsigned>(k) + field_dword_offset) * 4u);
        }
    }
    return iVar2;
}

RH_ScopedInstall(PlayerBlock2Field08Get, 0x00424070);

// ---------------------------------------------------------------------------
// GetDat008994c0  --  0x004241b0
//
// Original: FUN_004241b0 (0x004241b0..0x004241b5; 6 bytes including RET)
// Signature: undefined4 FUN_004241b0(void)
//   no parameters
// Returns: *(uint32*)0x008994c0  [cited at 0x004241b1]
//
// Algorithm:
//   Single instruction: MOV EAX, [0x008994c0]; RET.
//   No branches, no calls, no writes.
//
// Constants cited:
//   0x008994c0 — sole memory access address  [0x004241b1]
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_004241b0.md
// ---------------------------------------------------------------------------

// 0x004241b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat008994c0()
{
    // Load and return DAT_008994c0.  [0x004241b1]
    return *reinterpret_cast<std::uint32_t*>(0x008994c0u);
}

RH_ScopedInstall(GetDat008994c0, 0x004241b0);
