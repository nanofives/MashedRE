// Mashed RE — Render command-buffer + video-display leaf reimplementations
//             (c3-batch-o session 4).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x004cd060  AllocatorSlotGet           — 10B getter; returns DAT_007d3ff8+0x108
//   0x004cc7f0  RwFreeListCreateWrapper    — 32B wrapper; forwards 4 args to FUN_004cc820 with literals 1/0
//   0x004cd140  RwRenderCommandBufferReset — 46B; Im3D staging-block clear (named)
//   0x0042b890  RenderWidthSet             — 11B setter; DAT_0067ea54 render-width copy (16-bit)
//   0x0042b8a0  RenderHeightSet            — 11B setter; DAT_0067ea56 render-height copy (16-bit)
//
// Analysis notes:
//   re/analysis/promote_c2_rw_render_submit/004cd060.md
//   re/analysis/promote_c2_rw_render_submit/004cc7f0.md
//   re/analysis/promote_c2_rw_render_submit/004cd140.md
//   re/analysis/promote_c2_video_display/0042b890.md
//   re/analysis/promote_c2_video_display/0042b8a0.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees
// ---------------------------------------------------------------------------

// 0x004cc820  FUN_004cc820  — C2 (RwFreeList create / pool allocator, 6-param form)
// Exact signature not fully determined; called with 6 args.
static auto* const s_FUN_004cc820 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, std::uint32_t,
                                    std::uint32_t, std::uint32_t,
                                    std::uint32_t, std::uint32_t)>(0x004cc820);

// ---------------------------------------------------------------------------
// AllocatorSlotGet  --  0x004cd060
//
// Original: FUN_004cd060 (10 bytes, 0x004cd060..0x004cd069)
// Signature: void* FUN_004cd060(void) — no parameters
// Returns: DAT_007d3ff8 + 0x108 (address of the RW allocator malloc vtable slot)
//
// Body (pure address computation, no branches):
//   return (void*)(DAT_007d3ff8 + 0x108);  // [0x004cd060]
//
// Constants (cited from 0x004cd060 body):
//   0x007d3ff8 — RW allocator globals base address   [0x004cd060]
//   0x108 (264)— offset to malloc function pointer   [0x004cd060]
//
// Note: analysis note U-3740 flags that the "RW render-prim sibling" label in
//   the batch prompt is address-proximity inference; this function is actually
//   an allocator-slot getter. No structural [UNCERTAIN] markers.
//
// ref: re/analysis/promote_c2_rw_render_submit/004cd060.md
// ---------------------------------------------------------------------------

// 0x004cd060
extern "C" __declspec(dllexport) void* __cdecl AllocatorSlotGet()
{
    // Read base from DAT_007d3ff8 and add the malloc-slot offset 0x108. [0x004cd060]
    std::uint32_t base = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    return reinterpret_cast<void*>(base + 0x108u);
}

RH_ScopedInstall(AllocatorSlotGet, 0x004cd060);

// ---------------------------------------------------------------------------
// RwFreeListCreateWrapper  --  0x004cc7f0
//
// Original: FUN_004cc7f0 (32 bytes, 0x004cc7f0..0x004cc80f)
// Signature: void FUN_004cc7f0(uint32_t param_1, uint32_t param_2,
//                               uint32_t param_3, uint32_t param_4)
//   param_1, param_2, param_3, param_4: forwarded to FUN_004cc820
// Returns: void
//
// Body (single unconditional call, no branches):
//   FUN_004cc820(param_1, param_2, param_3, 1, 0, param_4);  [0x004cc7f7/fa]
//
// Constants (cited from 0x004cc7f0 body):
//   0x1 (1) — injected as 4th arg to FUN_004cc820  [0x004cc7f7]
//   0x0 (0) — injected as 5th arg to FUN_004cc820  [0x004cc7fa]
//
// Callee: FUN_004cc820 (C2, pool allocator 6-param form).
//
// ref: re/analysis/promote_c2_rw_render_submit/004cc7f0.md
// ---------------------------------------------------------------------------

// 0x004cc7f0
extern "C" __declspec(dllexport) void __cdecl RwFreeListCreateWrapper(
    std::uint32_t param_1, std::uint32_t param_2,
    std::uint32_t param_3, std::uint32_t param_4)
{
    // Forward to FUN_004cc820 with literal 1 at arg4 and literal 0 at arg5.
    // [0x004cc7f7]: literal 1 injected; [0x004cc7fa]: literal 0 injected.
    s_FUN_004cc820(param_1, param_2, param_3, 1u, 0u, param_4);
}

RH_ScopedInstall(RwFreeListCreateWrapper, 0x004cc7f0);

// ---------------------------------------------------------------------------
// RwRenderCommandBufferReset  --  0x004cd140
//
// Original: FUN_004cd140 (46 bytes, 0x004cd140..0x004cd16d)
// Ghidra name: RwRenderCommandBufferReset (named)
// Signature: int FUN_004cd140(void) — no parameters
// Returns: 0 if no staged handle (early exit); 1 after clearing staging block.
//
// Body:
//   1. Read handle field: iVar1 = *(int*)(DAT_00911d00 + 0x3c + DAT_007d3ff8)  [0x004cd140]
//   2. If iVar1 == 0: return 0.                                                  [guard]
//   3. Else: puVar2 = (undefined4*)(DAT_00911d00 + 0x38 + DAT_007d3ff8)         [0x004cd155]
//   4. Loop iVar1 = 0xf (15) down to 0: *puVar2++ = 0;                          [0x004cd14a]
//      — zeroes 15 × 4 = 60 bytes starting at +0x38.
//   5. Return 1.
//
// Constants (cited from 0x004cd140 body):
//   0x007d3ff8 — RW globals base                              [0x004cd140]
//   0x00911d00 — Im3D context / staging-block root           [0x004cd140]
//   0x3c (60)  — handle field offset (+0x3c from base+root)  [0x004cd140]
//   0x38 (56)  — staging-block base offset (+0x38)           [0x004cd155]
//   0xf (15)   — loop count = 15 DWORDs = 60 bytes           [0x004cd14a]
//
// STOP-AND-ASK rule: if this diff is RED (live-state side effect — it modifies
//   the active Im3D staging block), defer; do not try to fix.
//
// ref: re/analysis/promote_c2_rw_render_submit/004cd140.md
// ---------------------------------------------------------------------------

// 0x004cd140
extern "C" __declspec(dllexport) std::int32_t __cdecl RwRenderCommandBufferReset()
{
    // Compute shared base: DAT_00911d00 + DAT_007d3ff8. [0x004cd140]
    std::uint32_t rw_base  = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    std::uint32_t ctx_base = *reinterpret_cast<std::uint32_t*>(0x00911d00u);
    std::uint32_t combined = ctx_base + rw_base;

    // Read handle field at +0x3c. If zero: nothing staged, return 0. [guard]
    std::int32_t handle = *reinterpret_cast<std::int32_t*>(combined + 0x3cu);
    if (handle == 0) {
        return 0;
    }

    // Zero 15 DWORDs starting at +0x38 (60 bytes = staging block). [0x004cd155/14a]
    auto* puVar2 = reinterpret_cast<std::uint32_t*>(combined + 0x38u);
    for (std::int32_t i = 0xf; i > 0; --i) {
        *puVar2++ = 0u;
    }

    return 1;
}

RH_ScopedInstall(RwRenderCommandBufferReset, 0x004cd140);

// ---------------------------------------------------------------------------
// RenderWidthSet  --  0x0042b890
//
// Original: FUN_0042b890 (11 bytes, 0x0042b890..0x0042b89a)
// Signature: void FUN_0042b890(undefined2 param_1)
//   param_1: render width in pixels (low 16 bits used; caller passes uint16_t)
// Returns: void
//
// Body (pure 16-bit store, no branches):
//   DAT_0067ea54 = (uint16_t)param_1;  // [0x0042b890]
//
// Constants (cited from 0x0042b890 body):
//   0x0067ea54 — 2-byte render-width copy global  [0x0042b890]
//
// Context: caller FUN_004921d0 (DisplayInit, C2) passes uVar2 — the return
//   value of FUN_00498bc0 (render width from DAT_00616028).
//   Paired sibling: FUN_0042b8a0 writes DAT_0067ea56 (render height).
//   Together they form a {width, height} uint16_t[2] at 0x0067ea54.
//
// ref: re/analysis/promote_c2_video_display/0042b890.md
// ---------------------------------------------------------------------------

// 0x0042b890
extern "C" __declspec(dllexport) void __cdecl RenderWidthSet(std::uint16_t param_1)
{
    // Write low 16 bits of param_1 to DAT_0067ea54 (render-width copy). [0x0042b890]
    *reinterpret_cast<std::uint16_t*>(0x0067ea54u) = param_1;
}

RH_ScopedInstall(RenderWidthSet, 0x0042b890);

// ---------------------------------------------------------------------------
// RenderHeightSet  --  0x0042b8a0
//
// Original: FUN_0042b8a0 (11 bytes, 0x0042b8a0..0x0042b8aa)
// Signature: void FUN_0042b8a0(undefined2 param_1)
//   param_1: render height in pixels (low 16 bits used; caller passes uint16_t)
// Returns: void
//
// Body (pure 16-bit store, no branches):
//   DAT_0067ea56 = (uint16_t)param_1;  // [0x0042b8a0]
//
// Constants (cited from 0x0042b8a0 body):
//   0x0067ea56 — 2-byte render-height copy global (= 0x0067ea54 + 2)  [0x0042b8a0]
//
// Context: caller FUN_004921d0 (DisplayInit, C2) passes uVar3 — the return
//   value of FUN_00498bd0 (render height from DAT_0061602c).
//   Paired sibling: FUN_0042b890 writes DAT_0067ea54 (render width).
//   Together they form a {width, height} uint16_t[2] at 0x0067ea54.
//
// ref: re/analysis/promote_c2_video_display/0042b8a0.md
// ---------------------------------------------------------------------------

// 0x0042b8a0
extern "C" __declspec(dllexport) void __cdecl RenderHeightSet(std::uint16_t param_1)
{
    // Write low 16 bits of param_1 to DAT_0067ea56 (render-height copy). [0x0042b8a0]
    *reinterpret_cast<std::uint16_t*>(0x0067ea56u) = param_1;
}

RH_ScopedInstall(RenderHeightSet, 0x0042b8a0);
