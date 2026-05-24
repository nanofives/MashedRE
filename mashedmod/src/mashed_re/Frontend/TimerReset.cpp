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

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TimerArrayClear, 0x00422b30);


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

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TimerGlobalsReset, 0x0040b810);


// ─── 0x0042af50  MenuReadinessCheck ───────────────────────────────────────────
// 156 bytes. Returns 1 if menu is "ready", 0 if not. Read-only; no writes.
//
// Guard (0x0042af50 entry):
//   if (DAT_0067eab0 != 2) && (DAT_0067e7c8 == 0) && (DAT_00898ab0 != 0): return 0.
//
// Branch on DAT_007f1a0c == 0x1000:
//   True path: scans char array B (base 0x007f1501, stride 0x4c), skipping
//     entries where charA=='\0' OR charB!='\0'. Returns 1 if found within
//     0x007f1501..0x007f1760, else 0.
//   False path: scans int index array (base 0x007f1a14, stride 0x10), checking
//     slot != -1, FUN_0040e470(iVar4)!=2, charA[slot*0x4c]!='\0', charB[slot*0x4c]=='\0'.
//     Returns 1 if found within 0x007f1a14..0x007f1a53, else 0.
//
// All constants cited from 0x0042af50 body.

// Callee: FUN_0040e470 — per-car state getter; called in false-path loop.
// VA 0x0040e470 cited at 0x0042af50 false-path body.
static auto* const s_FUN_0040e470 =
    reinterpret_cast<int(__cdecl*)(int)>(0x0040e470);

// Guard globals (0x0042af50 guard):
static constexpr std::uintptr_t kGuard_0067eab0 = 0x0067eab0u;
static constexpr std::uintptr_t kGuard_0067e7c8 = 0x0067e7c8u;
static constexpr std::uintptr_t kGuard_00898ab0 = 0x00898ab0u;
// Dispatch (0x0042af50 branch):
static constexpr std::uintptr_t kDispatch_007f1a0c = 0x007f1a0cu;
// True-path arrays (0x0042af50 true-path):
//   charB = DAT_007f1501, element stride 0x4c; upper bound ptr 0x7f1760.
//   charA = charB - 0x4c0 = DAT_007f1041.
static constexpr std::uintptr_t kCharB_base     = 0x007f1501u; // DAT_007f1501
static constexpr std::uintptr_t kTrueBound_ptr  = 0x7f1760u;   // upper bound (as int)
static constexpr std::uint32_t  kCharStride     = 0x4cu;       // 76 bytes per slot
// charA is 0x4c0 bytes before charB: DAT_007f1041 = DAT_007f1501 - 0x4c0.
// Accessed via pcVar1 pointer offset in code below.
// False-path index array (0x0042af50 false-path):
//   piVar3 base = DAT_007f1a14, element stride 0x10 (int), upper bound 0x7f1a53.
static constexpr std::uintptr_t kIdxArray_base  = 0x007f1a14u; // DAT_007f1a14
static constexpr std::uint32_t  kIdxStride      = 0x10u;       // bytes between index entries
static constexpr std::uintptr_t kFalseBound_ptr = 0x7f1a53u;   // upper bound (as int)

// 0x0042af50
extern "C" __declspec(dllexport) int __cdecl MenuReadinessCheck() {
    const std::uint32_t guard_a = *reinterpret_cast<const std::uint32_t*>(kGuard_0067eab0);
    const std::uint32_t guard_b = *reinterpret_cast<const std::uint32_t*>(kGuard_0067e7c8);
    const std::uint32_t guard_c = *reinterpret_cast<const std::uint32_t*>(kGuard_00898ab0);

    // Guard: if (DAT_0067eab0 != 2) && (DAT_0067e7c8 == 0) && (DAT_00898ab0 != 0): return 0.
    if ((guard_a != 2u) && (guard_b == 0u) && (guard_c != 0u)) {
        return 0;
    }

    const std::uint32_t dispatch = *reinterpret_cast<const std::uint32_t*>(kDispatch_007f1a0c);

    if (dispatch == 0x1000u) {
        // True path: scan char array B starting at 0x007f1501, stride 0x4c.
        // Skip while: charA == '\0' OR charB != '\0'.
        // charA is at (pcVar1 - 0x4c0).
        const char* pcVar1 = reinterpret_cast<const char*>(kCharB_base);
        // charA at (pcVar1 - 0x4c0) — same as DAT_007f1041 + same-slot offset.
        while ((*(pcVar1 - 0x4c0) == '\0') || (*pcVar1 != '\0')) {
            pcVar1 += kCharStride;
            if (reinterpret_cast<std::uintptr_t>(pcVar1) > kTrueBound_ptr) {
                return 0;
            }
        }
        return 1;
    } else {
        // False path: scan int index array at 0x007f1a14, stride 0x10.
        // Skip while: *piVar3 == -1 OR FUN_0040e470(iVar4)==2
        //             OR charA[*piVar3 * 0x4c]=='\0' OR charB[*piVar3 * 0x4c]!='\0'.
        // charA base = 0x007f1041 = kCharB_base - 0x4c0; computed as uintptr_t subtraction.
        const char* charA = reinterpret_cast<const char*>(kCharB_base - 0x4c0u);
        const char* charB = reinterpret_cast<const char*>(kCharB_base);
        const std::int32_t* piVar3 = reinterpret_cast<const std::int32_t*>(kIdxArray_base);
        int iVar4 = 0;
        while ((*piVar3 == -1) ||
               (s_FUN_0040e470(iVar4) == 2) ||
               (charA[(*piVar3) * static_cast<int>(kCharStride)] == '\0') ||
               (charB[(*piVar3) * static_cast<int>(kCharStride)] != '\0')) {
            piVar3 = reinterpret_cast<const std::int32_t*>(
                reinterpret_cast<const std::uint8_t*>(piVar3) + kIdxStride);
            iVar4 += 1;
            if (reinterpret_cast<std::uintptr_t>(piVar3) > kFalseBound_ptr) {
                return 0;
            }
        }
        return 1;
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(MenuReadinessCheck, 0x0042af50);
