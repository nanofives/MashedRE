// Mashed RE — WS-A8: run the ported vehicle-physics chain in mashed_re.exe.
//
// Standalone driver for the verbatim physics chain (A3 VehicleInit -> A4
// VehicleControlIntegrate -> A5 ForceIntegrator -> A6a Integrate2 -> A6b
// AeroStabilize), behind the MASHED_REAL_PHYSICS env toggle (default OFF -> the
// kinematic scaffold in TrackRenderer::UpdateCar stays the shipping path).
//
// This is the "make it RUN" half (NOT bit-identity C4 — that is WS-A-VERIFY-3).
// It allocates the 0xd04 record array (standalone mirror of DAT_008815a0), runs
// a SIMPLIFIED core of the FUN_00470c70 dispatcher (the global-binding + per-car
// integrate + contact pass — NOT the elimination/respawn/AI-targeting branches),
// and adapts the standalone car state <-> the record.
//
// STATUS: PENDING runtime-smoke (MAIN tree) + C4. WS-A8-GAPS (2026-06-17) status:
//   - [U-A8-SUBSTEP] CLOSED: StepPlayer subdivides the frame into <=50 fixed ~1ms
//     substeps (the FUN_00470c70 chunk granularity, local_24 = min(remaining,0x32)).
//   - terrain contacts CLOSED: VehiclePhysics_SetWorld() builds the contact soup
//     from the track collision tris -> WheelContactSolver reports grounded wheels
//     -> A5 suspension force is live (caller must SetWorld at track load).
//   - [U-A8-GRAVITY] RESOLVED as VESTIGIAL: ref-to(0x00803338) -> the ONLY writer
//     is FUN_00470c70's frame-end zeroing (0x004713fc, =0) and the only reader is
//     A5 (0x0046e316). So g_gravX/Y/Z are ALWAYS 0 in the original; our stub=0 is
//     faithful. Vertical dynamics come from suspension/contacts, not this term.
//   - [U-A8-STEER] STILL OPEN: FUN_00470670 consumes input[0]=accel/[1]=brake/[5]
//     only; the steer byte in the input descriptor (&DAT_007f1038 + map*0x13) ->
//     wheel steer-angle (+0x3c) path is unmapped, so the caller still applies steer
//     kinematically. Needs the descriptor steer-field RE (a follow-up session).
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Vehicle {

// Player car <-> record adapter payload (world space).
struct PlayerCarIO {
    float pos[3];           // in/out (caller integrates pos from vel)
    float vel[3];           // in/out -> record +0x9b0 (kVelocity)
    float yaw;              // in/out (radians); forward = {cos,0,sin}
    float speed;            // out    -> record +0x9e4 (kSpeed)
    // The vehicle input descriptor (&DAT_007f1038 + slot*0x4c). Byte map RESOLVED
    // (re/analysis/ai_ctrl_byte_map_RESOLVED_2026-06-16.md): [0]/[1]=steer cmd
    // (sign A/B; FUN_00470670 secondary body-impulse channel), [4]=accelerator,
    // [5]=brake/reverse. The PRIMARY throttle/brake consumer is FUN_00467650 (A6a),
    // which reads [4]/[5] — these MUST be set for the car to drive.
    std::uint8_t input[8];  // [4]=accel 0..255, [5]=brake/reverse 0..255
    // Standalone wheel-grounding hint (substitute for the un-portable RW BSP
    // broadphase FUN_00538c80 + device transform FUN_004c3d90): the caller knows
    // the car is on the track via its own GroundHeight collision; when set, the
    // chain marks the 4 wheels grounded so the drive/suspension blocks engage.
    int grounded;           // 1 = car is in ground contact (caller's GroundHeight ok)
};

bool VehiclePhysics_Enabled();                       // MASHED_REAL_PHYSICS set once
void VehiclePhysics_Init(int carCount, int trackType);
// Feed the track collision triangles (TrackRenderer col_verts_/col_tris_) to the
// wheel solver's broadphase (Collision::g_worldTris). Call once at track load.
void VehiclePhysics_SetWorld(const float* verts, int vertCount,
                             const unsigned* tris, int triCount);
void VehiclePhysics_StepPlayer(float dt, PlayerCarIO& io);

}  // namespace Vehicle
}  // namespace mashed_re
