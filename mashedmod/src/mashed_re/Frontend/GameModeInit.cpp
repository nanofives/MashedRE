// Mashed RE — Game-mode struct initialisation helpers.
// C2->C3 promotions from the game_mode_cont2 cluster.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046dc00  EntityFieldSet
//
// Writes param_2 to field [+0x00] of element param_1 in the entity array
// at 0x008815a8.  Stride = 0x341 DWORDs = 0xD04 bytes per element.
//
// Ghidra decomp at 0x0046dc00 (20 bytes, pure leaf):
//   (&DAT_008815a8)[param_1 * 0x341] = param_2;
//
// Byte formula: *(uint32*)(0x008815a8 + param_1 * 0xD04) = param_2.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kEntityBase_8815a8 = 0x008815a8u;
static constexpr std::uint32_t  kEntityDWordStride  = 0x341u; // = 0xD04 / 4

extern "C" __declspec(dllexport) void __cdecl EntityFieldSet(int param_1, std::uint32_t param_2) {
    reinterpret_cast<std::uint32_t*>(kEntityBase_8815a8)[param_1 * kEntityDWordStride] = param_2;
}
RH_ScopedInstall(EntityFieldSet, 0x0046dc00);  // re-enabled 2026-05-24 c3-frontend-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x00492340  CarSlotInit
//
// Conditional struct-field initialiser.  Reads guard field at element
// [param_1][+4] of the player-entry array at 0x007F1058 (stride 0x4C bytes).
// If guard != 0: writes 1/3/10/0xFF to fields +0x00/+0x10/+0x0C/+0x14.
//
// Ghidra decomp at 0x00492340 (46 bytes, pure leaf):
//   int iVar1 = param_1 * 0x4c;
//   if ((&DAT_007f105c)[param_1 * 0x13] != 0) {   // guard: element[+4]
//     *(uint*)(0x7f1058 + iVar1)        = 1;       // [+0x00] active/enable
//     *(uint*)(0x7f1068 + iVar1)        = 3;       // [+0x10] mode or state
//     *(uint*)(0x7f1064 + iVar1)        = 10;      // [+0x0c] count or timer
//     *(uint*)(0x7f106c + iVar1)        = 0xff;    // [+0x14] sentinel
//   }
//
// Address math:
//   0x7F105C = 0x7F1058 + 4  → guard is element[+4]
//   param_1 * 0x13 DWORDs   = param_1 * 76 bytes = param_1 * 0x4C bytes from 0x7F105C
//   0x7F1068 = 0x7F1058 + 0x10  (+0x10 field)
//   0x7F1064 = 0x7F1058 + 0x0C  (+0x0C field)
//   0x7F106C = 0x7F1058 + 0x14  (+0x14 field)
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kPlayerBase_7f1058 = 0x007f1058u; // array base
static constexpr std::uintptr_t kPlayerGuard_7f105c = 0x007f105cu; // guard field (base+4)
static constexpr std::uint32_t  kPlayerByteStride   = 0x4cu;       // 76 bytes/element
static constexpr std::uint32_t  kPlayerDWordStride   = 0x13u;      // 19 DWORDs = 76 bytes

extern "C" __declspec(dllexport) void __cdecl CarSlotInit(int param_1) {
    const int iVar1 = param_1 * static_cast<int>(kPlayerByteStride);

    // Guard: element[param_1][+4] must be non-zero.
    if (reinterpret_cast<const std::uint32_t*>(kPlayerGuard_7f105c)[param_1 * kPlayerDWordStride] != 0u) {
        *reinterpret_cast<std::uint32_t*>(kPlayerBase_7f1058 + iVar1)        = 1u;
        *reinterpret_cast<std::uint32_t*>(kPlayerBase_7f1058 + iVar1 + 0x10) = 3u;
        *reinterpret_cast<std::uint32_t*>(kPlayerBase_7f1058 + iVar1 + 0x0c) = 10u;
        *reinterpret_cast<std::uint32_t*>(kPlayerBase_7f1058 + iVar1 + 0x14) = 0xffu;
    }
}
RH_ScopedInstall(CarSlotInit, 0x00492340);  // re-enabled 2026-05-24 c3-frontend-b
