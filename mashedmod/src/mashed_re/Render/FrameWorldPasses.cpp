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

// MATCH-S06: the callee pointers s_FUN_004e4320 / s_FUN_004e4350 went with the bodies (the matched source
// binds FUN_004e4320 -> RwPluginLinkSet, FUN_004e4350 -> Clear4e4350).

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

// MATCH-S06: the body of 0x00426670 (WorldRenderDispatch_Begin) moved to Match/MatchS06_00426670.cpp,
// which compiles the byte-matched source re/match/s06/00426670.c.

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

// MATCH-S06: the body of 0x004266b0 (WorldRenderDispatch_End) moved to Match/MatchS06_004266b0.cpp,
// which compiles the byte-matched source re/match/s06/004266b0.c.
