// Mashed RE — Frontend menu-score and lap-data getters.
// Six trivial pure-leaf getters from the Frontend subsystem.
// All are indexed reads or direct global dereferences with no callees.
// Per the NO-GUESSING rule, names are semantically grounded in the
// Ghidra decomp literal — no inferred intent beyond what the disasm shows.
//
// 0x0040b6b0  FUN_0040b6b0  12B  c3-batch-b-s1  C2 -> C3
//   Disasm: MOV EAX, [DAT_008a9530 + param_1*4]   ; indexed read, mode-score array
//           RET
//
// 0x0040b7a0  FUN_0040b7a0  6B   c3-batch-b-s1  C2 -> C3
//   Disasm: A1 EC B8 63 00   MOV EAX, [0x0063b8ec]
//           C3               RET
//
// 0x0040b7b0  FUN_0040b7b0  66B  c3-batch-b-s1  C2 -> C3
//   Disasm: switch(param_2) { 0: DAT_008a9530[param_1]; 1: DAT_008a9560[param_1];
//                              2: DAT_008a9540[param_1]; 3: DAT_008a9550[param_1]; default: 0 }
//
// 0x00429870  FUN_00429870  79B  c3-batch-b-s1  C2 -> C3
//   Disasm: time_A = DAT_0067d98c*0x3c + DAT_0067d994 + _DAT_0067d99c
//           time_B = DAT_0067d990*0x3c + DAT_0067d998 + _DAT_0067d9a0
//           return (time_A < time_B) ? 1 : 0
//
// 0x00429a70  FUN_00429a70  12B  c3-batch-b-s1  C2 -> C3
//   Disasm: FLD  [DAT_0067d99c + param_1*4]   ; indexed float read, frac array
//           FSTP [esp-4] ; return float
//           RET
//
// 0x00429a80  FUN_00429a80  12B  c3-batch-b-s1  C2 -> C3
//   Disasm: MOV EAX, [DAT_0067d98c + param_1*4]   ; indexed read, laps array
//           RET// Mashed RE - Frontend menu score / race-data getter reimplementations.
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

// 0x0040b6b0
// Returns DAT_008a9530[param_1] — per-slot mode-score array element.
extern "C" __declspec(dllexport) std::uint32_t __cdecl ModeScoreGetBySlot(std::uint32_t param_1) {
    return reinterpret_cast<std::uint32_t*>(0x008a9530)[param_1];
}
RH_ScopedInstall(ModeScoreGetBySlot, 0x0040b6b0);  // re-enabled 2026-05-24 c3-frontend-a

// 0x0040b7a0
// Returns DAT_0063b8ec — hotkey string base global.
extern "C" __declspec(dllexport) std::uint32_t __cdecl HotkeyStringBaseGet() {
    return *reinterpret_cast<std::uint32_t*>(0x0063b8ec);
}
RH_ScopedInstall(HotkeyStringBaseGet, 0x0040b7a0);  // re-enabled 2026-05-24 batch-frontend

// 0x0040b7b0
// 4-table dispatch: returns one of four per-player arrays indexed by param_1,
// selected by param_2. Returns 0 for out-of-range param_2.
extern "C" __declspec(dllexport) std::uint32_t __cdecl PlayerHotkeyTableGet(std::uint32_t param_1, std::uint32_t param_2) {
    switch (param_2) {
        case 0: return reinterpret_cast<std::uint32_t*>(0x008a9530)[param_1];   // 0x0040b7c4
        case 1: return reinterpret_cast<std::uint32_t*>(0x008a9560)[param_1];   // 0x0040b7cc
        case 2: return reinterpret_cast<std::uint32_t*>(0x008a9540)[param_1];   // 0x0040b7da
        case 3: return reinterpret_cast<std::uint32_t*>(0x008a9550)[param_1];   // 0x0040b7e8
        default: return 0;
    }
}
RH_ScopedInstall(PlayerHotkeyTableGet, 0x0040b7b0);  // re-enabled 2026-05-24 c3-frontend-a

// 0x00429870
// Lap time comparison: returns 1 if time_A < time_B, else 0.
// time = laps*0x3c + secs + frac (float).
extern "C" __declspec(dllexport) std::uint32_t __cdecl LapTimeALessThanB() {
    std::uint32_t laps_a = *reinterpret_cast<std::uint32_t*>(0x0067d98c);   // 0x00429878
    std::uint32_t secs_a = *reinterpret_cast<std::uint32_t*>(0x0067d994);   // 0x00429883
    float         frac_a = *reinterpret_cast<float*>(0x0067d99c);           // 0x0042988e
    std::uint32_t laps_b = *reinterpret_cast<std::uint32_t*>(0x0067d990);   // 0x00429898
    std::uint32_t secs_b = *reinterpret_cast<std::uint32_t*>(0x0067d998);   // 0x004298a3
    float         frac_b = *reinterpret_cast<float*>(0x0067d9a0);           // 0x004298ae

    float time_a = static_cast<float>(laps_a * 0x3c + secs_a) + frac_a;
    float time_b = static_cast<float>(laps_b * 0x3c + secs_b) + frac_b;
    return (time_a < time_b) ? 1u : 0u;
}
RH_ScopedInstall(LapTimeALessThanB, 0x00429870);  // re-enabled 2026-05-24 c3-frontend-a

// 0x00429a70
// Returns (float)DAT_0067d99c[param_1] — indexed read of lap frac array (float).
extern "C" __declspec(dllexport) float __cdecl LapFracGetBySlot(std::uint32_t param_1) {
    return reinterpret_cast<float*>(0x0067d99c)[param_1];
}
RH_ScopedInstall(LapFracGetBySlot, 0x00429a70);  // re-enabled 2026-05-24 c3-frontend-a

// 0x00429a80
// Returns DAT_0067d98c[param_1] — indexed read of lap laps array.
extern "C" __declspec(dllexport) std::uint32_t __cdecl LapLapsGetBySlot(std::uint32_t param_1) {
    return reinterpret_cast<std::uint32_t*>(0x0067d98c)[param_1];
}
RH_ScopedInstall(LapLapsGetBySlot, 0x00429a80);  // re-enabled 2026-05-24 c3-frontend-a// ---------------------------------------------------------------------------
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

RH_ScopedInstall(LapSecsGetBySlot, 0x00429a90);  // re-enabled 2026-05-24 c3-frontend-a

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

RH_ScopedInstall(IsMultiplayerMode, 0x00430760);  // re-enabled 2026-05-24 c3-frontend-b

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

RH_ScopedInstall(SlotSortByModeScore, 0x0040b620);  // re-enabled 2026-06-12 promote-round-19 (outbuf_only harness-ext)