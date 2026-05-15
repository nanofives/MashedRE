// Mashed RE -- timer subarray init loop.
// 0x00422120  sub_00422120  timer_d3_cont1_b-20260513  C2 -> C3 candidate
//
// Asm (28 bytes):
//   Iterates ptr from DAT_0063fb90 to DAT_006403b0 (exclusive),
//   stride 0x208 per iteration (4 iterations total: 0x820 / 0x208 = 4).
//   Calls FUN_00421c50() each pass. Callee implicitly uses or accesses
//   the current element; no explicit arg in decomp.
//
// Note: FUN_004222c0 (0x004222c0) is a confirmed thunk of this function
// (identical body, thunk=true in Ghidra).
//
// Callee FUN_00421c50 (0x00421c50): not yet reversed (C0). This loop
// wrapper calls the original via function pointer -- see below.
// Drift-promote: outer loop structure fully confirmed; callee opaque.
#include "../Core/HookSystem.h"

#include <cstdint>

// Loop constants -- all cited from 0x00422120 body.
static constexpr std::uintptr_t kLoopBase   = 0x0063fb90u; // loop start ptr
static constexpr std::uintptr_t kLoopLimit  = 0x006403b0u; // loop end (exclusive)
static constexpr std::uint32_t  kLoopStride = 0x00000208u; // 520 bytes per element

// FUN_00421c50 -- callee; C0, not yet reversed.
// Called once per iteration; no explicit args visible in decomp.
// We call the original implementation directly via its VA.
// NOTE: in this project, "RVA" values in hooks.csv are actual virtual addresses
// (MASHED.exe preferred base 0x00400000 = load address, so RVA == VA).
// 0x00421c50 is the VA of FUN_00421c50 in the running process.
static constexpr std::uintptr_t kFUN_00421c50_va = 0x00421c50u;

// 0x00422120
extern "C" __declspec(dllexport) void __cdecl TimerSubarrayInit() {
    typedef void (__cdecl *FUN_00421c50_t)();
    const auto fn_00421c50 = reinterpret_cast<FUN_00421c50_t>(kFUN_00421c50_va);

    // Loop: ptr = 0x0063fb90; ptr < 0x006403b0; ptr += 0x208
    for (std::uintptr_t ptr = kLoopBase; ptr < kLoopLimit; ptr += kLoopStride) {
        fn_00421c50();
    }
}

RH_ScopedInstall(TimerSubarrayInit, 0x00422120);
