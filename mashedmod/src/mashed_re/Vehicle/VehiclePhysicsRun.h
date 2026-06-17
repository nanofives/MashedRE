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
//   - [U-A8-STEER] RESOLVED (WS-A8-STEER, 2026-06-17, Ghidra pool11 read-only):
//     The steer path is NOT a separate function — it IS the A4 (FUN_00470670)
//     channel previously mislabelled "drive torque". CORRECTED MODEL (RVA-cited):
//       * STEER cmd = descriptor bytes [0]/[1] (sign A / sign B; 0..255 magnitude).
//         AI writer FUN_00416250 stores *param_3 (=[0]) / param_3[1] (=[1]) from
//         the FUN_00415e20 signed steering-angle error (band split at _DAT_005cd09c,
//         0x004164.. / 0x0041650..). Human cook FUN_00496530 writes [0]/[1] + the
//         derived analog floats at +0x14/+0x18. ([4]/[5]=accel/brake — A6a path.)
//       * A4 FUN_00470670 READS input[0]/[1] (asm MOV EDI,EAX @0x470679; CMP [EBP],
//         BL @0x470732 = *param_3, [EBP+1] @0x470754 = param_3[1]) and writes the
//         scaled result to record +0x1a8 (input[0]) and +0x26c (input[1]). Those
//         two offsets ARE the WHEEL-0 / WHEEL-1 STEER-ANGLE slots (wheelN steer =
//         +0x16c + N*0xC4 + 0x3c => w0=+0x1a8, w1=+0x26c, w2=+0x330, w3=+0x3f4).
//         A4 zeroes all four every frame (0x004706c1..d3). Steer scale = input *
//         (+0x190 = 34.0; init FUN_0046b540 @0x42080000) * (1/256, _DAT_005ceaa8) *
//         0.5 -> ~16.93 deg at full lock. (+0x330/+0x3f4 have NO writer in
//         0x466000-0x472000 besides init + A4-clear -> rear wheels never steer =
//         FRONT-WHEEL steering; init sets wheel0/1 committed-mode +0x168/+0x22c = 2.)
//       * A5 FUN_0046ddb0 Phase 0 reads each wheel's steer angle piVar12[0xf]
//         (=+0x1a8/+0x26c/+0x330/+0x3f4); nonzero -> FUN_004c4d20(m, up=DAT_006146fc,
//         angleDEG, 0) rotates that wheel's forward axis. A6a (FUN_00467650) per-
//         wheel lateral grip then yaws the body via angular velocity +0x9bc (vec3;
//         world-Y component +0x9c0 = yaw rate).
//     WIRED: StepPlayer sets input[0]/[1] from io.steer; the chain produces the body
//     angular velocity; StepPlayer integrates io.yaw from the read-back yaw rate
//     (+0x9c0). The kinematic steer*kSteer*dt yaw stopgap is REMOVED when
//     MASHED_REAL_PHYSICS is on (TrackRenderer no longer kinematically yaws).
//     PENDING C4 (needs the matched-input installed-hook race diff, WS-A-VERIFY).
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Vehicle {

// Player car <-> record adapter payload (world space).
struct PlayerCarIO {
    float pos[3];           // in/out (caller integrates pos from vel)
    float vel[3];           // in/out -> record +0x9b0 (kVelocity)
    float yaw;              // in/out (radians); forward = {cos,0,sin}. When physics
                            // steering is on, StepPlayer advances yaw from the
                            // chain-integrated yaw rate (+0x9c0), NOT kinematically.
    float speed;            // out    -> record +0x9e4 (kSpeed)
    // The vehicle input descriptor (&DAT_007f1038 + slot*0x4c). Byte map RESOLVED
    // (ai_ctrl_byte_map_RESOLVED + WS-A8-STEER): [0]/[1]=STEER cmd (sign A/B, 0..255
    // magnitude — A4 FUN_00470670 reads these, writes the front-wheel steer angle to
    // +0x1a8/+0x26c), [4]=accelerator, [5]=brake/reverse (A6a FUN_00467650 reads
    // these). StepPlayer derives input[0]/[1] from `steer` and input[4]/[5] from the
    // caller-set throttle bytes — both MUST be set for a full drive+steer.
    std::uint8_t input[8];  // [0]/[1]=steer A/B, [4]=accel, [5]=brake (0..255)
    // Steering command in [-1,+1] (left/right; caller's in.steer). StepPlayer maps it
    // to the mutually-exclusive descriptor steer bytes [0]/[1] the original AI/human
    // writers use (FUN_00416250/FUN_00496530): +steer -> input[0], -steer -> input[1].
    float steer;            // in     -> descriptor steer bytes [0]/[1]
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
