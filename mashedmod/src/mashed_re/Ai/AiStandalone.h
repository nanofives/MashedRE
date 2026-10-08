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
// behaviour tree (modes 1..10) and the FUN_00442a60 reference-distance array. Ported
// D3 2026-09-26: FUN_00415220 fire decision, FUN_00443440 curvature walk, the
// FUN_00443dc0 wall-march tail (FUN_00443300 was already verbatim).
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
    // [D3 2026-09-26] body FORWARD vector (X,Z) of vehicle v: FUN_0046d510 transforms
    // DAT_00614708 = (0,0,1) by the car matrix and returns rec+0x9d4/+0x9dc. FUN_00415e20
    // takes its heading from this, not from the velocity.
    void  (*own_fwd_xz)(int v, float* fx, float* fz);
    // float field of vehicle v's 0xd04 record (base 0x008815a0 in the original):
    // +0x9e4 (FUN_0046d6d0) and +0xb0c (FUN_0046d6a0), read by FUN_00416250/FUN_00415220.
    float (*veh_f32)(int v, int off);
    // power-up type code vehicle v holds (*[0x0088fc88 + v*0xb4], 7..19), 0 = none.
    int   (*held_powerup)(int v);
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

// [D3 2026-09-26] Advance the AI clock by one frame's tick budget: DAT_007f1008 = units,
// DAT_007f0ff4 += units, DAT_007f0ff8 += units (original 0x0040fc63 / 0x0040fe5e).
// The original's budget is 50 per 1/60 s frame. Call once per race frame before the tick.
void Ai_AdvanceClock(int units);
// FUN_00413fe0 per-vehicle AI-state reset + race-clock zeroing. Call at race start.
void Ai_ResetRace();

// [GATEFIRE 2026-10-08] FUN_0040e480 CarSlotStateSet, exported so the race-setup
// bridge in D3d9Render/TrackRenderer.cpp can fill the slot-STATE cells. The body
// (AiStandalone.cpp) early-returns when 0x005f2770 holds no pointer, so calling it
// without MASHED_SLOTSTATE_SEED is a no-op rather than a wild write.
void Ai_SetCarSlotState(int v, int state);
// FUN_00414030(v): force vehicle v's next lookahead to take the true nearest point.
void Ai_ResetVehicleIndex(int v);

// [D3 2026-09-27] Last control-step stack locals for vehicle v, so MASHED_AI_STEPDUMP
// can carry the SAME seven columns the original-side probe at 0x004165a5 reads out of
// FUN_00416250's frame (see re/frida/scenario_launch.py STEP_LOCALS). Order:
//   look_x, look_z, curv, own_x, own_z  (+ the two steer-history globals, read direct)
// march_n / march_idx0 mirror the original's FUN_00416230 call count and last argument
// (0x00444a2c / 0x0041623b): how many times FUN_00443dc0's phase-8 wall-march stepped
// the target back, and whether it ended at walk point 0. look_best / look_idx are
// standalone-only and carry the pre- and post-march walk index.
// `valid` is false until ControlStep has run once for v.
struct StepLocals { float look_x, look_z, curv, err, own_x, own_z;
                    int mode, march_n, march_idx0, look_best, look_idx, look_blk; bool valid; };
const StepLocals& Ai_LastStepLocals(int v);

} // namespace Ai
