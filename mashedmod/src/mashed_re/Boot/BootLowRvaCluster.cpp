// Mashed RE — Boot/BootLowRvaCluster.cpp
// Boot game-code cluster (C2->C3 promotions, session c3-sweep/wave1-s5).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (6):
//   0x004026d0  BootQueueFlush          — param_1-iteration drain-and-refill of a per-slot queue
//   0x00402f50  BootDefaultParamsInit   — 5-write global-defaults setter (20-byte cluster at 0x636ae8)
//   0x004114c0  VtableTeardown_114c0    — vtable-dispatched teardown call; hard-coded return 1
//   0x00431ae0  DefaultParam_SetField04 — writes float 0.7f to global 0x007f0f04
//   0x00431af0  DefaultParam_SetField08 — writes float 0.7f to global 0x007f0f08
//   0x00431b00  DefaultParam_SetField00 — writes float 0.7f to global 0x007f0f00
//
// Analysis notes:
//   re/analysis/promote_c2_boot_lowrva/0x004026d0.md
//   re/analysis/promote_c2_boot_lowrva/0x00402f50.md
//   re/analysis/promote_c2_boot_lowrva/0x004114c0.md
//   re/analysis/promote_c2_boot_lowrva/0x00431ae0.md
//   re/analysis/promote_c2_boot_lowrva/0x00431af0.md
//   re/analysis/promote_c2_boot_lowrva/0x00431b00.md
//
// Note on menu-fire status:
//   These may not fire at quiescent main-menu state (PIZ asset loaders and
//   queue-drain helpers are state-dependent). They are authored here for C3
//   promotion; Wave 2 will determine which ones fire at the menu.

#include "../Core/HookSystem.h"
#include <cstdint>


// ============================================================================
// Callee function-pointer declarations
// (File-static; no link-time collision with other TUs that declare the same.)
// ============================================================================

// 0x004671a0 — multi-arity handle accessor (varargs-style cdecl).
// Called with 3 args: (slot, payload_ptr, mode)
// Called with 1 arg:  (slot)
// Called with 2 args: (slot, payload_ptr)  — not used in this TU but typedef kept for clarity
// Cited at 0x004026d0 body (all three arity variants).
using FUN_004671a0_3arg_t = std::uint32_t (__cdecl*)(std::uint32_t, void*, std::uint32_t);
using FUN_004671a0_1arg_t = std::uint32_t (__cdecl*)(std::uint32_t);
static FUN_004671a0_3arg_t const s_FUN_004671a0_3 =
    reinterpret_cast<FUN_004671a0_3arg_t>(0x004671a0u);
static FUN_004671a0_1arg_t const s_FUN_004671a0_1 =
    reinterpret_cast<FUN_004671a0_1arg_t>(0x004671a0u);

// 0x004c1bb0 — RW cam op: commit/upload; void(uint32 handle).
// Stub S-0172. Cited at 0x004026d0 body (first inner call after mode-3 alloc).
using FUN_004c1bb0_t = void (__cdecl*)(std::uint32_t);
static FUN_004c1bb0_t const s_FUN_004c1bb0 =
    reinterpret_cast<FUN_004c1bb0_t>(0x004c1bb0u);

// 0x004c1a00 — RW cam op: query state; int32(uint32 handle). Non-zero = needs flush.
// Stub S-0173. Cited at 0x004026d0 body.
using FUN_004c1a00_t = std::int32_t (__cdecl*)(std::uint32_t);
static FUN_004c1a00_t const s_FUN_004c1a00 =
    reinterpret_cast<FUN_004c1a00_t>(0x004c1a00u);

// 0x004c19f0 — RW cam op: flush/discard; void(uint32 handle).
// Stub S-0174. Cited at 0x004026d0 body (conditional flush branch).
using FUN_004c19f0_t = void (__cdecl*)(std::uint32_t);
static FUN_004c19f0_t const s_FUN_004c19f0 =
    reinterpret_cast<FUN_004c19f0_t>(0x004c19f0u);

// 0x004c1be0 — RW cam op: finalize; void(uint32 handle).
// Stub S-0175. Cited at 0x004026d0 body (after mode-1 alloc).
using FUN_004c1be0_t = void (__cdecl*)(std::uint32_t);
static FUN_004c1be0_t const s_FUN_004c1be0 =
    reinterpret_cast<FUN_004c1be0_t>(0x004c1be0u);


// ============================================================================
// BootQueueFlush  --  0x004026d0
//
// Original: FUN_004026d0 (119 bytes). `void FUN_004026d0(int param_1)`.
//
// Body (verbatim from analysis note 0x004026d0.md):
//   local_4 = 0xff000000;                         // opaque-black ARGB scratch
//   if (0 < param_1) {
//       do {
//           uVar1 = FUN_004671a0(0, &local_4, 3);  // alloc-or-fetch, mode=3, payload=&local_4
//           FUN_004c1bb0(uVar1);                   // commit/upload
//           uVar1 = FUN_004671a0(0);               // re-fetch handle
//           iVar2 = FUN_004c1a00(uVar1);           // query state
//           if (iVar2 != 0) {
//               uVar1 = FUN_004671a0(0);
//               FUN_004c19f0(uVar1);               // flush/discard
//           }
//           uVar1 = FUN_004671a0(0, 0, 1);         // alloc-or-fetch, mode=1, payload=0
//           FUN_004c1be0(uVar1);                   // finalize
//           param_1--;
//       } while (param_1 != 0);
//   }
//   return;
//
// Constants cited (from analysis table):
//   0x004026d0 decomp  0xff000000  — local_4 init (raw u32; meaning [UNCERTAIN U-0170])
//   0x004026d0 decomp  0           — first arg to FUN_004671a0 (all 3 call sites)
//   0x004026d0 decomp  3           — mode arg to first FUN_004671a0
//   0x004026d0 decomp  1           — mode arg to last FUN_004671a0
//
// Stubs: S-0171 (FUN_004671a0), S-0172 (FUN_004c1bb0), S-0173 (FUN_004c1a00),
//        S-0174 (FUN_004c19f0), S-0175 (FUN_004c1be0).
// [UNCERTAIN U-0170]: whether 0xff000000 is a color, mask, or arbitrary u32.
// ============================================================================

// 0x004026d0
extern "C" __declspec(dllexport)
void __cdecl BootQueueFlush(std::int32_t param_1)
{
    // 0x004026d0 decomp: local_4 = 0xff000000 [UNCERTAIN U-0170]
    std::uint32_t local_4 = 0xff000000u;

    // 0x004026d0 decomp: if (0 < param_1) do { ... } while (param_1 != 0);
    if (0 < param_1) {
        do {
            // uVar1 = FUN_004671a0(0, &local_4, 3)
            std::uint32_t uVar1 = s_FUN_004671a0_3(
                0u,
                reinterpret_cast<void*>(&local_4),
                3u);

            // FUN_004c1bb0(uVar1) — commit/upload (Stub S-0172)
            s_FUN_004c1bb0(uVar1);

            // uVar1 = FUN_004671a0(0) — re-fetch handle
            uVar1 = s_FUN_004671a0_1(0u);

            // iVar2 = FUN_004c1a00(uVar1) — query state (Stub S-0173)
            const std::int32_t iVar2 = s_FUN_004c1a00(uVar1);

            if (iVar2 != 0) {
                // uVar1 = FUN_004671a0(0); FUN_004c19f0(uVar1) — flush (Stub S-0174)
                uVar1 = s_FUN_004671a0_1(0u);
                s_FUN_004c19f0(uVar1);
            }

            // uVar1 = FUN_004671a0(0, 0, 1) — mode-1 alloc; FUN_004c1be0(uVar1) (Stub S-0175)
            uVar1 = s_FUN_004671a0_3(0u, nullptr, 1u);
            s_FUN_004c1be0(uVar1);

            param_1--;
        } while (param_1 != 0);
    }
}

RH_ScopedInstall(BootQueueFlush, 0x004026d0);  // re-enabled 2026-05-24 phase-a2 GREEN int_scalar(count=0)


// ============================================================================
// BootDefaultParamsInit  --  0x00402f50
//
// Original: FUN_00402f50 (47 bytes). `void FUN_00402f50(void)`.
// Pure 5-write global-defaults setter. No callees.
//
// Body (verbatim from analysis note 0x00402f50.md):
//   _DAT_00636ae8 = 0;           // cluster+0
//   _DAT_00636af0 = 0;           // cluster+8
//   DAT_00636af8  = 0;           // cluster+16
//   _DAT_00636af4 = 0x42200000;  // cluster+0xc  (float 40.0f)
//   _DAT_00636aec = 0x42700000;  // cluster+4    (float 60.0f)
//
// Constants cited:
//   0x00402f50 decomp  0x00636ae8  — cluster dword 0 target
//   0x00402f50 decomp  0x00636aec  — cluster dword 1 target (float 60.0f)
//   0x00402f50 decomp  0x00636af0  — cluster dword 2 target
//   0x00402f50 decomp  0x00636af4  — cluster dword 3 target (float 40.0f)
//   0x00402f50 decomp  0x00636af8  — cluster dword 4 target
//   0x00402f50 decomp  0x42200000  — IEEE 754 float 40.0f
//   0x00402f50 decomp  0x42700000  — IEEE 754 float 60.0f
//
// [UNCERTAIN U-0171]: semantic of the 40.0f / 60.0f pair.
// No stubs (zero callees).
// ============================================================================

// Globals — 20-byte cluster at 0x00636ae8
// struct Cluster_0x636ae8 { int field_00; float field_04; int field_08; float field_0c; int field_10; }
static std::uint32_t* const g_DAT_00636ae8 =
    reinterpret_cast<std::uint32_t*>(0x00636ae8u);  // cluster+0  ← 0
static std::uint32_t* const g_DAT_00636aec =
    reinterpret_cast<std::uint32_t*>(0x00636aecu);  // cluster+4  ← 0x42700000 (float 60.0f)
static std::uint32_t* const g_DAT_00636af0 =
    reinterpret_cast<std::uint32_t*>(0x00636af0u);  // cluster+8  ← 0
static std::uint32_t* const g_DAT_00636af4 =
    reinterpret_cast<std::uint32_t*>(0x00636af4u);  // cluster+0xc ← 0x42200000 (float 40.0f)
static std::uint32_t* const g_DAT_00636af8 =
    reinterpret_cast<std::uint32_t*>(0x00636af8u);  // cluster+0x10 ← 0

// 0x00402f50
extern "C" __declspec(dllexport)
void __cdecl BootDefaultParamsInit()
{
    // 0x00402f50 decomp: writes in source order (+0, +8, +0x10, +0xc, +4)
    *g_DAT_00636ae8 = 0u;                   // cluster+0   = 0
    *g_DAT_00636af0 = 0u;                   // cluster+8   = 0
    *g_DAT_00636af8 = 0u;                   // cluster+0x10 = 0
    *g_DAT_00636af4 = 0x42200000u;          // cluster+0xc = 40.0f [UNCERTAIN U-0171]
    *g_DAT_00636aec = 0x42700000u;          // cluster+4   = 60.0f [UNCERTAIN U-0171]
}

RH_ScopedInstall(BootDefaultParamsInit, 0x00402f50);  // re-enabled 2026-05-24 phase-a2 GREEN void_write_observe


// ============================================================================
// VtableTeardown_114c0  --  0x004114c0
//
// Original: FUN_004114c0 (26 bytes). `undefined4 FUN_004114c0(void)`.
// Single vtable-dispatched call; hard-coded return 1.
//
// Body (verbatim from analysis note 0x004114c0.md):
//   (*(*(DAT_007d3ff8 + 0x10c)))(DAT_008a94a8);   // vtable call: obj@DAT_007d3ff8, slot +0x10c
//   return 1;
//
// Constants cited:
//   0x004114c0 decomp  0x007d3ff8  — global holding vtable-pointer
//   0x004114c0 decomp  0x10c       — vtable byte offset (dword slot 67)
//   0x004114c0 decomp  0x008a94a8  — argument global passed to the vtable method
//   0x004114c0 decomp  1           — hard-coded return value
//
// No stubs. Vtable slot +0x10c semantics unknown; recorded as teardown-side
// dispatch per C1 plate annotation. No new uncertainties.
// ============================================================================

// Globals for VtableTeardown_114c0
// DAT_007d3ff8 — pointer-to-vtable; reading *DAT_007d3ff8 gives the vtable base pointer.
static void** const g_DAT_007d3ff8 =
    reinterpret_cast<void**>(0x007d3ff8u);
// DAT_008a94a8 — argument passed to the vtable method (opaque global handle).
static void* const* const g_DAT_008a94a8 =
    reinterpret_cast<void* const*>(0x008a94a8u);

// Vtable-method typedef: void(__cdecl*)(void* arg) — observed signature from body.
using VtableSlot10c_t = void (__cdecl*)(void*);

// 0x004114c0
extern "C" __declspec(dllexport)
std::int32_t __cdecl VtableTeardown_114c0()
{
    // 0x004114c0 decomp: (*(*(DAT_007d3ff8 + 0x10c)))(DAT_008a94a8)
    // Step 1: load vtable base from *DAT_007d3ff8
    std::uintptr_t vtable_base =
        *reinterpret_cast<std::uintptr_t*>(
            *reinterpret_cast<std::uintptr_t*>(0x007d3ff8u));

    // Step 2: index the vtable at byte offset +0x10c (= dword slot 67)
    VtableSlot10c_t fn = *reinterpret_cast<VtableSlot10c_t*>(vtable_base + 0x10cu);

    // Step 3: call with DAT_008a94a8 as the argument
    fn(*reinterpret_cast<void**>(0x008a94a8u));

    // 0x004114c0 decomp: return 1 (hard-coded teardown-succeeded signal)
    return 1;
}

// MASS-DISABLED 2026-05-24 needs-canonical-vtable-state: RH_ScopedInstall(VtableTeardown_114c0, 0x004114c0);
// Phase A2 audit 2026-05-24: function derefs *DAT_007d3ff8 (renderer vtable
// global). At diff-attach time the global may be NULL or stale; calling
// dereferences uninitialised state. Canonical-scenario at teardown is the
// proper validation. NO-ENTRY in registry — synthetic test infeasible.


// ============================================================================
// Boot_DefaultParams_007f0f00 cluster — 3 siblings
//
// struct Boot_DefaultParams_007f0f00 {   // 16-byte cluster; 4 floats, each set to 0.7f
//     float field_00;   // 0x007f0f00 (set by DefaultParam_SetField00 — 0x00431b00)
//     float field_04;   // 0x007f0f04 (set by DefaultParam_SetField04 — 0x00431ae0)
//     float field_08;   // 0x007f0f08 (set by DefaultParam_SetField08 — 0x00431af0)
//     float field_0c;   // 0x007f0f0c (set by 0x00431b10 — out of this batch)
// };
//
// [UNCERTAIN]: semantic of the 0.7f default for all fields
//   (could be friction, damping, threshold, lerp factor, etc.).
//
// Cited constants for all three:
//   0x3f333333  — IEEE 754 float 0.7f (raw = 1060320051 u32)
//
// No stubs for any of the three (zero callees each).
// ============================================================================

// Globals — Boot_DefaultParams_007f0f00
static std::uint32_t* const g_DAT_007f0f00 =
    reinterpret_cast<std::uint32_t*>(0x007f0f00u);  // field_00
static std::uint32_t* const g_DAT_007f0f04 =
    reinterpret_cast<std::uint32_t*>(0x007f0f04u);  // field_04
static std::uint32_t* const g_DAT_007f0f08 =
    reinterpret_cast<std::uint32_t*>(0x007f0f08u);  // field_08

// ============================================================================
// DefaultParam_SetField04  --  0x00431ae0
//
// Original: FUN_00431ae0 (10 bytes). `void FUN_00431ae0(void)`.
//
// Body:
//   DAT_007f0f04 = 0x3f333333;   // float 0.7f
//   return;
//
// Cited at 0x00431ae0 decomp:
//   0x007f0f04  — global target (Boot_DefaultParams_007f0f00.field_04)
//   0x3f333333  — float 0.7f (raw u32 = 1060320051)
// ============================================================================

// 0x00431ae0
extern "C" __declspec(dllexport)
void __cdecl DefaultParam_SetField04()
{
    // 0x00431ae0 decomp: DAT_007f0f04 = 0x3f333333 (float 0.7f)
    *g_DAT_007f0f04 = 0x3f333333u;
}

RH_ScopedInstall(DefaultParam_SetField04, 0x00431ae0);  // re-enabled 2026-05-24 phase-a2 GREEN void_write_observe (0.7f)


// ============================================================================
// DefaultParam_SetField08  --  0x00431af0
//
// Original: FUN_00431af0 (10 bytes). `void FUN_00431af0(void)`.
//
// Body:
//   DAT_007f0f08 = 0x3f333333;   // float 0.7f
//   return;
//
// Cited at 0x00431af0 decomp:
//   0x007f0f08  — global target (Boot_DefaultParams_007f0f00.field_08)
//   0x3f333333  — float 0.7f
// ============================================================================

// 0x00431af0
extern "C" __declspec(dllexport)
void __cdecl DefaultParam_SetField08()
{
    // 0x00431af0 decomp: DAT_007f0f08 = 0x3f333333 (float 0.7f)
    *g_DAT_007f0f08 = 0x3f333333u;
}

RH_ScopedInstall(DefaultParam_SetField08, 0x00431af0);  // re-enabled 2026-05-24 phase-a2 GREEN void_write_observe (0.7f)


// ============================================================================
// DefaultParam_SetField00  --  0x00431b00
//
// Original: FUN_00431b00 (10 bytes). `void FUN_00431b00(void)`.
//
// Body:
//   DAT_007f0f00 = 0x3f333333;   // float 0.7f
//   return;
//
// Cited at 0x00431b00 decomp:
//   0x007f0f00  — global target (Boot_DefaultParams_007f0f00.field_00, cluster head)
//   0x3f333333  — float 0.7f
// ============================================================================

// 0x00431b00
extern "C" __declspec(dllexport)
void __cdecl DefaultParam_SetField00()
{
    // 0x00431b00 decomp: DAT_007f0f00 = 0x3f333333 (float 0.7f)
    *g_DAT_007f0f00 = 0x3f333333u;
}

RH_ScopedInstall(DefaultParam_SetField00, 0x00431b00);  // re-enabled 2026-05-24 phase-a2 GREEN void_write_observe (0.7f)
