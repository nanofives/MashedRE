// Mashed RE — WS-C-STANDALONE: opponent-AI reimplementation for mashed_re.exe.
//
// The dev-`.asi` AI port (Ai/AiController.cpp, Ai/AiTargeting.cpp) calls the
// ORIGINAL functions via absolute-RVA casts + RH_ScopedInstall, so it runs ONLY
// inside the injected MASHED.exe. This module is the STANDALONE reimplementation:
// zero original-code calls, so it can run in the rebased mashed_re.exe (where the
// RVA range is image-pad — DATA globals are valid writable memory, CODE is zeroed).
//
// All runtime inputs the original AI read through original code (own car world
// pos/velocity, game mode, round type, track index, alive flags, vehicle type)
// are abstracted behind Ai::Host so this file links with ZERO 0x00xxxxxx code
// casts. The WS-C-WIRE session binds Host to the standalone race state.
//
// STATUS: PHASED standalone reimpl, PENDING diff-original C4. Faithful: the
// tick/step/bank-select spine + the nearest-point lookahead core. Structural
// (constants flagged [UNCERTAIN]): the steering-angle calc (FUN_00415e20).
// STUBBED with RVA TODO (see .cpp header): the full targeting behaviour tree
// (modes 1..10), the 4/9/8 control-step variants, rubber-banding (FUN_004177b0),
// the bank-switch timer/RNG (FUN_00417180 detail), powerup activation
// (FUN_00415220), the spline interpolation+curvature-walk+wall-march refinement
// (FUN_00443300 / tail of FUN_00443dc0), and the .AI parser.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#pragma once

#include <cstdint>

namespace Ai {

// Host interface — bound by the standalone (WS-C-WIRE). Defaults are safe no-ops
// so the module links + is inert until wired. Every member replaces an original
// code call the AI cluster made (RVA cited).
struct Host {
    int   (*game_sub_mode)();                 // FUN_0040e350  (race=6, mode 5/7/8...)
    int   (*round_type)();                    // FUN_0042f6a0  (3/4/5/10 = AI-enabled rounds)
    int   (*game_mode_fd0)();                 // DAT_007f0fd0  dispatch selector (4/8/9 variants)
    int   (*track_index)();                   // FUN_00426c00  (0x21 = powerup-seek track)
    int   (*car_alive)(int v);                // FUN_0046c7b0  (1 = alive)
    int   (*veh_type)(int v);                 // FUN_0040e470  (0/1 = human; else AI)
    int   (*ai_target_enable)();              // FUN_00443080
    // own car world position (X,Z) and planar velocity (vx,vz) for vehicle v.
    // original: FUN_0046d4a0 (struct ptr, +0x30/+0x38) / FUN_0046d510 (velocity).
    void  (*own_xz)(int v, float* x, float* z);
    void  (*own_vel_xz)(int v, float* vx, float* vz);
};

// Install the host (call once at race start). Passing nullptr restores no-op defaults.
void Ai_SetHost(const Host* host);

// Per-frame entry — standalone reimpl of FUN_00418860 (AiTickLoop). Guarded on the
// race-line spline count (DAT_00801ca0 > 3); inert if the .AI splines are unloaded.
void Ai_Standalone_Tick();

} // namespace Ai
