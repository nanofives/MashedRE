// Mashed RE — Boot/VideoConfig.cpp
// Video config getters + display dim helpers + RW driver-system wrappers.
//
// Covers:
//   0x00498bc0  VideoGetRenderWidth     — leaf getter for DAT_00616028 (render width)
//   0x00498bd0  VideoGetRenderHeight    — leaf getter for DAT_0061602c (render height)
//   0x00498bf0  DisplayGetCursorGate    — leaf getter for DAT_00773204 (cursor/display gate)
//   0x00498b60  VideoModeArraysFree     — void teardown: frees 3 globals (driver-system clean-up)
//   0x004c2f00  RwEngineGetCurrentMode  — wrapper for driver-system cmd 0x0a
//
// Note: 0x004c2ed0 RwEngineGetModeInfo was deferred from an earlier session
// and is now implemented in Render/RwPluginHelpers_o3.cpp (c3-batch-o-s3).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/promote_c2_launch_handshake/00498bc0.md
//   re/analysis/promote_c2_video_display/00498bd0.md
//   re/analysis/promote_c2_video_display/00498bf0.md
//   re/analysis/promote_c2_video_cfg/00498b60.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2f00.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2ed0.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2c90.md  (dispatcher callee)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// External callee: FUN_004c2c90 — RW driver-system dispatcher (C2 in hooks.csv).
// Signature observed: undefined4 FUN_004c2c90(undefined4 ctx, int cmd,
//                                              undefined4 out, int in1, int in2)
// Used by RwEngineGetCurrentMode and RwEngineGetModeInfo.
// ---------------------------------------------------------------------------
using FUN_004c2c90_t = std::int32_t (__cdecl*)(std::int32_t, std::int32_t,
                                                std::int32_t, std::int32_t,
                                                std::int32_t);
static FUN_004c2c90_t const s_FUN_004c2c90 =
    reinterpret_cast<FUN_004c2c90_t>(0x004c2c90);

// ---------------------------------------------------------------------------
// VideoGetRenderWidth  --  0x00498bc0
//
// Original: FUN_00498bc0 (5 bytes, 0x00498bc0..0x00498bc4).
// Asm: A1 28 60 61 00  MOV EAX, [0x00616028]  /  C3 RET
// Single global read; returns DAT_00616028 (render width in pixels).
// ---------------------------------------------------------------------------

// 0x00498bc0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VideoGetRenderWidth() {
    // 0x00498bc0: MOV EAX, [DAT_00616028] — render width
    return *reinterpret_cast<const std::uint32_t*>(0x00616028u);
}

RH_ScopedInstall(VideoGetRenderWidth, 0x00498bc0);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// VideoGetRenderHeight  --  0x00498bd0
//
// Original: FUN_00498bd0 (5 bytes, 0x00498bd0..0x00498bd4).
// Asm: A1 2C 60 61 00  MOV EAX, [0x0061602c]  /  C3 RET
// Sibling of VideoGetRenderWidth at +4; returns DAT_0061602c (render height).
// ---------------------------------------------------------------------------

// 0x00498bd0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VideoGetRenderHeight() {
    // 0x00498bd0: MOV EAX, [DAT_0061602c] — render height
    return *reinterpret_cast<const std::uint32_t*>(0x0061602cu);
}

RH_ScopedInstall(VideoGetRenderHeight, 0x00498bd0);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// DisplayGetCursorGate  --  0x00498bf0
//
// Original: FUN_00498bf0 (5 bytes, 0x00498bf0..0x00498bf4).
// Asm: A1 04 32 77 00  MOV EAX, [0x00773204]  /  C3 RET
// Returns DAT_00773204 — cursor-visibility / display-active gate flag.
// Caller FUN_004951f0 calls ShowCursor(0) when this returns non-zero.
// ---------------------------------------------------------------------------

// 0x00498bf0
extern "C" __declspec(dllexport) std::uint32_t __cdecl DisplayGetCursorGate() {
    // 0x00498bf0: MOV EAX, [DAT_00773204] — cursor/display gate
    return *reinterpret_cast<const std::uint32_t*>(0x00773204u);
}

RH_ScopedInstall(DisplayGetCursorGate, 0x00498bf0);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// VideoModeArraysFree  --  0x00498b60
//
// Deferred from this C3 session: destructive teardown. At the main menu,
// DAT_00773408, DAT_007731f8, DAT_007731fc are all live pointers (allocated
// by FUN_00498c00 VideoModeTableInit during boot). A 10-call Frida A/B harness
// would free them on call #1 and double-free on call #2, crashing both orig
// and reimpl. Requires a custom save/zero/restore harness that doesn't exist
// today; promotion deferred until VideoModeArraysFree gets a dedicated test
// arg_type, or the function is exercised in a fresh-state scenario.
// Analysis: re/analysis/promote_c2_video_cfg/00498b60.md (C2)
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// RwEngineGetCurrentMode  --  0x004c2f00
//
// Original: FUN_004c2f00 (44 bytes).
// Decompiled body (verbatim from analysis plate):
//   undefined4 FUN_004c2f00(void) {
//     int iVar1;
//     undefined4 local_4;
//     iVar1 = FUN_004c2c90(DAT_007d3ff8 + 0x10, 10, &local_4, 0, 0);
//     if (iVar1 != 0) return local_4;
//     return 0xffffffff;
//   }
// Dispatcher cmd 0x0a — RW 3.x analog: rwDEVICESYSTEMGETMODE.
// Returns current video-mode index, or 0xffffffff on failure.
// ---------------------------------------------------------------------------

// 0x004c2f00
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwEngineGetCurrentMode() {
    // 0x004c2f00 body: load DAT_007d3ff8, add 0x10, pass cmd=10 with &local_4 out.
    std::uint32_t local_4 = 0;
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    const std::int32_t result = s_FUN_004c2c90(
        ctx + 0x10,
        10,
        reinterpret_cast<std::int32_t>(&local_4),
        0,
        0);
    if (result != 0) {
        return local_4;
    }
    return 0xffffffffu;
}

RH_ScopedInstall(RwEngineGetCurrentMode, 0x004c2f00);  // re-enabled 2026-05-24 c3-render-b

// 0x004c2ed0 RwEngineGetModeInfo — implemented in Render/RwPluginHelpers_o3.cpp (c3-batch-o-s3).
