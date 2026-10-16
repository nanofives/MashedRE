// Mashed RE - Frontend pure-leaf reimplementations (c3_batch_ad session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// HARVEST batch c3-batch-ad-s2: 4 viable pure leaves authored of 11 candidates.
// The other 7 were classified SKIP (non-leaf / live-state / needs-new-arg_type)
// and are documented in the PROMOTION_QUEUE row — not reimplemented here.
// refs: re/analysis/frontend_c1_to_c2_s3,s4,s5/

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FUN_00426cb0 (slot index -> pointer into the 0x4c-stride table at 0x00663664) was
// ported here first as FrontendSlotTablePtr426cb0; its one-body-per-RVA copy is
// SlotIndexToPtr in Frontend/MenuNearLeaves_s6.cpp. The duplicate that sat here was
// removed 2026-10-16 (dupinstall).
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426cb0.md
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// FrontendFloatGet426de0  --  0x00426de0
//
// Original: FUN_00426de0   float10 FUN_00426de0(void) { return (float10)_DAT_0064435c; }
// Pure float-getter leaf. Loads the 32-bit float at 0x0064435c and returns it
// in ST0 (the float is widened to 80-bit on the x87 stack by FLD; the __cdecl
// float-return ABI puts the result in ST0, so the reimpl is instruction-
// equivalent).  No callees, no branches.
// ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426de0.md
// ---------------------------------------------------------------------------

// 0x00426de0
extern "C" __declspec(dllexport) float __cdecl FrontendFloatGet426de0(void) {
    return *reinterpret_cast<float*>(0x0064435Cu);
}

RH_ScopedInstall(FrontendFloatGet426de0, 0x00426de0);

// ---------------------------------------------------------------------------
// FrontendFloatGet426df0  --  0x00426df0
//
// Original: FUN_00426df0   float10 FUN_00426df0(void) { return (float10)_DAT_00644360; }
// Companion of FUN_00426de0; reads the next 32-bit float at 0x00644360
// (= 0x0064435c + 4). Same pure-leaf shape.
// ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426df0.md
// ---------------------------------------------------------------------------

// 0x00426df0
extern "C" __declspec(dllexport) float __cdecl FrontendFloatGet426df0(void) {
    return *reinterpret_cast<float*>(0x00644360u);
}

RH_ScopedInstall(FrontendFloatGet426df0, 0x00426df0);

// ---------------------------------------------------------------------------
// FrontendQuadParamInit427580  --  0x00427580
//
// Original: FUN_00427580   void FUN_00427580(void)
//   _DAT_008991c0 = 0x3e660000;                              // raw u32 store
//   _DAT_008991c8 = _DAT_0067d830 * _DAT_005cd5f8;
//   _DAT_008991cc = _DAT_0067d834 * _DAT_005cd5f4;
//   _DAT_008991c4 = _DAT_005cd5f0 - _DAT_0067d834 * _DAT_005cd5f4;
// Pure leaf (no callees, no branches). Writes 4 consecutive globals at
// 0x008991c0..0x008991cc from two .data source floats (0x0067d830/0x0067d834)
// and three .rdata scale constants (0x005cd5f0/f4/f8). Write order preserved
// exactly as decompiled (c0, c8, cc, c4).
// ref: re/analysis/frontend_c1_to_c2_s5/FUN_00427580.md
// ---------------------------------------------------------------------------

// 0x00427580
extern "C" __declspec(dllexport) void __cdecl FrontendQuadParamInit427580(void) {
    *reinterpret_cast<std::uint32_t*>(0x008991C0u) = 0x3E660000u;
    *reinterpret_cast<float*>(0x008991C8u) =
        *reinterpret_cast<float*>(0x0067D830u) * *reinterpret_cast<float*>(0x005CD5F8u);
    *reinterpret_cast<float*>(0x008991CCu) =
        *reinterpret_cast<float*>(0x0067D834u) * *reinterpret_cast<float*>(0x005CD5F4u);
    *reinterpret_cast<float*>(0x008991C4u) =
        *reinterpret_cast<float*>(0x005CD5F0u) -
        *reinterpret_cast<float*>(0x0067D834u) * *reinterpret_cast<float*>(0x005CD5F4u);
}

RH_ScopedInstall(FrontendQuadParamInit427580, 0x00427580);
