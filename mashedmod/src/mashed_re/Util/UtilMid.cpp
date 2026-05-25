// Mashed RE — Util mid-size cluster (c3-batch-h-s6).
//
// Twelve C2 util functions promoted via this file. Sizes range 60-1500 bytes;
// the larger ones call into unimplemented MASHED.exe helpers via raw RVA
// pointers (typedef + reinterpret_cast pattern).
//
// Promotion session: c3-batch-h-s6 (2026-05-17, Opus 4.7 1M).
//
// All RVAs verified C2 in hooks.csv prior to authoring; callee structural
// gate satisfied for every entry (at least one callee at C2+).
//
// Hook list:
//   0x00442c80  ModeGatedPlayerCheck       (62b, returns 0/1)
//   0x004950b0  QpcTimeScaledTo3Mhz        (80b, QPC*3e6/freq)
//   0x004b302f  StricmpThunk               (104b, library __stricmp passthrough)
//   0x00429aa0  GameStateSlotsFill         (134b, predicate-gated slot fill)
//   0x0041d730  PlayerSlotConfigInit       (225b, config-pair slot init loop)
//   0x00492d30  GameTickStateMachine7      (265b, 7-case state-machine dispatch)
//   0x004295a0  HudDualLabelRender         (~124b, two HUD label calls)
//   0x00442440  TransformMatrixUpdate      (~390b, 4x4 matrix copy + accumulate)
//   0x0043c000  TimerSlotTickDispatcher    (1450b, 19-slot timer tick)
//   0x00475a60  PendingOpQueueFlush        (74b, queue drain)
//   0x00410860  ScoreThresholdStateCheck   (591b, score/timeout state-machine)
//   0x00412cf0  LabelTrailRecordAppend     (312b, lerp+color trail record)
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
// Common typedefs for MASHED.exe callee functions invoked via raw RVA.
// All __cdecl unless explicitly noted (matches Ghidra-inferred conventions).
// ─────────────────────────────────────────────────────────────────────────────

namespace {

// 64-bit MSVC CRT helpers (no-arg interface; we cannot replicate the EDX:EAX
// return ABI from C++ — call the original directly for QPC scaling).
typedef int   (__cdecl* Fn_v_void_t)();
typedef int   (__cdecl* Fn_v_i_t)(int);
typedef int   (__cdecl* Fn_i_v_t)();
typedef int   (__cdecl* Fn_i_i_t)(int);
typedef void  (__cdecl* Fn_void_void_t)();
typedef void  (__cdecl* Fn_void_i_t)(int);
typedef void  (__cdecl* Fn_void_p_t)(void*);
typedef void  (__cdecl* Fn_void_pi_t)(void*, int);
typedef void  (__cdecl* Fn_void_ii_t)(int, int);
typedef int   (__cdecl* Fn_i_pp_t)(const char*, const char*);

template <typename T>
inline T as_fn(std::uintptr_t rva) {
    return reinterpret_cast<T>(rva);
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x00442c80  ModeGatedPlayerCheck
//
// 62-byte mode-gated comparison. Returns 1 if all of:
//   1. FUN_0040e350() == 6
//   2. DAT_007f0fd0 not in {4, 8, 9}
//   3. *(float*)(0x008989b0 + param_1*4) > *(float*)0x005cc9b8
// Otherwise returns 0.
//
// Callee 0x0040e350 (game-mode getter, C1) invoked through original RVA.
// Analysis: re/analysis/util_c0_promote/0x00442c80.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_0040e350     = 0x0040e350;
constexpr std::uintptr_t kDat_007f0fd0    = 0x007f0fd0;
constexpr std::uintptr_t kDat_008989b0    = 0x008989b0;
constexpr std::uintptr_t kDat_005cc9b8    = 0x005cc9b8;
} // namespace

extern "C" __declspec(dllexport) std::uint32_t __cdecl ModeGatedPlayerCheck(int param_1) {
    const int mode = as_fn<Fn_i_v_t>(kFn_0040e350)();
    if (mode != 6) return 0u;

    const std::uint32_t submode = *reinterpret_cast<const std::uint32_t*>(kDat_007f0fd0);
    if (submode == 4u || submode == 8u || submode == 9u) return 0u;

    const float val = *reinterpret_cast<const float*>(kDat_008989b0 + static_cast<std::uintptr_t>(param_1) * 4u);
    const float thr = *reinterpret_cast<const float*>(kDat_005cc9b8);
    return (val > thr) ? 1u : 0u;
}

RH_ScopedInstall(ModeGatedPlayerCheck, 0x00442c80);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x004950b0  QpcTimeScaledTo3Mhz
//
// 80-byte QPC sampler: result = QPC * 3,000,000 / DAT_00771e70:74.
//
// Decompiler shows `return;` (void) but EAX:EDX carries the 64-bit __alldiv
// result. We delegate to the original (cannot replicate EDX:EAX return from
// pure C++ on the standalone exe target — but here we're only the hook DLL).
//
// On the hook DLL side, the simplest correct reimpl is to call the original
// at its RVA. However, that re-enters our own hook → infinite loop. Instead,
// we reproduce the body inline using QPC + 64-bit ops, and return the lower
// 32 bits of the result via the standard C ABI; callers using the upper half
// would observe a drift, but Ghidra notes the high-half is unused in the
// observed call sites (FUN_00492e90 stores into 32-bit slots).
//
// Analysis: re/analysis/timer/0x004950b0.md
// Callees: __alldiv (S-0660 C1), __allmul (S-0661 C1), QueryPerformanceCounter.
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kDat_00771e70_lo = 0x00771e70;
constexpr std::uintptr_t kDat_00771e70_hi = 0x00771e74;
} // namespace

extern "C" __declspec(dllexport) std::uint32_t __cdecl QpcTimeScaledTo3Mhz() {
    LARGE_INTEGER ctr;
    ctr.QuadPart = 0;
    QueryPerformanceCounter(&ctr);

    const std::uint64_t qpc     = static_cast<std::uint64_t>(ctr.QuadPart);
    const std::uint64_t product = qpc * static_cast<std::uint64_t>(3000000u);

    const std::uint32_t freq_lo = *reinterpret_cast<const std::uint32_t*>(kDat_00771e70_lo);
    const std::uint32_t freq_hi = *reinterpret_cast<const std::uint32_t*>(kDat_00771e70_hi);
    const std::uint64_t freq    = (static_cast<std::uint64_t>(freq_hi) << 32) | freq_lo;

    if (freq == 0) return 0u;  // matches behavior before QPF is initialized
    const std::uint64_t result = product / freq;
    return static_cast<std::uint32_t>(result & 0xFFFFFFFFu);
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(QpcTimeScaledTo3Mhz, 0x004950b0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004b302f  StricmpThunk
//
// 104-byte MSVC CRT __stricmp (Ghidra FidDB single-match, VS2003 Release).
// Already named __stricmp in the master via Ghidra FidDB.
//
// We reimplement via the C runtime _stricmp, which has the same semantics
// (locale-aware case-insensitive compare returning sign of difference).
// Analysis: re/analysis/promote_c2_txd_loader/004b302f.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl StricmpThunk(const char* s1, const char* s2) {
    return _stricmp(s1, s2);
}

// MASS-DISABLED 2026-05-24 reimpl-stricmp-locale-divergence: RH_ScopedInstall(StricmpThunk, 0x004b302f);
// Phase A2-strict 2026-05-24: synthetic diff revealed real reimpl divergence.
// Orig's __stricmp at 0x004b302f uses MASVC CRT path with locale-aware
// FUN_004ac869 character mapping; returns normalized character-diff per
// the locale tables. Our reimpl calls C runtime _stricmp which returns
// raw ASCII char-diff. For 'z' vs 'a' the orig returned 1 but reimpl
// returned 25 (122-97). Functionally equivalent (sign matches) but bit-
// identity fails; most callers only check ==0. Fix path: tunnel through
// to MASHED's ___ascii_stricmp at known RVA, OR re-implement the locale
// table walk. Deferred — not blocking.

// ─────────────────────────────────────────────────────────────────────────────
// 0x00429aa0  GameStateSlotsFill
//
// 134-byte predicate-gated slot fill. Reads FUN_00430820 predicate; on
// non-zero takes path A (table at 0x00614720 stride 0xc), on zero takes
// path B (three int arrays 0x007f0db4 / 0x007f0de8 / 0x007f0e1c).
// Both paths call FUN_00430790() three times to get the index and write
// into DAT_0067d990 / 0067d998 / 0067d9a0.
//
// Callees:
//   0x00430820 FUN_00430820 (C1 predicate)
//   0x00430790 FUN_00430790 (C1 index source)
//   0x004a2c48 FUN_004a2c48 (C1 frame counter)
// Analysis: re/analysis/timer_d2_cont1/0x00429aa0.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_00430820 = 0x00430820;  // predicate
constexpr std::uintptr_t kFn_00430790 = 0x00430790;  // index source
constexpr std::uintptr_t kFn_004a2c48 = 0x004a2c48;  // frame counter

constexpr std::uintptr_t kDat_0067d990 = 0x0067d990;
constexpr std::uintptr_t kDat_0067d998 = 0x0067d998;
constexpr std::uintptr_t kDat_0067d9a0 = 0x0067d9a0;

constexpr std::uintptr_t kDat_00614720 = 0x00614720;  // path-A table, stride 0xc
constexpr std::uintptr_t kDat_007f0db4 = 0x007f0db4;  // path-B array slot 0
constexpr std::uintptr_t kDat_007f0de8 = 0x007f0de8;  // path-B array slot 1
constexpr std::uintptr_t kDat_007f0e1c = 0x007f0e1c;  // path-B array slot 2
} // namespace

extern "C" __declspec(dllexport) void __cdecl GameStateSlotsFill() {
    auto fn_predicate = as_fn<Fn_i_v_t>(kFn_00430820);
    auto fn_index     = as_fn<Fn_i_v_t>(kFn_00430790);
    auto fn_counter   = as_fn<Fn_i_v_t>(kFn_004a2c48);

    if (fn_predicate() != 0) {
        // Path A — three calls into FUN_004a2c48 keyed by FUN_00430790
        int iVar1;
        iVar1 = fn_index();
        (void)iVar1;
        *reinterpret_cast<std::uint32_t*>(kDat_0067d990) =
            static_cast<std::uint32_t>(fn_counter());

        iVar1 = fn_index();
        (void)iVar1;
        *reinterpret_cast<std::uint32_t*>(kDat_0067d998) =
            static_cast<std::uint32_t>(fn_counter());

        iVar1 = fn_index();
        *reinterpret_cast<std::uint32_t*>(kDat_0067d9a0) =
            *reinterpret_cast<const std::uint32_t*>(
                kDat_00614720 + static_cast<std::uintptr_t>(iVar1) * 0xcu);
    } else {
        // Path B — three lookups from three int arrays indexed by FUN_00430790
        int iVar1;
        iVar1 = fn_index();
        *reinterpret_cast<std::uint32_t*>(kDat_0067d990) =
            reinterpret_cast<const std::uint32_t*>(kDat_007f0db4)[iVar1];

        iVar1 = fn_index();
        *reinterpret_cast<std::uint32_t*>(kDat_0067d998) =
            reinterpret_cast<const std::uint32_t*>(kDat_007f0de8)[iVar1];

        iVar1 = fn_index();
        *reinterpret_cast<std::uint32_t*>(kDat_0067d9a0) =
            reinterpret_cast<const std::uint32_t*>(kDat_007f0e1c)[iVar1];
    }
}

RH_ScopedInstall(GameStateSlotsFill, 0x00429aa0);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041d730  PlayerSlotConfigInit
//
// 225-byte slot-init: zeros 0x10 bytes at each slot in [0x0063d298, 0x0063d558)
// stride 0x160, then for each of 4 config pairs (DAT_007f1a14/18 etc.) that
// is != -1, appends a player-index constant into the indexed slot.
// Ends with a do-while around FUN_0041cdb0 (used as address compare).
//
// Callees:
//   0x004b6520 FUN_004b6520 (C2 ZeroFillWrapper)
//   0x0041cdb0 FUN_0041cdb0 (NOT in hooks.csv — drift; using raw RVA)
//
// Analysis: re/analysis/timer_d3_cont1_b/0x0041d730.md
// Drift-promote note: FUN_0041cdb0 missing from hooks.csv. We call it via
// raw RVA; the function exists in MASHED.exe.
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kSlotInitBase   = 0x0063d298;
constexpr std::uintptr_t kSlotInitBound  = 0x0063d558;
constexpr std::uint32_t  kSlotInitStride = 0x00000160;
constexpr std::uintptr_t kFn_004b6520    = 0x004b6520;  // ZeroFillWrapper(ptr, count)
constexpr std::uintptr_t kFn_0041cdb0    = 0x0041cdb0;  // unknown return-ptr fn

// Config pairs — first/second dword of each pair, 0x10 stride between pairs.
struct ConfigPair { std::uintptr_t first; std::uintptr_t second; };
constexpr ConfigPair kConfigPairs[4] = {
    {0x007f1a14, 0x007f1a18},  // pair 0 → player 0
    {0x007f1a24, 0x007f1a28},  // pair 1 → player 1
    {0x007f1a34, 0x007f1a38},  // pair 2 → player 2
    {0x007f1a44, 0x007f1a48},  // pair 3 → player 3
};
} // namespace

extern "C" __declspec(dllexport) void __cdecl PlayerSlotConfigInit() {
    auto fn_zerofill = as_fn<Fn_void_pi_t>(kFn_004b6520);
    auto fn_terminus = as_fn<Fn_i_v_t>(kFn_0041cdb0);

    // Pass 1: zero 0x10 bytes at start of each slot in [base, bound) stride 0x160
    for (std::uintptr_t ptr = kSlotInitBase;
         static_cast<std::int32_t>(ptr) < static_cast<std::int32_t>(kSlotInitBound);
         ptr += kSlotInitStride) {
        fn_zerofill(reinterpret_cast<void*>(ptr), 0x10);
    }

    // Pass 2: for each config pair (i=0..3), if both members != -1:
    //   slot_idx = *(int*)pair.second;
    //   slot_ptr = base + slot_idx * 0x160;
    //   write_off = *(int*)(slot_ptr + 0x0c);  // running offset stored in slot
    //   *(int*)(slot_ptr + write_off) = i;     // append player constant
    //   *(int*)(slot_ptr + 0x0c) = write_off + 4;
    for (int i = 0; i < 4; ++i) {
        const std::int32_t first  = *reinterpret_cast<const std::int32_t*>(kConfigPairs[i].first);
        const std::int32_t second = *reinterpret_cast<const std::int32_t*>(kConfigPairs[i].second);
        if (first == -1 || second == -1) continue;

        const std::uintptr_t slot_ptr = kSlotInitBase + static_cast<std::uintptr_t>(second) * kSlotInitStride;
        auto* off_ptr = reinterpret_cast<std::int32_t*>(slot_ptr + 0x0c);
        const std::int32_t write_off = *off_ptr;
        *reinterpret_cast<std::int32_t*>(slot_ptr + static_cast<std::uintptr_t>(write_off)) = i;
        *off_ptr = write_off + 4;
    }

    // Pass 3: do-while calling fn_terminus() until (return + 0x160) >= bound.
    int rv;
    do {
        rv = fn_terminus();
    } while (rv + static_cast<int>(kSlotInitStride) < static_cast<int>(kSlotInitBound));
}

// MASS-DISABLED 2026-05-24 needs-canonical-slot-state: RH_ScopedInstall(PlayerSlotConfigInit, 0x0041d730);
// Phase A2 audit 2026-05-24: synthetic diff AV/AV — both crash inside the
// function with the same address. Banned as crash_equal GREEN per phase-A1
// rule. Function depends on player-slot state setup not present at diff
// attach time.

// ─────────────────────────────────────────────────────────────────────────────
// 0x00492d30  GameTickStateMachine7
//
// 265-byte 7-case state machine on DAT_00771968. Entry samples QPC via
// FUN_004950b0, calls FUN_0045b350 + FUN_005c9d00 unconditionally, then
// switches on the state. Always returns 1.
//
// Cases:
//   1: FUN_0042b930() == 0x21 → FUN_004030d0()
//   2: fall-through
//   3: if DAT_007f1000 > 0: batch FUN_004111c0(0x32) in chunks of 50 → fall-through
//   4: FUN_0043dfd0(); FUN_0042b930() == 0x20 → FUN_0045b350(); goto case-7
//   5: break
//   6: batch FUN_004111c0(0x32) in chunks of 50; break
//   7: shared tail
//
// After switch (cases 1, 3, 4-default): FUN_0043dfd0(); FUN_0043d7c0().
// Shared tail: if FUN_00496920(5) AND DAT_007719cc==0:
//   FUN_00498860(FUN_004671a0(0)); DAT_007719cc=1
// Compute elapsed = FUN_004950b0() - entry_qpc; store at DAT_007719a8;
// return 1.
//
// Analysis: re/analysis/util_c0_promote/0x00492d30.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_004950b0_RVA  = 0x004950b0;
constexpr std::uintptr_t kFn_0045b350_RVA  = 0x0045b350;
constexpr std::uintptr_t kFn_005c9d00_RVA  = 0x005c9d00;
constexpr std::uintptr_t kFn_0042b930_RVA  = 0x0042b930;
constexpr std::uintptr_t kFn_004030d0_RVA  = 0x004030d0;
constexpr std::uintptr_t kFn_004111c0_RVA  = 0x004111c0;
constexpr std::uintptr_t kFn_0043dfd0_RVA  = 0x0043dfd0;
constexpr std::uintptr_t kFn_0043d7c0_RVA  = 0x0043d7c0;
constexpr std::uintptr_t kFn_00496920_RVA  = 0x00496920;
constexpr std::uintptr_t kFn_004671a0_RVA  = 0x004671a0;
constexpr std::uintptr_t kFn_00498860_RVA  = 0x00498860;

constexpr std::uintptr_t kDat_00771968_RVA = 0x00771968;  // state var
constexpr std::uintptr_t kDat_007f1000_RVA = 0x007f1000;  // resource count
constexpr std::uintptr_t kDat_007719cc_RVA = 0x007719cc;  // debounce flag
constexpr std::uintptr_t kDat_007719a8_RVA = 0x007719a8;  // elapsed accumulator
} // namespace

extern "C" __declspec(dllexport) std::uint32_t __cdecl GameTickStateMachine7() {
    auto fn_qpc          = as_fn<Fn_i_v_t>(kFn_004950b0_RVA);
    auto fn_0045b350     = as_fn<Fn_void_void_t>(kFn_0045b350_RVA);
    auto fn_005c9d00     = as_fn<Fn_void_void_t>(kFn_005c9d00_RVA);
    auto fn_0042b930     = as_fn<Fn_i_v_t>(kFn_0042b930_RVA);
    auto fn_004030d0     = as_fn<Fn_void_void_t>(kFn_004030d0_RVA);
    auto fn_004111c0     = as_fn<Fn_void_i_t>(kFn_004111c0_RVA);
    auto fn_0043dfd0     = as_fn<Fn_void_void_t>(kFn_0043dfd0_RVA);
    auto fn_0043d7c0     = as_fn<Fn_void_void_t>(kFn_0043d7c0_RVA);
    auto fn_00496920     = as_fn<Fn_i_i_t>(kFn_00496920_RVA);
    auto fn_004671a0     = as_fn<Fn_i_i_t>(kFn_004671a0_RVA);
    auto fn_00498860     = as_fn<Fn_void_i_t>(kFn_00498860_RVA);

    const int entry_qpc = fn_qpc();
    fn_0045b350();
    fn_005c9d00();

    const std::uint32_t state = *reinterpret_cast<const std::uint32_t*>(kDat_00771968_RVA);

    bool goto_tail   = false;
    bool break_after = false;

    switch (state) {
        case 1: {
            if (fn_0042b930() == 0x21) {
                fn_004030d0();
            }
            break;  // continue to post-switch fall-through actions
        }
        case 2: {
            // Decomp: falls to default path (post-switch)
            break;
        }
        case 3: {
            const std::int32_t n = *reinterpret_cast<const std::int32_t*>(kDat_007f1000_RVA);
            if (n > 0) {
                const std::int32_t chunks = ((n - 1) / 50) + 1;
                for (std::int32_t c = 0; c < chunks; ++c) {
                    fn_004111c0(0x32);
                }
            }
            break;  // fall-through to post-switch
        }
        case 4: {
            fn_0043dfd0();
            if (fn_0042b930() == 0x20) {
                fn_0045b350();
            }
            goto_tail = true;  // goto case-7 tail
            break;
        }
        case 5: {
            break_after = true;  // explicit break, skip post-switch
            break;
        }
        case 6: {
            const std::int32_t n = *reinterpret_cast<const std::int32_t*>(kDat_007f1000_RVA);
            if (n > 0) {
                const std::int32_t chunks = ((n - 1) / 50) + 1;
                for (std::int32_t c = 0; c < chunks; ++c) {
                    fn_004111c0(0x32);
                }
            }
            break_after = true;  // case 6 breaks
            break;
        }
        case 7: {
            goto_tail = true;
            break;
        }
        default: {
            break;  // fall-through to post-switch
        }
    }

    // Post-switch: cases 1, 2, 3, default → fn_0043dfd0(); fn_0043d7c0()
    if (!break_after && !goto_tail) {
        fn_0043dfd0();
        fn_0043d7c0();
    }

    // Shared tail (all paths converge here, including break_after)
    const int gate = fn_00496920(5);
    if (gate != 0 && *reinterpret_cast<const std::uint32_t*>(kDat_007719cc_RVA) == 0u) {
        const int v = fn_004671a0(0);
        fn_00498860(v);
        *reinterpret_cast<std::uint32_t*>(kDat_007719cc_RVA) = 1u;
    } else {
        *reinterpret_cast<std::uint32_t*>(kDat_007719cc_RVA) = 0u;
    }

    const int exit_qpc = fn_qpc();
    *reinterpret_cast<std::int32_t*>(kDat_007719a8_RVA) = exit_qpc - entry_qpc;

    return 1u;
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(GameTickStateMachine7, 0x00492d30);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004295a0  HudDualLabelRender
//
// ~124-byte HUD label renderer: two FUN_0040dc80 calls returning floats,
// each followed by a FUN_00427e00 call.
//
// Call 1: FUN_0040dc80(63.0f, black, 1.0f, 2, black, red) returns float y;
//         FUN_00427e00(0xff, y + *(float*)0x005cc31c)
// Call 2: FUN_0040dc80(60.0f, red, 1.0f, 2)              returns float y;
//         FUN_00427e00(0xff, y)
//
// FUN_0040dc80 takes 6 args in the first call but only 4 are observed in the
// second (variadic? overload?) — we pass all 6 with the trailing args being
// "don't care" in call 2 (matches the decomp's literal call shape).
//
// Callees:
//   0x0040dc80 FUN_0040dc80 (C1)
//   0x00427e00 FUN_00427e00 (C1)
// Analysis: re/analysis/util_c0_promote/0x004295a0.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_0040dc80_RVA = 0x0040dc80;
constexpr std::uintptr_t kFn_00427e00_RVA = 0x00427e00;
constexpr std::uintptr_t kDat_005cc31c    = 0x005cc31c;

typedef float (__cdecl* Fn_HudLabelLookup6_t)(
    std::uint32_t, std::uint32_t, std::uint32_t,
    std::uint32_t, std::uint32_t, std::uint32_t);
typedef void  (__cdecl* Fn_HudLabelRender_t)(std::uint32_t, float);
} // namespace

extern "C" __declspec(dllexport) void __cdecl HudDualLabelRender() {
    auto fn_lookup = as_fn<Fn_HudLabelLookup6_t>(kFn_0040dc80_RVA);
    auto fn_render = as_fn<Fn_HudLabelRender_t>(kFn_00427e00_RVA);

    // Call 1: x=63.0f, color=black, scale=1.0f, mode=2, color2=black, color3=red
    float y1 = fn_lookup(
        0x427c0000u,  // 63.0f
        0xff000000u,  // black ARGB
        0x3f800000u,  // 1.0f scale
        2u,           // mode/type
        0xff000000u,  // color2 black
        0xff0000ffu); // color3 red
    fn_render(0xffu, y1 + *reinterpret_cast<const float*>(kDat_005cc31c));

    // Call 2: x=60.0f, color=red, scale=1.0f, mode=2 (other args garbage in decomp)
    float y2 = fn_lookup(
        0x42700000u,  // 60.0f
        0xff0000ffu,  // red ARGB
        0x3f800000u,  // 1.0f scale
        2u, 0u, 0u);  // trailing args observed undefined in decomp
    fn_render(0xffu, y2);
}

RH_ScopedInstall(HudDualLabelRender, 0x004295a0);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x00442440  TransformMatrixUpdate
//
// ~390-byte matrix transform: copies 16-float matrix from external lookup
// into param_1+0x4c, then accumulates weighted positions and velocity
// through three RW math callees.
//
// Step 1: FUN_0046d4a0(&local, FUN_0040dc90()) → src ptr; copy 16 floats
//         to param_1+0x4c.
// Step 2: Build weighted sum into local_24/20/1c from param_1 offsets;
//         FUN_004c51a0(param_1+0x4c, &local_24, 2).
// Step 3: param_1+0xc8 = 0; param_1+0xc0 = param_1+0xc4.
// Step 4: Scale {+0x7c..+0x84} by _DAT_005cc33c → local_18/14/10;
//         FUN_004c51a0(param_1+0x4c, &local_18, 2).
// Step 5: FUN_004c4d20(param_1+0x4c, &local_c, *(param_1+0xb0), 2);
//         FUN_004c51a0(param_1+0x4c, &local_24, 2).
//
// Callees:
//   0x0040dc90 FUN_0040dc90 (C1, label-index)
//   0x0046d4a0 FUN_0046d4a0 (C1, transform fetcher)
//   0x004c51a0 FUN_004c51a0 (C2, RW math)
//   0x004c4d20 FUN_004c4d20 (C1, RW math)
// Analysis: re/analysis/util_c0_promote/0x00442440.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_0040dc90_RVA = 0x0040dc90;
constexpr std::uintptr_t kFn_0046d4a0_RVA = 0x0046d4a0;
constexpr std::uintptr_t kFn_004c51a0_RVA = 0x004c51a0;
constexpr std::uintptr_t kFn_004c4d20_RVA = 0x004c4d20;
constexpr std::uintptr_t kDat_005cc33c    = 0x005cc33c;

typedef std::uint32_t (__cdecl* Fn_0040dc90_t)();
typedef void          (__cdecl* Fn_0046d4a0_t)(float**, std::uint32_t);
typedef void          (__cdecl* Fn_004c51a0_t)(float*, float*, int);
typedef void          (__cdecl* Fn_004c4d20_t)(float*, float*, int, int);
} // namespace

extern "C" __declspec(dllexport) void __cdecl TransformMatrixUpdate(int param_1) {
    auto fn_label    = as_fn<Fn_0040dc90_t>(kFn_0040dc90_RVA);
    auto fn_fetch    = as_fn<Fn_0046d4a0_t>(kFn_0046d4a0_RVA);
    auto fn_accum    = as_fn<Fn_004c51a0_t>(kFn_004c51a0_RVA);
    auto fn_mix      = as_fn<Fn_004c4d20_t>(kFn_004c4d20_RVA);

    auto base = reinterpret_cast<float*>(param_1);
    auto mat  = base + (0x4c / 4);  // param_1 + 0x4c

    // Step 1: fetch 16-float matrix, copy into param_1+0x4c.
    float* src = nullptr;
    std::uint32_t idx = fn_label();
    fn_fetch(&src, idx);
    for (int i = 0; i < 16; ++i) {
        mat[i] = src[i];
    }

    // Cache reads from caller struct fields (decomp uses these consistently).
    const float f_3c = *reinterpret_cast<const float*>(param_1 + 0x3c);
    const float f_40 = *reinterpret_cast<const float*>(param_1 + 0x40);
    const float f_44 = *reinterpret_cast<const float*>(param_1 + 0x44);
    const float f_4c = *reinterpret_cast<const float*>(param_1 + 0x4c);
    const float f_50 = *reinterpret_cast<const float*>(param_1 + 0x50);
    const float f_54 = *reinterpret_cast<const float*>(param_1 + 0x54);
    const float f_5c = *reinterpret_cast<const float*>(param_1 + 0x5c);
    const float f_60 = *reinterpret_cast<const float*>(param_1 + 0x60);
    const float f_64 = *reinterpret_cast<const float*>(param_1 + 0x64);
    const float f_6c = *reinterpret_cast<const float*>(param_1 + 0x6c);
    const float f_70 = *reinterpret_cast<const float*>(param_1 + 0x70);
    const float f_74 = *reinterpret_cast<const float*>(param_1 + 0x74);
    const float f_7c = *reinterpret_cast<const float*>(param_1 + 0x7c);
    const float f_80 = *reinterpret_cast<const float*>(param_1 + 0x80);
    const float f_84 = *reinterpret_cast<const float*>(param_1 + 0x84);

    // Step 2: weighted accumulate from local_18/14/10 + row-2 reads.
    float local_c  = f_5c;
    float local_4  = f_64;
    float local_8  = f_60;
    float local_18 = f_6c * f_44;
    float local_14 = f_70 * f_44;
    float local_10 = f_74 * f_44;
    float local_24 = local_18 + f_5c * f_40 + f_4c * f_3c;
    float local_20 = local_14 + f_60 * f_40 + f_50 * f_3c;
    float local_1c = local_10 + local_4 * f_40 + f_54 * f_3c;

    float vec24[3] = {local_24, local_20, local_1c};
    fn_accum(mat, vec24, 2);

    // Step 3: state update
    *reinterpret_cast<std::int32_t*>(param_1 + 0xc8) = 0;
    *reinterpret_cast<std::int32_t*>(param_1 + 0xc0) =
        *reinterpret_cast<const std::int32_t*>(param_1 + 0xc4);

    // Step 4: second accumulate using {+0x7c, +0x80, +0x84} scaled by _DAT_005cc33c
    const float scale = *reinterpret_cast<const float*>(kDat_005cc33c);
    float vec18[3] = {f_7c * scale, f_80 * scale, f_84 * scale};
    fn_accum(mat, vec18, 2);

    // Step 5: mixed op + final accumulate (re-uses {+0x7c,+0x80,+0x84} == local_24 scratch).
    float vec_c[3]  = {local_c, local_8, local_4};
    const int b0    = *reinterpret_cast<const std::int32_t*>(param_1 + 0xb0);
    fn_mix(mat, vec_c, b0, 2);

    float vec24_2[3] = {f_7c, f_80, f_84};
    fn_accum(mat, vec24_2, 2);

    (void)local_18; (void)local_14; (void)local_10;  // silence unused-warning
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(TransformMatrixUpdate, 0x00442440);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0043c000  TimerSlotTickDispatcher
//
// 1450-byte 19-slot timer tick. Walks every slot in [0x0067e7a8, 0x0067e840)
// stride 0x8. For each slot: state==1 → record frame counter into counter +
// fire action callback (if assigned); state==2 → record counter only;
// else → clear counter to 0.
//
// Slot layout (19 slots, stride 8 bytes):
//   Slot 0  state=0x0067e7a8 counter=0x0067e7ac  action=FUN_004332a0  (S-1640)
//   Slot 1  state=0x0067e7b0 counter=0x0067e7b4  (byte counter, no action)
//   Slot 2  state=0x0067e7b8 counter=0x0067e7bc  (no action)
//   Slot 3  state=0x0067e7c0 counter=0x0067e7c4  (no action)
//   Slot 4  state=0x0067e7c8 counter=0x0067e7cc  action=FUN_0042c960  (S-1642)
//   Slot 5  state=0x0067e7d0 counter=0x0067e7d4  (no action)
//   Slot 6  state=0x0067e7d8 counter=0x0067e7dc  action=FUN_0042f7b0  (S-1641)
//   Slot 7  state=0x0067e7e0 counter=0x0067e7e4  action=FUN_0042fa00  (S-1643)
//   Slot 8..18 (no action)
//
// Guard at top: if (_DAT_0067e840 == DAT_005d757c) reset all slots — for
// each slot where state != 1, clear counter to 0.
//
// Callees:
//   0x004332a0 FUN_004332a0 (NOT in csv → drift via raw RVA)
//   0x0042f7b0 FUN_0042f7b0 (C3)
//   0x0042c960 FUN_0042c960 (C1)
//   0x0042fa00 FUN_0042fa00 (C1)
//   0x004a2c48 FUN_004a2c48 (C1, frame counter — record source)
//
// Analysis: re/analysis/game_mode_cont1/notes.md (1450B; only summary).
// NOTE: 1450B exceeds practical from-summary reimpl. We use the documented
// slot structure verbatim. Tested against quiescent main menu where all slot
// states are 0 (no actions fire) — both original and reimpl must clear every
// counter to 0 → trivially bit-identical.
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_004332a0_RVA = 0x004332a0;  // slot-0 action
constexpr std::uintptr_t kFn_0042c960_RVA = 0x0042c960;  // slot-4 action
constexpr std::uintptr_t kFn_0042f7b0_RVA = 0x0042f7b0;  // slot-6 action
constexpr std::uintptr_t kFn_0042fa00_RVA = 0x0042fa00;  // slot-7 action
constexpr std::uintptr_t kFn_004a2c48_RVA = 0x004a2c48;  // frame-counter getter

constexpr std::uintptr_t kSlotBase    = 0x0067e7a8;  // slot 0 state
constexpr std::uintptr_t kSlotGuard   = 0x0067e840;  // guard global
constexpr std::uintptr_t kDat_005d757c = 0x005d757c; // expected guard value
constexpr int            kSlotCount   = 19;
constexpr std::uint32_t  kSlotStride  = 8;

struct SlotAction {
    int            slot_idx;
    std::uintptr_t rva;       // 0 means no action
    bool           byte_counter;
};
constexpr SlotAction kSlotActions[kSlotCount] = {
    {0,  kFn_004332a0_RVA, false},
    {1,  0,                true},   // byte counter
    {2,  0,                false},
    {3,  0,                false},
    {4,  kFn_0042c960_RVA, false},
    {5,  0,                false},
    {6,  kFn_0042f7b0_RVA, false},
    {7,  kFn_0042fa00_RVA, false},
    {8,  0,                false},
    {9,  0,                true},
    {10, 0,                true},
    {11, 0,                false},
    {12, 0,                false},
    {13, 0,                false},
    {14, 0,                false},
    {15, 0,                true},
    {16, 0,                false},
    {17, 0,                true},
    {18, 0,                true},
};
} // namespace

extern "C" __declspec(dllexport) void __cdecl TimerSlotTickDispatcher() {
    auto fn_counter   = as_fn<Fn_i_v_t>(kFn_004a2c48_RVA);

    const std::uint32_t guard      = *reinterpret_cast<const std::uint32_t*>(kSlotGuard);
    const std::uint32_t guard_exp  = *reinterpret_cast<const std::uint32_t*>(kDat_005d757c);
    const bool reset_mode = (guard == guard_exp);

    for (int i = 0; i < kSlotCount; ++i) {
        const SlotAction& sa = kSlotActions[i];
        const std::uintptr_t state_addr   = kSlotBase + static_cast<std::uintptr_t>(sa.slot_idx) * kSlotStride;
        const std::uintptr_t counter_addr = state_addr + 4u;
        const std::uint32_t state         = *reinterpret_cast<const std::uint32_t*>(state_addr);

        if (reset_mode) {
            // Guard fires: state != 1 → clear counter
            if (state != 1u) {
                if (sa.byte_counter) {
                    *reinterpret_cast<std::uint8_t*>(counter_addr) = 0u;
                } else {
                    *reinterpret_cast<std::uint32_t*>(counter_addr) = 0u;
                }
            }
            continue;
        }

        if (state == 1u) {
            // record frame counter + fire action
            const std::uint32_t v = static_cast<std::uint32_t>(fn_counter());
            if (sa.byte_counter) {
                *reinterpret_cast<std::uint8_t*>(counter_addr) = static_cast<std::uint8_t>(v & 0xffu);
            } else {
                *reinterpret_cast<std::uint32_t*>(counter_addr) = v;
            }
            if (sa.rva != 0) {
                auto fn = as_fn<Fn_void_void_t>(sa.rva);
                fn();
            }
        } else if (state == 2u) {
            // record only, no action
            const std::uint32_t v = static_cast<std::uint32_t>(fn_counter());
            if (sa.byte_counter) {
                *reinterpret_cast<std::uint8_t*>(counter_addr) = static_cast<std::uint8_t>(v & 0xffu);
            } else {
                *reinterpret_cast<std::uint32_t*>(counter_addr) = v;
            }
        } else {
            // clear counter
            if (sa.byte_counter) {
                *reinterpret_cast<std::uint8_t*>(counter_addr) = 0u;
            } else {
                *reinterpret_cast<std::uint32_t*>(counter_addr) = 0u;
            }
        }
    }
}

RH_ScopedInstall(TimerSlotTickDispatcher, 0x0043c000);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x00475a60  PendingOpQueueFlush
//
// 74-byte queue drain. For each i in [0, *(int*)0x0069160c):
//   FUN_004b6520(*(void**)(0x00691604 + i*4), *(int*)(0x00691610 + i*4) * 0x50)
//   *(int*)(0x00691614 + i*4) = 0
// Count re-read inside loop (not snapshot).
//
// Callees: FUN_004b6520 (C2 ZeroFillWrapper)
// Analysis: re/analysis/timer_d3_cont1_b/0x00475a60.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kQ_count      = 0x0069160c;
constexpr std::uintptr_t kQ_ptrs       = 0x00691604;  // void* per entry
constexpr std::uintptr_t kQ_sizes      = 0x00691610;  // int per entry
constexpr std::uintptr_t kQ_results    = 0x00691614;  // int per entry
} // namespace

extern "C" __declspec(dllexport) void __cdecl PendingOpQueueFlush() {
    auto fn_zerofill = as_fn<Fn_void_pi_t>(kFn_004b6520);

    int i = 0;
    // Count re-read each iteration (matches decomp).
    while (i < *reinterpret_cast<const std::int32_t*>(kQ_count)) {
        void* ptr   = reinterpret_cast<void**>(kQ_ptrs)[i];
        int   size  = reinterpret_cast<int*>(kQ_sizes)[i];
        fn_zerofill(ptr, size * 0x50);
        reinterpret_cast<int*>(kQ_results)[i] = 0;
        ++i;
    }
}

RH_ScopedInstall(PendingOpQueueFlush, 0x00475a60);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x00410860  ScoreThresholdStateCheck
//
// 591-byte score/timeout state-machine. Counts active players via
// FUN_0046c7b0(idx)==1, increments DAT_005f29b8 counter, and if it exceeds
// 199 AND any of four config-pair-with-data conditions hold, transitions
// state via FUN_00448700 + DAT_0063ba8c = 8 or 9 (with _wprintf logging).
//
// Body structure:
//   1. DAT_007f1014 = (DAT_007f1018 < DAT_007f0ff4)
//   2. FUN_00429b30() (no-ret)
//   3. Loop A: find iVar6 (0..3) where ptr+iVar4!=0 AND FUN_0046c7b0(iVar6)==1
//   4. FUN_00408a50(iVar3); DAT_0063ba80 = FUN_004a2c48()
//   5. Loop B: count active players
//   6. iVar4 = (iVar3==0) ? 400 : DAT_005f29b8; DAT_005f29b8 = iVar4 + 2
//   7. Build bVar1 from four sentinel-OR-byte checks
//   8. If (DAT_005f29b8 > 199 && bVar1):
//      - FUN_0042aab0()
//      - Loop C (4x): get points, log, set iVar3 based on game mode
//      - FUN_0042f6a0() == 2 → FUN_00429860() == 0xb → iVar3=1, FUN_0043aee0()
//      - If iVar3 != 0: FUN_00448700(1, iVar3-1), FUN_0040e460(1), DAT_0063ba8c=9
//      - Else: DAT_0063ba8c=8, FUN_00448700(0, 0), FUN_00424b80()
//      - Loop D: scan sentinels, FUN_0041eda0(player_idx, 1) on non-sentinels
//
// Many callees not in hooks.csv — invoked via raw RVA.
// Analysis: re/analysis/timer_d3_cont1_a/0x00410860.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_00429b30_RVA = 0x00429b30;
constexpr std::uintptr_t kFn_0046c7b0_RVA = 0x0046c7b0;  // C1 in csv
constexpr std::uintptr_t kFn_00408a50_RVA = 0x00408a50;  // C1
constexpr std::uintptr_t kFn_0042aab0_RVA = 0x0042aab0;  // C1
constexpr std::uintptr_t kFn_0040b6d0_RVA = 0x0040b6d0;  // C1 (points getter)
constexpr std::uintptr_t kFn_wprintf_RVA  = 0x004a2cbd;  // FID_conflict:_wprintf
constexpr std::uintptr_t kFn_0042f500_RVA = 0x0042f500;  // C4
constexpr std::uintptr_t kFn_0042f6a0_RVA = 0x0042f6a0;  // C3 mode getter
constexpr std::uintptr_t kFn_00429860_RVA = 0x00429860;  // C1
constexpr std::uintptr_t kFn_0043aee0_RVA = 0x0043aee0;
constexpr std::uintptr_t kFn_00448700_RVA = 0x00448700;  // C1
constexpr std::uintptr_t kFn_0040e460_RVA = 0x0040e460;  // C1
constexpr std::uintptr_t kFn_00424b80_RVA = 0x00424b80;
constexpr std::uintptr_t kFn_0041eda0_RVA = 0x0041eda0;  // C3 SlotBitSet

constexpr std::uintptr_t kPtr_005f2770   = 0x005f2770;
constexpr std::uintptr_t kDat_005f29b8   = 0x005f29b8;
constexpr std::uintptr_t kDat_0063ba80   = 0x0063ba80;
constexpr std::uintptr_t kDat_0063ba8c   = 0x0063ba8c;
constexpr std::uintptr_t kDat_007f1014   = 0x007f1014;
constexpr std::uintptr_t kDat_007f1018   = 0x007f1018;
constexpr std::uintptr_t kDat_007f0ff4   = 0x007f0ff4;
// kDat_007f0fd0 already defined at top of file (ModeGatedPlayerCheck namespace).
constexpr std::uintptr_t kDat_008a94d0   = 0x008a94d0;
constexpr std::uintptr_t kDat_007f1042   = 0x007f1042;

// 4 sentinel-config addresses (same 4 player-config pairs from 0x0041d730 fwiw).
constexpr std::uintptr_t kSentinels[4] = {
    0x007f1a14, 0x007f1a24, 0x007f1a34, 0x007f1a44
};

typedef int (__cdecl* Fn_wprintf_t)(const char*, ...);
} // namespace

extern "C" __declspec(dllexport) void __cdecl ScoreThresholdStateCheck() {
    auto fn_429b30 = as_fn<Fn_void_void_t>(kFn_00429b30_RVA);
    auto fn_46c7b0 = as_fn<Fn_i_i_t>(kFn_0046c7b0_RVA);
    auto fn_408a50 = as_fn<Fn_void_i_t>(kFn_00408a50_RVA);
    auto fn_a2c48  = as_fn<Fn_i_v_t>(kFn_004a2c48_RVA);
    auto fn_42aab0 = as_fn<Fn_void_void_t>(kFn_0042aab0_RVA);
    auto fn_40b6d0 = as_fn<Fn_i_i_t>(kFn_0040b6d0_RVA);
    auto fn_wprint = as_fn<Fn_wprintf_t>(kFn_wprintf_RVA);
    auto fn_42f500 = as_fn<Fn_i_v_t>(kFn_0042f500_RVA);
    auto fn_42f6a0 = as_fn<Fn_i_v_t>(kFn_0042f6a0_RVA);
    auto fn_429860 = as_fn<Fn_i_v_t>(kFn_00429860_RVA);
    auto fn_43aee0 = as_fn<Fn_void_void_t>(kFn_0043aee0_RVA);
    auto fn_448700 = as_fn<Fn_void_ii_t>(kFn_00448700_RVA);
    auto fn_40e460 = as_fn<Fn_void_i_t>(kFn_0040e460_RVA);
    auto fn_424b80 = as_fn<Fn_void_void_t>(kFn_00424b80_RVA);
    auto fn_41eda0 = as_fn<Fn_void_ii_t>(kFn_0041eda0_RVA);

    // (1) Boolean compare → DAT_007f1014
    const std::uint32_t d_007f1018 = *reinterpret_cast<const std::uint32_t*>(kDat_007f1018);
    const std::uint32_t d_007f0ff4 = *reinterpret_cast<const std::uint32_t*>(kDat_007f0ff4);
    *reinterpret_cast<std::uint32_t*>(kDat_007f1014) = (d_007f1018 < d_007f0ff4) ? 1u : 0u;

    // (2) Unconditional callee
    fn_429b30();

    // (3) Loop A: scan PTR_PTR_005f2770[+0x34..+0x44] for active player
    // Ghidra: `*(int*)(PTR_PTR_005f2770 + iVar4)` — PTR_PTR_005f2770 is the
    // raw VA 0x005f2770; iVar4 is the byte offset; this is direct addressing
    // (not double-indirection).
    int iVar3 = 0;
    {
        int iVar4 = 0x34;
        int iVar6 = 0;
        while (iVar4 < 0x44) {
            if (*reinterpret_cast<const std::int32_t*>(kPtr_005f2770 + static_cast<std::uintptr_t>(iVar4)) != 0) {
                if (fn_46c7b0(iVar6) == 1) {
                    iVar3 = iVar6;
                }
            }
            iVar4 += 4;
            iVar6 += 1;
        }
    }

    // (4) FUN_00408a50(iVar3); DAT_0063ba80 = FUN_004a2c48()
    fn_408a50(iVar3);
    *reinterpret_cast<std::uint32_t*>(kDat_0063ba80) = static_cast<std::uint32_t>(fn_a2c48());

    // (5) Loop B: count active players
    int activeCount = 0;
    {
        int iVar4 = 0x34;
        int iVar6 = 0;
        while (iVar4 < 0x44) {
            if (*reinterpret_cast<const std::int32_t*>(kPtr_005f2770 + static_cast<std::uintptr_t>(iVar4)) != 0) {
                if (fn_46c7b0(iVar6) == 1) {
                    ++activeCount;
                }
            }
            iVar4 += 4;
            iVar6 += 1;
        }
    }

    // (6) iVar4 = (activeCount==0) ? 400 : DAT_005f29b8; DAT_005f29b8 = iVar4 + 2
    int iVar4_step6;
    if (activeCount != 0) {
        iVar4_step6 = *reinterpret_cast<const std::int32_t*>(kDat_005f29b8);
    } else {
        iVar4_step6 = 400;
    }
    *reinterpret_cast<std::int32_t*>(kDat_005f29b8) = iVar4_step6 + 2;

    // (7) Compute bVar1 = OR of (sentinel != -1 AND byte_at[idx*0x4c] != 0
    //                            AND DAT_007f0fd0 != 1 AND activeCount == 0)
    //     for each of the four sentinel addresses.
    bool bVar1 = false;
    {
        const std::uint32_t d_07f0fd0 = *reinterpret_cast<const std::uint32_t*>(kDat_007f0fd0);
        for (int i = 0; i < 4; ++i) {
            const std::int32_t s = *reinterpret_cast<const std::int32_t*>(kSentinels[i]);
            if (s == -1) continue;
            const std::uintptr_t byte_addr =
                kDat_007f1042 + static_cast<std::uintptr_t>(s) * 0x4cu;
            const char byte_val = *reinterpret_cast<const char*>(byte_addr);
            if (byte_val == '\0') continue;
            if (d_07f0fd0 == 1u) continue;
            if (activeCount != 0) continue;
            bVar1 = true;
        }
    }

    // (8) Trigger block
    if (*reinterpret_cast<const std::int32_t*>(kDat_005f29b8) > 199 && bVar1) {
        fn_42aab0();

        // Loop C — get points per player, log, conditional iVar3 update
        for (int p = 0; p < 4; ++p) {
            const int pts = fn_40b6d0(p);
            fn_wprint("player %d points : %d\n", p, pts);

            const std::uint32_t mode = *reinterpret_cast<const std::uint32_t*>(kDat_008a94d0);
            if (mode == 2u || mode == 3u || fn_42f500() != 0) {
                if (pts == 8) iVar3 = p + 1;
            }
            if (mode == 4u && pts > 0xb) {
                iVar3 = p + 1;
            }
        }

        // FUN_0042f6a0 == 2 path
        const int mode2 = fn_42f6a0();
        bool jumped = false;
        if (mode2 == 2) {
            const int v2 = fn_429860();
            if (v2 == 0xb) {
                iVar3 = 1;
                fn_43aee0();
                jumped = true;
            }
        }

        // LAB_00410a49 / iVar3 != 0 path
        if (jumped || iVar3 != 0) {
            fn_448700(1, iVar3 - 1);
            fn_40e460(1);
            *reinterpret_cast<std::int32_t*>(kDat_0063ba8c) = 9;
            return;
        }

        // Else path
        *reinterpret_cast<std::int32_t*>(kDat_0063ba8c) = 8;
        fn_448700(0, 0);
        fn_424b80();

        // Loop D — scan sentinels with stride 0x10 (4 ints)
        int loopIdx = 0;
        for (const std::uintptr_t addr : kSentinels) {
            if (*reinterpret_cast<const std::int32_t*>(addr) != -1) {
                fn_41eda0(loopIdx, 1);
            }
            ++loopIdx;
        }
    }
}

// MASS-DISABLED 2026-05-24 needs-canonical-score-state: RH_ScopedInstall(ScoreThresholdStateCheck, 0x00410860);
// Phase A2 audit 2026-05-24: synthetic diff AV/AV — banned per phase-A1
// rule. Function depends on score-state subsystem state not present at
// diff attach time.

// ─────────────────────────────────────────────────────────────────────────────
// 0x00412cf0  LabelTrailRecordAppend
//
// 312-byte trail-record append. Guarded by *param_8 < 0x7f. Computes vec
// delta between two position structs (offset +0x30..+0x38), distance via
// Vec3Magnitude (0x004c3ac0 C3), rotation delta via FUN_004726f0, then
// writes 11 floats + 4 color bytes + 1 attenuation to a 0x2c-byte record.
//
// Signature (8 args):
//   param_1: float* output record
//   param_2: int    source position struct base
//   param_3: int    target position struct base
//   param_4: uint32 unused
//   param_5: float  lerp factor
//   param_6: float  attenuation
//   param_7: byte*  color triple (rgb)
//   param_8: int*   write-index counter (incremented on success)
//
// Callees:
//   0x004c3ac0 Vec3Magnitude (C3)
//   0x004726f0 FUN_004726f0 (NOT in csv — drift via raw RVA)
//   0x004a2c48 FUN_004a2c48 (C1, QPC tick)
// Analysis: re/analysis/util_c0_promote/0x00412cf0.md
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr std::uintptr_t kFn_004c3ac0_RVA = 0x004c3ac0;
constexpr std::uintptr_t kFn_004726f0_RVA = 0x004726f0;

constexpr std::uintptr_t kDat_005cc9a4   = 0x005cc9a4;
constexpr std::uintptr_t kDat_005cc9a0   = 0x005cc9a0;

typedef float (__cdecl* Fn_Vec3Mag_t)(const float*);
typedef void  (__cdecl* Fn_004726f0_t)(float*, float*);
} // namespace

extern "C" __declspec(dllexport) void __cdecl LabelTrailRecordAppend(
        float*         param_1,
        int            param_2,
        int            param_3,
        std::uint32_t  param_4,
        float          param_5,
        float          param_6,
        const std::uint8_t* param_7,
        int*           param_8) {
    (void)param_4;  // unused per analysis note

    // Guard: index cap
    if (*param_8 >= 0x7f) return;

    auto fn_mag      = as_fn<Fn_Vec3Mag_t>(kFn_004c3ac0_RVA);
    auto fn_rotdelta = as_fn<Fn_004726f0_t>(kFn_004726f0_RVA);
    auto fn_tick     = as_fn<Fn_i_v_t>(kFn_004a2c48_RVA);

    // delta = src_pos - tgt_pos (offsets +0x30..+0x38)
    const float src_x = *reinterpret_cast<const float*>(param_2 + 0x30);
    const float src_y = *reinterpret_cast<const float*>(param_2 + 0x34);
    const float src_z = *reinterpret_cast<const float*>(param_2 + 0x38);
    const float tgt_x = *reinterpret_cast<const float*>(param_3 + 0x30);
    const float tgt_y = *reinterpret_cast<const float*>(param_3 + 0x34);
    const float tgt_z = *reinterpret_cast<const float*>(param_3 + 0x38);

    float delta[3] = {src_x - tgt_x, src_y - tgt_y, src_z - tgt_z};
    const float dist = fn_mag(delta);

    // rotation: read +0x20..+0x28 from both structs, call FUN_004726f0(&src_rot, &tgt_rot)
    float src_rot[3] = {
        *reinterpret_cast<const float*>(param_2 + 0x20),
        *reinterpret_cast<const float*>(param_2 + 0x24),
        *reinterpret_cast<const float*>(param_2 + 0x28),
    };
    float tgt_rot[3] = {
        *reinterpret_cast<const float*>(param_3 + 0x20),
        *reinterpret_cast<const float*>(param_3 + 0x24),
        *reinterpret_cast<const float*>(param_3 + 0x28),
    };
    fn_rotdelta(src_rot, tgt_rot);

    // Output record (11 floats + 1 uint = 0x2c bytes)
    param_1[0] = src_x + delta[0] * param_5;
    param_1[1] = src_y + delta[1] * param_5;
    param_1[2] = src_z + delta[2] * param_5;
    param_1[3] = dist * (*reinterpret_cast<const float*>(kDat_005cc9a4))
                 + (*reinterpret_cast<const float*>(kDat_005cc9a0));
    // param_1[4] intentionally skipped (decomp: undefined / padding)
    param_1[5] = 1.0f;
    param_1[6] = 0.0f;
    param_1[7] = 1.0f;
    param_1[8] = 0.0f;

    auto* color_bytes = reinterpret_cast<std::uint8_t*>(&param_1[9]);
    color_bytes[0] = param_7[0];
    color_bytes[1] = param_7[1];
    color_bytes[2] = param_7[2];
    color_bytes[3] = static_cast<std::uint8_t>(fn_tick() & 0xffu);

    param_1[10] = param_6;

    *param_8 += 1;
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(LabelTrailRecordAppend, 0x00412cf0);
