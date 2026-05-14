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
//           RET
#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0040b6b0
// Returns DAT_008a9530[param_1] — per-slot mode-score array element.
extern "C" __declspec(dllexport) std::uint32_t __cdecl ModeScoreGetBySlot(std::uint32_t param_1) {
    return reinterpret_cast<std::uint32_t*>(0x008a9530)[param_1];
}
RH_ScopedInstall(ModeScoreGetBySlot, 0x0040b6b0);

// 0x0040b7a0
// Returns DAT_0063b8ec — hotkey string base global.
extern "C" __declspec(dllexport) std::uint32_t __cdecl HotkeyStringBaseGet() {
    return *reinterpret_cast<std::uint32_t*>(0x0063b8ec);
}
RH_ScopedInstall(HotkeyStringBaseGet, 0x0040b7a0);

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
RH_ScopedInstall(PlayerHotkeyTableGet, 0x0040b7b0);

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
RH_ScopedInstall(LapTimeALessThanB, 0x00429870);

// 0x00429a70
// Returns (float)DAT_0067d99c[param_1] — indexed read of lap frac array (float).
extern "C" __declspec(dllexport) float __cdecl LapFracGetBySlot(std::uint32_t param_1) {
    return reinterpret_cast<float*>(0x0067d99c)[param_1];
}
RH_ScopedInstall(LapFracGetBySlot, 0x00429a70);

// 0x00429a80
// Returns DAT_0067d98c[param_1] — indexed read of lap laps array.
extern "C" __declspec(dllexport) std::uint32_t __cdecl LapLapsGetBySlot(std::uint32_t param_1) {
    return reinterpret_cast<std::uint32_t*>(0x0067d98c)[param_1];
}
RH_ScopedInstall(LapLapsGetBySlot, 0x00429a80);
