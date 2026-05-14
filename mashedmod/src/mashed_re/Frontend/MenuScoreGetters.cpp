// Mashed RE - Frontend menu score / race-data getter reimplementations.
// Analysis notes:
//   re/analysis/frontend_promote_menus_b/00429a90.md   (LapSecsGetBySlot)
//   re/analysis/frontend_promote_menus_b/00430760.md   (IsMultiplayerMode)
//   re/analysis/frontend_promote_menus_b/0040b620.md   (SlotSortByModeScore)
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// LapSecsGetBySlot  --  0x00429a90
//
// Original: FUN_00429a90 (12 bytes, 0x00429a90..0x00429a9b)
// Returns: (&DAT_0067d994)[param_1] — indexed read of lap-time seconds component
//   by slot index.  Same pattern as FUN_00429870 (time-A seconds).
// No callees, no branches, no side-effects.
// ref: re/analysis/frontend_promote_menus_b/00429a90.md
// ---------------------------------------------------------------------------

// 0x00429a90
extern "C" __declspec(dllexport) std::uint32_t __cdecl LapSecsGetBySlot(int param_1) {
    return reinterpret_cast<std::uint32_t*>(0x0067d994u)[param_1];
}

RH_ScopedInstall(LapSecsGetBySlot, 0x00429a90);

// ---------------------------------------------------------------------------
// IsMultiplayerMode  --  0x00430760
//
// Original: FUN_00430760 (38 bytes, 0x00430760..0x00430786)
// Returns: 1 if DAT_0067e9fc (game mode global, cited at 0x00430764) is any of
//   {2, 3, 4, 5, 10}; else 0.
// Decomp: if (DAT_0067e9fc == 2 || DAT_0067e9fc == 10 || DAT_0067e9fc == 3
//              || DAT_0067e9fc == 4 || DAT_0067e9fc == 5) return 1; return 0;
// No callees, no side-effects.
// ref: re/analysis/frontend_promote_menus_b/00430760.md
// ---------------------------------------------------------------------------

// 0x00430760
extern "C" __declspec(dllexport) std::uint32_t __cdecl IsMultiplayerMode() {
    std::uint32_t mode = *reinterpret_cast<std::uint32_t*>(0x0067e9fcu);
    if (mode == 2u || mode == 10u || mode == 3u || mode == 4u || mode == 5u)
        return 1u;
    return 0u;
}

RH_ScopedInstall(IsMultiplayerMode, 0x00430760);

// ---------------------------------------------------------------------------
// SlotSortByModeScore  --  0x0040b620
//
// Original: FUN_0040b620 (135 bytes, 0x0040b620..0x0040b6a7)
// Signature: void FUN_0040b620(int* param_1)
//   param_1: pointer to int[4] output array; filled with slot indices sorted
//            descending by per-slot mode score.
// Globals:
//   DAT_008a9530  (+4 stride, 4 elements) — per-slot mode scores (0x0040b630)
//   DAT_007f1a14  (+0x10 stride, 4 elements) — slot activity flags (0x0040b638)
// Logic:
//   For each slot i (0..3):
//     score[i] = (DAT_007f1a14[i*4] == -1) ? -100 : DAT_008a9530[i*4]
//     output[i] = i
//   Bubble sort output[] descending by score[].
// No callees.
// ref: re/analysis/frontend_promote_menus_b/0040b620.md
// NOTE: arg_type 'void_out_ptr' (pass ptr, call, readback 4-element array) is
//   not supported by the Frida diff harness. This function is OMITTED from the
//   automated diff run.  Manual / behavioral verification only at C3 gate.
// ---------------------------------------------------------------------------

// 0x0040b620
extern "C" __declspec(dllexport) void __cdecl SlotSortByModeScore(int* param_1) {
    // Build local score and index arrays (4 slots).
    std::int32_t score[4];
    for (int i = 0; i < 4; i++) {
        // Activity flag: DAT_007f1a14 + i*0x10 (stride 0x10, int32 per slot).
        std::int32_t flag = *reinterpret_cast<std::int32_t*>(0x007f1a14u + i * 0x10);
        if (flag == -1) {
            score[i] = -100;
        } else {
            // Mode score: DAT_008a9530 + i*4 (stride 4, int32 per slot).
            score[i] = *reinterpret_cast<std::int32_t*>(0x008a9530u + i * 4);
        }
        // Fill output with identity permutation.
        param_1[i] = i;
    }

    // Bubble sort: descending by score.
    for (int pass = 0; pass < 4; pass++) {
        for (int j = 0; j < 3; j++) {
            if (score[param_1[j]] < score[param_1[j + 1]]) {
                int tmp = param_1[j];
                param_1[j] = param_1[j + 1];
                param_1[j + 1] = tmp;
            }
        }
    }
}

RH_ScopedInstall(SlotSortByModeScore, 0x0040b620);
