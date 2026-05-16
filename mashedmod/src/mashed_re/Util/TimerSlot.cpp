// Mashed RE — Timer slot helpers: slot data copy, slot bit-set, slot array clear.
// Session: c3-batch-e-s13 (2026-05-15)
//
// Drift-promote (C1->C2):
//   0x004b6520  FUN_004b6520  ZeroFillWrapper
//     20-byte wrapper: FUN_004b64e0(param_1, 0, param_2)
//     FUN_004b64e0 is a word-at-a-time memset(dest, fill, count).
//     Entire wrapper equivalent to: memset(param_1, 0, param_2).
//
// C2 candidates:
//   0x0041f000  FUN_0041f000  SlotDataCopy
//     Source = *(int*)0x0063dc10 + param_1 * 0x2ac.
//     Copies 6 dwords from source to *param_2.
//
//   0x0041eda0  FUN_0041eda0  SlotBitSet
//     target = 0x0063dc74 + param_1 * 0x2ac.
//     if param_2 != 0: *target |= 0x8; else: *target &= ~0x8.
//
//   0x00420d40  FUN_00420d40  SlotArrayClear
//     6-iter loop over [0x0063e4c4, 0x0063e554) stride 0x24.
//     Each iter: ZeroFillWrapper(ptr-0xc, 8); ptr[-1]=0; *ptr=0; ptr[1]=0; ptr[2]=0.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
// Drift-promote: 0x004b6520  ZeroFillWrapper
// ─────────────────────────────────────────────────────────────────────────────
// Wraps FUN_004b64e0(param_1, 0, param_2) — zero-fills param_2 bytes at param_1.
// Analysis note: re/analysis/input_lua_d2/0x004b6520.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl ZeroFillWrapper(void* param_1, unsigned int param_2) {
    std::memset(param_1, 0, param_2);
}

RH_ScopedInstall(ZeroFillWrapper, 0x004b6520);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041f000  SlotDataCopy  void(int param_1, int* param_2)
// ─────────────────────────────────────────────────────────────────────────────
// Source ptr: 0x0063dc10 + param_1 * 0x2ac (direct address arithmetic, no deref)
// Copies 6 dwords from source ptr to *param_2.
// Analysis note: re/analysis/timer_d3_cont1_b/0x0041f000.md
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kSlotDataBase = 0x0063dc10;  // 0x0041f000 body
static constexpr int            kSlotStride   = 0x2ac;       // 0x0041f000 body
static constexpr int            kSlotDwords   = 6;           // 0x0041f000 body

extern "C" __declspec(dllexport) void __cdecl SlotDataCopy(int param_1, int* param_2) {
    const int* src = reinterpret_cast<const int*>(
        kSlotDataBase +
        static_cast<std::uintptr_t>(param_1) * static_cast<std::uintptr_t>(kSlotStride));
    for (int i = 0; i < kSlotDwords; i++) {
        param_2[i] = src[i];
    }
}

RH_ScopedInstall(SlotDataCopy, 0x0041f000);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041eda0  SlotBitSet  void(int param_1, int param_2)
// ─────────────────────────────────────────────────────────────────────────────
// target = 0x0063dc74 + param_1 * 0x2ac
// if param_2 != 0: *target |= 0x8
// else:            *target &= ~0x8
// Analysis note: re/analysis/timer_d3_cont1_b/0x0041eda0.md
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kSlotBitBase = 0x0063dc74;   // 0x0041eda0 body

extern "C" __declspec(dllexport) void __cdecl SlotBitSet(int param_1, int param_2) {
    int* target = reinterpret_cast<int*>(
        kSlotBitBase + static_cast<std::uintptr_t>(param_1) * kSlotStride);
    if (param_2 != 0) {
        *target |= 0x8;
    } else {
        *target &= ~0x8;
    }
}

RH_ScopedInstall(SlotBitSet, 0x0041eda0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00420d40  SlotArrayClear  void(void)
// ─────────────────────────────────────────────────────────────────────────────
// 6-iter loop: ptr starts at 0x0063e4c4, stride 0x24, bound 0x0063e554.
// Per iter: ZeroFillWrapper(ptr-0xc, 8); ptr[-1]=0; *ptr=0; ptr[1]=0; ptr[2]=0.
// Analysis note: re/analysis/timer_d3_cont1_b/0x00420d40.md
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kSlotArrayBase  = 0x0063e4c4;  // 0x00420d40 body
static constexpr std::uintptr_t kSlotArrayBound = 0x0063e554;  // 0x00420d40 body
static constexpr std::size_t    kSlotArrayStride = 0x24;        // 0x00420d40 body

extern "C" __declspec(dllexport) void __cdecl SlotArrayClear() {
    unsigned int* puVar1 = reinterpret_cast<unsigned int*>(kSlotArrayBase);
    while (puVar1 < reinterpret_cast<unsigned int*>(kSlotArrayBound)) {
        // ZeroFillWrapper(puVar1 - 0xc, 8): zero 8 bytes at ptr-12
        std::memset(reinterpret_cast<std::uint8_t*>(puVar1) - 0xc, 0, 8);
        puVar1[-1] = 0u;  // offset -4
        puVar1[ 0] = 0u;  // offset  0
        puVar1[ 1] = 0u;  // offset +4
        puVar1[ 2] = 0u;  // offset +8
        // advance by stride (0x24 bytes = 9 dwords)
        puVar1 = reinterpret_cast<unsigned int*>(
            reinterpret_cast<std::uint8_t*>(puVar1) + kSlotArrayStride);
    }
}

RH_ScopedInstall(SlotArrayClear, 0x00420d40);
