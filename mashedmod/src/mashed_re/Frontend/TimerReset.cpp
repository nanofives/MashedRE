// Mashed RE - Timer reset / global-clear reimplementations.
// timer_d2_cont1 cluster — all pure leaves, no callees, no branches outside loops.
//
// RVA 0x00422b30  FUN_00422b30  "TimerArrayClear"
//   16 bytes. Loops 0x138 (312) times, writing 0 to successive dwords starting
//   at DAT_00899e80. Equivalent to memset(0x00899e80, 0, 312 * 4 = 1248 bytes).
//   No calls, no branches outside loop.
//
// RVA 0x0040b810  FUN_0040b810  "TimerGlobalsReset"
//   124 bytes. Writes 0 to 21 explicit global addresses across three address
//   ranges (0x008a94f0..0x008a94fc, 0x008a9530..0x008a953c, 0x008a9540..0x008a956c)
//   plus one isolated write at 0x0063b8ec. Decomp write order reproduced exactly.
//   No calls, no branches.

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ─── 0x00422b30  TimerArrayClear ──────────────────────────────────────────────
// DAT_00899e80: start of 312-dword (1248-byte) block zeroed on each call.
// Loop count 0x138 = 312 confirmed at 0x00422b30 body.
static constexpr std::uintptr_t kTimerArray = 0x00899e80u;

// 0x00422b30
extern "C" __declspec(dllexport) void __cdecl TimerArrayClear() {
    std::memset(reinterpret_cast<void*>(kTimerArray), 0, 312u * sizeof(std::uint32_t));
}

RH_ScopedInstall(TimerArrayClear, 0x00422b30);  // re-enabled 2026-05-24 c3-frontend-a


// ─── 0x0040b810  TimerGlobalsReset ────────────────────────────────────────────
// 21 explicit dword writes in exact decomp order. Addresses cited from
// 0x0040b810 body. Three address ranges plus one isolated write.

// Writes a single uint32_t zero to a raw MASHED.exe global address.
static inline void zero32(std::uintptr_t addr) {
    *reinterpret_cast<volatile std::uint32_t*>(addr) = 0u;
}

// 0x0040b810
extern "C" __declspec(dllexport) void __cdecl TimerGlobalsReset() {
    // Range A: 0x008a94f0..0x008a94fc (4 dwords)
    // Range B: 0x008a9530..0x008a953c (4 dwords)
    // Range C: 0x008a9540..0x008a956c (13 dwords)
    // Isolated: 0x0063b8ec
    //
    // Decomp write order (interleaved — reproduced exactly from 0x0040b810 body):
    zero32(0x008a9550u);
    zero32(0x008a9540u);
    zero32(0x008a9554u);
    zero32(0x008a9544u);
    zero32(0x008a9558u);
    zero32(0x008a9548u);
    zero32(0x008a955cu);
    zero32(0x008a954cu);
    zero32(0x008a9560u);
    zero32(0x008a9530u);
    zero32(0x008a94f0u);
    zero32(0x008a9564u);
    zero32(0x008a9534u);
    zero32(0x008a94f4u);
    zero32(0x008a9568u);
    zero32(0x008a9538u);
    zero32(0x008a94f8u);
    zero32(0x008a956cu);
    zero32(0x008a953cu);
    zero32(0x008a94fcu);
    zero32(0x0063b8ecu);
}

RH_ScopedInstall(TimerGlobalsReset, 0x0040b810);  // re-enabled 2026-05-24 c3-frontend-a


// ---------------------------------------------------------------------------
// FUN_0042af50 (menu readiness check) was ported here first as MenuReadinessCheck; its
// one-body-per-RVA copy is MenuReadinessCheckC in Frontend/MenuStateMachine.cpp, the
// body hooks.csv's evidence cites (diff_menu_readiness_check_c.csv). Both bodies read
// identically on both paths (guard, 0x1000 dispatch, 0x007f1501/0x007f1041 char tables
// at stride 0x4c, 0x007f1a14 index array, 0x0040e470 callee). The duplicate that sat
// here was removed 2026-10-16 (dupinstall).
// ---------------------------------------------------------------------------
