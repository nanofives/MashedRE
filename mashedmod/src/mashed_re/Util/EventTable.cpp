// Mashed RE — event-record table accessors.
// 0x0041f1c0  FUN_0041f1c0  util_c0_promote  C2
// 0x0041f090  FUN_0041f090  util_c0_promote  C2
#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041f1c0 — pure table lookup from event-record table.
//
// Table at 0x0063d9e0: flat array of uint32 entries.
// Layout: per-player row stride = 0xab (171) entries; each entry = 4 bytes.
// Callers treat non-zero return as "event active".
//
// Formula (23-byte body):
//   return *(uint32*)(0x0063d9e0 + (param_1 * 0xab + param_2) * 4)
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetEventFlag(int param_1, int param_2) {
    const auto* table = reinterpret_cast<const std::uint32_t*>(0x0063d9e0);
    return table[param_1 * 0xab + param_2];
}

// 0x0041f090 — extracts bit 4 and bit 5 from per-player state word.
//
// State array base: 0x0063dc74; stride 0x2ac (684) bytes per player.
// param_2 (nullable) receives (state_word & 0x10); bit 4.
// param_3 (nullable) receives (state_word & 0x20); bit 5.
// Callers: FUN_00424eb0, FUN_00412f30.
extern "C" __declspec(dllexport) void __cdecl GetPlayerStateBits(
        int            param_1,
        std::uint32_t* param_2,
        std::uint32_t* param_3) {
    const auto word = *reinterpret_cast<const std::uint32_t*>(
        reinterpret_cast<const std::uint8_t*>(0x0063dc74) + param_1 * 0x2ac);
    if (param_2) *param_2 = word & 0x10u;
    if (param_3) *param_3 = word & 0x20u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(GetEventFlag,       0x0041f1c0);
RH_ScopedInstall(GetPlayerStateBits, 0x0041f090);  // re-enabled 2026-05-24 c3-safe
