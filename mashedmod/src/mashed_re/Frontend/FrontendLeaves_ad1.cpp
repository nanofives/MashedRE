// Mashed RE - Frontend leaves, c3-batch-ad session 1 (HARVEST).
//
// Candidate authored:
//   0x004241c0  TeamBlockOneGet  — team-aggregating block-1 getter (int_scalar)
//
// Skipped this session (recorded in re/SCRIBE_QUEUE / promotion notes, not diffable
// as clean leaves):
//   0x00428140  sprite-draw w/ alpha-fade — live D3D state + indirect call + global write
//   0x004288a0  fixed menu-layout renderer — sequential draw calls, no scalar observable
//   0x00428a30  title-screen draw — branches/timer/_strrchr/sprintf, live state
//   0x00428bf0  alternate title-screen draw — live state
//   0x00423b00  29B guard+4-call menu-input dispatcher — no observable; not a leaf
//   0x00424270  14-case rank-sort dispatcher — 16 callees, needs populated 4-player
//               state + array-out arg_type; would diff only degenerately at menu
//   0x00424b80  end-of-match stats flush + variadic debug printf — global mutation
//   0x00425b90  RpClumpForAllAtomics wrapper — requires live RpClump
//   0x00425bf0  perm.piz preload one-liner — real file-IO side effect, no observable
//   0x00425c00  copter asset registration — asset load + array write side effects
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis note:
//   re/analysis/frontend_c1_to_c2_s3/FUN_004241c0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// TeamBlockOneGet  --  0x004241c0
//
// Original: FUN_004241c0 (0x004241c0..0x0042426b)
// Signature: int FUN_004241c0(int param_1)
//   param_1: player slot index 0..3
//   return: int — block-1 stat field for param_1, plus team-partner contributions
//           when team mode is active (partners with active-slot != -1 only)
//
// Exact structural twin of FUN_00424100 (TeamBlockZeroGet, C3-GREEN). Differs
// ONLY in the block base address: block-1 root DAT_00899e80 vs block-0 root
// DAT_008999a0. Same team-ID array (0x007f1a18), same active-slot guards
// (0x007f1a14/24/34/44), same stride 0x4e int32s, same -1 active guard.
//
// Control flow (from analysis note):
//   1. Call FUN_0042f500(); if 0: return (&DAT_00899e80)[param_1 * 0x4e] directly.
//   2. Load team-ID of param_1: (&DAT_007f1a18)[param_1 * 4].
//   3. Seed iVar2 = (&DAT_00899e80)[param_1 * 0x4e].
//   4. For each partner slot k in {0,1,2,3}: add (&DAT_00899e80)[k * 0x4e] only if
//      (team-ID of k == iVar1) AND (k != param_1) AND (active-slot of k != -1).
//   5. Return iVar2.
//
// Constants (cited from analysis note):
//   0x00899e80 — block-1 root, stride 0x4e int32s per player
//   0x007f1a18 — team-ID array for player 0; index param_1 * 4
//   0x007f1a14 — active-slot for player 0
//   0x007f1a24 — active-slot for player 1
//   0x007f1a34 — active-slot for player 2
//   0x007f1a44 — active-slot for player 3
//   0xFFFFFFFF (-1) — sentinel for inactive/absent player
//
// Callee: FUN_0042f500 (0x0042f500) — team-mode flag getter; C4.
// Caller: FUN_0042c960 (0x0042c960) — C2.
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_004241c0.md
// ---------------------------------------------------------------------------

// Team-mode flag getter 0x0042f500 is our ported C4 export GetDat0067ea64
// (GameState/StateAccessors.cpp) -- call the port directly rather than
// trampolining through the original image.
extern "C" std::uint32_t __cdecl GetDat0067ea64();   // 0x0042f500

// 0x004241c0
extern "C" __declspec(dllexport) std::int32_t __cdecl TeamBlockOneGet(std::int32_t param_1)
{
    // Step 1: if not team mode, return block-1 field directly. [0x004241c0]
    if (GetDat0067ea64() == 0) {
        // (&DAT_00899e80)[param_1 * 0x4e] — stride 0x4e int32s.
        return reinterpret_cast<const std::int32_t*>(0x00899e80u)[param_1 * 0x4e];
    }

    // Step 2: load team-ID of param_1. [0x004241c0 + team-id load]
    const std::int32_t iVar1 =
        reinterpret_cast<const std::int32_t*>(0x007f1a18u)[param_1 * 4];

    // Step 3: seed accumulator with param_1's own block-1 field.
    std::int32_t iVar2 =
        reinterpret_cast<const std::int32_t*>(0x00899e80u)[param_1 * 0x4e];

    // Active-slot array: 4 separate globals (not strided uniformly).
    static const std::uintptr_t kActiveSlots[4] = {
        0x007f1a14u, 0x007f1a24u, 0x007f1a34u, 0x007f1a44u
    };
    static const std::uintptr_t kTeamIdBase = 0x007f1a18u;

    // Step 4: accumulate partner contributions.
    for (std::int32_t k = 0; k < 4; ++k) {
        if (k == param_1) continue;
        const std::int32_t partnerTeamId =
            reinterpret_cast<const std::int32_t*>(kTeamIdBase)[k * 4];
        if (partnerTeamId != iVar1) continue;
        const std::int32_t partnerActiveSlot =
            *reinterpret_cast<const std::int32_t*>(kActiveSlots[k]);
        if (partnerActiveSlot == -1) continue;
        iVar2 += reinterpret_cast<const std::int32_t*>(0x00899e80u)[k * 0x4e];
    }

    // Step 5: return accumulated value.
    return iVar2;
}

RH_ScopedInstall(TeamBlockOneGet, 0x004241c0);
