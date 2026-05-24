// Mashed RE — Render frame world-pass dispatch stubs (c3-batch-r session 3).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file (2 promoted, 3 deferred):
//   0x00426670  WorldRenderDispatch_Begin — void(int camera_ptr); selects world A/B then
//                                            calls FUN_004e4320(world_handle, camera_ptr)
//   0x004266b0  WorldRenderDispatch_End   — void(int camera_ptr); mirror of Begin,
//                                            calls FUN_004e4350(world_handle, camera_ptr)
//
// Deferred (not in this file):
//   0x00426030  WorldRenderPrePass        — requires pre-seeded DAT_0063d7e4 (track
//                                            descriptor) to avoid crash in callee
//                                            TrackNodeFieldCmp10; no fitting arg_type
//                                            in diff_template.js for this void(void)
//                                            + global-ptr-seed pattern.  Unblock:
//                                            add `seed_global_ptr_void` arg_type.
//   0x0040de30  MinimapCameraOrthoSetup   — requires valid live RwCamera pointer;
//                                            U-1709 (structural) unresolved.
//   0x0040df20  MinimapCameraRestore      — requires valid live RwCamera pointer;
//                                            paired with 0x0040de30 (same blocker).
//
// Analysis notes:
//   re/analysis/render_promote_c2_track_loader/0x00426670.md
//   re/analysis/render_promote_c2_track_loader/0x004266b0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees
// ---------------------------------------------------------------------------

// 0x004e4320  FUN_004e4320 (C2) — world scene begin dispatcher
// DAT_007d716c[+0xc] slot setter; stores param_1 at slot+0xc conditionally via
// FUN_004c0e50.  Called here with (world_handle, camera_ptr).
static auto* const s_FUN_004e4320 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, std::uint32_t)>(0x004e4320);

// 0x004e4350  FUN_004e4350 (C2) — world scene end dispatcher
// DAT_007d716c[+0xc] slot clearer; clears slot+0xc and slot+8 if non-zero; returns
// param_1 or 0.  Called here with (world_handle, camera_ptr).
static auto* const s_FUN_004e4350 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, std::uint32_t)>(0x004e4350);

// ---------------------------------------------------------------------------
// WorldRenderDispatch_Begin  --  0x00426670
//
// Original: FUN_00426670 (58 bytes, 0x00426670..0x004266a9)
// Signature: void FUN_00426670(undefined4 param_1)
//   param_1: RwCamera pointer (passed through to FUN_004e4320 as second arg)
//
// Guard: reads DAT_0066d704 — if 0, returns immediately (world not loaded).
// World selector: reads DAT_0066d700 — non-zero means world-B handle, else world-A.
//   World-B handle: DAT_00656ee8
//   World-A handle: DAT_0065742c
// Calls FUN_004e4320(world_handle, param_1) with the selected handle.
//
// Constants (cited from 0x00426670 body):
//   0x0066d704 — world-loaded guard flag  [0x00426675]
//   0x0066d700 — world-B selector         [0x0042667f]
//   0x00656ee8 — physics world handle B   [0x0042668b]
//   0x0065742c — physics world handle A   [0x00426699]
//
// Anti-island: callers FUN_0040df20 (C2), FUN_00492e90 (C2);
//   callee FUN_004e4320 (C2). C2+ satisfied on both sides.
//
// Diff strategy: int_scalar, signature {ret:'void', args:['int32']}.
//   At quiescent main menu, DAT_0066d704 == 0 → early return on all test
//   vectors → void_match → GREEN.  param_1 is never dereferenced (guard fires
//   first), so passing any int32 sentinel is safe.
//
// ref: re/analysis/render_promote_c2_track_loader/0x00426670.md
// ---------------------------------------------------------------------------

// 0x00426670
extern "C" __declspec(dllexport) void __cdecl WorldRenderDispatch_Begin(std::int32_t param_1)
{
    // Guard: if world is not loaded (DAT_0066d704 == 0), return immediately. [0x00426675]
    if (*reinterpret_cast<std::int32_t*>(0x0066d704u) == 0) {
        return;
    }

    // Select world handle based on world-B flag. [0x0042667f]
    std::uint32_t world_handle;
    if (*reinterpret_cast<std::int32_t*>(0x0066d700u) != 0) {
        // World-B: use handle at DAT_00656ee8. [0x0042668b]
        world_handle = *reinterpret_cast<std::uint32_t*>(0x00656ee8u);
    } else {
        // World-A: use handle at DAT_0065742c. [0x00426699]
        world_handle = *reinterpret_cast<std::uint32_t*>(0x0065742cu);
    }

    // Call world scene begin dispatcher with selected handle and camera pointer. [0x004266a0]
    s_FUN_004e4320(world_handle, static_cast<std::uint32_t>(param_1));
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(WorldRenderDispatch_Begin, 0x00426670);

// ---------------------------------------------------------------------------
// WorldRenderDispatch_End  --  0x004266b0
//
// Original: FUN_004266b0 (58 bytes, 0x004266b0..0x004266e9)
// Signature: void FUN_004266b0(undefined4 param_1)
//   param_1: RwCamera pointer (passed through to FUN_004e4350 as second arg)
//
// Exact mirror of WorldRenderDispatch_Begin (0x00426670), substituting
// FUN_004e4350 for FUN_004e4320.  Same guard, same world-A/B selector,
// same globals read.
//
// Constants (cited from 0x004266b0 body):
//   0x0066d704 — world-loaded guard flag  [0x004266b5]
//   0x0066d700 — world-B selector         [0x004266bf]
//   0x00656ee8 — physics world handle B   [0x004266cb]
//   0x0065742c — physics world handle A   [0x004266d9]
//
// Anti-island: callers FUN_0040df20 (C2), FUN_00492e90 (C2);
//   callee FUN_004e4350 (C2). C2+ satisfied on both sides.
//
// Diff strategy: same as WorldRenderDispatch_Begin (int_scalar, guard fires
//   at menu, void_match).
//
// ref: re/analysis/render_promote_c2_track_loader/0x004266b0.md
// ---------------------------------------------------------------------------

// 0x004266b0
extern "C" __declspec(dllexport) void __cdecl WorldRenderDispatch_End(std::int32_t param_1)
{
    // Guard: if world is not loaded (DAT_0066d704 == 0), return immediately. [0x004266b5]
    if (*reinterpret_cast<std::int32_t*>(0x0066d704u) == 0) {
        return;
    }

    // Select world handle based on world-B flag. [0x004266bf]
    std::uint32_t world_handle;
    if (*reinterpret_cast<std::int32_t*>(0x0066d700u) != 0) {
        // World-B: use handle at DAT_00656ee8. [0x004266cb]
        world_handle = *reinterpret_cast<std::uint32_t*>(0x00656ee8u);
    } else {
        // World-A: use handle at DAT_0065742c. [0x004266d9]
        world_handle = *reinterpret_cast<std::uint32_t*>(0x0065742cu);
    }

    // Call world scene end dispatcher with selected handle and camera pointer. [0x004266e0]
    s_FUN_004e4350(world_handle, static_cast<std::uint32_t>(param_1));
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(WorldRenderDispatch_End, 0x004266b0);
