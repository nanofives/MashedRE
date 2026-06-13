// Mashed RE — promote-round round 41 (global-field getter cluster @0x0063d7e4).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified `A1 E4 D7 63 00 8B 40 <off> C3`
//   mov eax,[0x0063d7e4]; mov eax,[eax+off]; ret
//   => return *(u32*)(*(u32*)0x0063d7e4 + off)
// 10-field accessor cluster reading the struct pointed to by global 0x0063d7e4.
// Diffed via early_window_leaf_diff global_field_read (point the global at a seeded
// buffer; the test value lands at +off, distinct per test -> non-degenerate). All
// 10 callers C2+ (FUN_004270f0/00426030/00426b40/00426640/00426c50/00426c90 C2;
// TimerDispatch10/30/70 0x00426c10/c30/c70 C4).

#include "../Core/HookSystem.h"

#include <cstdint>

#define GF63D7E4(NAME, OFF, RVA) \
    extern "C" __declspec(dllexport) std::uint32_t __cdecl NAME(void) { \
        return *reinterpret_cast<const std::uint32_t*>( \
            *reinterpret_cast<const std::uintptr_t*>(0x0063d7e4u) + (OFF)); /* cited at RVA */ \
    } \
    RH_ScopedInstall(NAME, RVA);

GF63D7E4(GlobalField63d7e4_1c, 0x1cu, 0x0041e9f0)
GF63D7E4(GlobalField63d7e4_20, 0x20u, 0x0041ea00)
GF63D7E4(GlobalField63d7e4_24, 0x24u, 0x0041ea10)
GF63D7E4(GlobalField63d7e4_28, 0x28u, 0x0041ea20)
GF63D7E4(GlobalField63d7e4_2c, 0x2cu, 0x0041ea30)
GF63D7E4(GlobalField63d7e4_30, 0x30u, 0x0041ea40)
GF63D7E4(GlobalField63d7e4_34, 0x34u, 0x0041ea50)
GF63D7E4(GlobalField63d7e4_38, 0x38u, 0x0041ea60)
GF63D7E4(GlobalField63d7e4_3c, 0x3cu, 0x0041ea70)
GF63D7E4(GlobalField63d7e4_40, 0x40u, 0x0041ea80)
