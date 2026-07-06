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
// tick/step/bank-select spine + the nearest-point lookahead core + the 4/9/8
// control-step variants + bank-switch timer/RNG + post-step powerup-brake
// (P4a 2026-07-04) + pre-tick rubber-banding/rubber-band flag machine + the
// override-replay tail + the CarSlotStateSet alive-poke (P4b 2026-07-04).
// Structural (constants flagged [UNCERTAIN]): the steering-angle calc
// (FUN_00415e20). STUBBED with RVA TODO (see .cpp header): the full targeting
// behaviour tree (modes 1..10), powerup activation (FUN_00415220), the spline
// interpolation+curvature-walk+wall-march refinement (FUN_00443300 / tail of
// FUN_00443dc0), and the .AI parser.
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
    // Line-of-sight clearance for the spline lookahead's wall-march (FUN_00443dc0
    // Phase 8). Returns 1 if the straight segment (ax,az)->(bx,bz) stays on the
    // drivable surface, 0 if it crosses off-track. The original marches the AI tile
    // grid (DAT_007f1a9c/DAT_007f9a9c); the standalone bridge backs this with the
    // track collision (GroundHeight). Default no-op returns 1 (= always clear), so
    // the lookahead keeps its farthest target when unbound.
    int   (*los_clear)(float ax, float az, float bx, float bz);
};

// Install the host (call once at race start). Passing nullptr restores no-op defaults.
void Ai_SetHost(const Host* host);

// Per-frame entry — standalone reimpl of FUN_00418860 (AiTickLoop). Guarded on the
// race-line spline count (DAT_00801ca0 > 3); inert if the .AI splines are unloaded.
void Ai_Standalone_Tick();

// Faithful racing-line lookahead target for vehicle v at world (ownX,ownZ): selects
// the vehicle's spline bank (FUN_00418560) and runs the ported FUN_00443dc0 lookahead
// (Catmull-Rom closest-param + forward walk + LOS step-back). Writes the target XZ and
// returns true; returns false (target untouched) if the .AI banks aren't loaded. This is
// the "where to go" used by the standalone faithful-nav + robust-motion opponent drive
// (the verbatim ControlStep bands' accel+brake deadlock against the approximate physics
// chain is bypassed — see re/analysis/ai_spline_lookahead.md). Requires Ai_SetHost first.
bool Ai_ComputeTarget(int v, float ownX, float ownZ, float* outTx, float* outTz);

} // namespace Ai
