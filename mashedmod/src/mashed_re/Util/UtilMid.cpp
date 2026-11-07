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
//   (0x00442c80 ModeGatedPlayerCheck: deleted dcleaf 2026-10-22; the one body is
//    Vehicle/ForceIntegratorLeaves.cpp:RubberBandGate, installed through RubberBandGateEntry)
//   (0x004950b0 QpcTimeScaledTo3Mhz: MATCH-FIX-4950B0, body in Match/MatchS16_004950b0.cpp)
//   0x004b302f  StricmpThunk               (104b, library __stricmp passthrough)
//   0x00429aa0  GameStateSlotsFill         (134b, predicate-gated slot fill; re-ported 2026-10-12)
//   0x0041d730  PlayerSlotConfigInit       (225b, config-pair slot init loop)
//   (0x00492d30 GameTickStateMachine7: MATCH-FIX-492D30, body in Match/MatchS15_00492d30.cpp)
//   0x004295a0  HudDualLabelRender         (125b, two HUD label calls; re-ported 2026-10-12)
//   0x00442440  TransformMatrixUpdate      (~390b, 4x4 matrix copy + accumulate)
//   (0x0043c000 TimerSlotTickDispatcher: MATCH-S08, body in Match/MatchS08_0043c000.cpp)
//   0x00475a60  PendingOpQueueFlush        (74b, queue drain)
//   0x00410860  ScoreThresholdStateCheck   (591b, score/timeout state-machine)
//   0x00412cf0  LabelTrailRecordAppend     (312b, lerp+color trail record)
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"

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

// MATCH-FIX-4950B0: the body of 0x004950b0 (QpcTimeScaledTo3Mhz) moved to Match/MatchS16_004950b0.cpp,
// which compiles the byte-matched source re/match/s16/004950b0.c (signed __int64 math, no zero-freq guard).
// Its install stays off there (MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred).

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

// MATCH-S06: the body of 0x00429aa0 (GameStateSlotsFill) moved to Match/MatchS06_00429aa0.cpp,
// which compiles the byte-matched source re/match/s06/00429aa0.c.

// MATCH-S04: the body of 0x0041d730 (PlayerSlotConfigInit) moved to Match/MatchS04_0041cdb0.cpp,
// which compiles the byte-matched source re/match/s04/0041cdb0.c (it also holds 0x0041cdb0).
// Its install stays off there (MASS-DISABLED 2026-05-24 needs-canonical-slot-state).

// MATCH-FIX-492D30: the body of 0x00492d30 (GameTickStateMachine7) moved to Match/MatchS15_00492d30.cpp,
// which compiles the byte-matched source re/match/s15/00492d30.c (install still mass-disabled there).

// MATCH-S06: the body of 0x004295a0 (HudDualLabelRender) moved to Match/MatchS06_004295a0.cpp,
// which compiles the byte-matched source re/match/s06/004295a0.c.

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

static void TransformMatrixUpdate_impl(int param_1) {
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

// ─── 0x00442440 shadow A/B (TT-11) ──────────────────────────────────────────────────
// Promoted through the in-process shadow lane because the FRIDA lane cannot diff it:
// D-10793 refused it at C3 with "harness has no struct_ptr arg_type for matrices". Here the
// argument is passed by the compiler at the real call site, so no arg_type is needed at all.
//
// OUTPUT REGION. Everything this function mutates lives in one contiguous span of the caller
// struct, so a single snapshot covers it:
//   param_1+0x4c .. +0x8b   the 16-float matrix (written in step 1, then accumulated into by
//                           RwMatrixTranslate 0x004c51a0 / RwMatrixRotate 0x004c4d20)
//   param_1+0xc0, +0xc8     the two int state fields (steps 3)
// 0x4c + 0x80 = 0xcc covers all three; the 0x8c..0xbf gap is untouched by both sides.
//
// SAFE TO DOUBLE-EXECUTE: the sampled call runs the original AND our port. The other two
// callees are pure getters - 0x0040dc90 UtilSlotIndexCondGet (C4) and 0x0046d4a0
// PtrCompute881ec8 (C3) - and the two matrix ops write only inside the snapshotted span,
// so nothing escapes the restore.
extern "C" __declspec(dllexport) void __cdecl TransformMatrixUpdate(int param_1) {
    SHADOW_AB_COUNTER(ab, "TransformMatrixUpdate", 0x00442440u, ShadowAB::kPhaseRace);
    ShadowAB::RunRegion(ab, TransformMatrixUpdate_impl,
                        reinterpret_cast<void*>(param_1 + 0x4c), 0x80, param_1);
}
// STILL DISABLED 2026-09-09 (TT-11 shadow A/B REFUSED the promotion, see D-10793):
// the port does NOT reproduce the original. 48/48 sampled in-race calls diverged, always at
// mat[12] pos.x and mat[14] pos.z, while mat[13] pos.y matched in all 48. (The other three
// differing fields -- mat[7], mat[11], mat[15] -- are RwMatrix PADs, which this project
// excludes from bit-identity by convention.) Re-enable only to work the defect.
// RH_ScopedInstall(TransformMatrixUpdate, 0x00442440);

namespace {
constexpr std::uintptr_t kFn_004a2c48_RVA = 0x004a2c48;  // used by 0x00410860 / 0x00412cf0 below
} // namespace

// MATCH-S08: the body of 0x0043c000 (TimerSlotTickDispatcher) moved to Match/MatchS08_0043c000.cpp,
// which compiles the byte-matched source re/match/s08/0043c000.c.

// MATCH-S12: the body of 0x00475a60 (PendingOpQueueFlush) moved to Match/MatchS12_00475a60.cpp,
// which compiles the byte-matched source re/match/s12/00475a60.c.

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
constexpr std::uintptr_t kDat_007f0fd0   = 0x007f0fd0;   // (was shared with the deleted ModeGatedPlayerCheck block, dcleaf)
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
// B-0003 FIX 2026-09-01: was `typedef void (__cdecl*)(float*, float*)`. 0x004726f0 returns a
// float in ST0 (its own C3 reimpl, PromoLoop_sessionB.cpp Dot4726f0, is declared `float` and
// every exit path leaves one value on the stack). Calling it through a VOID fn-ptr never emits
// the FSTP, so each call leaked one x87 register -- the same class that overflowed the 8-deep
// stack and froze the sim in the ai lane ([[x87-st0-float10-fnptr-void-leak]]). The ORIGINAL
// also discards the result (0x00412cf0 decomp: `FUN_004726f0(&local_c,&local_18);` as a
// statement), but a compiler discarding a float return emits the pop; a void-typed indirect
// call does not. Declared float and the value explicitly discarded below so the pop is emitted.
typedef float (__cdecl* Fn_004726f0_t)(float*, float*);
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
    // Result is genuinely unused here -- the original discards it too (0x00412cf0 decomp calls
    // FUN_004726f0 as a bare statement). Bound to a variable rather than called bare so the
    // float return is popped off the x87 stack; see the Fn_004726f0_t note above (B-0003).
    const float rot_delta_unused = fn_rotdelta(src_rot, tgt_rot);
    (void)rot_delta_unused;

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
