// Mashed RE — Render cluster C2->C3 promotions (c3-batch-ab session s1).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x004d40c0  GetPipelinePtr         — 5B pure getter; returns DAT_007d4710
//   0x004671c0  GetOverlayCamera       — 5B pure getter; returns DAT_006905b4
//   0x004cbb60  GetRenderStateBlockPtr — 6B pure getter; returns &DAT_00911fa0
//
// Deferred this session:
//   0x004039e0  FUN_004039e0  — returns float10 (x87 80-bit extended);
//                               no Frida NativeFunction arg_type handles
//                               80-bit ST0 return; cannot produce
//                               bit-identical synthetic diff.
//   0x004cbb70  FUN_004cbb70  — FrustumTestSphere: takes two struct pointers
//                               (frustum context + sphere[4] float);
//                               no matching arg_type in diff_template.js.
//
// Analysis refs:
//   re/analysis/render_5_c1_to_c2_s6/004d40c0.md
//   re/analysis/render_c1_to_c2_s3/FUN_004671c0.md
//   re/analysis/render_4_c1_to_c2_s6/FUN_004cbb60.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetPipelinePtr — 0x004d40c0
//
// Original: FUN_004d40c0 (5 bytes, 0x004d40c0..0x004d40c5)
// Signature: undefined4 FUN_004d40c0(void)
// Body (single instruction): return DAT_007d4710.
//
// Key address:
//   0x007d4710 — global RW pipeline pointer (set by FUN_004d3db0)
//              — cited from 0x004d40c0 body (MOV EAX, [DAT_007d4710])
//
// Callers:
//   FUN_004cd2d0 (RwRenderIndexedSubmit, C2)
//   FUN_004cd430 (C2)
//
// Leaf-function exemption applies (no callees; re/CONFIDENCE.md §C2→C3).
// ref: re/analysis/render_5_c1_to_c2_s6/004d40c0.md
// ---------------------------------------------------------------------------

// 0x004d40c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetPipelinePtr(void)
{
    // Load global RW pipeline pointer [0x004d40c0: MOV EAX, [0x007d4710]]
    return *reinterpret_cast<const std::uint32_t*>(0x007d4710u);
}

RH_ScopedInstall(GetPipelinePtr, 0x004d40c0);

// ---------------------------------------------------------------------------
// GetOverlayCamera — 0x004671c0
//
// Original: FUN_004671c0 (5 bytes, 0x004671c0..0x004671c5)
// Signature: undefined4 FUN_004671c0(void)
// Body (single instruction): return DAT_006905b4.
//
// Key address:
//   0x006905b4 — overlay/minimap RwCamera* global
//              — cited from 0x004671c0 body (MOV EAX, [DAT_006905b4])
//
// Caller:
//   FUN_00492e90 (per-frame render loop, C2+)
//
// Leaf-function exemption applies (no callees; re/CONFIDENCE.md §C2→C3).
// ref: re/analysis/render_c1_to_c2_s3/FUN_004671c0.md
// ---------------------------------------------------------------------------

// 0x004671c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetOverlayCamera(void)
{
    // Load overlay/minimap RwCamera* global [0x004671c0: MOV EAX, [0x006905b4]]
    return *reinterpret_cast<const std::uint32_t*>(0x006905b4u);
}

RH_ScopedInstall(GetOverlayCamera, 0x004671c0);

// ---------------------------------------------------------------------------
// GetRenderStateBlockPtr — 0x004cbb60
//
// Original: FUN_004cbb60 (6 bytes, 0x004cbb60..0x004cbb65)
// Signature: undefined4* FUN_004cbb60(void)
// Body (single instruction): return &DAT_00911fa0.
//
// Key address:
//   0x00911fa0 — render-state block global (callers store ptr and
//                dereference at +0xC4/+0xCC per C1 note)
//              — cited from 0x004cbb60 body (LEA EAX, [DAT_00911fa0])
//
// Callers (6):
//   FUN_004951f0, FUN_004f46a0, FUN_00530c00, FUN_005327d0,
//   FUN_00543710, FUN_00549970
//
// Leaf-function exemption applies (no callees; re/CONFIDENCE.md §C2→C3).
// ref: re/analysis/render_4_c1_to_c2_s6/FUN_004cbb60.md
// ---------------------------------------------------------------------------

// 0x004cbb60
extern "C" __declspec(dllexport) std::uint32_t* __cdecl GetRenderStateBlockPtr(void)
{
    // Return address of global render-state block [0x004cbb60: LEA EAX, [0x00911fa0]]
    return reinterpret_cast<std::uint32_t*>(0x00911fa0u);
}

RH_ScopedInstall(GetRenderStateBlockPtr, 0x004cbb60);
